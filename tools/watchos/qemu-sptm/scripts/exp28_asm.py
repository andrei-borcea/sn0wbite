#!/usr/bin/env python3
"""
exp28: fixed logger trampoline using explicit SUB SP + STP+LDP pairs.

Previously exp27 used incorrect STP pre-indexed encoding
(0xA9B67BFD was imm=+632 instead of -0x50), causing stack corruption
and later kfree panic. This version uses:
  SUB SP, SP, #0x60
  STP X29, X30, [SP]
  STP X0, X1, [SP, #0x10]
  ...
  LDP ... [SP, #0x10]
  LDP X29, X30, [SP]
  ADD SP, SP, #0x60

All with plain unsigned-offset form.
"""
import struct

PAD_BASE = 0xfffffff00a1d4778
PAD_EXP28 = PAD_BASE + 0xA0  # avoid conflicts entirely
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

def add_imm_x(rd, rn, imm12, sf=1):
    # ADD (immediate) 64-bit: sf | 0 | 0 | 10001 | shift(2) | imm12 | Rn | Rd
    # top10 bits for sf=1: 1001000100 → base 0x91000000
    return (sf << 31) | (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def sub_imm_x(rd, rn, imm12, sf=1):
    # SUB (immediate) 64-bit: sf | 1 | 0 | 10001 | shift(2) | imm12 | Rn | Rd
    # top10 bits for sf=1: 1101000100 → base 0xD1000000
    return (sf << 31) | (0b1101000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def stp_x_offset(rt, rt2, rn, offset_bytes):
    """STP Xt, Xt2, [Xn, #offset] (signed offset, no writeback)."""
    assert offset_bytes % 8 == 0
    imm7 = (offset_bytes // 8) & 0x7F
    # STP 64-bit signed offset: 10 101 0 010 0 imm7 Rt2 Rn Rt
    return (0b1010100100 << 22) | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt

def ldp_x_offset(rt, rt2, rn, offset_bytes):
    assert offset_bytes % 8 == 0
    imm7 = (offset_bytes // 8) & 0x7F
    # LDP 64-bit signed offset: 10 101 0 010 1 imm7 Rt2 Rn Rt
    return (0b1010100101 << 22) | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt

def mov_reg_x(rd, rm):
    return (0b10101010 << 24) | (rm << 16) | (0b11111 << 5) | rd

insts = []
v = PAD_EXP28

# SUB SP, SP, #0x60
insts.append(sub_imm_x(31, 31, 0x60))
# STP X29, X30, [SP, #0]
insts.append(stp_x_offset(29, 30, 31, 0))
# STP X0, X1, [SP, #0x10]
insts.append(stp_x_offset(0, 1, 31, 0x10))
# STP X2, X3, [SP, #0x20]
insts.append(stp_x_offset(2, 3, 31, 0x20))
# STP X4, X5, [SP, #0x30]
insts.append(stp_x_offset(4, 5, 31, 0x30))
# STP X6, X7, [SP, #0x40]
insts.append(stp_x_offset(6, 7, 31, 0x40))

# MOV X3, X30
insts.append(mov_reg_x(3, 30))
# MOV X2, X1 (code)
insts.append(mov_reg_x(2, 1))
# MOV X1, X0 (ns)
insts.append(mov_reg_x(1, 0))

# ADRP X0, fmt_page / ADD X0, X0, #imm — fmt placed after all instructions
# Count remaining instructions to know fmt offset
# instructions so far: 9. After adrp+add+bl+ldps+ldp+add+pacibsp+b = 9+2+1+5+1+1+1+1 = 21 instructions
# fmt at offset 21*4 = 84 bytes
fmt_offset_in_tramp = 21 * 4
fmt_va = PAD_EXP28 + fmt_offset_in_tramp
va_adrp = v + len(insts)*4
insts.append(adrp(va_adrp, fmt_va & ~0xFFF, 0))
insts.append(add_imm_x(0, 0, fmt_va & 0xFFF))

# BL print
va_bl = v + len(insts)*4
insts.append(b_imm(va_bl, PRINT, bl=True))

# Restore regs
insts.append(ldp_x_offset(6, 7, 31, 0x40))
insts.append(ldp_x_offset(4, 5, 31, 0x30))
insts.append(ldp_x_offset(2, 3, 31, 0x20))
insts.append(ldp_x_offset(0, 1, 31, 0x10))
insts.append(ldp_x_offset(29, 30, 31, 0))
# ADD SP, SP, #0x60
insts.append(add_imm_x(31, 31, 0x60))

# PACIBSP (re-execute replaced inst)
insts.append(0xd503237f)
# B RESUME
va_b_back = v + len(insts)*4
insts.append(b_imm(va_b_back, RESUME, bl=False))

tramp = b''.join(enc(i) for i in insts)
assert len(tramp) == len(insts)*4

fmt_str = b"RSNC ns=%u code=%llu lr=%p\n\0"
while len(fmt_str) % 4 != 0:
    fmt_str += b'\0'

pad_full = tramp + fmt_str

print(f"Trampoline @ 0x{PAD_EXP28:x} ({len(pad_full)}B total):")
for i, w in enumerate(insts):
    print(f"  0x{v+i*4:x}  {w:08x}  {enc(w).hex()}")
print(f"  fmt @ 0x{v+len(tramp):x}: {fmt_str.hex()}")

patch = b_imm(FN_ENTRY, PAD_EXP28, bl=False)
orig_pad = b'\x00' * len(pad_full)
print(f"\n---YAML---")
print(f'''  - name: exp28-logger-trampoline-fixed
    va:    0x{PAD_EXP28:X}
    orig:  "{orig_pad.hex()}"
    new:   "{pad_full.hex()}"
  - name: exp28-os-reason-hook
    va:    0x{FN_ENTRY:X}
    orig:  "7f2303d5"
    new:   "{enc(patch).hex()}"
''')
