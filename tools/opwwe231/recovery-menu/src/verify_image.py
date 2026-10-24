#!/usr/bin/env python3
"""Offline check of a built recovery image against the stock one: header, ramdisk, what was added/changed/dropped.
Usage: python3 -I verify_image.py <stock recovery.img> <new recovery.img> <sn0wmenu binary> [<sn0w-targets.conf>]"""
import difflib, gzip, struct, sys

stock, new, binary = sys.argv[1:4]
conf = sys.argv[4] if len(sys.argv) > 4 else None
LIMIT, MARGIN = 12083200, 65536


def ramdisk(path):
    img = open(path, "rb").read()
    hdr = img[:4096]
    ksz, rsz = struct.unpack_from("<II", hdr, 8)
    assert hdr[:8] == b"ANDROID!" and struct.unpack_from("<I", hdr, 40)[0] == 4 and ksz == 0, "header"
    raw = gzip.decompress(img[4096:4096 + rsz])
    assert img[4096 + rsz:] == b"\0" * (len(img) - 4096 - rsz) or len(img) % 4096 == 0
    off, ents = 0, {}
    while True:
        assert raw[off:off + 6] == b"070701"
        f = [int(raw[off + 6 + 8 * i: off + 14 + 8 * i], 16) for i in range(13)]
        name = raw[off + 110: off + 110 + f[11] - 1].decode()
        d0 = (off + 110 + f[11] + 3) & ~3
        if name == "TRAILER!!!":
            return img, hdr, ents
        ents[name] = (f, raw[d0:d0 + f[6]])
        off = (d0 + f[6] + 3) & ~3


simg, shdr, sents = ramdisk(stock)
nimg, nhdr, nents = ramdisk(new)
assert shdr[:12] == nhdr[:12] and shdr[16:] == nhdr[16:], "header differs outside the ramdisk size"
assert len(nimg) % 4096 == 0 and len(nimg) <= LIMIT - MARGIN, f"size {len(nimg)}"
added = sorted(set(nents) - set(sents))
dropped = sorted(set(sents) - set(nents))
changed = sorted(n for n in nents if n in sents and nents[n][1] != sents[n][1])
meta = sorted(n for n in nents if n in sents and nents[n][0][1:5] != sents[n][0][1:5])
order = list(nents)
for n in order:
    parent = n.rsplit("/", 1)[0] if "/" in n else ""
    assert not parent or (parent in nents and order.index(parent) < order.index(n)), f"{n}: parent directory missing or listed later"
print("every entry comes after its parent directory")
print(f"image {len(nimg)} B (stock {len(simg)} B), headroom to the AVB metadata {LIMIT - len(nimg)} B")
print(f"stock {len(sents)} entries, new {len(nents)}: added {added}")
print(f"changed {changed}")
print(f"dropped {len(dropped)}: " + ", ".join(dropped[:4]) + " ...")
assert not meta, f"mode/uid/gid/nlink differ for {meta}"
assert all(n.startswith("res/images/loop") or n.endswith(".erofs") for n in dropped), "unexpected drops"
for n in ("system/bin/recovery", "sepolicy", "system/etc/selinux/precompiled_sepolicy"):
    if n in sents:
        assert nents[n][1] == sents[n][1], n + " differs"
        print(f"identical to stock: {n}")
assert nents["system/bin/sn0wmenu"][1] == open(binary, "rb").read(), "binary differs from the build"
mode = nents["system/bin/sn0wmenu"][0][1]
print(f"system/bin/sn0wmenu mode {mode:o}, same as recovery {mode == nents['system/bin/recovery'][0][1]}")
if conf:
    assert nents["system/etc/sn0w-targets.conf"][1] == open(conf, "rb").read()
for n in changed:
    print(f"--- diff {n}")
    for l in difflib.unified_diff(sents[n][1].decode().splitlines(), nents[n][1].decode().splitlines(), lineterm="", n=1):
        print(l)
print("VERIFY OK")
