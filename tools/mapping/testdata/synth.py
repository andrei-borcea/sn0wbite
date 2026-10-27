#!/usr/bin/env python3
"""Build a tiny synthetic Android boot image (header v2 with embedded DTB) for
the mapping engine's smoke test. No external deps.

Writes <outdir>/boot_a.img. The embedded FDT describes a GIC-v3-based device
with 2 cores, 1 GiB DRAM at 0x80000000 and a ramoops reserved region — enough
to exercise every extractor path.
"""
import pathlib
import struct
import sys

PAGE = 4096


# ----------- FDT builder (writes a minimal v17 blob) ---------------------
class Fdt:
    def __init__(self):
        self._strings = bytearray(b"\x00")
        self._struct = bytearray()
        self._open = 0

    def _str(self, s: str) -> int:
        tok = s.encode() + b"\x00"
        off = self._strings.find(tok)
        if off >= 0:
            return off
        off = len(self._strings)
        self._strings += tok
        return off

    def begin(self, name: str):
        self._struct += struct.pack(">I", 1)
        n = name.encode() + b"\x00"
        self._struct += n + b"\x00" * ((-len(n)) & 3)
        self._open += 1

    def end(self):
        self._struct += struct.pack(">I", 2)
        self._open -= 1

    def prop(self, name: str, value: bytes):
        self._struct += struct.pack(">III", 3, len(value), self._str(name))
        self._struct += value + b"\x00" * ((-len(value)) & 3)

    def prop_u32(self, name: str, *vals: int):
        self.prop(name, b"".join(struct.pack(">I", v) for v in vals))

    def prop_str(self, name: str, s: str):
        self.prop(name, s.encode() + b"\x00")

    def prop_stringlist(self, name: str, items: list[str]):
        self.prop(name, b"".join(i.encode() + b"\x00" for i in items))

    def finish(self) -> bytes:
        assert self._open == 0, "unclosed nodes"
        self._struct += struct.pack(">I", 9)  # FDT_END
        # rsvmap (empty): 16 zero bytes terminator
        rsv = b"\x00" * 16
        off_struct = 40 + len(rsv)
        off_strings = off_struct + len(self._struct)
        total = off_strings + len(self._strings)
        header = struct.pack(
            ">10I",
            0xd00dfeed, total, off_struct, off_strings, 40,
            17, 16, 0, len(self._strings), len(self._struct),
        )
        return header + rsv + bytes(self._struct) + bytes(self._strings)


def build_sample_dtb() -> bytes:
    f = Fdt()
    f.begin("")  # root
    f.prop_u32("#address-cells", 2)
    f.prop_u32("#size-cells", 2)
    f.prop_stringlist("compatible", ["qcom,monaco-mtp", "qcom,monaco"])
    f.prop_str("model", "synthetic sn0wbite test device")

    f.begin("chosen")
    f.prop_str("stdout-path", "serial0:115200n8")
    f.end()

    f.begin("aliases")
    f.prop_str("serial0", "/soc/serial@a90000")
    f.end()

    f.begin("cpus")
    f.prop_u32("#address-cells", 1)
    f.prop_u32("#size-cells", 0)
    for i in range(2):
        f.begin(f"cpu@{i}")
        f.prop_str("device_type", "cpu")
        f.prop_stringlist("compatible", ["arm,cortex-a53"])
        f.prop_u32("reg", i)
        f.end()
    f.end()  # /cpus

    # memory@80000000, reg = <0x0 0x80000000 0x0 0x40000000>
    f.begin("memory@80000000")
    f.prop_str("device_type", "memory")
    f.prop_u32("reg", 0, 0x80000000, 0, 0x40000000)
    f.end()

    f.begin("reserved-memory")
    f.prop_u32("#address-cells", 2)
    f.prop_u32("#size-cells", 2)
    f.begin("ramoops@71f00000")
    f.prop_stringlist("compatible", ["ramoops"])
    f.prop_u32("reg", 0, 0x71f00000, 0, 0x80000)
    f.end()
    f.end()  # /reserved-memory

    f.begin("soc")
    f.prop_u32("#address-cells", 1)
    f.prop_u32("#size-cells", 1)

    f.begin("serial@a90000")
    f.prop_stringlist("compatible", ["qcom,msm-geni-uart"])
    f.prop_u32("reg", 0xa90000, 0x4000)
    f.end()

    f.begin("interrupt-controller@17a00000")
    f.prop_stringlist("compatible", ["arm,gic-v3"])
    f.prop("interrupt-controller", b"")
    f.prop_u32("#interrupt-cells", 3)
    # GIC-v3 reg: dist + redist
    f.prop_u32("reg", 0x17a00000, 0x10000, 0x17a60000, 0x100000)
    f.end()

    f.end()  # /soc
    f.end()  # /
    return f.finish()


# ----------- bootimg v2 builder ------------------------------------------
def build_bootimg_v2(kernel: bytes, ramdisk: bytes, dtb: bytes, cmdline: str) -> bytes:
    def pad(b: bytes, p: int = PAGE) -> bytes:
        return b + b"\x00" * ((-len(b)) & (p - 1))

    hdr = struct.pack(
        "<8s" "10I" "16s" "512s" "8I" "1024s" "IQI" "IQ",
        b"ANDROID!",
        len(kernel), 0x80008000,          # kernel_size, kernel_addr
        len(ramdisk), 0x81000000,         # ramdisk_size, ramdisk_addr
        0, 0,                             # second_size, second_addr
        0x80000100,                       # tags_addr
        PAGE,                             # page_size
        2,                                # header_version
        0,                                # os_version
        b"synthetic",
        cmdline.encode()[:511] + b"\x00",
        *([0] * 8),                       # id[8]
        b"",                              # extra_cmdline
        0, 0, PAGE * 2 + 16 + 12,         # v1 tail: recovery_dtbo_size, offset, header_size
        len(dtb), 0x80000000,             # v2 tail: dtb_size, dtb_addr
    )
    out = pad(hdr)
    out += pad(kernel)
    out += pad(ramdisk)
    out += pad(dtb)
    return out


def main() -> int:
    if len(sys.argv) != 2:
        sys.stderr.write("usage: synth.py <outdir>\n")
        return 2
    outdir = pathlib.Path(sys.argv[1])
    outdir.mkdir(parents=True, exist_ok=True)

    kernel = b"\x55" * 128  # 128 bytes of nonsense; the extractor doesn't look inside
    ramdisk = b"\xaa" * 128
    dtb = build_sample_dtb()
    img = build_bootimg_v2(kernel, ramdisk, dtb, "console=ttyMSM0,115200n8")

    (outdir / "boot_a.img").write_bytes(img)
    print(f"wrote {outdir / 'boot_a.img'} ({len(img)} bytes, dtb {len(dtb)}B)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
