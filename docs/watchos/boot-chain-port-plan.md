# watchOS boot-chain port plan (toward the UI, in emulation)

Goal: boot the watchOS 6.3.1 (Watch5,4 / T8006) kernel far enough to reach the user interface. The UI target is reachable in
**full-system emulation** (kernel → launchd → SpringBoard with a framebuffer), not on the watch's Qualcomm hardware (which lacks
PAC, the 16 KB granule and the entire Apple platform — see [bring-up status](./bring-up-status.md)). This is the step-by-step plan,
with honest status and the vehicle for each phase.

Two emulation vehicles, by what they provide:
- **Unicorn** (CPU only, no devices, no PAC): good for the reset vector and early boot with the Apple-isms patched out. It is where
  we validated the [kernel entry contract](./boot-chain-reconstruction.md). It cannot reach userland (no interrupts, no timer, no
  devices).
- **QEMU full-system** (`-cpu max` has FEAT_PAuth and the 16 KB granule **natively**): the vehicle for userland and the UI. Because
  QEMU implements PAC, the 331,911 PAC sites do **not** need patching on this path. What it needs is an Apple **T8006 machine**:
  the Apple IMPDEF system registers (82 distinct, [census](../../research/opwwe231/2026-10-24-watchos-boot/pac-and-register-census.txt))
  and the device models ([device map](./platform-device-map.md)). Reference forks that already do this for other Apple SoCs:
  [`qemu-t8030`](https://github.com/TrungNguyen1909/qemu-t8030) (A13, boots iOS 14) and
  [Aleph `xnu-qemu-arm64`](https://github.com/alephsecurity/xnu-qemu-arm64) (iOS 12).

## Phases

| # | Phase | Vehicle | Status |
| --- | --- | --- | --- |
| 0 | OTA triage, kernelcache + DeviceTree extract, `boot_args` + entry contract, hardware feature probe | — | **done** |
| 1 | Reset vector → MMU enable → ~6 M instructions of the C boot path (PAC-strip + Apple-reg stub + flat memory + real DeviceTree) | Unicorn | **done** ([harness](../../tools/opwwe231/watchos/emu/boot_harness.py)) |
| 1b | Full patch/census scope: 331,911 PAC sites, 82 Apple IMPDEF registers | — | **done** ([tools/port](../../tools/opwwe231/watchos/port/)) |
| 2a | Reference fork acquired and its XNU loader / CPU / patchfinder / device models mapped to T8006 | QEMU | **done** (below) |
| 2b | **QEMU fork built** (7.1.0, t8030 machine); **our watchOS kernelcache loads and the patchfinder runs** on it with our DeviceTree -- see [build & boot](./qemu-build-and-boot.md) | QEMU | **in progress** |
| 2c | Kernel console output: adapt machine+CPU to T8006. **Machine init completes** (nvram made optional) and the kernel launches; the **[sn0w CPU layer](../../tools/opwwe231/watchos/qemu/sn0w/)** RAZ/WIs the Apple IMPDEF space so the kernel runs deep into early boot (PAN set, per-CPU PPL lookups). **`"No cpus found!"` RESOLVED** (the DT filter stripped the `apple,tempest` `/cpus` nodes; fixed by whitelisting them in `KEEP_COMP`). Kernel now runs far into early init; **new blocker: `panic("rtclock timebase_callback: invalid constant")`** — the mach-timebase is `{0,0}` because `gPEClockFrequencyInfo` is unset when rtclock runs (init-ordering; see [build & boot](./qemu-build-and-boot.md)). | QEMU | **in progress** ([patch](../../tools/opwwe231/watchos/qemu/t8006-adaptation.patch)) |
| 2d | Stub the coprocessor mailboxes (SEP/ANS/AOP/RTP/SMC/SIO) so boot does not hang; model ANS enough to read the rootfs | QEMU | planned |
| 2e | VM/pmap/scheduler come up → **launchd** starts (userland reached) | QEMU | planned |
| 3a | Root filesystem: our OTA is an update delta; reconstruct or obtain a full Watch5,4 rootfs + the `arm64_32` dyld shared cache | — | open risk |
| 3b | launchd → the watchOS **SpringBoard** (carousel) runs | QEMU | planned |
| 3c | Model `disp0` as a framebuffer → the **UI renders** in QEMU | QEMU | the goal |

## Phase 2a: the reference fork, mapped (done)

`qemu-t8030` (study-only clone) already implements, generically, most of what "patching the whole boot chain" means for XNU. The
reusable pieces and how they map to our target:

| Reference component | What it does | For T8006 |
| --- | --- | --- |
| `hw/arm/xnu.c` | loads an XNU kernelcache, **builds `boot_args` + the flattened DeviceTree**, places the kernel, enters at EL1 | reuse as-is; feed our kernelcache + the real `DeviceTree.n146bap` |
| `include/hw/arm/xnu.h` `struct xnu_arm64_boot_args` | the boot_args layout | **matches our reverse-engineered struct exactly** (Revision, virtBase, physBase, memSize, topOfKernelData, deviceTreeP, CommandLine, bootFlags, memSizeActual) -- independent confirmation of our analysis |
| `hw/arm/apple_a13.c` | the Apple CPU: PAC, GXF, and the Apple IMPDEF registers (`ARM64_REG_HID*`, `EHID*`, `APCTL` = `S3_x_C15_x`) | adapt the HID/EHID register set to `apple,tempest` (S5); most carry over |
| `hw/arm/xnu_kpf.c` | the **kernel patchfinder**: bypasses AMFI/trustcache (unsigned code runs) and APFS root-auth (rootfs mounts) by pattern | adapt the match patterns to watchOS 6.3.1 (17U224) |
| `hw/intc/apple_aic.c` | AIC interrupt controller (+ timebase) | re-base to our AIC `0x2d180000` / timer `0x2d188000` |
| `hw/char/apple_uart.c` | the `uart-1,samsung` console | re-base to our uart0 `0x2e500000` |
| `hw/arm/apple_dart.c` + `apple_dart-stub.c` | the DART IOMMUs (+ a stub) | our 13 DARTs; stub to pass through |
| `hw/arm/apple_sep.c`, `apple_sart.c` | Secure Enclave + SART | stub SEP; SART for ANS |

Memory map alignment (no change needed): the reference uses DRAM base `0x800000000` and the SoC/AMCC window `0x200000000` -- the
same conventions our `boot_args` and DeviceTree use.

The remaining new work (phase 2b) is a `hw/arm/t8006.c` machine: copy `t8030.c`, swap the device addresses to our
[T8006 device map](./platform-device-map.md), set the CPU to the 2-core Tempest, trim the coprocessor set, and wire AIC + timer +
PMGR + uart0. Then build the fork (meson/ninja) and boot our kernelcache with our DeviceTree to reach kernel console output (2c).

## Tools in place

- [`tools/opwwe231/watchos/bootargs/`](../../tools/opwwe231/watchos/bootargs/) — `boot_args.h` + `mk_bootargs` (the x0 struct; offsets verified against the reset vector).
- [`tools/opwwe231/watchos/emu/boot_harness.py`](../../tools/opwwe231/watchos/emu/boot_harness.py) — Unicorn early-boot test (phase 1).
- [`tools/opwwe231/watchos/port/pac_census.py`](../../tools/opwwe231/watchos/port/pac_census.py) — Capstone census of the PAC and Apple-register sites (phase 1b).
- [`tools/opwwe231/watchos/port/kernel_patch.py`](../../tools/opwwe231/watchos/port/kernel_patch.py) — rewrites the bulk PAC forms (Unicorn path only; QEMU needs no PAC patch).
- [`tools/opwwe231/watchos/qemu/sn0w/`](../../tools/opwwe231/watchos/qemu/sn0w/) — the **sn0w compat layer**: a modular shim that RAZ/WIs a CPU's IMPDEF system-register space so a QEMU core can run a foreign kernel; mounted into the fork by the adaptation patch. Extensible to other SoCs (see its README).
- [`tools/opwwe231/watchos/probe/cpuprobe.c`](../../tools/opwwe231/watchos/probe/cpuprobe.c) — the watch's core ID registers.

## Honest risks

- **Rootfs.** The supplied asset is a SoftwareUpdate OTA (a delta), not a full restore image. Reaching SpringBoard needs a complete
  Watch5,4 root filesystem; the OTA payload may be insufficient and a full IPSW (or on-device extraction) might be required.
- **T8006 machine.** No public QEMU machine exists for the S5 watch; it is adapted from the A13 (`qemu-t8030`) fork. The SoC, the
  coprocessor set and the display differ, so this is substantial work with intermediate milestones (kernel console, then launchd).
- **Display.** The watchOS UI needs the Apple display pipeline modelled as a framebuffer — the last and hardest device.
- **Not the watch.** This whole plan targets emulation. Running the result on the OnePlus watch is a separate, hardware-blocked effort.
