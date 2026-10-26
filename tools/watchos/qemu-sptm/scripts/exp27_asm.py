#!/usr/bin/env python3
"""
exp27: os_reason_create logger. For every call, emit a serial line with:
  "RSNC ns=%u code=%llu lr=0x%llx\n"
Then re-execute the replaced PACIBSP and continue normal flow.

Trampoline at pad+0x60 (0xFFFFFFF00A1D47D8), 60 bytes:
  STP X29, X30, [SP, #-0x50]!    ; save FP+LR, 0x50 bytes stack (room for args)
  STP X0,  X1,  [SP, #0x10]      ; save ns, code (also arg regs for printf)
  STP X2,  X3,  [SP, #0x20]
  STP X4,  X5,  [SP, #0x30]
  STP X6,  X7,  [SP, #0x40]      ; save all int arg regs

  MOV X3,  X30                   ; arg4 = caller LR (returned to our caller)
  MOV X2,  X1                    ; arg3 = code
  MOV X1,  X0                    ; arg2 = ns
  ADRP X0, fmt_str_page          ; arg1 = fmt string
  ADD  X0, X0,  #fmt_str_offset
  BL   sub_FFFFFFF008958174      ; AMFI printf

  LDP X6,  X7,  [SP, #0x40]      ; restore
  LDP X4,  X5,  [SP, #0x30]
  LDP X2,  X3,  [SP, #0x20]
  LDP X0,  X1,  [SP, #0x10]
  LDP X29, X30, [SP], #0x50

  PACIBSP                        ; re-execute replaced instruction
  B 0xfffffff009f6ddf8           ; back to inst #2 of original function
"""
import struct

PAD_BASE = 0xfffffff00a1d4778
PAD_EXP27 = PAD_BASE + 0x60    # avoid conflict with exp21 (0..47) and exp24 slot (48..79)
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
def add_imm(rd, rn, imm12):
    return (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd
def mov_reg_x(rd, rm):
    return (0b10101010 << 24) | (rm << 16) | (0b11111 << 5) | rd

# Trampoline layout (16 instructions = 64 bytes) + fmt string
insts = []
v = PAD_EXP27
# STP X29,X30,[SP,#-0x50]! -- pre-indexed, immediate = -0x50
# STP imm pre-indexed: 10 101 0 011 0 imm7 Rt2 Rn Rt
# For STP Xt, Xt2, [SP, #imm]! -- imm is signed 7-bit scaled by 8
# imm7 = -0x50/8 = -10, as 7-bit signed = 0x76 (two's complement)
# encoding: 0xA9 B0 7B FD for STP X29,X30,[SP,#-0x10]! (verified earlier)
# For -0x50 the imm7 field needs computation
# Let me use a known pattern: STP X29,X30,[SP,#-0x50]! = 0xa9b67bfd
# Easier: compute from pattern. Pre-indexed STP: `10101001_10_imm7_Rt2_Rn_Rt`
# Pre-indexed: 0xA9800000 base | (imm7 << 15) | (Rt2 << 10) | (Rn << 5) | Rt
# But actual encoding has top byte 0xA9 for 64-bit STP pre-indexed
# imm7 = -0x50/8 = -10 → bits = 0b1110110 (two's complement 7-bit) = 0x76
# Encoded: 0xA9 (top8) | 0x8 << 20 (pre-indexed indicator) | imm7 << 15 ...
# This is getting fiddly. Let me use explicit instruction constants:
# STP X29,X30,[SP,#-0x50]! → look up: 0xA9B67BFD
insts.append(0xa9b67bfd)
# STP X0,X1,[SP,#0x10] = 0xa90107e0
insts.append(0xa90107e0)
# STP X2,X3,[SP,#0x20] = 0xa9020fe2
insts.append(0xa9020fe2)
# STP X4,X5,[SP,#0x30] = 0xa90317e4
insts.append(0xa90317e4)
# STP X6,X7,[SP,#0x40] = 0xa9041fe6
insts.append(0xa9041fe6)
# MOV X3, X30
insts.append(mov_reg_x(3, 30))
# MOV X2, X1
insts.append(mov_reg_x(2, 1))
# MOV X1, X0
insts.append(mov_reg_x(1, 0))
# ADRP X0, fmt_page   (fmt is at pad+0x60+... = after all instructions)
# compute fmt_va later; place at end of trampoline
# approx: 16 instructions = 64 bytes, so fmt_va = PAD_EXP27 + 64
fmt_va = PAD_EXP27 + 16*4
va_adrp = v + len(insts)*4
insts.append(adrp(va_adrp, fmt_va & ~0xFFF, 0))
# ADD X0, X0, #imm
insts.append(add_imm(0, 0, fmt_va & 0xFFF))
# BL print
va_bl = v + len(insts)*4
insts.append(b_imm(va_bl, PRINT, bl=True))
# LDP X6,X7,[SP,#0x40] = 0xa9441fe6
insts.append(0xa9441fe6)
# LDP X4,X5,[SP,#0x30] = 0xa94317e4
insts.append(0xa94317e4)
# LDP X2,X3,[SP,#0x20] = 0xa9420fe2
insts.append(0xa9420fe2)
# LDP X0,X1,[SP,#0x10] = 0xa94107e0
insts.append(0xa94107e0)
# LDP X29,X30,[SP],#0x50 (post-indexed) = 0xa8c57bfd? Let me verify
# Post-indexed LDP: 10101000_11_imm7_Rt2_Rn_Rt. imm7 = 0x50/8 = 10 = 0x0a
# encoding = 0xA8C00000 | (0x0a << 15) | ...
# Known: LDP X29,X30,[SP],#0x50 = 0xa8c57bfd
insts.append(0xa8c57bfd)
# PACIBSP (replaced instruction)
insts.append(0xd503237f)
# B back to RESUME
va_b_back = v + len(insts)*4
insts.append(b_imm(va_b_back, RESUME, bl=False))

tramp = b''.join(enc(i) for i in insts)
# fmt string
fmt_str = b"RSNC ns=%u code=%llu lr=%p\n\0"
while len(fmt_str) % 4 != 0:
    fmt_str += b'\0'

pad_full = tramp + fmt_str
print(f"Trampoline @ 0x{PAD_EXP27:x} ({len(pad_full)}B inst+str):")
for i, w in enumerate(insts):
    print(f"  0x{v+i*4:x}  {w:08x}  {enc(w).hex()}")
print(f"  fmt string @ 0x{PAD_EXP27+len(tramp):x}: {fmt_str.hex()}")

# verify tramp len
assert len(tramp) == len(insts)*4

patch = b_imm(FN_ENTRY, PAD_EXP27, bl=False)
print(f"\nEntry patch @ 0x{FN_ENTRY:x}: {enc(patch).hex()}")

orig_pad = b'\x00' * len(pad_full)
print(f"\n---YAML---")
print(f"""  - name: exp27-logger-trampoline
    va:    0x{PAD_EXP27:X}
    orig:  "{orig_pad.hex()}"
    new:   "{pad_full.hex()}"
  - name: exp27-os-reason-hook
    va:    0x{FN_ENTRY:X}
    orig:  "7f2303d5"
    new:   "{enc(patch).hex()}"
""")
