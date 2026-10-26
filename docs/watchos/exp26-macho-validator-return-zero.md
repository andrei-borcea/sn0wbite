# watchOS 27 Watch7,11 — exp26: Mach-O validator always returns 0

**Date:** 2026-10-26 (session end: continuation of exp21-25)
**Base:** `bootkc.exp21-18-23` (AMFI trust-cache + CS_PLATFORM_BINARY +
fatal-signal gate + Sandbox 4-site shenanigans NOPs)
**Patched sha256:** `2dc6ee998e00bebe…`
**Status:** no observable change vs. exp21-18-23. The `initproc failed
to start — exit reason namespace 9 subcode 0x1 description: none`
panic still fires at the same site. Composing with exp17 (CBZ at the
other BAD_MACHO branch inside `sub_FFFFFFF009EA7D28`) also makes no
difference.

## What the patch does

`sub_FFFFFFF009F56B48` is the Mach-O structural validator the exec
imgact calls. It has four error-return sites:

| VA | Original | New |
|---|---|---|
| 0xFFFFFFF009F56BA4 | `MOV W0, #2` | `MOV W0, #0` |
| 0xFFFFFFF009F56C54 | `MOV W0, #7` | `MOV W0, #0` |
| 0xFFFFFFF009F570F0 | `MOV W0, #4` | `MOV W0, #0` |
| 0xFFFFFFF009F57118 | `MOV W0, #2` | `MOV W0, #0` |

The function's side effects (writes to `*(a5 + 68)` bitfields and
other state) run FIRST; only the final return value is overwritten.
Unlike exp25 (which bypassed the entire validator block and crashed
downstream when uninitialised state was dereferenced), this patch
keeps every setter intact.

## Why it doesn't help

The initproc panic is **not** fired by this validator. Two
alternative hypotheses remain:

1. The BAD_MACHO reason is constructed somewhere other than
   `exec_mach_imgact`'s `sub_FFFFFFF009F56B48` → `MOVZ W0,#9 /
   MOVZ W1,#1 / BL sub_FFFFFFF009F6DDF4` sequence. The three
   known sites found by raw pattern search are:
   - 0xFFFFFFF009EA81AC (BAD_MACHO site 1 in exec_mach_imgact)
   - 0xFFFFFFF009EA81E8 (BAD_MACHO site 2 in exec_mach_imgact)
   - 0xFFFFFFF009EAFCFC (OS_REASON_EXEC code 9, **not** BAD_MACHO)
   So exactly two BAD_MACHO sites found; both ought to be
   unreachable with exp26 + exp17 applied, yet the panic persists.
   A fourth construction path, likely through a helper that builds
   the OS_REASON differently (e.g., `exec_mach_imgact`'s own
   wrapper or a different exec stage like `__mac_execve`'s policy
   check), is unaccounted for.
2. The `description: none` field suggests the reason was
   constructed WITHOUT a description buffer, which is the normal
   outcome of `os_reason_create(ns, code)` alone (without
   `os_reason_alloc_buffer`). The code may be constructed by a
   simpler helper or inlined call.

**Recommendation for the next REA session:** instrument the entry of
`sub_FFFFFFF009F6DDF4` to log every call with its (ns, code) args
and the caller address (from LR). The one that produces (9, 1) for
init is the actual site to patch. The AMFI printf wrapper at
`sub_FFFFFFF008958174` is safe to BL from a function hook (exp15
proved this).

## Reproducing

```
patchkit.py apply bootkc.exp21-18-23 \
  -m manifest-exp26-macho-validator-return-zero.yaml \
  -o bootkc.exp21-18-23-26
# → sha256 2dc6ee998e00bebe…
# boot: initproc failed to start — same as exp21-18-23.
```
