# port-gap: opwwe231 → watchos-6.3.1-watch5

Target: **watchOS 6.3.1 (17U224)** on Watch5,4 (n146bap)  
Profile partial: `False`

## Summary

| Verdict | Count |
| --- | ---: |
| MATCH | 12 |
| SHIM | 5 |
| BLOCKER | 1 |

## Items

### ⛔ `cpu.isa_features.tgran16` — BLOCKER

- compare: `feature`
- expected: `True`
- actual: `False`
- remediation: **kernel-patch** → `tcr_el1`
  > Kernelcache programs TCR_EL1 = 0x000800226519a519 unconditionally (TG0/TG1 = 16KB). On a core without TGran16, rebuild the page-table bootstrap at 4KB granule — massive change, no small patch.

### 🟡 `cpu.isa_features.pauth` — SHIM

- compare: `feature`
- expected: `True`
- actual: `False`
- remediation: **el2-trap-shim** → `sn0w-el2`
  > 331,911 PAC instructions in the kernelcache. On a non-PAC core, trap-emulate PAC MSRs (apia/apib/apda/apdb/apga key lo/hi) and strip pac*/aut*/braa*/retaa* via sn0w EL2 shim or static kernel patch.

### 🟡 `cpu.isa_features.lse` — SHIM

- compare: `feature`
- expected: `True`
- actual: `False`
- remediation: **el2-trap-shim** → `sn0w-el2`
  > 10,211 LSE atomic instructions in the kernelcache. On a non-LSE core, trap-emulate via EL2 or patch to ldxr/stxr pairs.

### 🟡 `memory.dram_base` — SHIM

- compare: `equal`
- expected: `0x800000000`
- actual: `0x0`
- remediation: **boot_args**
  > Apple reference uses DRAM base 0x800000000; the OPW2 (Qualcomm W5) maps DRAM at 0x80000000 or 0x40000000 (confirm from device profile). Loader must synthesize boot_args.physBase matching whatever DRAM the kernelcache will be relocated to; the Apple-vs-W5 base need not agree.

### ✅ `boot.kernel_physbase` — MATCH

- compare: `present`
- actual: `0x8000`
- remediation: **boot_args**
  > The watchOS loader must set boot_args.physBase to the physical load address of the kernelcache. tools/watchos/bootargs/mk_bootargs.c is the host-side builder; the on-device loader uses the same struct layout (boot_args.h, 2d8 bytes).

### 🟡 `interrupt_controller.type` — SHIM

- compare: `equal`
- expected: `aic`
- actual: `gic-v3`
- remediation: **platform-model** → `aic-shim`
  > The watchOS kernelcache expects an Apple AIC at the DT-declared base (0x2d180000 on T8006). On a GIC-v3 host we must either (a) emulate AIC in sn0w EL2 by trap-ing its MMIO window, or (b) rebuild the kernel's AppleInterruptController kext against GIC — not viable without SDK. Option (a) is the sn0w path.

### 🟡 `interrupt_controller.base` — SHIM

- compare: `present`
- actual: `None`
- remediation: **platform-model**
  > Even if the host IC is GIC, sn0w EL2 maps a shadow MMIO window to the Apple AIC base (0x2d180000) and emulates reg access there.

### ✅ `console_uart.physaddr` — MATCH

- compare: `present`
- actual: `0x4a98000`
- remediation: **platform-model**
  > Apple watchOS expects a Samsung-style UART at uart0 base 0x2e500000. The host provides a Qualcomm GENI/MSM UART at a different base; either sn0w EL2 traps the Apple window and forwards to the native UART, or the kernelcache patch retargets early-UART physaddr.

### ✅ `_apple_platform.aic_base` — MATCH

- compare: `note`
- expected: `0x2d180000`
- actual: `0x2d180000`
- note: AIC MMIO base, size 0x8000.

### ✅ `_apple_platform.aic_timebase` — MATCH

- compare: `note`
- expected: `0x2d188000`
- actual: `0x2d188000`
- note: aic-timebase, size 0x1000.

### ✅ `_apple_platform.pmgr_base` — MATCH

- compare: `note`
- expected: `0x2d000000`
- actual: `0x2d000000`
- note: PMGR base, size 0x180000.

### ✅ `_apple_platform.uart0_base` — MATCH

- compare: `note`
- expected: `0x2e500000`
- actual: `0x2e500000`
- note: uart0 (uart-1,samsung), IRQ 0x106.

### ✅ `_apple_platform.disp0_base` — MATCH

- compare: `note`
- expected: `0x18000000`
- actual: `0x18000000`
- note: display pipeline, size 0x2f0000.

### ✅ `_apple_platform.coprocessors` — MATCH

- compare: `note`
- expected: `['sep', 'ans', 'aop', 'rtp', 'smc', 'sio']`
- actual: `['sep', 'ans', 'aop', 'rtp', 'smc', 'sio']`
- note: 7 ASC coprocessors (per DT): SEP 0x42400000, ANS 0x77400000, AOP 0x4c400000, RTP 0x4cc00000, SMC 0x3e400000, SIO 0x2f400000. Early boot: stub all 7 as unresponsive mailboxes; ANS modeled enough to serve rootfs is the launchd gate.

### ✅ `_apple_sysregs.required_count` — MATCH

- compare: `note`
- expected: `82`
- actual: `82`
- note: 82 distinct Apple IMPDEF system registers (S3_*_C15_*). Full census in research/opwwe231/2026-10-24-watchos-boot/pac-and-register-census.txt. RAZ/WI covers most config writes; APRR_EL0/EL1 need genuine R/W backing (write-then-read-back semantics). Mirror of the sn0w layer in tools/watchos/qemu/sn0w/.

### ✅ `_apple_entry.tcr_el1` — MATCH

- compare: `note`
- expected: `0x000800226519a519`
- actual: `0x000800226519a519`
- note: TG0=TG1=16KB granule, T0SZ=T1SZ=25 (39-bit VA), IPS=2 (40-bit PA). Loaded unconditionally — no 4KB path in start.

### ✅ `_apple_entry.x0` — MATCH

- compare: `note`
- expected: `boot_args_phys`
- actual: `boot_args_phys`
- note: x0 holds a physical pointer to a boot_args struct (see boot_args.h).

### ✅ `_apple_entry.el` — MATCH

- compare: `note`
- expected: `EL1`
- actual: `EL1`
- note: Entry is EL1, MMU off, caches off, DAIF.daifset=0xf.
