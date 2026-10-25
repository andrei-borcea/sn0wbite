# QEMU T8006 adaptation (work in progress)

`t8006-adaptation.patch` is applied on top of the `qemu-t8030` fork (see
[build & boot](../../../../docs/watchos/qemu-build-and-boot.md)) to run our
watchOS 6.3.1 (Watch5,4 / T8006) kernelcache on its `t8030` machine. It is the
incremental machine/CPU adaptation toward kernel console output (phase 2c).

Reusable, modular piece: the **[sn0w compat layer](./sn0w/)** (`sn0w_cpu.{c,h}`),
a documented shim for mounting a foreign kernel's CPU on a QEMU core. See its
README for the porting guide.

## What the patch does

1. **A13 CPU node optional** (`apple_a13.c`): make `cluster-id` / `cluster-type`
   optional so the single-cluster Tempest (S5) CPU node is accepted (the A13 is
   big.LITTLE with those props; Tempest has neither). Fixes the first crash.
2. **sn0w CPU layer + hook** (`apple_a13.c`, `sn0w_cpu.{c,h}`, `meson.build`):
   RAZ/WI the Apple IMPDEF (`op0=3, CRn=15`) register space before the A13's real
   table, so the S5/Tempest IMPDEF registers the A13 model does not implement no
   longer trap to Undefined Instruction. This is what the "neutralize 82 IMPDEF
   registers" scope item becomes on the QEMU path. Registers the kernel reads
   back are then *promoted* from RAZ/WI to real definitions: so far the **APRR**
   permission-remap pair `APRR_EL0`/`APRR_EL1` (`S3_4_C15_C2_0`/`_1`), which
   `sn0w_cpu_install_apple_aprr()` gives genuine **read/write** backing
   (`CPUARMState.sprr.aprr_el[]`, a one-field `target/arm/cpu.h` add), seeded to
   their arch defaults. R/W (not CONST) is required because the kernel reads them
   back twice: XNU's pmap PPL/xPRR check wants the defaults, and the PPL call gate
   toggles `APRR_EL1` to the PPL-writable value and verifies the readback. (The
   A13 core models only the A14-style SPRR encodings at `opc1=6`; APRR is the
   older `opc1=4` pair.)
3. **Minimal boot-to-console machine** (`t8030.c`): skip the coprocessor device
   creates (ANS/SART/SMC/SIO/SPMI/PMU/USB/DART/GPIO/I2C/display) that assert on
   the T8006 DeviceTree, and make the `nvram` device optional in
   `t8030_memory_setup` (nvram is an NVMe namespace off the ANS we skip, and is
   not needed to reach kernel console — boot proceeds with an empty nvram proxy).

## Status (25 October 2026)

- **Machine init completes**: with nvram optional, the machine reaches the
  init-done notifier without aborting and the kernel is launched
  (`entry 0x…805df8538`, cmdline passed).
- **IMPDEF space neutralized**: the first Undefined-Instruction trap
  (`msr s3_4_c15_c2_1` at `0xfffffff008b44010`, REA `ev_a791894f…`) is gone; the
  kernel now runs deep into early boot (sets PAN, performs per-CPU PPL lookups
  keyed on `MPIDR_EL1 & 0xff`).
- **`"No cpus found!"` fixed**: the DT filter `macho_dtb_node_process` removed the
  `apple,tempest` `/cpus` nodes because that compatible was missing from
  `KEEP_COMP` (REA `ev_aa85dcf4`/`ev_189cc62f`). Whitelisting `apple,tempest`
  clears the 2.4 M-trap death spiral; the kernel now runs far into early
  per-CPU/pmap init.
- **Timebase panics fixed**: both `rtclock timebase_callback: invalid constant`
  and the generic-timer `invalid bit index` came from `gPEClockFrequencyInfo
  .timebase_frequency_hz = 0`. Root cause is one uninitialised seed, so the patch
  writes `24 MHz` into it at load time (and ships `timebase-frequency` in the DT).
  Boot advances through the timers into IOKit/platform-expert init.
- **IOKit interrupt-controller resolution bypassed**: `IODeviceTreeAlloc`
  phase-1 registers no controller, so the per-node interrupt resolver NULL-derefs
  on the empty arrays for the AIC phandle `0x10`. Bring-up tolerant no-op of the
  resolver (`movz w0,#1; ret`) gets past it; interrupts are not routed (clean AIC
  registration still pending). See [build & boot](../../../docs/watchos/qemu-build-and-boot.md).
- **pmap APRR check + ppl_dispatch fixed (APRR made R/W)**: two panics,
  `"%s: modifying a PPL mapping …"` (`0x…08b22650`, REA `ev_87531d34`) and
  `"ppl_dispatch: failed due to bad arguments/state"` (`0x…08b83130`), both came
  from the kernel reading back `APRR_EL0`/`APRR_EL1` (`S3_4_C15_C2_0/1`): the pmap
  check demands the arch defaults, and the PPL call gate (`0x…08b33fe0`, no
  `genter` — this kernel has no GXF) writes `APRR_EL1` to the PPL-writable value
  then verifies the readback. RAZ/WI (and then CONST) failed both; making the pair
  genuine R/W (item 2) clears both. PPL routines now execute.
- **APRR MMU permission-remap fixed (bring-up grant)**: PPL mapped its pages
  read-only, non-exec (`ap=2,xn=1`) and relied on APRR to make them writable in PPL
  mode, but the core remaps only the A14 SPRR path — so PPL's `bzero`/`dc zva` of a
  fresh page-table page looped on a write perm fault (ESR `0x9600004f`).
  Instrumenting `get_S1prot` showed the PPL-mode APRR tables do **not** mark those
  indices writable, so the write-grant is the "PPL mode" state itself; `ptw.c` now
  grants EL1 R/W to EL1-RO non-exec pages. PPL writes succeed; boot runs through PPL
  and VM init.
- **Physical-timer FIQ delivery fixed**: boot reaches the **scheduler / idle
  thread** (`0x…0784aca8`) but the timer interrupt was never delivered —
  `apple_a13.c` wired only the virtual timer (`GTIMER_VIRT`) to the FIQ, while
  watchOS drives the **physical** timer (`CNTP`; gdbstub: `CNTP_CTL_EL0=0x5`,
  `CNTV_CTL_EL0=0`). Wired `GTIMER_PHYS` to the FIQ OR-gate; verified the FIQ
  vector (`VBAR+0x300=0x…07df4100`) now fires.
- **Current blocker**: the **bootstrap thread does not progress** (still no
  console). The scheduler is up and the idle thread runs, but the thread that would
  run `console_init`/IOKit never becomes runnable (not a CPU1 wait — `-smp 1`
  stalls the same; CPU1 just parks at the reset vector). It is blocked on a
  stubbed/missing device or a completion that never arrives (coprocessors skipped,
  interrupt resolver no-op'd → only the per-CPU timer FIQ is wired, no AIC-routed
  device IRQs). Next: trace what the non-idle thread blocks on and provide that
  completion (likely AIC-routed interrupt delivery). Still upstream of `serial_init`.

Apply: `cd qemu-t8030 && git apply <this>/t8006-adaptation.patch` then rebuild.
