#!/usr/bin/env python3
"""
exp24: filter os_reason_create to return NULL for (ns=9, code=1).

Trampoline at pad+0x30 (after exp21's 48 bytes):
  CMP   W0, #9
  B.NE  prolog     ; not namespace 9 → normal reason
  CMP   X1, #1
  B.NE  prolog     ; not BAD_MACHO → normal reason
  MOV   X0, #0     ; return NULL (no reason created)
  RET
prolog:
  PACIBSP          ; re-execute replaced function entry
  B 0xfffffff009f6ddf8  ; jump to inst #2 of original function

Entry patch: replace PACIBSP at 0xfffffff009f6ddf4 with B pad.
"""
import struct

PAD_BASE = 0xfffffff00a1d4778
PAD_EXP24 = PAD_BASE + 0x30  # after exp21's 48 bytes
FN_ENTRY = 0xfffffff009f6ddf4
RESUME = FN_ENTRY + 4

def enc(x): return struct.pack('<I', x & 0xFFFFFFFF)
def b_imm(frm, to, bl=False):
    delta = to - frm; imm26 = (delta // 4) & 0x3FFFFFF
    return ((0b100101 if bl else 0b000101) << 26) | imm26
def bcond(frm, to, cond_bits):
    delta = to - frm; imm19 = (delta // 4) & 0x7FFFF
    return 0x54000000 | (imm19 << 5) | cond_bits

insts = []
v = PAD_EXP24
# CMP W0, #9  = SUBS WZR, W0, #9  sf=0 opc=11 S=1 shift=0 imm12=9 Rn=0 Rd=31
cmp_w0_9 = 0x71000000 | (9 << 10) | (0 << 5) | 31
insts.append(cmp_w0_9)
# B.NE prolog (6 insts from here = +0x18 bytes; current = v+4, target = v+0x1C)
va_bne_1 = v + 4
# compute later once we know prolog position
# For now: positions are 0..7 instructions
# [0] CMP W0,#9  [1] B.NE  [2] CMP X1,#1  [3] B.NE  [4] MOV X0,#0  [5] RET  [6] PACIBSP  [7] B back
# B.NE at [1] goes to [6] = +5 insts = +0x14 bytes from [1]
insts.append(bcond(v + 4, v + 4 + 5*4, 1))  # NE
# CMP X1, #1 = SUBS XZR, X1, #1 sf=1 imm12=1 Rn=1 Rd=31
cmp_x1_1 = 0xF1000000 | (1 << 10) | (1 << 5) | 31
insts.append(cmp_x1_1)
# B.NE prolog at [3] goes to [6] = +3 insts = +0xC
insts.append(bcond(v + 12, v + 12 + 3*4, 1))
# MOV X0, #0 (MOVZ X0, #0)
insts.append(0xd2800000)
# RET
insts.append(0xd65f03c0)
# PACIBSP (replaced)
insts.append(0xd503237f)
# B back to RESUME
insts.append(b_imm(v + 28, RESUME, bl=False))

tramp = b''.join(enc(i) for i in insts)
assert len(tramp) == 32

print(f"Trampoline @ 0x{PAD_EXP24:x} ({len(tramp)}B):")
for i, w in enumerate(insts):
    print(f"  0x{v + i*4:x}  {w:08x}  {enc(w).hex()}")

# Entry patch: B pad_exp24 (replaces PACIBSP = 7f2303d5)
patch = b_imm(FN_ENTRY, PAD_EXP24, bl=False)
print(f"\nEntry patch: {enc(patch).hex()}")

# Original pad bytes expected: 0x00s (part of our pad area which is zero-filled)
# But wait — the pad also contains exp21's trampoline from 0..47.
# PAD_EXP24 is at offset +0x30 = 48 bytes in. Need to check if bytes 48..80 are still zero.
# After exp21 trampoline (48B), bytes should be the next zero bytes.
# Original bootkc stock: pad is zeros. If we apply exp21 then exp24, pad[0..48] = exp21, pad[48..80] = exp24.
# For reproducibility, orig for exp24 patch is zeros at PAD_EXP24 location in bootkc.before-exp10 (stock pad).

orig_pad = b'\x00' * len(tramp)
print(f"\n---YAML entries---")
print(f"""  - name: exp24-os-reason-filter-trampoline
    va:    0x{PAD_EXP24:X}
    orig:  "{orig_pad.hex()}"
    new:   "{tramp.hex()}"

  - name: exp24-os-reason-create-hook
    va:    0x{FN_ENTRY:X}
    orig:  "7f2303d5"
    new:   "{enc(patch).hex()}"
""")
