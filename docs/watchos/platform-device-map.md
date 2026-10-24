# T8006 platform device map (the QEMU machine spec)

From the watchOS DeviceTree (`device-tree/arm-io`, Apple S5 / T8006, Watch5,4). Raw decode:
[`t8006-platform-device-map.txt`](../../research/opwwe231/2026-10-24-watchos-boot/t8006-platform-device-map.txt). This is the
device-model spec for a QEMU machine that boots this kernel. Offsets are within the `arm-io` MMIO window; the absolute address is
the SoC base (0x2_0000_0000 class) plus the offset.

| Device | compatible / type | offset | size | IRQ | role |
| --- | --- | --- | --- | --- | --- |
| `aic` | interrupt-controller | `0x2d180000` | `0x8000` | — | **Apple Interrupt Controller** (essential) |
| `aic-timebase` | timer | `0x2d188000` | `0x1000` | — | **system timer** (scheduler tick, essential) |
| `pmgr` | pmgr | `0x2d000000` | `0x180000` | | **power/clock manager** (essential; the kernel ungates clocks) |
| `uart0` | `uart-1,samsung` | `0x2e500000` | `0x4000` | `0x106` | **serial console** (essential to see boot output) |
| `uart2` | uart | `0x2e508000` | `0x4000` | `0x108` | aux uart |
| `wdt` | wdt | `0x3d2b0000` | `0x4000` | `0xad` | watchdog |
| `i2c1`/`i2c3` | i2c | `0x2e014000`/`0x2e01c000` | `0x1000` | `0x11c`/`0x11e` | sensor/PMU buses |
| `sep` | sep | `0x42400000` | `0xc000` | | Secure Enclave (mailbox IOP → stub) |
| `ans` | ans | `0x77400000` | `0xc000` | | NAND storage (mailbox IOP → stub, then model for the rootfs) |
| `aop` / `rtp` / `smc` / `sio` | IOP | `0x4c400000` / `0x4cc00000` / `0x3e400000` / `0x2f400000` | | | always-on / real-time / SMC / serial IO coprocessors (stub) |
| `disp0` | display-subsystem | `0x18000000` | `0x2f0000` | | **display** (needed only for the UI milestone) |

The CPUs are `apple,tempest` (2 cores) — the S5's A-class core, which has FEAT_PAuth, the 16 KB granule and LSE, so a QEMU
`-cpu max` core matches its ISA. The coprocessors (`sep`, `ans`, `aop`, `rtp`, `smc`, `sio`) are RTBuddy/mailbox IOPs: the kernel
talks to them over a shared-memory mailbox, so they can be stubbed (ack the mailbox, report ready) to get past boot before any of
them is modelled for real. 13 DARTs (IOMMUs) sit in front of the DMA masters; for emulation they can pass through.

A minimal "boot to kernel console" QEMU machine therefore needs: the CPU (`-cpu max`), the `arm-io` MMIO window, **AIC**,
**aic-timebase**, **PMGR**, and **uart0**. That is the first concrete milestone, well before the UI. See the
[port plan](./boot-chain-port-plan.md).
