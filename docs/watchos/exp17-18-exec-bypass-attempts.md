# watchOS 27 Watch7,11 — exp17 & exp18: exec-side bypass attempts

**Date:** 2026-10-26 (continuation of exp16 breakthrough)
**Base:** `bootkc.exp16` (exp16 AMFI trust-cache override + 9-patch APFS)
**Status:** both experiments move the panic site, neither reaches shell.
  The real root cause is now in the **Sandbox kext**'s evaluation of
  our wide-open AMFI trust: it reaches an invariant that is never
  supposed to hold, and `panic("shenanigans!" @evaluate.c:5234)` fires
  from within `com.apple.security.sandbox`.

## exp17 — CBZ flip at the first BAD_MACHO site

Patched the branch guarding one of two `MOVZ W0,#9 ; MOVZ W1,#1`
pairs inside `sub_FFFFFFF009EA7D28` (the exec_mach_imgact-equivalent).

```
VA 0xFFFFFFF009EA8198
  orig: e0090034     (CBZ W0, loc_FFFFFFF009EA82D4)
  new:  4f000014     (B   loc_FFFFFFF009EA82D4)
```

**Result:** zero behavioural change vs. exp16. The boot log diff
(filtered on AMFI/APFS/dyld/Serial/OS_REASON/signal lines) is empty.
The BAD_MACHO fires from a different site. There are **9** calls to
`sub_FFFFFFF009F6DDF4` (`os_reason_create`) inside
`sub_FFFFFFF009EA7D28` and 43 across the kernelcache, so hunting the
specific firing path site-by-site is impractical.

## exp18 — proc_is_fatal_signal gate → always false

Rather than hunt every BAD_MACHO emitter, neutralise the final gate
that decides "this signal is fatal to init, panic the kernel":
`sub_FFFFFFF009EB5118`, called from the kernel's own
`psignal_internal` (`sub_FFFFFFF009ED5494`). Applied the exp14
pattern (preserve `BTI c`, replace the next three instructions with
`MOV X0,#0 ; RET ; NOP`):

```
VA 0xFFFFFFF009EB5118
  orig: 5f2403d5200400b4882a00f0108543f9
  new:  5f2403d5000080d2c0035fd61f2003d5
```

**Result:** the previous `init[1] fatal signal 9 namespace 9 code 0x1`
panic is gone. In its place, the first panic is now:

```
panic(cpu 0 caller 0xfffffff02977ac58):
   "shenanigans!" @evaluate.c:5234
```

The caller is inside `com.apple.security.sandbox(300.0)` at the
sandbox kext's slide-adjusted VA (`0xfffffff02975fd50 →
0xfffffff029797147`). The backtrace walks up through
`sub_FFFFFFF009EAFEB8` (one of the three candidate BAD_MACHO
containers from the earlier search), so the exec is still rejected,
just at a different "fatal" gate inside the sandbox evaluator.

## What this tells us

Our wide-open AMFI trust-cache override (exp16) satisfies
`cs_shared_region_create` but confuses `AppleMobileFileIntegrity`'s
peer kext, Sandbox, which calls back into AMFI during profile
evaluation and asserts an impossible result. A principled next step
is one of:

1. **Narrow the AMFI patch**: only emit "trust-cache" for the
   dyld_shared_cache path (vnode path contains
   `/System/Library/Caches/com.apple.dyld/`), and let AMFI's own
   logic handle every other code-signature check normally. This
   requires inspecting the vnode's path from inside the trampoline,
   which is a 20-30 instruction addition that reads vp→v_name or
   similar.
2. **Neutralise Sandbox**: find the entry of the sandbox-check MACF
   hook and apply the exp14/exp16 pattern (preserve `BTI c`,
   `MOV X0,#0 ; RET`). The sandbox kext VA range is
   `0xFFFFFFF02975FD50 .. 0xFFFFFFF029797147` once KASLR-adjusted,
   which translates in the static kernelcache to
   `0xFFFFFFF00975FD50 .. 0xFFFFFFF009797147`. Entry points reachable
   from the MACF dispatcher are candidates for a surgical patch.
3. **Accept the current ceiling**: APFS mount + dyld shared cache
   accepted + libSystem loaded + IOKit serial driver match reached
   is a real and useful bringup milestone for the W2 porting plan.
   The shell goal is deferred to a future session where there is
   bandwidth to work through the Sandbox evaluator chain.

## Reproducing

```
patchkit.py apply bootkc.exp16 \
  -m manifest-exp17-bad-macho-skip.yaml \
  -o bootkc.exp16-17   # → sha256 1736b85e02b885d5…

patchkit.py apply bootkc.exp16 \
  -m manifest-exp18-fatal-signal-skip.yaml \
  -o bootkc.exp16-18   # → sha256 edcbb45b655db926…

# boot bootkc.exp16-17: identical to exp16 (BAD_MACHO still kills init).
# boot bootkc.exp16-18: new panic in com.apple.security.sandbox
#                      "shenanigans!" @evaluate.c:5234.
```
