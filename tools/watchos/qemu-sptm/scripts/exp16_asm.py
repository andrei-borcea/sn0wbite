#!/usr/bin/env python3
"""
Assemble exp16 trust-cache override for _vnode_check_signature.

Strategy:
- Preserve BTI c at 0xFFFFFFF00893DA30 (first instruction — ARMv8.5
  landing pad for BLRAA from the MAC dispatcher).
- Replace PACIBSP at 0xFFFFFFF00893DA34 with B to pad trampoline.
- Pad trampoline (9 instructions):
    STP X29, X30, [SP,#-0x10]!     ; preserve caller's PAC-signed LR
    MOV X0, X3                     ; a4 (output ptr) → arg 1
    ADRP X2, trust-cache string
    ADD X2, X2, #<low12>
    MOV W1, #1                     ; trust_id = 1 (trust-cache)
    BL sub_FFFFFFF00893BFD0        ; set *a4 = {1, "trust-cache"}
    LDP X29, X30, [SP], #0x10
    MOV X0, #0                     ; return 0 (success)
    RET

Known-good source: at VA 0xFFFFFFF00893DE1C the AMFI function itself
executes the same ADRL+MOV+BL sequence to write "trust-cache" into the
output pointer, so we are reusing a path AMFI is already prepared for.
"""
import struct

PAD_VA        = 0xfffffff00a1d4778
TRUST_STR_VA  = 0xfffffff0074b9f46         # verified from AMFI's own ADRL
AMFI_SET_TRUST = 0xfffffff00893bfd0         # sub_FFFFFFF00893BFD0
AMFI_ENTRY    = 0xfffffff00893da30
PACIBSP_VA    = AMFI_ENTRY + 4

def encode(x):
    return struct.pack('<I', x & 0xFFFFFFFF)

def b_imm(from_va, to_va, bl=False):
    delta = to_va - from_va
    assert delta % 4 == 0
    imm26 = (delta // 4) & 0x3FFFFFF
    opc = 0b100101 if bl else 0b000101
    return (opc << 26) | imm26

def adrp(from_va, to_va, rd):
    pc_page = from_va & ~0xFFF
    target_page = to_va & ~0xFFF
    delta_pages = (target_page - pc_page) >> 12
    imm21 = delta_pages & 0x1FFFFF
    immlo = imm21 & 0x3
    immhi = (imm21 >> 2) & 0x7FFFF
    return (1 << 31) | (immlo << 29) | (0b10000 << 24) | (immhi << 5) | rd

def add_imm(rd, rn, imm12):
    return (0b1001000100 << 22) | (imm12 << 10) | (rn << 5) | rd

def movz_wd(rd, imm16, sf=0):
    # sf=0 for Wd, sf=1 for Xd
    return (sf << 31) | (0b10100101 << 23) | (imm16 << 5) | rd

def mov_reg(rd, rm):
    # MOV Xd, Xm = ORR Xd, XZR, Xm
    # 0b10101010000mmmmm000000nnnnnddddd  (sf=1 for 64-bit)
    return (0b10101010 << 24) | (rm << 16) | (0b11111 << 5) | rd

# ---- trampoline ----
insts = []
va = PAD_VA

insts.append(0xa9bf7bfd)                                                # STP X29,X30,[SP,#-0x10]!
insts.append(mov_reg(0, 3))                                             # MOV X0, X3

va_adrp = PAD_VA + len(insts)*4
insts.append(adrp(va_adrp, TRUST_STR_VA, 2))                            # ADRP X2, page
insts.append(add_imm(2, 2, TRUST_STR_VA & 0xFFF))                       # ADD X2, X2, #low12

insts.append(movz_wd(1, 1, sf=0))                                       # MOVZ W1, #1

va_bl = PAD_VA + len(insts)*4
insts.append(b_imm(va_bl, AMFI_SET_TRUST, bl=True))                     # BL set_trust

insts.append(0xa8c17bfd)                                                # LDP X29,X30,[SP],#0x10
insts.append(movz_wd(0, 0, sf=1))                                       # MOVZ X0, #0
insts.append(0xd65f03c0)                                                # RET

tramp = b''.join(encode(i) for i in insts)
assert len(tramp) == 36

print("Trampoline @ 0x%x (%d bytes):" % (PAD_VA, len(tramp)))
for i, w in enumerate(insts):
    off = PAD_VA + i*4
    print("  0x%x  %08x  %s" % (off, w, encode(w).hex()))

# Entry hook: patch at PACIBSP_VA (0xFFFFFFF00893DA34) → B pad
entry_patch = b_imm(PACIBSP_VA, PAD_VA, bl=False)
print()
print("Entry hook: replace PACIBSP at 0x%x with B 0x%x" % (PACIBSP_VA, PAD_VA))
print("  orig: 7f2303d5  (PACIBSP)")
print("  new:  %s  (B %#x, encoded 0x%08x)" % (encode(entry_patch).hex(), PAD_VA, entry_patch))

# Pad origin bytes = zeros
orig_pad = b'\x00' * len(tramp)
print()
print("---- YAML snippets ----")
print(f"""
  - name: exp16-trust-cache-trampoline
    effect: >
      pad trampoline that invokes sub_FFFFFFF00893BFD0(a4, 1,
      "trust-cache") then returns 0, overriding every AMFI vnode
      signature verdict to trust-cache.
    va:    0x{PAD_VA:X}
    orig:  "{orig_pad.hex()}"
    new:   "{tramp.hex()}"

  - name: exp16-amfi-vcs-hook
    effect: >
      replace PACIBSP at offset +4 of _vnode_check_signature with
      B 0x{PAD_VA:x}, preserving the BTI c landing pad at offset 0 so
      the MAC-dispatcher BLRAA keeps landing on a valid target.
    va:    0x{PACIBSP_VA:X}
    orig:  "7f2303d5"
    new:   "{encode(entry_patch).hex()}"
""")
