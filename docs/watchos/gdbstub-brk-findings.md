# watchOS 27 Watch7,11 — QEMU gdbstub + BRK findings

**Date:** 2026-10-27 (continuation session)
**Setup:** `qemu-system-aarch64 … -gdb tcp::1234 -S` + `lldb -o 'gdb-remote 1234'`
**Baseline:** `9-patch + exp16` (bootkc.exp16), the consolidated chain.

## What the gdbstub session achieved

1. **LLDB gdb-remote connection to the SPTM qemu works.** Attach via
   `(lldb) gdb-remote localhost:1234` succeeds immediately after
   QEMU is launched with `-gdb tcp::1234 -S`. The CPU is paused at
   `0x8070a0388` (reset vector).

2. **Software and hardware breakpoints DO NOT fire** at kernel
   VAs (`0xFFFFFFF029EA81B4`, etc.). lldb reports the breakpoint
   as "resolved" but the kernel reaches those addresses without
   trapping. Likely cause: SPTM's R-X mapping of `__TEXT_EXEC`
   prevents software BRK insertion, and arm64e PAC authentication
   interferes with hardware watchpoints too.

3. **BRK #imm16 instructions inserted into the bootkc file DO
   trap** — kernel panics `"Break 0xB487 instruction exception from
   kernel. Panic (by design) at pc 0xfffffff029ea81f0, lr
   0xfffffff029ea8060"`. The panic message includes a full
   register dump — effectively a one-shot debugger.

4. **Not every file-level byte patch takes effect at runtime.**
   Specifically:
   - exp30's `MOV W0,#9 → MOV W0,#0` at **0xFFFFFFF009EA81AC** and
     **0xFFFFFFF009EA81E8**: ✅ confirmed effective (ns changed
     from 9 to 0 in the panic).
   - BRK `0xB487` at **0xFFFFFFF009EA81F0**: ✅ fires.
   - exp17's `CBZ W0 → B` at **0xFFFFFFF009EA8198**: ❌ no effect
     (bogus BL at `0xFFFFFFF009EA81B4` is still reached when
     logger is applied).
   - exp26's four `MOV W0,#N → MOV W0,#0` at **0xFFFFFFF009F56BA4**,
     **…C54**, **…70F0**, **…7118**: ❌ no effect (validator
     still returns > 11, triggering the 2nd BAD_MACHO path).

5. **The pattern isn't cache-line aligned.** 0x9EA8198 and 0x9EA81AC
   are in the SAME cache line (0x9EA8180–0x9EA81BF) — one works,
   the other doesn't. The most plausible explanation: **SPTM
   verifies specific signed hashes over specific instruction
   ranges at load time, and serves the ORIGINAL byte from a signed
   copy for those protected regions.** The granularity isn't
   obvious from the data.

## The confirmed firing site

With a BRK at `0xFFFFFFF009EA81F0` and bootkc.exp16 as baseline,
the panic captures:

```
pc:  0xfffffff029ea81f0   (BRK)
lr:  0xfffffff029ea8060   (return address inside sub_FFFFFFF009EA7D28)
x0:  0x9                  (ns = OS_REASON_EXEC)
x1:  0x1                  (code = BAD_MACHO)
x2:  0xffffffe0cded8000   (OS_REASON options buffer, maybe)
x3:  0x4000               (page/alignment constant)
x4:  0xffffffe0cded8080   (buffer+0x80)
x19: 0xffffffdf01f27800   (proc struct ptr, likely)
x20: 0x55 = 85            (v1 = 85 — "v34 > 0xB" path from Hex-Rays)
x21: 0xffffffdf01f5cf00   (p_cs_blob or thread struct)
x22: 0xffffffdfe7d81a80   (current thread)
x23: 0xffffffdf01f5d668   (exec state struct)
x24: 0xffffffe65fc7c000   (userspace addr — Mach-O being validated)
x25: 0xffffffdfe7d91300   (exec image data)
x27: 0x104                (platform id / flags)
esr: 0xf200b487           (BRK #0xB487)
```

**Key signal: x20 = 0x55 (85).** From the Hex-Rays decompile of
`sub_FFFFFFF009EA7D28`:

```c
if ( v34 != 0 )
{
    if ( v34 > 0xB )
        v1 = 85;      // ← x20 = 85
    ...
}
```

So the Mach-O validator `sub_FFFFFFF009F56B48` (which exp26
attempted to zero out) returns a value **> 11** at runtime, even
though its Hex-Rays shows return values at most 7. Either Hex-Rays
missed a code path, or the validator propagates a return from a
sub-function (which isn't patched by exp26).

## Reproducibility artifacts

Preserved under `tools/watchos/qemu-sptm/gdbstub/`:
- `lldb_bp_cb.py` — LLDB python callback for breakpoint handlers
- `lldb_init_debug.py` — reference commentary with command sequences

The BRK-at-byte-offset technique is more reliable than lldb
breakpoints for the SPTM kernel. For each investigation site:

```python
# bootkc file offset for VA X: X - 0xFFFFFFF007004000
# BRK #imm encoding: 0xD4200000 | (imm16 << 5)
```

## Takeaways for the next investigation

1. **Patches at BAD_MACHO reason-create sites** (0xEA81AC, 0xEA81E8,
   0xEA81F0) **work**, but changing arg values only changes the
   panic message — the exec still fails and init still dies.

2. **Patches at the validator return sites** (0xF56BA4 cluster) and
   **patches at the CBZ gates** (0xEA8060, 0xEA8198) **do not work**
   under runtime SPTM. These need a different strategy — ideally
   finding equivalent patches at adjacent addresses that DO take
   effect, or inserting a BRK + manual lldb instruction override.

3. **init process is dying because exec of some binary fails
   BAD_MACHO.** The validator `sub_FFFFFFF009F56B48` rejects the
   Mach-O with an unknown error (returned >11, meaning not in the
   small-table range 1..10). To proceed:
   - Find WHICH Mach-O file is being validated (via x19 → proc,
     then proc->p_textvp, then vp->v_name).
   - Understand which structural check in the validator rejects
     it.
   - Either patch that check (if its address is unprotected) or
     replace the file.

4. **Full userspace debugging would require** attaching lldb to
   a running kernel without BRK traps (via `-S` + manual single-
   step), or using a custom shared cache + rebuilt dyld to
   bypass the AMFI chain entirely.
