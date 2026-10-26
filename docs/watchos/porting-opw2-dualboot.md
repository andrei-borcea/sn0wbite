# Porting watchOS/XNU to OnePlus Watch 2 — dual-boot plan

**Date:** 2026-10-26
**Prerequisite:** working qemu-sptm reference bringup
  (see `qemu-sptm-watch7-11-boot.md` / `exp10-14-dead-ends.md`)
**Target recovery image:** `backups/oneplus-watch-2/tools/recovery-menu/v9/recovery_sn0wmenu_v9b.img`
  (11,767,808 B, tested on hardware; see memory `watch2-dual-boot-state`)

**Scope of this document.** This is a planning document, not a bringup
log. No W2 flashing in the session that wrote it. It identifies the
realistic path to a serial-UART shell running XNU on the OnePlus Watch
2 in dual-boot alongside Wear OS 4, with explicit stop points.

## OnePlus Watch 2 — hardware baseline

| Component | Detail |
|---|---|
| SoC | Qualcomm Snapdragon W5 Gen 1 (SW5100) |
| CPU | 4× ARM Cortex-A53 @ 1.7 GHz, ARMv8-A (NOT ARMv8.3 arm64e) |
| GPU | Adreno 702 |
| RAM | 2 GB LPDDR4X |
| Storage | 32 GB eMMC |
| Display | 1.43" AMOLED, 466×466 |
| Stock OS | Wear OS 4 (Android 13) |
| Bootloader | Unlocked (orange state), AVB tolerates hash mismatch |
| Recovery slot | Hard VBMeta limit: `recovery.img < 12,083,200 B` |
| Input | qpnp_pon keys: `114 = next`, `116 = select` |
| Peripherals | Qualcomm display stack (MDSS), I²C touch, eMMC storage, PMIC, Wi-Fi/BT, no cellular |

### The arm64e wall

watchOS 27's bootkc is **arm64e**: every function prolog is
`PACIBSP`/`PACIBZ`, every function pointer indirection goes through
`BLRAA`/`BLRAB`/`AUTDA`/`AUTIB`, and every return is `RETAB`. These
instructions require the Pointer Authentication Code extension
(ARMv8.3), which the W5's Cortex-A53 does not have — the A53 traps
them as **undefined instructions**.

This is the dominant structural blocker and the single reason this
plan does **not** try to run a watchOS 27 kernelcache on the W2.
Realistic ways around the wall:

| Option | Feasibility | Notes |
|---|---|---|
| A. PAC-strip the entire bootkc + dyld cache + every dylib | Infeasible | Thousands of patch sites, no sealed-boot signature to re-sign them against |
| B. Trap-and-emulate PAC in the kernel exception vector | Feasible but slow | 100–1000× slowdown; the SpringBoard thread alone makes this unusable |
| C. Downgrade target: watchOS 7 (last plain-arm64 Watch OS, Apple Watch S4/S5) | Realistic | Several months of Qualcomm driver bringup; the current repo's T8006 Watch5,4 work is adjacent |
| D. Linux-native "watch look-alike" on the W2 (AsteroidOS-style) | Weeks | Not watchOS — a UI resembling it on a Linux stack |

This document describes a **C-track** plan terminated at serial-UART
shell: the first provable milestone, with a decision point before any
userspace or SpringBoard work.

## Phase 0 — hardware diagnostics

Nothing in this phase touches partitions. All of it is a prerequisite
to Phase 1.

1. **UART breakout.** Locate the UART1 TX/RX pads on the W2 PCB
   (unknown; needs probing with a logic analyser against known-good
   console output from a recovery boot). Without serial, there is no
   kernel-side diagnostic channel pre-userspace. Expected test
   points: ground plane + a two-pad cluster near the SoC's southern
   edge, which is the usual placement on W5-reference designs.
2. **Fastboot ergonomics.** Confirm `fastboot boot <payload.img>`
   works and returns to Wear OS cleanly after reset. Also confirm
   the deadman timer (150 s per memory) actually reboots when a
   payload hangs — this is the safety net for the whole project.
3. **BCB and recovery autoheal.** Verify the memory
   `watch2-dual-boot-state` entry: the stock OS re-flashes the
   recovery partition on each boot, so any custom recovery or
   payload needs the `flash-just-before-enter` protocol.
4. **vendor_flash_recovery autoheal timing.** Measure the window
   between flashing a recovery image and entering it; this bounds
   the "flash + reboot into it" workflow.

## Phase 1 — xnuloader port (one-shot boot payload)

Target: a fastboot-flashable boot image that loads a kernelcache,
device tree, and ramdisk from its own sections and jumps into the
kernel. This is the HackiOS approach (public reference:
oosexgang/hackios-xnu-kernel), which demonstrated XNU boot on a
Snapdragon 778G via Mesa shimming the Adreno. Starting target OS for
Phase 1 should be **iOS 16.5 arm64** (verified HackiOS target, no
arm64e), *not* watchOS — the watchOS 7 downgrade is only taken on
Phase 7+.

Deliverables:

- `tools/opwwe231/watch2/xnuloader/` — a Snapdragon-flavoured
  xnuloader derived from the HackiOS source. Builds as a
  fastboot-compatible Android boot image: `vmlinux` section =
  xnuloader stub, DTB section = synthetic Apple-style DT, ramdisk
  section = our RAM payload.
- Device-tree sections for the W5: memory map (DRAM at Qualcomm
  default base vs Apple's 0x800000000), peripheral MMIO windows
  (`arm-io` style with a GIC-at-SMMU shim), timer bindings, boot
  console UART1 node.
- Minimum boot: `putc('A')` through the console UART, visible on the
  Phase-0 serial breakout. Nothing more at this phase.

Decision point after Phase 1: if serial `A` doesn't appear on the
first boot and we can't diagnose it from the fastboot-side logs,
abandon the W2 target. The remaining phases depend on this working.

## Phase 2 — exception-vector + MMU bringup

- Hook up VBAR_EL1 to a Qualcomm-aware vector table. GIC trap IDs
  differ from Apple's AIC; the simplest path is a thin GIC→AIC
  shim that lets XNU's own AIC driver bind to GIC interrupts.
- SMMU vs DART: iOS kexts expect `IODMACommand`/`IOMMUMapper` to
  back onto a DART. A pass-through SMMU in stage-2 is the right
  first step; DART emulation is deferred.
- Timer: ARM generic timer (CNTFRQ, CNTP_*), which XNU already
  speaks on non-Apple cores — no shim.
- Clock tree: Qualcomm Snapdragon RPM clocks need a simplified PLL
  bringup (one 1.4 GHz PLL for all four A53 cores is enough).

Milestone: `AMFI: Booted in a VM` or equivalent XNU-side console
output appears over UART. This proves kernel init completed past
PE_init / cpu0 online.

## Phase 3 — IOKit stubs for first-wave drivers

Before any userspace, XNU wants:

- `IOPMrootDomain` (power management root)
- `IONVRAM` (store or stub to tolerated-failure)
- Some kind of serial driver matching the IODT node from Phase 1

The restore-ramdisk path on qemu-sptm already lives in the "IONVRAM
tolerated failure" regime (see qemu-sptm log). The same `publishResource`
side-channel used there (`IODTNVRAM` doc fix) applies verbatim here.

## Phase 4 — ramdisk + launchd exec

- Reuse the custom ramdisk + launchd job + trustcache construction
  from the exp14 effort (`watchos/qemu-sptm` work in progress).
- First exec target: a stripped `/sbin/launchd` running a single
  `com.sn0wbite.bash.plist` with a statically-linked `bash` and
  `libSystem.B.dylib`.
- `exec_mach_imgact` + `cs_blob_create` second-validator block
  (BAD_MACHO code 0x1 from the qemu-sptm exp14 session) must be
  resolved in-tree *first*, as a reusable patch, before this phase.

Milestone: `bash-5.x#` prompt over UART. **This is the Phase-A
success criterion.** Stop here; everything after it is optional
and far more expensive.

## Phase 5 — display / touch / storage

(Only if post-Phase-4 the user wants SpringBoard-side work.)

- Display: Qualcomm MDSS → DCP shim. HackiOS has a reference pattern
  but it is adhesive on a 778G, not direct.
- Touch: I²C controller + custom panel firmware; the W2 panel's
  touch IC vendor is unknown and may need its own stack.
- Storage: eMMC via Qualcomm `sdhci-msm` — map to `IOBlockStorageDevice`.
- GPU: Adreno 702 via Mesa (HackiOS-pattern) if any UI runs on
  the display. Deferred unless Phase 4 succeeds and SpringBoard
  is the next target.

## Phase 6 — recovery menu integration

The W2 recovery (`recovery_sn0wmenu_v9b.img`) has a "WATCHOS TEST"
slot dimmed in `src/bootmenu2.c`. To light it up:

1. Add a new menu item that boots the Phase 1 payload via
   `fastboot boot` semantics (RAM-only, no partition write).
2. Bump the version to v10 and rebuild under the same VBMeta
   constraints (image < 12,083,200 B, BCB clear-before-exit, display
   SETCRTC-close-reopen pattern preserved, qpnp_pon keys unchanged).
3. Keep the flash-just-before-enter workflow (vendor_flash_recovery
   autoheal defense).

See `backups/oneplus-watch-2/tools/recovery-menu/` for the current
image + build scripts and the `flash_recovery.sh` protocol.

## Phase 7+ (speculative) — watchOS downgrade target

Only consider after Phase 4 holds up over multiple boots. The watchOS
7 kernelcache (Apple Watch S4/S5 era, arm64 non-arm64e) is the only
realistic watchOS target for an arm64-base SoC. Even then,
SpringBoard needs BackBoardd + CoreAnimation render server + a GPU
shim for Adreno, which is PhD-level work with no public prior art
for watchOS specifically.

## Safety checkpoints (hard rules)

1. **Never flash the W2 without explicit per-session user confirmation.**
2. **Never touch the recovery partition without the existing
   `flash_recovery.sh` protocol.**
3. **BCB clear-before-exit is tassativo** on every custom boot.
4. **Always prefer `fastboot boot` to `fastboot flash` for
   experiments.** Boot is RAM-only; a bad payload hits the deadman
   and reboots to Wear OS without corrupting storage.
5. **Deadman 150 s is the ultimate backstop.** If anything hangs
   for longer, the device returns to Wear OS automatically; keep
   that invariant intact.

## Session artefacts (none written in this session)

This document is the plan. No code in `tools/opwwe231/watch2/`, no
recovery rebuild, no fastboot payload has been produced in the
session that writes this plan. Each phase above becomes work in a
future session, after the qemu-sptm shell milestone is reached.
