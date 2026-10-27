#!/usr/bin/env python3
# Android boot image extractor for the sn0wbite mapping engine.
#
# Parses Google's bootimg header v0..v4 and vendor_boot v3/v4 (magic VNDRBOOT),
# emits a profile fragment plus split kernel/ramdisk/dtb payloads under a work
# directory for downstream extractors. No external dependencies.
#
# Reference: Android source system/tools/mkbootimg/include/bootimg/bootimg.h.
import argparse
import pathlib
import struct
import sys

BOOT_MAGIC = b"ANDROID!"
VENDOR_MAGIC = b"VNDRBOOT"


def _hex(n: int) -> str:
    return f"0x{n:x}"


def _round_up(n: int, page: int) -> int:
    return ((n + page - 1) // page) * page


def _cstr(b: bytes) -> str:
    i = b.find(b"\x00")
    return (b if i < 0 else b[:i]).decode("utf-8", errors="replace")


class BootImgError(ValueError):
    pass


def parse_bootimg_header(data: bytes) -> dict:
    """Return a dict describing the header. Supports v0..v4 and vendor_boot v3/v4."""
    if len(data) < 16:
        raise BootImgError("file too small")
    magic = data[:8]
    if magic == BOOT_MAGIC:
        # Read header_version at offset 10*4 = 40 for v0..v2, or at offset 36 for v3+.
        # Both layouts begin with magic + kernel_size (u32); use the common field to
        # branch: offset 36 is header_version for v3+, 'tags_addr' for v0..v2.
        # The reliable discriminator is: v3+ has header_size at a fixed offset and
        # header_version in the first 40 bytes. Try the modern layout first.
        hv = struct.unpack_from("<I", data, 40)[0]
        if hv >= 3:
            return _parse_v3plus(data)
        return _parse_v0_v2(data)
    if magic == VENDOR_MAGIC:
        return _parse_vendor_v3plus(data)
    raise BootImgError(f"unknown magic: {magic!r}")


def _parse_v0_v2(data: bytes) -> dict:
    # struct boot_img_hdr_v0..v2 (packed, little-endian)
    fmt = "<8s" "10I" "16s" "512s" "8I" "1024s"
    size = struct.calcsize(fmt)
    if len(data) < size:
        raise BootImgError("v0..v2 header truncated")
    (magic, kernel_size, kernel_addr, ramdisk_size, ramdisk_addr,
     second_size, second_addr, tags_addr, page_size, header_version,
     os_version, name, cmdline, *_id, extra_cmdline) = struct.unpack_from(fmt, data, 0)
    # id[8] came out as 8 individual ints
    hdr = {
        "magic": "ANDROID!",
        "header_version": header_version,
        "page_size": page_size,
        "kernel_size": kernel_size,
        "kernel_addr": kernel_addr,
        "ramdisk_size": ramdisk_size,
        "ramdisk_addr": ramdisk_addr,
        "second_size": second_size,
        "tags_addr": tags_addr,
        "os_version": os_version,
        "name": _cstr(name),
        "cmdline": _cstr(cmdline),
        "extra_cmdline": _cstr(extra_cmdline),
        "dtb_size": 0,
        "recovery_dtbo_size": 0,
    }
    # v1 adds recovery_dtbo_{size,offset} + header_size at offset `size`.
    off = size
    if header_version >= 1 and off + 16 <= len(data):
        rec_size, rec_off, hdr_size = struct.unpack_from("<IQI", data, off)
        hdr["recovery_dtbo_size"] = rec_size
        hdr["header_size"] = hdr_size
        off += 16
    # v2 adds dtb_size + dtb_addr.
    if header_version >= 2 and off + 12 <= len(data):
        dtb_size, dtb_addr = struct.unpack_from("<IQ", data, off)
        hdr["dtb_size"] = dtb_size
        hdr["dtb_addr"] = dtb_addr
    return hdr


def _parse_v3plus(data: bytes) -> dict:
    # struct boot_img_hdr_v3 (page_size fixed at 4096; no DTB inside boot.img)
    #   magic[8], kernel_size, ramdisk_size, os_version, header_size,
    #   reserved[4], header_version, cmdline[1536]
    fmt_v3 = "<8s" "4I" "4I" "I" "1536s"
    size_v3 = struct.calcsize(fmt_v3)
    if len(data) < size_v3:
        raise BootImgError("v3+ header truncated")
    unpacked = struct.unpack_from(fmt_v3, data, 0)
    (_magic, kernel_size, ramdisk_size, os_version, header_size,
     _r0, _r1, _r2, _r3, header_version, cmdline) = unpacked
    hdr = {
        "magic": "ANDROID!",
        "header_version": header_version,
        "page_size": 4096,
        "kernel_size": kernel_size,
        "ramdisk_size": ramdisk_size,
        "os_version": os_version,
        "header_size": header_size,
        "cmdline": _cstr(cmdline),
        "extra_cmdline": "",
        "dtb_size": 0,
    }
    if header_version >= 4 and len(data) >= size_v3 + 4:
        (sig_size,) = struct.unpack_from("<I", data, size_v3)
        hdr["signature_size"] = sig_size
    return hdr


def _parse_vendor_v3plus(data: bytes) -> dict:
    # vendor_boot v3/v4 header: magic, header_version, page_size, kernel_addr,
    # ramdisk_addr, vendor_ramdisk_size, cmdline[2048], tags_addr, name[16],
    # header_size, dtb_size, dtb_addr
    fmt = "<8s" "5I" "2048s" "I" "16s" "2I" "Q"
    size = struct.calcsize(fmt)
    if len(data) < size:
        raise BootImgError("vendor_boot header truncated")
    (_magic, header_version, page_size, kernel_addr, ramdisk_addr,
     vrd_size, cmdline, tags_addr, name, header_size, dtb_size, dtb_addr
     ) = struct.unpack_from(fmt, data, 0)
    return {
        "magic": "VNDRBOOT",
        "header_version": header_version,
        "page_size": page_size,
        "kernel_addr": kernel_addr,
        "ramdisk_addr": ramdisk_addr,
        "vendor_ramdisk_size": vrd_size,
        "cmdline": _cstr(cmdline),
        "name": _cstr(name),
        "header_size": header_size,
        "dtb_size": dtb_size,
        "dtb_addr": dtb_addr,
        "kernel_size": 0,
        "ramdisk_size": 0,
    }


def split_payloads(data: bytes, hdr: dict, workdir: pathlib.Path) -> dict:
    """Write kernel/ramdisk/dtb payloads to workdir. Returns their file paths."""
    workdir.mkdir(parents=True, exist_ok=True)
    page = hdr.get("page_size", 4096)
    out: dict[str, pathlib.Path] = {}

    if hdr["magic"] == BOOT_MAGIC.decode():
        off = page  # header occupies one page
        for name, size_field in (("kernel", "kernel_size"),
                                  ("ramdisk", "ramdisk_size"),
                                  ("second", "second_size"),
                                  ("recovery_dtbo", "recovery_dtbo_size"),
                                  ("dtb", "dtb_size")):
            sz = hdr.get(size_field, 0)
            if not sz:
                continue
            p = workdir / name
            p.write_bytes(data[off:off + sz])
            out[name] = p
            off = _round_up(off + sz, page)
    elif hdr["magic"] == VENDOR_MAGIC.decode():
        # vendor_boot layout: header page, then vendor_ramdisk, then dtb
        off = _round_up(struct.calcsize("<8s5I2048sI16s2IQ"), page)
        vrd = hdr.get("vendor_ramdisk_size", 0)
        if vrd:
            p = workdir / "vendor_ramdisk"
            p.write_bytes(data[off:off + vrd])
            out["vendor_ramdisk"] = p
            off = _round_up(off + vrd, page)
        dtb = hdr.get("dtb_size", 0)
        if dtb:
            p = workdir / "dtb"
            p.write_bytes(data[off:off + dtb])
            out["dtb"] = p
    return out


def to_profile_fragment(hdr: dict, source: str) -> tuple[dict, dict]:
    """Return (profile_fragment, evidence_fragment).

    kernel_physbase is reported when the header provides an absolute address
    (v0..v2 and vendor_boot). v3/v4 BOOT headers don't carry it (iBoot / ABL
    picks it at load time), so the field is omitted from the fragment.
    """
    frag: dict = {"boot": {
        "bootimg_header_version": hdr["header_version"],
        "page_size": hdr["page_size"],
        "kernel_cmdline": hdr.get("cmdline", "") +
                          (" " + hdr["extra_cmdline"] if hdr.get("extra_cmdline") else ""),
        "kernel_size": hdr.get("kernel_size", 0),
        "ramdisk_size": hdr.get("ramdisk_size", hdr.get("vendor_ramdisk_size", 0)),
        "dtb_size": hdr.get("dtb_size", 0),
    }}
    if hdr.get("kernel_addr"):
        frag["boot"]["kernel_physbase"] = _hex(hdr["kernel_addr"])
    frag["boot"]["kernel_cmdline"] = frag["boot"]["kernel_cmdline"].strip()

    evidence = {}
    for field in ("boot.bootimg_header_version", "boot.page_size",
                  "boot.kernel_cmdline", "boot.kernel_size",
                  "boot.ramdisk_size", "boot.dtb_size",
                  "boot.kernel_physbase"):
        evidence[field] = [{"extractor": "extractors.bootimg", "source": source}]
    return frag, evidence


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Parse an Android boot/vendor_boot image.")
    ap.add_argument("image", help="path to boot.img / vendor_boot.img")
    ap.add_argument("--workdir", help="write kernel/ramdisk/dtb payloads here")
    ap.add_argument("--json", action="store_true", help="dump header as JSON")
    args = ap.parse_args(argv)

    data = pathlib.Path(args.image).read_bytes()
    hdr = parse_bootimg_header(data)
    if args.workdir:
        split_payloads(data, hdr, pathlib.Path(args.workdir))
    if args.json:
        import json
        print(json.dumps(hdr, indent=2, default=str))
    else:
        for k, v in hdr.items():
            print(f"  {k}: {v!r}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
