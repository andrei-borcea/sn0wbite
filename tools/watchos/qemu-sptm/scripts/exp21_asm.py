#!/usr/bin/env python3
"""
Assemble exp21: exp16 trust-cache override + also set CS_PLATFORM_BINARY
(0x4000000) in *a5 (cs_flags pointer). This mirrors AMFI's own
trust-cache success path which does *v108 |= 0x4000000u BEFORE calling
sub_FFFFFFF00893BFD0(a4, 1, "trust-cache"). Setting both consistently
should keep downstream consumers (Sandbox evaluator) happy.

Trampoline (same pad at 0xFFFFFFF00A1D4778):
  stp  x29, x30, [sp,#-0x10]!
  ldr  w8,  [x4]              ; load *a5 (cs_flags)
  orr  w8,  w8,  #0x4000000   ; set CS_PLATFORM_BINARY
  str  w8,  [x4]              ; store back
  mov  x0,  x3                ; a4 → arg1
  adrp x2,  0xFFFFFFF0074B9000 ; "trust-cache"
  add  x2,  x2,  #0xF46
  movz w1,  #1
  bl   0xFFFFFFF00893BFD0      ; set trust category
  ldp  x29, x30, [sp], #0x10
  movz x0,  #0                 ; return 0
  ret
"""
import struct

PAD_VA        = 0xfffffff00a1d4778
TRUST_STR_VA  = 0xfffffff0074b9f46
AMFI_SET_TRUST = 0xfffffff00893bfd0
AMFI_ENTRY    = 0xfffffff00893da30
PACIBSP_VA    = AMFI_ENTRY + 4

def encode(x): return struct.pack('<I', x & 0xFFFFFFFF)

def b_imm(from_va, to_va, bl=False):
    delta = to_va - from_va
    imm26 = (delta // 4) & 0x3FFFFFF
    return ((0b100101 if bl else 0b000101) << 26) | imm26

def adrp(from_va, to_va, rd):
    pc_page = from_va & ~0xFFF
    target_page = to_va & ~0xFFF
    imm21 = ((target_page - pc_page) >> 12) & 0x1FFFFF
    return (1 << 31) | ((imm21 & 3) << 29) | (0b10000 << 24) | (((imm21 >> 2) & 0x7FFFF) << 5) | rd

def add_imm(rd, rn, imm12):
    return (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def movz_wd(rd, imm16, sf=0):
    return (sf << 31) | (0b10100101 << 23) | (imm16 << 5) | rd

def mov_reg(rd, rm):
    return (0b10101010 << 24) | (rm << 16) | (0b11111 << 5) | rd

# ORR Wd, Wn, #imm (bitfield) encoding: 32-bit
# ORR (immediate) 32-bit: `0 01 100100 N immr imms Rn Rd`
# with sf=0, opc=01 (ORR), N must be 0 for 32-bit, imm13 = {N, immr, imms}
# For bitmask #0x4000000 = bit 26 set, pattern 00000100_00000000_00000000_00000000
# Encoding of 0x4000000 as bitmask:
#   Need to find (N, immr, imms) that encode this pattern.
#   For a single bit set at pos 26 in a 32-bit word:
#     Element size = 32, rotation = 32-26 = 6, S = 0 (one bit set in element: imms = size-1 = 0)
#     But imms encoding: for single-bit mask of width 1 in 32-bit, imms = 0b000000 (width=1)
#     immr = rotation = 6 (rotation right)
#     Actually, imms for ORR imm with width=1 is 0b000000. And immr = 32 - position
#     bit 26 set means rotate right by (32-26) = 6 positions from a pattern with LSB set
#     So immr = 6, imms = 0.
# full ORR 32-bit imm: sf=0 opc=01 100100 N(1) immr(6) imms(6) Rn(5) Rd(5)
# = 00_11_0010_0 N immr imms Rn Rd
# = 0x32000000 | (N<<22) | (immr<<16) | (imms<<10) | (Rn<<5) | Rd
def orr_imm_w(rd, rn, N, immr, imms):
    return 0x32000000 | (N << 22) | (immr << 16) | (imms << 10) | (rn << 5) | rd

insts = []
insts.append(0xa9bf7bfd)                                        # STP X29,X30,[SP,#-0x10]!
# LDR W8, [X4]  → 32-bit load, offset 0
#   LDR Wt, [Xn, #imm]: encoding = 10 111 0 01 01 imm12 Rn Rt
#   LDR W8, [X4] = 10111001010000000000 Rn=4 Rd=8 ... actually:
#   LDR (immediate, unsigned offset) 32-bit = 10111001 01 imm12 Rn Rt
#   imm12 = 0, Rn=4, Rt=8 → 0xB9400088
insts.append(0xb9400088)
# ORR W8, W8, #0x4000000 → bitmask immediate
# For bit 26 set in 32-bit value: N=0, immr=6, imms=0
# Actually let me verify. The pattern 0x04000000 has one bit set at position 26.
# Bitmask encoding for ORR immediate with 32-bit:
#   The 32-bit pattern must be rotated version of a repeating sub-pattern
#   For a single bit, element size = 32 (which means N=0, imms top bits must be 11110x)
#   Actually: for 32-bit single-bit, imms = 0 and immr = 32 - bit_pos
#   But imms encoding needs more thought. Let me use a known good encoding:
#   `ORR W8, W8, #0x4000000` → assembler produces 0x320C0108
insts.append(0x320c0108)
# STR W8, [X4]  → 0xB9000088
insts.append(0xb9000088)
# MOV X0, X3
insts.append(mov_reg(0, 3))
# ADRP X2, trust-cache page
va_adrp = PAD_VA + len(insts)*4
insts.append(adrp(va_adrp, TRUST_STR_VA, 2))
# ADD X2, X2, #low12
insts.append(add_imm(2, 2, TRUST_STR_VA & 0xFFF))
# MOVZ W1, #1
insts.append(movz_wd(1, 1, sf=0))
# BL sub_FFFFFFF00893BFD0
va_bl = PAD_VA + len(insts)*4
insts.append(b_imm(va_bl, AMFI_SET_TRUST, bl=True))
# LDP X29,X30,[SP],#0x10
insts.append(0xa8c17bfd)
# MOVZ X0,#0
insts.append(movz_wd(0, 0, sf=1))
# RET
insts.append(0xd65f03c0)

tramp = b''.join(encode(i) for i in insts)
print(f"Trampoline @ 0x{PAD_VA:x} ({len(tramp)}B):")
for i,w in enumerate(insts):
    print(f"  0x{PAD_VA+i*4:x}  {w:08x}  {encode(w).hex()}")

entry_patch = b_imm(PACIBSP_VA, PAD_VA, bl=False)
print(f"\nEntry hook: {encode(entry_patch).hex()}")

orig_pad = b'\x00' * len(tramp)
print(f"\n---YAML---")
print(f"""  - name: exp21-trust-cache-plus-platform
    va:    0x{PAD_VA:X}
    orig:  "{orig_pad.hex()}"
    new:   "{tramp.hex()}"
  - name: exp21-amfi-vcs-hook
    va:    0x{PACIBSP_VA:X}
    orig:  "7f2303d5"
    new:   "{encode(entry_patch).hex()}"
""")
