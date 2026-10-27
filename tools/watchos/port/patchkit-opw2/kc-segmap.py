#!/usr/bin/env python3
"""kc-segmap: inspect a kernelcache Mach-O and print the __TEXT vmaddr + fileoff
you plug into a patchkit-opw2 manifest.

patchkit.py translates `va` to a file offset with
    file_off = file_text_off + (va - va_base)
so va_base must be the __TEXT segment's vmaddr and file_text_off its fileoff
for VA-based patches to land correctly. Mach-O layouts vary (image_base !=
__TEXT.vmaddr in some kernelcaches), hence this helper rather than hardcoding.

Usage:
  kc-segmap.py <kernelcache.macho>
  kc-segmap.py <kernelcache.macho> --manifest manifest-v1.yaml --rewrite
"""
import argparse
import pathlib
import struct
import sys

LC_SEGMENT_64 = 0x19


def segments(b: bytes):
    magic, _cpu, _sub, _ftype, ncmds, _szcmds, _flg, _res = struct.unpack_from(
        "<IiIIIIII", b, 0)
    if magic not in (0xFEEDFACF, 0xCFFAEDFE):
        raise ValueError(f"not a 64-bit Mach-O (magic {magic:#x})")
    off = 32
    out = []
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<II", b, off)
        if cmd == LC_SEGMENT_64:
            name = b[off + 8:off + 24].rstrip(b"\x00").decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from(
                "<QQQQ", b, off + 24)
            if vmsize:
                out.append((name, vmaddr, vmsize, fileoff, filesize))
        off += cmdsize
    return out


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("kernelcache")
    ap.add_argument("--manifest", help="rewrite va_base/file_text_off in this manifest")
    ap.add_argument("--rewrite", action="store_true",
                    help="actually modify the manifest file (default: dry-run)")
    args = ap.parse_args(argv)

    data = pathlib.Path(args.kernelcache).read_bytes()
    segs = segments(data)

    text = next((s for s in segs if s[0] == "__TEXT"), None)
    if text is None:
        print("ERROR: no __TEXT segment", file=sys.stderr)
        return 2
    name, vmaddr, vmsize, fileoff, filesize = text

    print(f"segments ({len(segs)}):")
    for n, va, vs, fo, fs in segs:
        print(f"  {n:<16}  vm 0x{va:016x}+0x{vs:08x}  file 0x{fo:08x}+0x{fs:08x}")
    print()
    print("plug into manifest:")
    print(f"  va_base:       0x{vmaddr:016X}")
    print(f"  file_text_off: 0x{fileoff:X}")

    if args.manifest:
        p = pathlib.Path(args.manifest)
        txt = p.read_text()
        new_txt = _rewrite(txt, vmaddr, fileoff)
        if txt == new_txt:
            print("\n(manifest already matches; no change)")
        elif args.rewrite:
            p.write_text(new_txt)
            print(f"\nrewrote {p}")
        else:
            print(f"\n(dry-run; pass --rewrite to update {p})")
    return 0


def _rewrite(txt: str, vmaddr: int, fileoff: int) -> str:
    import re
    lines = txt.splitlines(keepends=True)
    out = []
    for L in lines:
        m = re.match(r"^(va_base:\s+)(\S+)(.*)$", L)
        if m:
            out.append(f"{m.group(1)}0x{vmaddr:016X}{m.group(3)}\n")
            continue
        m = re.match(r"^(file_text_off:\s+)(\S+)(.*)$", L)
        if m:
            out.append(f"{m.group(1)}0x{fileoff:X}{m.group(3)}\n")
            continue
        out.append(L)
    return "".join(out)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
