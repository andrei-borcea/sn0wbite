# watchOS 27 Watch7,11 — exp15 entry-marker: definitive negative result

**Date:** 2026-10-26
**Target:** `Watch7,11` build `24R365`, bootkc on qemu-sptm
**Base:** `bootkc.before-exp10` (9-patch APFS/md baseline) +
  `manifest-exp15-entry-marker.yaml`
**Patched sha256:** `c080df2f75e7b2573fe34ef1dfb97ba4a3b7b0b2b14f013807c0846850efe1a5`

## What the experiment did

Instrumented the entry of `sub_FFFFFFF009F79CA8` =
`shared_region_map_and_slide_setup` with a two-patch trampoline:

- Replace `PACIBSP` at `0xFFFFFFF009F79CA8` with
  `B 0xFFFFFFF00A1D4778` (a zero-filled pad in `__TEXT_EXEC`).
- At that pad: save X29/X30 + X0/X1, load `&"EXP15 marker=%u\n"`
  into X0, load `0xFE` into W1, `BL sub_FFFFFFF008958174` (AMFI
  printf wrapper), restore registers, re-execute `PACIBSP`, branch
  back to `0xFFFFFFF009F79CAC`.

Expected serial output if the function is entered during the adhoc
`dyld_shared_cache_arm64e` load:

```
EXP15 marker=254
```

## What happened

**Zero markers emitted.** 0 occurrences of `EXP15` in the full
17,906-byte boot log. Boot otherwise identical to prior experiments
(exp10b/11/12/13): same AMFI "adhoc signed" + "unsuitable CT policy"
+ dyld `check_np: -1 errno 12` + `launchd[1] fatal signal 6`
"Library not loaded: /usr/lib/libSystem.B.dylib" fatal panic, with
the exact same 9-frame nested-panic tail that is present in every
baseline.

## Conclusion

`sub_FFFFFFF009F79CA8` (`shared_region_map_and_slide_setup`) is
**never called** during the adhoc shared-cache rejection path. Which
means every single-site patch inside it — including exp12's bulk
flip of all 29 `MOV W25, #22` sites — is operating on dead code
from this scenario's point of view.

The rejection decision is made upstream, before the kernel reaches
the inner VM function. The AMFI logs ("adhoc signed", "unsuitable CT
policy") come from `_vnode_check_signature`
(`0xFFFFFFF00893DA30`), which executes INSIDE AMFI's MACF hook, in
the chain that EVALUATES the cache. The outer caller
(`sub_FFFFFFF009F7828C`, 0x10F8 bytes) short-circuits on an AMFI
reject return value *before* dispatching to
`sub_FFFFFFF009F79764 → sub_FFFFFFF009F79CA8`.

## Immediate implication for the plan

The next REA target is **`_vnode_check_signature` itself and its
caller `sub_FFFFFFF00A199B44` (MAC `vnode_check_signature`
wrapper)**, specifically:

1. Find the single return site in `_vnode_check_signature` and the
   value it propagates for the adhoc cache (likely a non-zero AMFI
   error, not just the "dynamic" trust category we hypothesised
   earlier).
2. Alternatively, patch the MAC wrapper
   (`sub_FFFFFFF00A199B44` at `0xFFFFFFF00A199B44`) to return 0
   regardless of what AMFI's `_vnode_check_signature` returned.
   This is a one-site patch that bypasses AMFI for all code
   signatures — brutal but surgical. The risk is other MACF
   consumers (trust-cache validation during mount, launchd exec)
   losing their gating; this risk is comparable to the
   `cs_enforcement_disable=1` dead end already explored.

Mapping this out, however, is a next-session task. This document
records the exp15 result as a definitive data point that closes the
`shared_region_map_and_slide_setup` branch of the REA tree.

## Reproducing

```
patchkit.py apply <bootkc.before-exp10> \
  -m tools/opwwe231/watchos/qemu-sptm/patchkit/manifest-exp15-entry-marker.yaml \
  -o <bootkc.exp15-entry>
# → sha256 c080df2f75e7b2573fe34ef1dfb97ba4a3b7b0b2b14f013807c0846850efe1a5
# boot: expected 0 "EXP15" lines in serial log, confirming function unreached
```
