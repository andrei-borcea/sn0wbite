# watchOS 6.3.1 (Watch5,4): boot chain, instruction requirements and port feasibility

Analysis date: **22 October 2026**. Subject: the Apple update package identified in
[watchOS reference](./watchos-reference.md) (watchOS **6.3.1 / 17U224**, `Watch5,4` /
`N146bAP`, chip ID `0x8006`, an Apple S4/S5-class SoC). Target: **OnePlus Watch 2
(OPWWE231)**, Qualcomm Snapdragon W5 Gen 1.

Statements are labelled **Observed** (read from the artifact or the device),
**Inferred** (follows from observations plus public documentation) or **Unknown**.
Apple files and derived binaries stay private, per the
[evidence policy](../evidence-policy.md); this note publishes conclusions and counts.

## Summary

- **The watchOS kernel in this package cannot execute natively on the W5 Gen 1 cores.**
  Observed: the kernel is `arm64e` (pointer authentication), its boot code programs a
  **16 KB translation granule**, and it uses ARMv8.1 atomics, `PAN` and 72 distinct
  Apple implementation-defined system registers. The watch's Kryo 2XX Silver cores are
  ARMv8.0 (A53-class).
- **The platform layer has no Qualcomm counterpart.** The DeviceTree describes Apple's
  interrupt controller, 13 DART IOMMUs, seven ASC coprocessors (including SEP and the
  NAND controller) and Apple display, GPU and neural-engine blocks.
- **The Apple boot chain cannot be reused.** Only the kernelcache and DeviceTree are
  readable; the iBoot family and SEP firmware are encrypted.
- **Verdict:** a native port is not practical. Intermediate milestones that are
  practical are listed under [Feasibility](#feasibility).
- **Dual boot:** Magisk alone cannot provide it on this device. See
  [Target-side findings](#target-side-findings-for-a-second-boot-slot).

## Artifact

| Item | Observed value |
| --- | --- |
| Package | `com.apple.MobileAsset.SoftwareUpdate`, 142 zip records, 1,588,005,421 bytes uncompressed |
| Build manifest | one identity, `n146bap`, "Customer Software Update", 37 components, chip `0x8006`, board `0x16` |
| Root filesystem | 105,655 BOM entries, 3,957,125,627 bytes; `dyld_shared_cache_arm64_32` is 1,060,896,768 bytes |
| Payload encoding | `payloadv2/payload.000..019` are `pbzx` streams of a proprietary entry format; not parsed |

## Boot chain as shipped

Order and flags come from `BuildManifest.plist`; the first stage is inferred.

| Stage | Component | Format | Protection | Notes |
| --- | --- | --- | --- | --- |
| 0 | SecureROM | not in package | n/a | Inferred from the Apple boot model |
| 1 | `LLB.n146b` | IM4P `illb`, iBoot-5540.144.2, 1,088,256 B | KBAG (2 keys), LZFSE to 1,412,288 B | Not inspectable without GID keys |
| 2 | `iBoot.n146b` | IM4P `ibot`, same build | same | Loads the items below |
| DFU | `iBSS`, `iBEC` | IM4P, same build | same | Recovery and DFU stages |
| 3 | `sep-firmware.n146b` | IM4P `sepi`, 4,898,816 B | KBAG (2 keys) | Secure Enclave firmware |
| 3 | `DeviceTree.n146bap` | IM4P `dtre`, 160,244 B | none | EmbeddedDeviceTrees-4811.144.1 |
| 3 | `kernelcache.release.watch5` | IM4P `krnl`, 11,252,055 B | LZFSE, no KBAG | Decodes to 30,111,912 B |
| 3 | trust caches | IM4P `trst` / `rtsc` | none | 39,756 B and 8,164 B |
| 3 | iBoot-loaded firmware | AOP, ANE, AGX, BT and Wi-Fi (Marconi), RTP, SCE, SIO, wireless power | none | Flagged `IsLoadedByiBoot` |
| OS | Multitouch, haptics, baseband, eUICC, secure element | assorted | not parsed | Loaded by the OS, not by iBoot |

Observed in the DeviceTree `chosen` node: `secure-boot`, `image4-supported`,
`amfi-allows-trust-cache-load` and `gid-aes-key` properties. Inferred: the images are
personalised per device (IM4M ticket bound to an Apple ECID), so the shipped files are
not directly bootable even on Apple hardware without an Apple-signed ticket.

## Kernelcache

| Item | Observed |
| --- | --- |
| Format | Mach-O 64-bit executable, `arm64e`, built for **watchOS 6.3.0**, UUID `81F63C65-74D2-3031-A68D-6AE3A06D16C5` |
| Layout | monolithic: `__PRELINK_TEXT` is empty, 557 kext Info.plists in `__PRELINK_INFO`, 1,334 init functions in `__kmod_init`, 20.1 MB in `__TEXT_EXEC` |
| Protection segments | `__PPLTEXT`, `__PPLTRAMP`, `__PPLDATA*` (Page Protection Layer), `__LAST,__pinst` |
| Entry | `LC_UNIXTHREAD` pc `0xfffffff007df8538` |
| MMU setup | boot `TCR_EL1` = `0x000800226519a519`: `TG0=16 KB`, `TG1=16 KB`, 39-bit VA, 40-bit PA; boot code builds 2048-entry tables with 32 MiB blocks |

## Instruction requirements

Counts come from a disassembly pass (Apple LLVM 17 `llvm-objdump`) over every code section;
the kernel has 5,041,526 decoded instructions and the userland sample (118 `arm64_32`
binaries from the update ramdisk, 91 MB) has 10,725,142. Undecodable words are mostly
data in code sections.

| Requirement | Kernel | Userland sample | W5 Gen 1 cores |
| --- | --- | --- | --- |
| Non-hint pointer authentication (`blraa`, `ldraa`, `retab`, `braa`, ...) | 267,607 | 2 | Absent. Observed: no PAuth detection in the boot log |
| Hint-space PAC (`pacibsp`, `autibsp`) | 64,195 | 0 | Execute as NOP |
| LSE atomics (`cas*`, `ldadd*`, `swp*`) | 10,211 | 10,959 | Absent. Observed: no LSE detection in the boot log |
| `msr PAN` | 89 | 0 | Absent. Observed: Linux emulates PAN with TTBR0 switching |
| `ldapr*` (RCpc) | 286 | 339 | Inferred absent (ARMv8.3) |
| Apple `S3_*_C15_*` system registers (all `CRn=15`) | 72 distinct, 117 `mrs`/`msr` forms | 3 reads, likely data | Absent |
| Unknown Apple opcodes (`0x00200820`, `0x00200c20`) | 2 | n/a | Absent; no `genter`/`gexit` |
| AES, SHA-1/2, PMULL, CRC32 | used | used | Present. Observed in `/proc/cpuinfo` |
| 16 KB translation granule | hard-coded | segments 16 KB aligned | 4 KB in use; 16 KB inferred unsupported |

The 16 KB row rests on public documentation: a 2017 Linux kernel thread
([lkml](https://lkml.iu.edu/hypermail/linux/kernel/1705.2/01984.html)) cites ARM DDI 0500F
for "CA53 doesn't support 16KB memory translation granule size" and logs
`ID_AA64MMFR0_EL1` as `0x1122` on the A53 cluster versus `0x101122` on an A73 cluster. A
2015 mailing-list post claims otherwise. The register has not been read on this unit.

Observed: the kernel's `TCR_EL1` setter tests bit 2 of `S3_4_C15_C1_2` before writing.
Inferred from XNU register naming: this is the VMSA lock, part of the APRR/KTRR block
(`S3_4_C15_C1_*`, `S3_4_C15_C2_*`) that PPL depends on.

## Hardware described by the DeviceTree

| Function | Apple (DeviceTree, observed) | W5 Gen 1 (watch, observed) |
| --- | --- | --- |
| CPU | 2 cores, `apple,tempest` | 4 cores, Kryo 2XX Silver rAp4, MIDR `0x51af8014` |
| Interrupts | AIC (`aic,1`) | GICv3, 640 SPIs |
| IOMMU | 13 DART instances | Arm SMMUv2 |
| Storage | NAND via the `ans` coprocessor | eMMC (`sdhci`) |
| Display | `disp0,t8006` with Synopsys DSI | Qualcomm DSI, ICNA3311 AMOLED panel |
| GPU | AGX (`gpu,t8006`, firmware `armfw_g11m`) | Qualcomm KGSL |
| Secure processor | SEP (ASC coprocessor) | Qualcomm TZ; kernel enters at EL1 (HYP mode unavailable) |
| UART | `uart-1,samsung` | Qualcomm GENI, `ttyMSM0` |
| Timer | Arm generic timer | Arm generic timer, 19.2 MHz |

Of 75 devices under `arm-io` with a `compatible` string, none maps to Qualcomm hardware.
Seven are ASC coprocessors (AOP, RTP, SMC, SEP, ANS, SIO, GPU).

## Feasibility

| Layer | Blocking? | Work needed | Confidence |
| --- | --- | --- | --- |
| 16 KB granule | **Yes**, if the core lacks it | Rebuild or deeply patch the kernel for 4 KB, or emulate | High from the binary; medium for the CPU until measured |
| Instruction set | Yes | Rewrite about 270,000 PAC and 10,200 LSE sites plus `PAN` and `ldapr`; trap or patch Apple registers | High |
| Boot protocol | Yes | Replace iBoot: synthesise `boot_args` and an Apple DeviceTree, no signature path | High |
| Platform drivers | Yes | Replace AIC, DART, ANS, PMGR, display, GPU, UART with Qualcomm equivalents | High |
| Secure features | Yes | PPL, KTRR, AMFI and SEP dependencies must be removed or faked | Medium |
| Userland | Yes | `arm64_32` code needs LSE; DSC and apps not yet surveyed | Medium |

Our estimate (judgment, not measured): a native port is a multi-person-year research
program and is not practical. Practical milestones, in order:

1. A second boot slot and a bare-metal bring-up payload with diagnostic output
   (pstore/ramoops): days to weeks, low to moderate risk.
2. Read `ID_AA64MMFR0_EL1`, `ID_AA64ISAR0/1_EL1` on the real core to settle the 16 KB
   question: days, depends on milestone 1.
3. Study-only execution of this kernelcache in an emulator on the Mac: not evaluated here.
4. Native kernel entry on the watch: blocked unless milestone 2 finds 16 KB support.

## Target-side findings for a second boot slot

All observed on the watch or in its stock backup.

- Non-A/B: no `_a`/`_b` partitions, empty `ro.boot.slot_suffix`. Running kernel 5.15.185
  has `CONFIG_KEXEC`, `KEXEC_FILE` and `CRASH_DUMP` disabled and starts at EL1
  (`kvm [1]: HYP mode not available`). Magisk therefore cannot add a slot or chain-load
  another kernel by itself.
- `recovery.img` is a v4 boot image with `kernel_size=0` and a 12 MB ramdisk, so the stock
  recovery runs on the `boot` kernel. `adb reboot recovery` is one-shot and `adb reboot`
  returns to Android (tested). In stock recovery `adb shell` aborts with an SELinux error;
  only `adb reboot` works.
- The bootloader is Qualcomm LinuxLoader. Disassembly shows it reads the recovery
  `header_version` (offset `0x28`) and `kernel_size` (offset `0x8`) and prints
  "Recovery partition has no kernel" only when `header_version >= 3` and
  `kernel_size == 0`; otherwise the recovery image is treated as a complete boot image.
  Booting a recovery image that carries its own kernel is therefore plausible but not yet
  traced end to end or tested on hardware.
- Rescue paths in the loader strings: "PRESS POWER KEY TO PAUSE BOOT" opens a menu (Recovery
  mode, FastBoot Mode, Restart bootloader, Power off); an image load failure on a
  non-multislot device enters fastboot.
- Creating real `boot_a`/`boot_b` partitions would put the loader into multi-slot mode but
  requires rewriting the live GPT, which the boot ROM reads; EDL is gated (see
  [recovery research](../opwwe231/recovery.md)). We do not recommend it.

## Not covered

- Full extraction of the root filesystem and a survey of the dyld shared cache.
- Encrypted images (iBoot family, SEP): no keys.
- ISA and behaviour of the coprocessor firmwares and the 557 built-in kexts individually.
- A direct read of CPU feature registers on the watch.

## Evidence

Tooling: REA 6.7.0 (CLI), Ghidra 12.1.4, rizin 0.9.1, Apple LLVM 17.

| Evidence | ID |
| --- | --- |
| `inspect_macho` on the decoded kernelcache (SHA-256 `a791894f...c343ef`) | `ev_8a0b3250f2275409a3623d3bfd732acd4e53d7a3908d5f7732700069bd1e26fd` |
| rizin cross-references to the recovery strings in LinuxLoader | `ev_716d4826a98fbf194496eac9ff7a9c3aaa148a195fb09991985ad53f15dae3b5` |
| rizin disassembly of the recovery decision | `ev_b0c6ae5a3cf232e27a53bd5522d5b183839f70b61689a12dc14edfbe69242f5c` |
| rizin disassembly of the unaligned-read helper and the following branch | `ev_fb6185edee14290f62ce9931bf871f95980e6c86adb494aa11618a31ba8c37ca` |

Instruction counts, DeviceTree dump and manifest tables come from ordinary shell and
script tools, not from REA evidence.
