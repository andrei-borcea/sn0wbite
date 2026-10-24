#!/usr/bin/env python3
"""Accurate census of the pointer-authentication (PAC) and Apple IMPDEF system-register sites in the watchOS arm64e kernelcache,
using Capstone. This is the scope of the boot-chain port patch: every one of these must be rewritten or stubbed to run the
kernel on a core without FEAT_PAuth / Apple registers.

  pac_census.py <kernelcache.macho>
No Apple firmware is committed; the path is supplied at runtime."""
import struct, sys, collections
import capstone

CODE_SEGS={"__TEXT","__TEXT_EXEC","__PPLTEXT","__PPLTRAMP","__KLD"}

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

def apple_sr(w):
    # MSR/MRS (register): 0xd51/0xd53 .. ; op0 = 2 + bit19 (ONE bit), op1[18:16], CRn[15:12], CRm[11:8], op2[7:5]
    if (w>>24)!=0xd5 or ((w>>20)&0xf) not in (0x1,0x3): return None
    op0=2+((w>>19)&1); op1=(w>>16)&7; crn=(w>>12)&0xf; crm=(w>>8)&0xf; op2=(w>>5)&7
    if op0==3 and (crn==15 or (crn==2 and op1==0 and crm in (1,2,3,4))):
        return f"s3_{op1}_c{crn}_c{crm}_{op2}"
    return None

def main():
    b=open(sys.argv[1],"rb").read()
    md=capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_LITTLE_ENDIAN); md.skipdata=True
    cats=collections.Counter(); apple=collections.Counter(); ninsn=0
    for name,vmaddr,fileoff,filesize in segments(b):
        if name not in CODE_SEGS or filesize<4: continue
        code=b[fileoff:fileoff+filesize]
        for ins in md.disasm(code, vmaddr):
            ninsn+=1
            m=ins.mnemonic
            if m.startswith(("pac","aut","xpac")): cats["pac_compute"]+=1
            elif m in ("braa","brab","braaz","brabz"): cats["braa_branch"]+=1
            elif m in ("blraa","blrab","blraaz","blrabz"): cats["blraa_call"]+=1
            elif m in ("retaa","retab"): cats["ret_auth"]+=1
            elif m in ("eretaa","eretab"): cats["eret_auth"]+=1
            elif m in ("ldraa","ldrab"): cats["ldraa_load"]+=1
            elif m in ("mrs","msr"):
                w=struct.unpack_from("<I",code,ins.address-vmaddr)[0]
                sr=apple_sr(w)
                if sr: apple[sr]+=1
    print("=== PAC / Apple-register census (Capstone) ===")
    print(f"instructions scanned: {ninsn}")
    tot=sum(cats.values())
    print(f"PAC sites total: {tot}")
    for k in sorted(cats,key=lambda x:-cats[x]): print(f"  {k:14} {cats[k]}")
    print(f"\nApple IMPDEF / PAC-key sysreg sites: {sum(apple.values())} ({len(apple)} distinct)")
    for k in sorted(apple,key=lambda x:-apple[x])[:30]: print(f"  {k:18} {apple[k]}")

main()
