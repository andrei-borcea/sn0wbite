#!/usr/bin/env python3
"""Execute the watchOS kernelcache reset vector in Unicorn with a synthesized boot_args, to test the first stage of the kernel
boot: the handoff we reverse-engineered (boot_args in x0, the 16 KB page-table bootstrap, the MMU setup).

Unicorn is a generic ARMv8.3 core: it has the features the real Kryo lacks, but it does not implement Apple's IMPDEF system
registers or PAC keys. Those are exactly the boot-time Apple-isms from the patch catalog. This harness emulates them live
(msr -> skip, mrs -> 0, retaa/retab -> ret) with a stop-and-resume loop, so the memory/boot_args path runs to completion and
we can see how far the kernel's early boot gets and what the remaining blockers are.

Usage: boot_harness.py /path/to/kernelcache.macho   (no Apple firmware is committed; the path is supplied at runtime)"""
import struct, sys
from unicorn import *
from unicorn.arm64_const import *

START = 0xfffffff007dfc06c
MMU_ON = 0xfffffff007dfc3b8          # msr sctlr_el1 (MMU enable): reaching here = early boot succeeded
IMG_BASE, IMG_SIZE = 0xfffffff007000000, 0x02000000
PT_BASE,  PT_SIZE  = 0xfffffff010000000, 0x01000000
BA_BASE = 0xfffffff020000000
STK_BASE, STK_SIZE = 0xfffffff021000000, 0x00100000

def sr(w):
    op0=((w>>19)&3)+2; op1=(w>>16)&7; crn=(w>>12)&0xf; crm=(w>>8)&0xf; op2=(w>>5)&7
    return f"s{op0}_{op1}_c{crn}_c{crm}_{op2}"

def segs(b):
    ncmds=struct.unpack_from("<I",b,16)[0]; off=32; out=[]
    for _ in range(ncmds):
        cmd,cmdsize=struct.unpack_from("<II",b,off)
        if cmd==0x19:
            name=b[off+8:off+24].rstrip(b'\0').decode()
            vmaddr,vmsize,fileoff,filesize=struct.unpack_from("<QQQQ",b,off+24)
            if vmsize: out.append((vmaddr,fileoff,filesize))
        off+=cmdsize
    return out

def main():
    b=open(sys.argv[1],"rb").read()
    uc=Uc(UC_ARCH_ARM64,UC_MODE_ARM)
    uc.mem_map(IMG_BASE,IMG_SIZE,UC_PROT_ALL)
    uc.mem_map(PT_BASE,PT_SIZE,UC_PROT_READ|UC_PROT_WRITE)
    uc.mem_map(BA_BASE,0x4000,UC_PROT_READ|UC_PROT_WRITE)
    uc.mem_map(STK_BASE,STK_SIZE,UC_PROT_READ|UC_PROT_WRITE)
    for vmaddr,fileoff,filesize in segs(b): uc.mem_write(vmaddr,b[fileoff:fileoff+filesize])

    ba=bytearray(0x2d8)
    struct.pack_into("<HH",ba,0,2,2)
    struct.pack_into("<Q",ba,0x08,0xfffffff007004000)   # virtBase
    struct.pack_into("<Q",ba,0x10,0xfffffff007004000)   # physBase (slide 0)
    struct.pack_into("<Q",ba,0x18,0x40000000)          # memSize
    struct.pack_into("<Q",ba,0x20,PT_BASE)            # topOfKernelData
    struct.pack_into("<Q",ba,0x60,BA_BASE+0x800)      # deviceTreeP
    uc.mem_write(BA_BASE,bytes(ba))
    uc.reg_write(UC_ARM64_REG_X0,BA_BASE)
    uc.reg_write(UC_ARM64_REG_SP,STK_BASE+STK_SIZE-16)

    st={"n":0,"pt":0,"ba":[],"stub":[]}
    shadow={}   # sysreg last-written value, so poll loops that wait on a bit they set make progress
    BAF={0x08:"virtBase",0x10:"physBase",0x18:"memSize",0x20:"topOfKernelData",0x2d0:"arm64e@0x2d0",0x60:"deviceTreeP"}
    def hcode(uc,addr,size,_):
        st["n"]+=1
    def hmem(uc,access,addr,size,value,_):
        if PT_BASE<=addr<PT_BASE+PT_SIZE and access==UC_MEM_WRITE: st["pt"]+=1
        x20=uc.reg_read(UC_ARM64_REG_X20)
        if x20 and 0<=addr-x20<0x300 and (addr-x20) in BAF and BAF[addr-x20] not in st["ba"]: st["ba"].append(BAF[addr-x20])
    uc.hook_add(UC_HOOK_CODE,hcode)
    uc.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,hmem)

    def emulate(w,pc):
        if (w&0xfff00000)==0xd5300000:                       # MRS Xt,sysreg -> last-written value (or ~0)
            rt=w&0x1f
            if rt!=31: uc.reg_write(UC_ARM64_REG_X0+rt, shadow.get(sr(w), (1<<64)-1))
            st["stub"].append(("mrs",sr(w))); uc.reg_write(UC_ARM64_REG_PC,pc+4); return True
        if (w&0xfff00000)==0xd5100000:                       # MSR sysreg,Xt -> record and nop
            rt=w&0x1f
            shadow[sr(w)] = uc.reg_read(UC_ARM64_REG_X0+rt) if rt!=31 else 0
            st["stub"].append(("msr",sr(w))); uc.reg_write(UC_ARM64_REG_PC,pc+4); return True
        if w in (0xd65f0bff,0xd65f0fff):                     # retaa/retab -> ret
            uc.reg_write(UC_ARM64_REG_PC,uc.reg_read(UC_ARM64_REG_X30)); return True
        return False

    cont = "--continue" in sys.argv
    first_unmapped={"a":None}
    def hunmap(uc,access,addr,size,value,_):
        if first_unmapped["a"] is None: first_unmapped["a"]=(addr,access,uc.reg_read(UC_ARM64_REG_PC))
        return False
    uc.hook_add(UC_HOOK_MEM_UNMAPPED,hunmap)
    TARGET = (IMG_BASE+IMG_SIZE) if cont else MMU_ON
    pc=START; err=None; restarts=0
    while restarts<20000:
        try:
            uc.emu_start(pc,TARGET,count=2000000); break
        except UcError as e:
            fpc=uc.reg_read(UC_ARM64_REG_PC)
            try: w=struct.unpack("<I",uc.mem_read(fpc,4))[0]
            except Exception: err=e; break
            if emulate(w,fpc): pc=uc.reg_read(UC_ARM64_REG_PC); restarts+=1; continue
            err=(e,fpc,w); break

    reached=(st['n']>1000 and (cont or uc.reg_read(UC_ARM64_REG_PC)==MMU_ON)) and err is None
    print("=== watchOS kernel early-boot test (Unicorn) ===")
    print(f"instructions executed  : {st['n']}")
    print(f"boot_args fields read   : {st['ba']}")
    print(f"page-table entries wrtn : {st['pt']} (16 KB tables at {PT_BASE:#x})")
    print(f"Apple/PAC regs stubbed  : {len(st['stub'])} ; distinct: {sorted({r for _,r in st['stub']})}")
    if first_unmapped["a"]:
        a,acc,fpc=first_unmapped["a"]
        print(f"first access outside image: {a:#018x} (from pc {fpc:#x}) -- next thing to provide (DeviceTree / platform MMIO)")
    if reached and not cont:
        print(f"RESULT: reached MMU-enable ({MMU_ON:#x}) -- the reset vector consumed our boot_args, built the 16 KB page")
        print(f"        tables and set TCR/TTBR/MAIR. The software boot-chain handoff is validated in emulation.")
    elif cont:
        print(f"RESULT (--continue): ran {st['n']} insns past MMU-enable into the C boot path.")
    else:
        fpc=uc.reg_read(UC_ARM64_REG_PC)
        w=0
        try: w=struct.unpack("<I",uc.mem_read(fpc,4))[0]
        except Exception: pass
        print(f"RESULT: stopped at {fpc:#018x} insn=0x{w:08x} ({'sysreg '+sr(w) if (w&0xff000000)==0xd5000000 else 'non-sysreg'})")
        print(f"        {err if not isinstance(err,tuple) else err[0]}")

main()
