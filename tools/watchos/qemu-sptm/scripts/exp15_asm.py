#!/usr/bin/env python3
"""
Assemble exp15 instrumentation trampoline for sub_FFFFFFF009F79CA8
(shared_region_map_and_slide_setup). Minimum variant: emit a single
"EXP15 ENTER" marker at function entry, then execute replaced PACIBSP
and resume at original site+4.

Pad area at VA 0xfffffff00a1d4778 (14472 bytes free).
Print primitive: sub_FFFFFFF008958174 (AMFI printf wrapper, variadic).
"""

import struct

PAD_VA   = 0xfffffff00a1d4778
PRINT_VA = 0xfffffff008958174
FN_VA    = 0xfffffff009f79ca8          # sub_FFFFFFF009F79CA8 PACIBSP
RESUME_VA = FN_VA + 4                   # next instruction after PACIBSP

def b_imm(from_va, to_va, opc_bl=False):
    """Encode B or BL with 26-bit signed PC-relative."""
    delta = to_va - from_va
    assert delta % 4 == 0
    imm26 = (delta // 4) & 0x3FFFFFF
    opc = 0b100101 if opc_bl else 0b000101
    return (opc << 26) | imm26

def adrp(from_va, to_page_va, rd):
    """Encode ADRP Xd, <label> where to_page_va is 4K-aligned."""
    assert (to_page_va & 0xFFF) == 0
    # ADRP uses PC[63:12] (not PC itself), so page_of(PC) relative
    pc_page = from_va & ~0xFFF
    delta_pages = (to_page_va - pc_page) >> 12
    imm21 = delta_pages & 0x1FFFFF
    immlo = imm21 & 0x3
    immhi = (imm21 >> 2) & 0x7FFFF
    return (1 << 31) | (immlo << 29) | (0b10000 << 24) | (immhi << 5) | rd

def add_imm(rd, rn, imm12):
    """ADD Xd, Xn, #imm12 (no shift)."""
    assert 0 <= imm12 < 0x1000
    return (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def movz_w(rd, imm16):
    """MOVZ Wd, #imm16."""
    assert 0 <= imm16 < 0x10000
    return (0b010100101 << 23) | (imm16 << 5) | rd

def encode_u32(x):
    return struct.pack('<I', x & 0xFFFFFFFF)

# ---- trampoline layout ----
# Target: at pad_va + 0, emit a marker then resume
# instructions (10 × 4B = 40B) + format string (padded to 24B)

insts = []
va = PAD_VA

# STP X29, X30, [SP, #-0x10]!   -> a9bf7bfd
insts.append( 0xa9bf7bfd )
# STP X0, X1, [SP, #-0x10]!     -> a9bf07e0
insts.append( 0xa9bf07e0 )
# compute fmt string VA (after 10 instructions = +40 bytes)
fmt_va = PAD_VA + 40
# ADRP X0, page(fmt_va)
va_at_adrp = PAD_VA + 8
insts.append( adrp(va_at_adrp, fmt_va & ~0xFFF, 0) )
# ADD X0, X0, #(fmt_va & 0xFFF)
insts.append( add_imm(0, 0, fmt_va & 0xFFF) )
# MOVZ W1, #0xFE (marker ID for ENTRY)
insts.append( movz_w(1, 0xFE) )
# BL sub_FFFFFFF008958174
va_at_bl = PAD_VA + 20
insts.append( b_imm(va_at_bl, PRINT_VA, opc_bl=True) )
# LDP X0, X1, [SP], #0x10       -> a8c107e0
insts.append( 0xa8c107e0 )
# LDP X29, X30, [SP], #0x10     -> a8c17bfd
insts.append( 0xa8c17bfd )
# PACIBSP (replaced instruction, re-executed here)  -> d503237f
insts.append( 0xd503237f )
# B RESUME_VA
va_at_b_return = PAD_VA + 36
insts.append( b_imm(va_at_b_return, RESUME_VA, opc_bl=False) )

tramp_bytes = b''.join(encode_u32(i) for i in insts)
assert len(tramp_bytes) == 40

# format string (null-terminated)
fmt_str = b"EXP15 marker=%u\n\0"
# pad to multiple of 4
while len(fmt_str) % 4 != 0:
    fmt_str += b'\0'
pad_full = tramp_bytes + fmt_str

print("Trampoline @ VA 0x%x (len 0x%x)" % (PAD_VA, len(pad_full)))
for i, w in enumerate(insts):
    off = PAD_VA + i*4
    print("  0x%x  %08x  %s" % (off, w, encode_u32(w).hex()))
print("  0x%x  <fmt str>  %s" % (PAD_VA + len(tramp_bytes), fmt_str.hex()))

# Original bytes at FN_VA (first instruction = PACIBSP = d503237f, 4 bytes)
# New bytes at FN_VA: B pad_va
entry_patch_insn = b_imm(FN_VA, PAD_VA, opc_bl=False)
print()
print("Entry patch @ VA 0x%x: B 0x%x" % (FN_VA, PAD_VA))
print("  orig: 7f2303d5   (PACIBSP)")
print("  new:  %s   (B %#x)" % (encode_u32(entry_patch_insn).hex(), PAD_VA))

# Original bytes at PAD_VA: verified to be all zeros (post-function alignment pad)
orig_pad = b'\x00' * len(pad_full)
print()
print("Pad original bytes (expected NOPs): %s" % orig_pad.hex())
print("Pad new bytes:                      %s" % pad_full.hex())

# Emit YAML snippets
print()
print("---- YAML patch entries ----")
print(f"""
  - name: exp15-entry-trampoline
    effect: "install trampoline at pad VA 0x{PAD_VA:x}: save regs, call AMFI print wrapper with 'EXP15 marker=%u\\n' + marker 0xFE, restore regs, re-execute PACIBSP, branch back to resume site"
    va:    0x{PAD_VA:X}
    orig:  "{orig_pad.hex()}"
    new:   "{pad_full.hex()}"

  - name: exp15-entry-hook
    effect: "replace sub_FFFFFFF009F79CA8 PACIBSP with B to pad trampoline"
    va:    0x{FN_VA:X}
    orig:  "7f2303d5"
    new:   "{encode_u32(entry_patch_insn).hex()}"
""")
