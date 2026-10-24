# watchOS bring-up: measurements on the watch

Measured **24 October 2026** on **OPWWE231 A.192**. This note extends the
[boot and ISA analysis](./watchos-boot-and-isa-analysis.md) with what the real cores report.

## What the cores implement

[`cpuprobe`](../../tools/opwwe231/watchos/probe/cpuprobe.c) is a static aarch64 program (the kernel runs 64-bit code even though the
Android userspace is 32-bit). It reads the ID registers through the kernel's EL0 emulation (`HWCAP_CPUID`, set on this kernel).

| Register field | Value | Meaning |
| --- | --- | --- |
| `MIDR_EL1` | `0x51af8014` | Qualcomm, part `0x801` (Kryo 2xx Silver), variant 10, revision 4 |
| `ID_AA64PFR0` EL0 / EL1 / EL2 / EL3 | 1 / 1 / 0 / 0 | AArch64 at EL0 and EL1 only: **no EL2, no EL3 visible** |
| `ID_AA64ISAR0` ATOMIC | 0 | **No LSE atomics** (the watchOS kernel uses them in about 10,200 places) |
| `ID_AA64ISAR1` APA / API / GPA / GPI | 0 / 0 / 0 / 0 | **No pointer authentication** (the kernel is `arm64e`) |
| `ID_AA64ISAR0` AES / SHA1 / SHA2 / CRC32 | 2 / 1 / 1 / 1 | crypto extensions present |
| `ID_AA64MMFR0` TGran16 | 0 | **Not conclusive**: the kernel hides this field in the emulated read (TGran4 and TGran64 also read as "not implemented" although 4 KB pages work) |

The 16 KB granule question therefore stays open until a register read at EL1. Arm's Cortex-A53 manual lists only 4 KB and 64 KB
granules, and these cores are A53-class, so we expect the answer to be "not implemented".

## Booting an image without flashing it

The bootloader has no RAM boot: `fastboot boot` answers `unknown command` (checked with the stock `boot.img`, which is a v4 image of
43,565,056 bytes). Every experiment that needs the bootloader to load another image has to use a partition the loader reads:
`boot`, or `recovery` (the loader accepts a recovery image that carries its own kernel, see the
[second boot slot findings](./watchos-boot-and-isa-analysis.md#target-side-findings-for-a-second-boot-slot)).
A reset must then lead back to Android, which the BCB rules in [Custom recovery](../custom-boot/custom-recovery.md) make possible.

## What this means

- The watchOS kernelcache cannot run natively on these cores: it needs pointer authentication, LSE atomics, a 16 KB granule and an
  Apple SoC platform. The ID registers confirm two of those four directly.
- What remains practical: a bare-metal payload that measures the registers the kernel hides (including TGran16) and draws on the
  panel, then study of the kernelcache in an emulator on the Mac.
