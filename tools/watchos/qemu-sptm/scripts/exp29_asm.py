#!/usr/bin/env python3
"""
exp29: FIXED logger — put varargs at [SP+0..16] per Darwin AArch64 ABI
so AMFI's printf (sub_FFFFFFF008958174 → sub_FFFFFFF00A0766B4) reads
them correctly from va_list.

Layout at call time (just before BL):
  [SP+0]  = ns (first vararg)
  [SP+8]  = code
  [SP+16] = lr
  [SP+24..0x30] = save X29/X30
  [SP+0x30..0x40] = save X0,X1  (keep orig regs for restore)

Actually: AMFI's printf va_list starts at the FIRST stacked arg
beyond the register args (X0-X7 take a1-a8). Our call has
X0=fmt, X1=ns, X2=code, X3=lr (named args in regs). The varargs
from printf's POV start right after the "declared" args, which
for sub_FFFFFFF008958174 is just (fmt, ...).

AMFI's thunk sub_FFFFFFF008958174 does `return sub_FFFFFFF00A0766B4(a1)`
— compiled as TAIL CALL (B), passing only X0 to the inner. Inner
then reads a2-a9 from X1-X7 and [SP+0]. The format processor at
line 45 of inner uses &a9 which maps to [FP+0] = [SP+<prolog>].
In caller terms: a9 at BL time = [SP+0].

Format "%u %llu %p" has 3 vararg slots. The va_list walker starts at
&a9 and reads sequential 8-byte slots. So:
  slot 0 (= &a9 = [SP+0 at BL]): first vararg (ns, %u reads low 32b)
  slot 1 (= &a9 + 8 = [SP+8]):   second (code, %llu reads 64b)
  slot 2 (= &a9 + 16 = [SP+16]): third (lr, %p reads 64b)

(Actually since a1-a8 are in regs, va_list has to compose from
both reg saves and stack. But Darwin's variadic for arm64 uses
a different convention: everything on stack regardless of X0-X7.
To be safe, place varargs at [SP+0], [SP+8], [SP+16].)
"""
import struct

PAD_BASE = 0xfffffff00a1d4778
PAD_EXP29 = PAD_BASE + 0x100  # fresh offset
PRINT = 0xfffffff008958174
FN_ENTRY = 0xfffffff009f6ddf4
RESUME = FN_ENTRY + 4

def enc(x): return struct.pack('<I', x & 0xFFFFFFFF)

def b_imm(frm, to, bl=False):
    delta = to - frm; imm26 = (delta // 4) & 0x3FFFFFF
    return ((0b100101 if bl else 0b000101) << 26) | imm26

def adrp(frm, to, rd):
    pc_page = frm & ~0xFFF; target_page = to & ~0xFFF
    imm21 = ((target_page - pc_page) >> 12) & 0x1FFFFF
    return (1 << 31) | ((imm21 & 3) << 29) | (0b10000 << 24) | (((imm21 >> 2) & 0x7FFFF) << 5) | rd

def add_imm_x(rd, rn, imm12):
    return (1 << 31) | (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def sub_imm_x(rd, rn, imm12):
    return (1 << 31) | (0b1101000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def stp_x_offset(rt, rt2, rn, offset_bytes):
    imm7 = (offset_bytes // 8) & 0x7F
    return (0b1010100100 << 22) | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt

def ldp_x_offset(rt, rt2, rn, offset_bytes):
    imm7 = (offset_bytes // 8) & 0x7F
    return (0b1010100101 << 22) | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt

def str_x_offset(rt, rn, offset_bytes):
    # STR Xt, [Xn, #imm] (unsigned offset, 64-bit): 11 111 0 01 00 imm12 Rn Rt
    imm12 = (offset_bytes // 8) & 0xFFF
    return (0b1111100100 << 22) | (imm12 << 10) | (rn << 5) | rt

def str_w_offset(rt, rn, offset_bytes):
    # STR Wt, [Xn, #imm] (unsigned offset, 32-bit): 10 111 0 01 00 imm12 Rn Rt
    imm12 = (offset_bytes // 4) & 0xFFF
    return (0b1011100100 << 22) | (imm12 << 10) | (rn << 5) | rt

def adrp_add(frm, to, rd):
    """ADRP + ADD pair, returns 2 instructions."""
    return [adrp(frm, to, rd), add_imm_x(rd, rd, to & 0xFFF)]

insts = []
v = PAD_EXP29

# SUB SP, SP, #0x60
insts.append(sub_imm_x(31, 31, 0x60))
# STP X29, X30, [SP, #0x30]  — save FP/LR up high (avoid conflict with varargs at [SP+0..16])
insts.append(stp_x_offset(29, 30, 31, 0x30))
# STP X0, X1, [SP, #0x40]
insts.append(stp_x_offset(0, 1, 31, 0x40))
# STP X2, X3, [SP, #0x50]
insts.append(stp_x_offset(2, 3, 31, 0x50))

# Put varargs on stack: ns at [SP+0], code at [SP+8], lr_hi at [SP+16], lr_lo at [SP+24]
# ns is in X0 (first arg to os_reason_create), code in X1, caller LR is in X30
# STR W0, [SP, #0]       — ns (32-bit stored, upper padding doesn't matter for %u)
insts.append(str_w_offset(0, 31, 0))
# STR X1, [SP, #8]       — code
insts.append(str_x_offset(1, 31, 8))
# split LR (X30) into hi and lo 32-bit halves
# LSR X8, X30, #32  → X8 = hi32(LR)
# Shift right encoding: LSR Xd, Xn, #imm = UBFM Xd, Xn, #imm, #63
# UBFM 64-bit: 1_1010011_0_1_immr_imms_Rn_Rd. immr=32, imms=63
# = 0xD3400000 | (0 << 22) | (32 << 16) | (63 << 10) | (30 << 5) | 8
# ...actually simpler: LSR X8, X30, #32 = 0xD360FFC8
insts.append(0xd360ffc8)
# STR W8, [SP, #0x10]    — lr_hi (store low 32 bits of X8 which = hi32 of X30)
insts.append(str_w_offset(8, 31, 0x10))
# STR W30, [SP, #0x18]   — lr_lo (store low 32 bits of X30)
insts.append(str_w_offset(30, 31, 0x18))

# Now X0 can be clobbered — load fmt string address
# fmt placed at end of trampoline
# count current insts to know fmt_offset
# After this ADRP+ADD+BL+5 LDP+ADD+PACIBSP+B = 7 + 2 + 1 + 5 + 1 + 1 + 1 = 18 instructions total
# So fmt starts at offset 18*4 = 72 bytes
num_insts_total = 18
fmt_offset = num_insts_total * 4
fmt_va = PAD_EXP29 + fmt_offset
# ADRP X0, fmt_page
va_adrp = v + len(insts)*4
insts.append(adrp(va_adrp, fmt_va & ~0xFFF, 0))
# ADD X0, X0, #imm
insts.append(add_imm_x(0, 0, fmt_va & 0xFFF))
# BL AMFI print
va_bl = v + len(insts)*4
insts.append(b_imm(va_bl, PRINT, bl=True))

# Restore
insts.append(ldp_x_offset(2, 3, 31, 0x50))
insts.append(ldp_x_offset(0, 1, 31, 0x40))
insts.append(ldp_x_offset(29, 30, 31, 0x30))
# ADD SP, SP, #0x60
insts.append(add_imm_x(31, 31, 0x60))

# PACIBSP (replaced inst)
insts.append(0xd503237f)
# B RESUME
va_b_back = v + len(insts)*4
insts.append(b_imm(va_b_back, RESUME, bl=False))

tramp = b''.join(enc(i) for i in insts)
assert len(tramp) == num_insts_total * 4, f"mismatch: got {len(tramp)} expected {num_insts_total*4}"

fmt_str = b"RSNC ns=%u code=%llu lrhi=%u lrlo=%u\n\0"
while len(fmt_str) % 4 != 0:
    fmt_str += b'\0'

pad_full = tramp + fmt_str

print(f"Trampoline @ 0x{PAD_EXP29:x} ({len(pad_full)}B total, {num_insts_total} insts):")
for i, w in enumerate(insts):
    print(f"  0x{v+i*4:x}  {w:08x}  {enc(w).hex()}")
print(f"  fmt @ 0x{v+len(tramp):x}: {fmt_str.hex()}")

patch = b_imm(FN_ENTRY, PAD_EXP29, bl=False)
orig_pad = b'\x00' * len(pad_full)
print(f"\n---YAML---")
print(f'''  - name: exp29-logger-stack-args
    va:    0x{PAD_EXP29:X}
    orig:  "{orig_pad.hex()}"
    new:   "{pad_full.hex()}"
  - name: exp29-os-reason-hook
    va:    0x{FN_ENTRY:X}
    orig:  "7f2303d5"
    new:   "{enc(patch).hex()}"
''')
