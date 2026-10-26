# watchOS 27 Watch7,11 — exp19 & exp20: narrow AMFI patch attempts (negative)

**Date:** 2026-10-26 (continuation of exp16 breakthrough + exp17/18 analysis)
**Status:** both experiments regress the boot progression to the pre-exp16
state (`launchd[1] fatal signal 6 Library not loaded`). Neither achieves
the AMFI trust-cache routing. The underlying AMFI gating is deeper than
a single predicate flip; exp16 remains the best known chain for AMFI
bypass, with the known downstream Sandbox issue.

## Goal: replace exp16's wholesale `*a4 = "trust-cache"` override with a
single-site patch that only redirects AMFI's own internal
trust-cache-vs-dynamic decision, leaving `*a4` to be written by AMFI's
own code paths. This should preserve correct trust-category output for
every file EXCEPT the ones AMFI naturally sees as trust-cache material.

## exp19 — predicate always returns true

```
VA 0xFFFFFFF00893BEF0   sub_FFFFFFF00893BEF0 (is_cdhash_in_trust_cache)
  orig: 5f2403d57f2303d5ffc300d1f44f01a9
  new:  5f2403d5200080d2c0035fd61f2003d5
```

Preserves `BTI c`, then overwrites `PACIBSP ; SUB SP, SP, #0x30 ;
STP X20, X19, …` with `MOVZ X0, #1 ; RET ; NOP`.

**Result:** boot regresses to the pre-exp16 failure
(`launchd[1] fatal signal 6 Library not loaded`). The AMFI "adhoc
signed" and "CT policy" log lines are still present, which means
`_vnode_check_signature` is being called but is still routing to the
reject path.

**Reason (post-mortem):** `sub_FFFFFFF00893BEF0` is called ONLY from a
single site inside `_vnode_check_signature` at VA `0xFFFFFFF00893DDD4`
and that call is gated by two earlier conditions:

```
0xFFFFFFF00893DDC4   TBZ W19, #0, loc_FFFFFFF00893DEF0     ; W19 bit 0 must be 1
0xFFFFFFF00893DDC8   CBZ X24, loc_FFFFFFF00893DEF8          ; X24 must be non-zero
0xFFFFFFF00893DDD4   BL   sub_FFFFFFF00893BEF0              ; <-- our patch
```

For an adhoc dyld_shared_cache_arm64e W19 bit 0 is 0, so the TBZ fires
and the entire trust-cache evaluation block (including our patched
predicate call) is skipped. The function instead goes through
`loc_FFFFFFF00893DEF0` (sets `X24 = 0 ; B 0x893DF14`) and eventually
reaches the "dynamic" fallback at line 961 of the Hex-Rays decompile.

## exp20 — NOP the TBZ gate to force the trust-cache block

```
VA 0xFFFFFFF00893DDC4
  orig: 73090036    (TBZ W19, #0, +0x12C)
  new:  1f2003d5    (NOP)
```

Composed with exp19 + exp11 (CT-policy B.NE unconditional, from the
dead-ends dir). Idea: NOP the TBZ so execution falls through to the
`CBZ X24` guard (preserved for null safety) and then into our
patched predicate (which returns 1), routing AMFI into the
trust-cache output branch for every signature that passes the
upstream validation.

**Result:** boot still regresses to pre-exp16 state. The AMFI prints
"adhoc signed" / "CT policy" are suppressed (exp11 bypass still
works), but `dyld[1] result from check_np(): -1 errno 12` still fires
and `launchd` dies with "Library not loaded".

**Why it didn't work:** either the `CBZ X24` guard fires (X24 null
for the adhoc shared cache call), bypassing our predicate too, or
the code after the predicate — in particular the `*v108 |= 0x4000000`
flag write + `sub_FFFFFFF008934734(3, v17, …)` call — needs its own
dependencies (`v108` not null, `v17` properly set up by earlier
blocks) that are not satisfied on the adhoc path.

Full root-cause narrowing would require instrumenting the predicate
call site (or `_vnode_check_signature`'s internal state) to see
exactly which earlier variable is wrong on the adhoc path. That's a
measurable REA step for a future session; this session stops the
narrow-attempt loop here.

## Record of exp19 and exp20

Both manifests are preserved in the patchkit directory as
reproducible evidence of this attempt, with a clear "do not use
alone" warning in their headers.

## Current best chain

The reproducible chain to the farthest boot checkpoint remains:

```
bootkc.before-exp10   (9-patch APFS/md base)
  +  manifest-exp16-amfi-trustcache.yaml  → bootkc.exp16
       → reaches IOKit serial-driver matching, then
         init[1] fatal signal 9 BAD_MACHO at posix_spawn
```

The next productive work is:
- A **more precise filter in the exp16 trampoline** that reads vp
  (which lives in X0 on entry per the MACF hook calling convention)
  and compares the resolved path against `/System/Library/Caches/`.
  Requires the correct offset of `v_name` in `struct vnode` on this
  XNU revision (candidates 0xB8 / 0xC0 / 0xD0 — needs REA on a
  function that uses it, or a sanity-test boot with each offset).
- Or a **targeted Sandbox hook neutraliser** for the specific
  `vnode_check_signature`-reachable sandbox path that fires
  `shenanigans!` after exp16+exp18.

Neither is a cheap next step; both will take their own session.
