#!/usr/bin/env python3
"""Patch the watchOS arm64e kernelcache so it can run on a core without PAC / Apple IMPDEF registers (the boot-chain port patch).

It censuses and rewrites, across every code-bearing segment, the pointer-authentication instructions (the ~v8.3 PAC family the
Kryo lacks) and reports the Apple IMPDEF system-register sites (which need a runtime stub or trampoline, not a single-word patch).

  kernel_patch.py <kernelcache.macho> [--apply out.macho]   census; with --apply also writes the PAC-stripped kernelcache

PAC rewrites (identity semantics, i.e. "unsigned pointers"):
  pacia/pacib/pacda/pacdb + z-forms, autia/.., xpaci/xpacd, pacga  -> nop
  braa/brab/braaz/brabz                                            -> br Xn
  blraa/blrab/blraaz/blrabz                                        -> blr Xn
  retaa/retab                                                      -> ret
  eretaa/eretab                                                    -> eret
No Apple firmware is committed; the kernelcache path is supplied at runtime."""
import array, struct, sys

CODE_SEGS = {"__TEXT","__TEXT_EXEC","__PPLTEXT","__PPLTRAMP","__KLD"}  # PPL/KLD are executable at runtime though Mach-O prot is rw
NOP=0xd503201f

def segments(b):
    ncmds=struct.unpack_from("<I",b,16)[0]; off=32; out=[]
    for _ in range(ncmds):
        cmd,cmdsize=struct.unpack_from("<II",b,off)
        if cmd==0x19:
            name=b[off+8:off+24].rstrip(b'\0').decode()
            vmaddr,vmsize,fileoff,filesize=struct.unpack_from("<QQQQ",b,off+24)
            if vmsize: out.append((name,vmaddr,fileoff,filesize))
        off+=cmdsize
    return out

def classify(w):
    """Return (category, replacement) for a PAC instruction, or (None,None)."""
    if (w & 0xffffe000)==0xdac10000 or (w & 0xfffffbe0)==0xdac143e0 or (w & 0xffe0fc00)==0x9ac03000:
        return ("pac_compute", NOP)                         # pac*/aut*/xpac*/pacga
    if w in (0xd65f0bff,0xd65f0fff): return ("ret_auth", 0xd65f03c0)   # retaa/retab -> ret
    if w in (0xd69f0bff,0xd69f0fff): return ("eret_auth", 0xd69f03e0)  # eretaa/eretab -> eret
    rn=(w>>5)&0x1f; t=w & 0xffe0fc00
    if t in (0xd71f0800,0xd71f0c00) or w in (0xd61f081f,0xd61f0c1f): return ("braa", 0xd61f0000|(rn<<5))
    if t in (0xd73f0800,0xd73f0c00) or w in (0xd63f081f,0xd63f0c1f): return ("blraa", 0xd63f0000|(rn<<5))
    return (None,None)

def apple_sysreg(w):
    """Return the sysreg name if w is an MSR/MRS to an Apple IMPDEF register (crn=15) or a PAC key (S3_0_C2_C1/2/3)."""
    if (w & 0xffd00000)!=0xd5100000: return None             # not MSR/MRS (register)
    op0=((w>>19)&3)+2; op1=(w>>16)&7; crn=(w>>12)&0xf; crm=(w>>8)&0xf; op2=(w>>5)&7
    if op0==3 and (crn==15 or (crn==2 and op1==0 and crm in (1,2,3))):
        return f"s3_{op1}_c{crn}_c{crm}_{op2}"
    return None

def main():
    b=open(sys.argv[1],"rb").read()
    buf=bytearray(b)
    cats={}; apple={}
    for name,vmaddr,fileoff,filesize in segments(b):
        if name not in CODE_SEGS or filesize<4: continue
        a=array.array("I"); a.frombytes(bytes(b[fileoff:fileoff+filesize]))
        changed=False
        for i,w in enumerate(a):
            cat,rep=classify(w)
            if cat:
                cats[cat]=cats.get(cat,0)+1
                if rep!=w: a[i]=rep; changed=True
            sr=apple_sysreg(w)
            if sr: apple[sr]=apple.get(sr,0)+1
        if changed: buf[fileoff:fileoff+filesize]=a.tobytes()
    total=sum(cats.values())
    print("=== watchOS kernelcache boot-chain patch census ===")
    print(f"PAC instructions (rewritten): {total}")
    for k in sorted(cats): print(f"  {k:14} {cats[k]}")
    print(f"\nApple IMPDEF / PAC-key sysreg sites (need runtime stub or trampoline): {sum(apple.values())} ({len(apple)} distinct)")
    for k in sorted(apple, key=lambda x:-apple[x])[:24]: print(f"  {k:16} x{apple[k]}")
    if "--apply" in sys.argv:
        out=sys.argv[sys.argv.index("--apply")+1]
        open(out,"wb").write(bytes(buf))
        print(f"\nwrote PAC-stripped kernelcache: {out} ({len(buf)} bytes)")

main()
