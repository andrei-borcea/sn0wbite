#!/usr/bin/env python3
"""Convert binary PPM files to PNG (no external modules). Usage: python3 -I ppm2png.py in.ppm [out.png] ..."""
import struct, sys, zlib

def conv(src, dst):
    b = open(src, "rb").read()
    assert b[:2] == b"P6"
    parts, i = [], 2
    while len(parts) < 3:
        while b[i:i + 1].isspace(): i += 1
        j = i
        while not b[j:j + 1].isspace(): j += 1
        parts.append(int(b[i:j])); i = j
    w, h, _ = parts
    raw = b[i + 1:i + 1 + w * h * 3]
    rows = b"".join(b"\0" + raw[y * w * 3:(y + 1) * w * 3] for y in range(h))
    def chunk(t, d): return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d))
    open(dst, "wb").write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(rows, 9)) + chunk(b"IEND", b""))

args = sys.argv[1:]
for a in args:
    if a.endswith(".ppm"): conv(a, a[:-4] + ".png")
