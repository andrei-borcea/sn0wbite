#!/usr/bin/env python3
# sn0wbite: rebuild an address->name map for a STRIPPED XNU kernelcache (Mach-O
# arm64) from __func__ strings. A C-identifier string in __cstring referenced by
# exactly one function (via adrp+add) names that function. Usage:
#   python3 symbolicate.py <kernelcache.macho> <out.json>
# Needs capstone. Produced ~6278 confident names on watchOS 6.3.1 (Watch5,4).
# No Apple binary is committed; this only derives analysis from an artifact you supply.

import sys, struct, re, bisect, json
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM

path = sys.argv[1]
data = open(path, 'rb').read()

magic = struct.unpack_from('<I', data, 0)[0]
assert magic == 0xFEEDFACF, f"not mach-o 64: {magic:#x}"
ncmds = struct.unpack_from('<I', data, 16)[0]
off = 32
segs = []           # (segname, vmaddr, vmsize, fileoff, filesize)
sects = []          # (segname, sectname, addr, size, offset)
func_starts_off = func_starts_size = 0
text_base = None
for _ in range(ncmds):
    cmd, cmdsize = struct.unpack_from('<II', data, off)
    if cmd == 0x19:  # LC_SEGMENT_64
        segname = data[off+8:off+24].split(b'\0')[0].decode()
        vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off+24)
        nsects = struct.unpack_from('<I', data, off+64)[0]
        segs.append((segname, vmaddr, vmsize, fileoff, filesize))
        so = off + 72
        for _ in range(nsects):
            sn = data[so:so+16].split(b'\0')[0].decode()
            sg = data[so+16:so+32].split(b'\0')[0].decode()
            addr, size = struct.unpack_from('<QQ', data, so+32)
            soff = struct.unpack_from('<I', data, so+48)[0]
            sects.append((sg, sn, addr, size, soff))
            so += 80
    elif cmd == 0x26:  # LC_FUNCTION_STARTS
        func_starts_off, func_starts_size = struct.unpack_from('<II', data, off+8)
    off += cmdsize

# text base = lowest segment vmaddr with a nonzero file mapping (the __TEXT seg)
text_base = min(v for (_, v, _, fo, fs) in segs if fs > 0)

# decode LC_FUNCTION_STARTS (uleb128 deltas from text_base)
fstarts = []
p = func_starts_off
end = func_starts_off + func_starts_size
addr = text_base
while p < end:
    shift = 0; val = 0
    while True:
        b = data[p]; p += 1
        val |= (b & 0x7f) << shift
        if not (b & 0x80): break
        shift += 7
    if val == 0: break
    addr += val
    fstarts.append(addr)
fstarts.sort()
print(f"segments={len(segs)} sections={len(sects)} func_starts={len(fstarts)} text_base={text_base:#x}")

# cstring sections: build (lo,hi,offset) list; identifier strings only
ident = re.compile(rb'^[A-Za-z_][A-Za-z0-9_]{2,63}$')
str_at = {}   # vaddr -> name (identifier strings only)
cstr_ranges = []
for (sg, sn, a, sz, soff) in sects:
    if sn == '__cstring':
        cstr_ranges.append((a, a+sz))
        blob = data[soff:soff+sz]
        i = 0
        base = a
        while i < sz:
            j = blob.find(b'\0', i)
            if j < 0: break
            s = blob[i:j]
            if ident.match(s):
                str_at[base+i] = s.decode()
            i = j+1
def in_cstr(x):
    for lo,hi in cstr_ranges:
        if lo <= x < hi: return True
    return False
print(f"identifier cstrings={len(str_at)}")

md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
md.detail = False  # fast path; we parse mnemonic/op text ourselves
md.skipdata = True  # Apple IMPDEF opcodes would otherwise stop the stream

def containing_func(a):
    i = bisect.bisect_right(fstarts, a) - 1
    return fstarts[i] if i >= 0 else None

# string vaddr -> set of containing functions that reference it
ref_funcs = {}
adrp_re = re.compile(r'adrp (x\d+), #?(0x[0-9a-f]+)')
add_re  = re.compile(r'add (x\d+), (x\d+), #(0x[0-9a-f]+)')
text_sects = [(a,sz,soff) for (sg,sn,a,sz,soff) in sects if sn=='__text']
print("text sects:", [(hex(a),hex(sz)) for (a,sz,_) in text_sects])
ninsn=0
for (a, sz, soff) in text_sects:
    code = data[soff:soff+sz]
    reg_page = {}
    for insn in md.disasm(code, a):
        ninsn += 1
        m = insn.mnemonic
        if m == 'adrp':
            mm = adrp_re.match(m+' '+insn.op_str)
            if mm: reg_page[mm.group(1)] = int(mm.group(2),16)
        elif m == 'add':
            mm = add_re.match(m+' '+insn.op_str)
            if mm and mm.group(2) in reg_page:
                tgt = reg_page[mm.group(2)] + int(mm.group(3),16)
                del reg_page[mm.group(2)]           # consume the adrp pairing
                if tgt in str_at:
                    f = containing_func(insn.address)
                    if f is not None:
                        ref_funcs.setdefault(tgt, set()).add(f)
        elif m not in ('nop','paciasp','pacibsp','autiasp','autibsp'):
            # a reg written by anything else invalidates its adrp page; cheaply
            # drop the destination reg if it's the first operand "xN,"
            op = insn.op_str
            if op[:1]=='x':
                r = op.split(',',1)[0]
                reg_page.pop(r, None)
print(f"disassembled insns={ninsn}  referenced identifier-strings={len(ref_funcs)}")

# func -> candidate names (strings it uniquely references)
func_name = {}
for sv, funcs in ref_funcs.items():
    if len(funcs) == 1:
        f = next(iter(funcs))
        func_name.setdefault(f, set()).add(str_at[sv])

# Resolve: if a function has exactly one unique identifier string, that's its name.
resolved = {}
for f, names in func_name.items():
    if len(names) == 1:
        resolved[f] = next(iter(names))
print(f"confidently named functions={len(resolved)}")

out = sys.argv[2] if len(sys.argv) > 2 else '/tmp/symmap.json'
json.dump({f"{k:#x}": v for k,v in sorted(resolved.items())}, open(out,'w'), indent=0)
print(f"wrote {out}")
