# watchOS 27 Watch7,11 — exp10–14 dead-end patch experiments

**Date:** 2026-10-26
**Target:** `Watch7,11` / build `24R365`, bootkc on qemu-sptm
**Baseline:** 9-patch chain in `manifest-watch7_11-27.0.1.yaml` (APFS mount + md 32-bit fix)
**Goal of these experiments:** get past AMFI/VM-shared-region rejection of the
adhoc-signed `dyld_shared_cache_arm64e` so `/sbin/launchd` can finish
bootstrapping and reach a shell.

**Status:** all 5 experiments failed to produce launchd progress beyond the
`dyld[1]: Library not loaded: /usr/lib/libSystem.B.dylib` panic. They are
preserved here as reproducible evidence so the next instrumentation pass
doesn't repeat them. Patch IDs are stable (`patchkit.py` verifies by
SHA-256 of the original bytes).

## Summary table

| # | Manifest | Target | Result | Verdict |
|---|---|---|---|---|
| 10 | `manifest-exp10-amfi-stub.yaml` | `_vnode_check_signature` prolog → `MOV X0,#0 ; RET` @ `0x893DA30` | `Kernel BTI failure (BTYPE=0x0002)` panic | **Dead end — must preserve `BTI c`** |
| 11 | `manifest-exp11-amfi-ct-bypass.yaml` | `B.NE 0x893E5EC` → unconditional `B` @ `0x893E5BC` | "unsuitable CT policy" log suppressed, "adhoc signed" still prints, `check_np()` still returns `-1 errno 12` | Dead end — single-site print bypass |
| 12 | `manifest-exp12-bulk-einval.yaml` | 30× `MOV Wd,#22 → MOV Wd,#0` inside `sub_FFFFFFF009F79CA8` | Identical rejection; AMFI still prints adhoc+CT policy before reaching this function | Dead end — rejection decided upstream |
| 13 | `manifest-exp13-mac-fatal-skip.yaml` | MAC wrapper `CBZ X8, 0xA199EE4` → unconditional `B` @ `0xA199CD4` | Identical rejection; `fatal_failure_desc_len` was already zero on the success path | Dead end — fatal-flag not the gate |
| 14 | `manifest-exp14-lc-bypass.yaml` | AMFI Launch Constraint hook prolog → `BTI c; MOV X0,#0; RET; NOP` @ `0x893A23C` | LC accepted, but subsequent `/bin/bash` exec dies with `OS_REASON_EXEC code 0x1` = `BAD_MACHO` | Partial win — LC bypassed, second validator remains |

## Evidence (boot-log excerpts)

### exp10 — AMFI stub without `BTI c`

```
AMFI: Booted in a VM
AMFI: trying to get developer mode status from ACM
panic(cpu 0 caller 0xfffffff02a1ce390):
  Kernel BTI failure (BTYPE=0x0002) at pc 0xfffffff02893da30,
  lr 0xfffffff02a199c2c
```

**Lesson.** Every AMFI MACF hook is reached via `BLRAA` from the MAC
framework dispatcher. The first instruction of each hook must be a
`BTI c` landing pad (`5f2403d5`), or ARMv8.5 Branch Target Identification
traps as "indirect branch to non-landing-pad". `exp14` corrects this by
emitting `BTI c ; MOV X0,#0 ; RET ; NOP` as the first 16 bytes.

### exp10b (CBZ nop @ `0x893E750`, real rootfs)

```
AMFI: '/System/Library/Caches/com.apple.dyld/dyld_shared_cache_arm64e' is adhoc signed.
AMFI: '…dyld_shared_cache_arm64e': unsuitable CT policy 0 for this platform/device, rejecting signature.
dyld[1]: dyld cache '(null)' not loaded: code signature registration for shared cache failed
dyld[1]: Library not loaded: /usr/lib/libSystem.B.dylib
panic: launchd[1] fatal signal 6 … namespace 6 code 0x1
```

Forcing `v31`→1 at the late CBZ makes AMFI write a "dynamic" trust into
`*a4` and return 0 (`check_np` would get success *from AMFI*), but the
`AMFI:` prints come from an earlier debug path and still fire; the shared
region path refuses the "dynamic" trust for a shared cache anyway.

### exp11 — CT-policy B.NE unconditional

```
dyld[1]: result from check_np(): -1, errno 12
AMFI: '…dyld_shared_cache_arm64e' is adhoc signed.
dyld[1]: dyld cache '(null)' not loaded: code signature registration for shared cache failed
```

The "unsuitable CT policy" line disappears (confirmed in log diff), but
the "adhoc signed" print and the downstream rejection are unchanged.
Each AMFI log line has an independent emit site; silencing one doesn't
change the function's return value.

### exp12 — Bulk EINVAL flip in `shared_region_map_and_slide_setup`

30 asm sites that load `#22` into `W25` (the function's return register)
were flipped to `#0`. Boot log is byte-identical to exp11 up to the
same panic. Two possible interpretations:

1. Control flow does not reach *any* of the 30 `MOV` sites (rejection
   happens upstream in AMFI or `cs_shared_region_create`).
2. It reaches them, the function now returns `KERN_SUCCESS`, but the
   mapping is not actually built, so the subsequent `check_np` /
   `vm_shared_region_start_address` call returns `ENOMEM`.

**Only live instrumentation can distinguish (1) from (2) — see
`exp15-einval-trace` planned next.**

### exp13 — MAC wrapper fatal-flag bypass

`sub_FFFFFFF00A199B44` (MAC `vnode_check_signature` wrapper) at
`0xA199CD4` tests `*fatal_failure_desc_len == 0` after calling the
AMFI hook. The branch target `0xA199EE4` is the clean-return path.
Forcing an unconditional branch to it makes no observable difference,
which means the fatal-flag was already zero on this path — the
rejection comes from AMFI's *return value*, not from the fatal-flag
side-channel.

### exp14 — AMFI Launch Constraint hook returns 0

With `BTI c ; MOV X0,#0 ; RET ; NOP` at `0x893A23C` (preserving the
BTI landing pad after exp10's lesson), the ramdisk's `launchd` can
spawn `xpcproxy` for a custom `com.jprx.bash.plist` without the AMFI
"Launch Constraint Violation (enforcing)" print.

The `exec_mach_imgact` path then reaches a second validator which
rejects the (adhoc-signed) `/bin/bash` with:

```
OS_REASON_EXEC code 0x1   = BAD_MACHO
```

This is a Mach-O load-time structural check, not a code-signing check —
`cs_blob_create` or similar on the main binary, which runs even when
`vnode_check_signature` returns success. Finding and patching this
site is the next step for the ramdisk-shell path (and may unlock the
full-rootfs path too, since the same validator runs on `/sbin/launchd`
and on every exec).

## What's next (plan: exp15 — EINVAL instrumentation)

Instead of flipping-and-praying, instrument the eleven EINVAL exits of
`sub_FFFFFFF009F79CA8` plus the entry/exit of `_shared_region_check_np`
and `_vm_shared_region_setup`. Each site gets a unique one-byte marker
emitted via the kernel's own `_kprintf` (or equivalent) through a
trampoline in a pad area of `__TEXT_EXEC`. One boot then tells us
exactly which rejection branch fires — or, if none fire, that the
decision is upstream in `cs_blob_create` / AMFI's return value, and
the next REA target shifts there.

## Files

```
tools/opwwe231/watchos/qemu-sptm/patchkit/
  manifest-exp10-amfi-stub.yaml
  manifest-exp11-amfi-ct-bypass.yaml
  manifest-exp12-bulk-einval.yaml
  manifest-exp13-mac-fatal-skip.yaml
  manifest-exp14-lc-bypass.yaml
```

All apply **on top of** the 9-patch base in
`manifest-watch7_11-27.0.1.yaml` via
`patchkit.py apply <base-patched-bootkc> -m <exp-manifest> -o <out>
--no-verify` (the `--no-verify` is needed because the base bootkc
already differs from stock).
