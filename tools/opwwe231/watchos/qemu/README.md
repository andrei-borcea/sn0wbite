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
   registers" scope item becomes on the QEMU path.
3. **Minimal boot-to-console machine** (`t8030.c`): skip the coprocessor device
   creates (ANS/SART/SMC/SIO/SPMI/PMU/USB/DART/GPIO/I2C/display) that assert on
   the T8006 DeviceTree, and make the `nvram` device optional in
   `t8030_memory_setup` (nvram is an NVMe namespace off the ANS we skip, and is
   not needed to reach kernel console — boot proceeds with an empty nvram proxy).

## Status (24 October 2026)

- **Machine init completes**: with nvram optional, the machine reaches the
  init-done notifier without aborting and the kernel is launched
  (`entry 0x…805df8538`, cmdline passed).
- **IMPDEF space neutralized**: the first Undefined-Instruction trap
  (`msr s3_4_c15_c2_1` at `0xfffffff008b44010`, REA `ev_a791894f…`) is gone; the
  kernel now runs deep into early boot (sets PAN, performs per-CPU PPL lookups
  keyed on `MPIDR_EL1 & 0xff`).
- **Next blocker (root cause found)**: the kernel panics **`"No cpus found!"`**
  (REA `ev_aa85dcf4`) — `ml_parse_cpu_topology` enumerates no CPU from the
  DeviceTree. The panic path (`panic` `0x…08b1af1c` -> `panic_trap_to_debugger`
  `0x…07824328`) then reads `tpidr_el1` (still 0 this early), faults
  (`ldr [tpidr_el1+0x4a0]`, FAR 0x4a0), and the exception entry corrupts its own
  text and loops on `udf #0`. Not a count issue (`-smp 2` is identical). The
  machine must fix up the `/cpus` node runtime properties (iBoot's job on real
  hardware) for the two Tempest cores — the next session's task.

Apply: `cd qemu-t8030 && git apply <this>/t8006-adaptation.patch` then rebuild.
