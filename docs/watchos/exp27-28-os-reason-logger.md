# watchOS 27 Watch7,11 — exp27 & exp28: os_reason_create logger

**Date:** 2026-10-26
**Goal:** instrument `sub_FFFFFFF009F6DDF4` (os_reason_create) to log
each (namespace, code, caller_LR) call to the serial console, so the
exact caller that fires (ns=9, code=1) for init can be pinpointed.
**Status:** trampoline infrastructure works, but AMFI's printf
wrapper formatting produces malformed output when passed non-literal
numeric and pointer args — the method of inferring the firing site
from the serial log is unreliable, so this instrumentation did not
locate the BAD_MACHO caller.

## exp27 — first logger attempt (STP pre-indexed encoding wrong)

Trampoline at pad+0x60 using pre-indexed `STP X29, X30, [SP, #-0x50]!`
encoding. The constant I wrote (0xA9B67BFD) actually decodes to
`STP X29, X30, [SP, #+632]!` (imm7 = 0x4F = +79, scaled by 8 = +632),
which writes caller-registers 632 bytes past SP into whatever memory
is there. The result was a `kfree: addr …trying to free with
nonsensical size` nested panic.

Lesson: the AArch64 pre-indexed STP imm7 field is signed and
scaled by 8 — easy to miscompute. For negative offsets, use
explicit `SUB SP, SP, #<n>` + plain offset-form STP instead.

## exp28 — fixed trampoline, logger lands

Rewrote the trampoline with:
```
SUB  SP, SP, #0x60
STP  X29, X30, [SP]
STP  X0, X1, [SP, #0x10]
STP  X2, X3, [SP, #0x20]
STP  X4, X5, [SP, #0x30]
STP  X6, X7, [SP, #0x40]
MOV  X3, X30         ; arg4 = caller LR
MOV  X2, X1          ; arg3 = code
MOV  X1, X0          ; arg2 = ns
ADRP X0, fmt_page    ; arg1 = fmt
ADD  X0, X0, #imm
BL   sub_FFFFFFF008958174
LDP  X6, X7, [SP, #0x40]
LDP  X4, X5, [SP, #0x30]
LDP  X2, X3, [SP, #0x20]
LDP  X0, X1, [SP, #0x10]
LDP  X29, X30, [SP]
ADD  SP, SP, #0x60
PACIBSP
B 0xFFFFFFF009F6DDF8
```

**Result:** boot panics with the same `initproc failed to start`,
and the serial log has exactly ONE `RSNC` line:
```
ns=2892839984 code=<ptr> lr=0x9
```
0x2892839984 = 0xACA92970, which isn't a valid namespace. The `<ptr>`
output for `%llu` and `lr=0x9` suggest AMFI's printf wrapper
(`sub_FFFFFFF008958174` → `sub_FFFFFFF00A0766B4`) doesn't follow
standard glibc printf semantics — likely uses its own argument
walker that reads from a va_list on the STACK rather than from the
registers I set. On AArch64 Darwin, variadic args beyond the first
are passed on the stack, not in X1..X7.

**Infrastructure validation:**
- The hook (B at `0xFFFFFFF009F6DDF4` → trampoline at pad+0xA0) fires
  cleanly. One print appeared, the kernel continued, PACIBSP + B back
  resumed the function body correctly.
- The AMFI printf wrapper writes to the SAME serial channel as its
  own "AMFI: Booted in a VM" lines (which also appeared in this boot).

**Instrumentation limitation:**
- The AArch64 Darwin variadic calling convention passes extra args
  on the stack, not in registers. Our trampoline put ns/code/lr in
  X1/X2/X3 but AMFI's printf expects them at [SP+0], [SP+8],
  [SP+16]. Hence the garbled output.

## What's missing to make this work

A follow-up trampoline that:
1. SUB SP, SP, #0x30 (space for 3 varargs on stack)
2. STR W_ns, [SP]           ; ns at offset 0
3. STR X_code, [SP, #8]     ; code at offset 8
4. STR X_lr, [SP, #16]      ; lr at offset 16
5. ADRP X0, fmt
6. ADD X0, X0, #imm
7. BL AMFI printf

Then the printf reads ns, code, lr from [SP+0..24], matching Darwin
variadic convention. This is one iteration away from a usable
logger.

**Alternative, cleaner approach**: write a tiny per-call record to a
STATIC buffer in __DATA (e.g., a 1KB ring at some pad address),
containing (ns, code, lr) tuples as raw 24-byte entries. No printf
needed. Then at panic time, OR after a controlled reset, dump the
ring. For a VM target, dumping via inspection of the bootkc runtime
image through QEMU gdbstub is viable.

Both are left for the next REA session.

## Current best chain of record (unchanged by exp27/28)

```
bootkc.before-exp10   (9-patch APFS/md base)
  + manifest-exp16-amfi-trustcache.yaml  → bootkc.exp16
       → reaches IOKit serial-driver matching, hits
         init[1] fatal signal 9 BAD_MACHO at posix_spawn
```

The composed chain exp21+exp18+exp23 reaches `initproc failed to
start` (one layer further; init can no longer be panicked for a
fatal signal and sandbox no longer catches shenanigans, but exec
BAD_MACHO still fires from an unidentified construction site).
