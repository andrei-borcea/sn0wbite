#!/usr/bin/env python3
"""Disassemble an arbitrary VA range from the Watch7,11 bootkc.
Finds the __TEXT_EXEC file offset automatically."""
import struct
import sys
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN

TEXT_EXEC_FOFF = 0xE28000
TEXT_EXEC_VA = 0xFFFFFFF007E2C000
TEXT_EXEC_SIZE = 0x23B4000


def main():
    bk = sys.argv[1]
    start_va = int(sys.argv[2], 0)
    end_va = int(sys.argv[3], 0)
    size = end_va - start_va
    assert start_va >= TEXT_EXEC_VA and end_va <= TEXT_EXEC_VA + TEXT_EXEC_SIZE
    foff = TEXT_EXEC_FOFF + (start_va - TEXT_EXEC_VA)

    with open(bk, "rb") as f:
        f.seek(foff)
        code = f.read(size)

    md = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    md.detail = False
    for ins in md.disasm(code, start_va):
        hexb = code[ins.address - start_va:ins.address - start_va + ins.size].hex()
        print(f"  0x{ins.address:x}  {hexb:<8}  {ins.mnemonic:<8} {ins.op_str}")


if __name__ == "__main__":
    main()
