#!/usr/bin/env python3
"""pack-bootimg: wrap stage1.bin into an Android boot.img v4 that the OPW2
Qualcomm bootloader will accept at slot B.

Usage:
  pack-bootimg.py stage1.bin -o stage1-slot-b.img

Reads the stock boot image at
backups/oneplus-watch-2/2026-10-21-full-system-backup/boot.img to copy its
`os_version`, cmdline and other header fields verbatim — the only differences
from stock are kernel payload and kernel_size. Ramdisk is left empty (OPW2
puts the ramdisk in init_boot, which we don't touch); dtb is 0 (v4 header
has no dtb slot; dtb lives in vendor_boot).

Boot img v4 header (Android source: system/tools/mkbootimg/include/bootimg):
   magic[8]           "ANDROID!"
   kernel_size        uint32
   ramdisk_size       uint32
   os_version         uint32
   header_size        uint32  (1580 for v3, 1584 for v4)
   reserved[4]        uint32[4]
   header_version     uint32  (3 or 4)
   cmdline[1536]      char[1536]
   signature_size     uint32  (v4 only; 0 for unsigned)
"""
import argparse
import pathlib
import struct
import sys

PAGE = 4096
HEADER_V3_SIZE = 1580
HEADER_V4_SIZE = 1584
MAGIC = b"ANDROID!"


def read_stock_header(stock_path: pathlib.Path) -> dict:
    data = stock_path.read_bytes()
    assert data[:8] == MAGIC, "stock boot.img magic mismatch"
    # v3+ layout
    fmt_v3 = "<8s" "4I" "4I" "I" "1536s"
    (_m, k_sz, r_sz, os_ver, hdr_sz,
     r0, r1, r2, r3, hv, cmdline) = struct.unpack_from(fmt_v3, data, 0)
    hdr = {
        "kernel_size": k_sz,
        "ramdisk_size": r_sz,
        "os_version": os_ver,
        "header_size": hdr_sz,
        "reserved": (r0, r1, r2, r3),
        "header_version": hv,
        "cmdline": cmdline,
    }
    if hv >= 4:
        hdr["signature_size"] = struct.unpack_from("<I", data, HEADER_V3_SIZE)[0]
    return hdr


def pack(kernel: bytes, stock_hdr: dict) -> bytes:
    hv = stock_hdr["header_version"]
    hdr_sz = HEADER_V4_SIZE if hv >= 4 else HEADER_V3_SIZE
    body = struct.pack(
        "<8s" "4I" "4I" "I" "1536s",
        MAGIC,
        len(kernel),                    # kernel_size
        0,                              # ramdisk_size (empty)
        stock_hdr["os_version"],
        hdr_sz,
        *stock_hdr["reserved"],
        hv,
        stock_hdr["cmdline"],
    )
    if hv >= 4:
        body += struct.pack("<I", 0)    # signature_size = 0 (unsigned)
    # pad header to one page
    body += b"\x00" * (PAGE - len(body))
    # kernel pages
    body += kernel
    body += b"\x00" * ((-len(kernel)) & (PAGE - 1))
    return body


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("kernel", help="stage1.bin (Linux arm64 Image-shaped)")
    ap.add_argument("-o", "--out", required=True)
    ap.add_argument("--stock-boot",
                    default="backups/oneplus-watch-2/"
                            "2026-10-21-full-system-backup/boot.img",
                    help="stock boot.img to copy header fields from")
    args = ap.parse_args(argv)

    repo = pathlib.Path(__file__).resolve().parents[3]
    stock = repo / args.stock_boot if not pathlib.Path(args.stock_boot).is_absolute() \
            else pathlib.Path(args.stock_boot)
    if not stock.is_file():
        sys.stderr.write(f"stock boot.img not found: {stock}\n")
        return 2

    stock_hdr = read_stock_header(stock)
    kernel = pathlib.Path(args.kernel).read_bytes()
    assert kernel[:4] == bytes.fromhex("10000014"), \
        "kernel does not start with 'b #0x40' — not a Linux arm64 Image?"

    img = pack(kernel, stock_hdr)
    out = pathlib.Path(args.out)
    out.write_bytes(img)
    print(f"wrote {out} ({len(img)} bytes)")
    print(f"  header_version   : {stock_hdr['header_version']}")
    print(f"  os_version       : 0x{stock_hdr['os_version']:08x}")
    print(f"  kernel_size      : {len(kernel)}")
    print(f"  cmdline (first)  : "
          f"{stock_hdr['cmdline'].split(bytes([0]),1)[0].decode(errors='replace')[:80]}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
