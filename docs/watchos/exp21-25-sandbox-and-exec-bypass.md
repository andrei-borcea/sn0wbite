# watchOS 27 Watch7,11 — exp21–25: Sandbox + exec validator bypass chain

**Date:** 2026-10-26 (same session as exp16 breakthrough + exp17/18/19/20)
**Base:** `bootkc.before-exp10` (9-patch APFS/md)

## The chain evolves

exp16 made AMFI's `vnode_check_signature` write "trust-cache" to the
output pointer. The downstream consumers (Sandbox evaluator, exec
validator) then saw an inconsistent state and panicked, each in a
different way. This document walks through the chain exp21–25 built
to patch each new failure in sequence, and where it stopped.

## exp21 — exp16 trampoline + CS_PLATFORM_BINARY bit

AMFI's own trust-cache success path (line 753 of the Hex-Rays
decompile) does `*v108 |= 0x4000000u` (set CS_PLATFORM_BINARY in the
output cs_flags) BEFORE calling the trust-category setter. exp16
skipped that step; downstream code (notably Sandbox) saw trust_cat ==
"trust-cache" but no platform-binary flag → inconsistent state.

exp21 extends the trampoline to:
```
ldr  w8,  [x4]              ; *a5 (cs_flags)
orr  w8,  w8,  #0x4000000   ; set CS_PLATFORM_BINARY
str  w8,  [x4]
```
before the trust-category setter. Standalone, exp21 reaches the same
`init[1] fatal signal 9 BAD_MACHO` panic as exp16, with the same
IOKit-serial-driver farthest checkpoint.

## exp22 / exp23 — Sandbox shenanigans assertions

Composing exp21 with exp18 (fatal-signal-to-init gate neutralised,
same patch as the standalone exp18 attempt on top of exp16) exposed
`sb_evaluate`'s internal bytecode-interpreter assertions:
```
panic("shenanigans!" @evaluate.c:5234)
```

Found in `sub_FFFFFFF00977A6B8`, the sandbox profile evaluator. Four
guard instructions fire the same shenanigans panic, each tagged with
a different evaluate.c line number:

```
0xFFFFFFF00977A8A8  B.NE  → 5234
0xFFFFFFF00977A8B0  CBNZ  → 5237
0xFFFFFFF00977A8BC  CBNZ  → (unknown line)
0xFFFFFFF00977A960  B.EQ  → (unknown line)
```

exp22 NOPs the first. exp23 NOPs all four. Composing exp21 + exp18 +
exp23 pushes past the Sandbox panic entirely and reaches a NEW failure:
```
panic:  initproc failed to start -- exit reason namespace 9 subcode 0x1
```
That is still OS_REASON_EXEC / BAD_MACHO, raised by the kernel's
`start_init` path (sub_FFFFFFF009EB441C → panic emitter at
0xFFFFFFF009EB4FB0).

## exp24 — os_reason_create (ns=9, code=1) → NULL

Attempt to short-circuit at the common sink by making
`sub_FFFFFFF009F6DDF4(ns=9, code=1)` return NULL. Trampoline at
pad+0x30 (`0xFFFFFFF00A1D47A8`) checks args and returns 0 before
the function builds any reason struct.

**Result:** `Kernel data abort at pc 0xFFFFFFF009EA8220, lr
0xFFFFFFF009EA81F4` with `far = 0xB1`. The caller
(`sub_FFFFFFF009EA7D28` second BAD_MACHO site at 0xFFFFFFF009EA81E8)
immediately dereferences the returned struct (`LDR X8, [X22, #0xB1]`
without a NULL check), so returning NULL crashes the kernel there.
Returning a dummy valid struct would require more careful trampoline
work and is deferred.

## exp25 — exec_mach_imgact Mach-O validator → force success

Alternative: patch `CBZ W0, loc_FFFFFFF009EA80B0` at
0xFFFFFFF009EA8060 (the test after `BL sub_FFFFFFF009F56B48`, the
Mach-O structural validator) to an unconditional
`B loc_FFFFFFF009EA80B0`. Routes every exec through the "validator
reported success" path, bypassing both BAD_MACHO code=1 sites inside
`sub_FFFFFFF009EA7D28`.

**Result:** different crash. `Kernel data abort at pc
0xFFFFFFF009A9EEDC, lr 0xFFFFFFF009EA813C`. The forced success path
calls `sub_FFFFFFF009A9EEB8(v40, v41)` and that function dereferences
state (`v40` / `v41`) that the validator was supposed to populate
BUT skipped because our patch bypasses the entire validator block,
not just its decision. **Lesson:** the exec validator does side-effect
setup on top of its validation decision; bypassing it altogether
leaves downstream code without its required state.

## Current state

Best reproducible chain: **`9-patch base + exp16` → farthest checkpoint
(IOKit serial driver matching) followed by init[1] fatal signal 9
BAD_MACHO panic.** Everything beyond is unresolved; each layer bypass
exposes the next.

For a serial shell goal, the realistic forward paths are both expensive:

1. **Precision exec validator**: find the specific invariant that
   `sub_FFFFFFF009F56B48` rejects for the adhoc-signed binaries
   launchd exec's, and patch just that invariant inside the validator
   so it returns 0 (success) WITH all side-effect state properly set
   up. This is a Hex-Rays-level deep REA of a 0x19c-byte function.
2. **Reason-struct trampoline**: make the exp24 filter return a valid
   dummy os_reason struct (allocated in __DATA or in the pad as a
   byte literal) so downstream caller code that writes/reads fields
   on it doesn't crash. Then make the caller's SIGKILL path end up a
   no-op somehow.
3. **Switch target**: pivot to building a minimal custom-rootfs
   payload that doesn't trigger the exec validator at all (jprx bash
   sysroot pattern) and only mount the stock rootfs after. The
   previous session's exp14 lc-bypass work had already reached
   this blocker from a different direction and recorded the same
   BAD_MACHO behaviour.

All five exp21–25 manifests are preserved in the patchkit directory
with "do not use alone" notes and reproducible sha256s in each
manifest header.
