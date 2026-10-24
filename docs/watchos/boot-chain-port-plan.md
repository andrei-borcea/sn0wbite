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
| 2a | Build a reference Apple-silicon QEMU fork (`qemu-t8030` / Aleph) | QEMU | next |
| 2b | Create a **T8006 machine**: CPU, `arm-io`, AIC, aic-timebase, PMGR, uart0; load the kernelcache with our `boot_args` + DeviceTree | QEMU | planned |
| 2c | Reach **kernel console output** over uart0 (the first big visible milestone) | QEMU | planned |
| 2d | Stub the coprocessor mailboxes (SEP/ANS/AOP/RTP/SMC/SIO) so boot does not hang; model ANS enough to read the rootfs | QEMU | planned |
| 2e | VM/pmap/scheduler come up → **launchd** starts (userland reached) | QEMU | planned |
| 3a | Root filesystem: our OTA is an update delta; reconstruct or obtain a full Watch5,4 rootfs + the `arm64_32` dyld shared cache | — | open risk |
| 3b | launchd → the watchOS **SpringBoard** (carousel) runs | QEMU | planned |
| 3c | Model `disp0` as a framebuffer → the **UI renders** in QEMU | QEMU | the goal |

## Tools in place

- [`tools/opwwe231/watchos/bootargs/`](../../tools/opwwe231/watchos/bootargs/) — `boot_args.h` + `mk_bootargs` (the x0 struct; offsets verified against the reset vector).
- [`tools/opwwe231/watchos/emu/boot_harness.py`](../../tools/opwwe231/watchos/emu/boot_harness.py) — Unicorn early-boot test (phase 1).
- [`tools/opwwe231/watchos/port/pac_census.py`](../../tools/opwwe231/watchos/port/pac_census.py) — Capstone census of the PAC and Apple-register sites (phase 1b).
- [`tools/opwwe231/watchos/port/kernel_patch.py`](../../tools/opwwe231/watchos/port/kernel_patch.py) — rewrites the bulk PAC forms (Unicorn path only; QEMU needs no PAC patch).
- [`tools/opwwe231/watchos/probe/cpuprobe.c`](../../tools/opwwe231/watchos/probe/cpuprobe.c) — the watch's core ID registers.

## Honest risks

- **Rootfs.** The supplied asset is a SoftwareUpdate OTA (a delta), not a full restore image. Reaching SpringBoard needs a complete
  Watch5,4 root filesystem; the OTA payload may be insufficient and a full IPSW (or on-device extraction) might be required.
- **T8006 machine.** No public QEMU machine exists for the S5 watch; it is adapted from the A13 (`qemu-t8030`) fork. The SoC, the
  coprocessor set and the display differ, so this is substantial work with intermediate milestones (kernel console, then launchd).
- **Display.** The watchOS UI needs the Apple display pipeline modelled as a framebuffer — the last and hardest device.
- **Not the watch.** This whole plan targets emulation. Running the result on the OnePlus watch is a separate, hardware-blocked effort.
