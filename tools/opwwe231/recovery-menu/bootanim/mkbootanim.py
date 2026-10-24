#!/usr/bin/env python3
"""Build the sn0wbite_bootanim v2 Magisk module: a large static logo with a live verbose console over the whole round screen.

 - renders the logo with render_base (host clang) from boot_ui.h, the code bootlog draws the console with
 - quantizes it to a 256 colour palette, writes K identical-size stored PNG frames and an animation zip whose desc.txt replays the
   frames M times with count-1 parts (the stock player decodes those frames when it shows them, so bootlog can rewrite them)
 - writes bootlog/idx (frame slots inside the zip), bootlog/base.bin and bootlog/pal.bin for the updater
Usage: python3 -I mkbootanim.py <bootlog binary (stripped ARM32)> <out module dir>
"""
import os, shutil, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from pnglive import png, build_zip, idat_payload

K, M, FPS = 30, 40, 10
FIXED = [0x000000, 0x0A100A, 0x2F5A1F, 0x3D6B2B, 0x4F8A38, 0x62A645, 0x7FBF5A, 0xA6DB86, 0xE6F7DC]  # black, console background and border, the six text shades of boot_ui.h
INIT_LINES = ["SN0WBITE BOOT", "WAITING FOR THE KERNEL LOG..."]


def ppm(path):
    b = open(path, "rb").read()
    parts, i = [], 2
    while len(parts) < 3:
        while b[i:i + 1].isspace(): i += 1
        j = i
        while not b[j:j + 1].isspace(): j += 1
        parts.append(int(b[i:j])); i = j
    w, h, _ = parts
    raw = b[i + 1:i + 1 + w * h * 3]
    return w, h, [raw[k] << 16 | raw[k + 1] << 8 | raw[k + 2] for k in range(0, len(raw), 3)]


def main():
    binary, out = sys.argv[1:3]
    work = os.path.join(out, ".build")
    os.makedirs(work, exist_ok=True)
    subprocess.run(["clang", "-O1", "-Wno-unused-function", "-o", f"{work}/render_base", f"{HERE}/render_base.c", "-lm", f"-I{HERE}", f"-I{HERE}/../src"], check=True, capture_output=True)
    open(f"{work}/init.txt", "w").write("\n".join(INIT_LINES) + "\n")
    subprocess.run([f"{work}/render_base", f"{work}/base.ppm"], check=True)
    subprocess.run([f"{work}/render_base", f"{work}/init.ppm", f"{work}/init.txt"], check=True)
    w, h, base = ppm(f"{work}/base.ppm")
    _, _, init = ppm(f"{work}/init.ppm")

    # palette: the fixed colours first, then the colours of the logo (reduced if there are too many)
    uniq = {}
    for c in base + init:
        uniq[c] = uniq.get(c, 0) + 1
    rest = [c for c in uniq if c not in FIXED]
    shift = 0
    while len({(c >> 16 >> shift, (c >> 8 & 255) >> shift, (c & 255) >> shift) for c in rest}) > 256 - len(FIXED):
        shift += 1
    reps = {}
    for c in sorted(rest, key=lambda c: -uniq[c]):
        reps.setdefault((c >> 16 >> shift, (c >> 8 & 255) >> shift, (c & 255) >> shift), c)
    pal = FIXED + list(reps.values())
    print(f"{len(uniq)} colours in the screen, palette {len(pal)} (reduced by {shift} bits)")
    rgb = [((c >> 16) & 255, (c >> 8) & 255, c & 255) for c in pal]
    cache = {}

    def index(c):
        if c not in cache:
            r, g, b = (c >> 16) & 255, (c >> 8) & 255, c & 255
            pool = [i for i in range(len(rgb)) if c in FIXED and pal[i] == c or c not in FIXED and pal[i] not in FIXED[1:]]
            cache[c] = min(pool, key=lambda i: (rgb[i][0] - r) ** 2 + (rgb[i][1] - g) ** 2 + (rgb[i][2] - b) ** 2)
        return cache[c]

    def rows(px): return [bytes(index(c) for c in px[y * w:(y + 1) * w]) for y in range(h)]

    base_rows, init_rows = rows(base), rows(init)
    frame = png(w, h, init_rows, rgb)
    payload_len = len(idat_payload(init_rows))
    desc = f"{w} {h} {FPS}\n" + "p 1 0 live\n" * M + "p 0 0 live\n"
    os.makedirs(f"{out}/system/media", exist_ok=True)
    os.makedirs(f"{out}/bootlog", exist_ok=True)
    os.makedirs(f"{out}/bin", exist_ok=True)
    idx = build_zip(f"{out}/system/media/bootanimation.zip", desc, {f"live/f{i:03d}.png": frame for i in range(K)})
    PAYLOAD_AT = 8 + 25 + (12 + 768) + 8  # signature, IHDR chunk, PLTE chunk, IDAT chunk header
    zraw = open(f"{out}/system/media/bootanimation.zip", "rb").read()
    with open(f"{out}/bootlog/idx", "w") as f:
        f.write(f"{w} {h} {FPS} {K}\n")
        for i in range(K):
            off, size = idx[f"live/f{i:03d}.png"]
            assert size == len(frame) and zraw[off + PAYLOAD_AT:off + PAYLOAD_AT + payload_len] == frame[PAYLOAD_AT:PAYLOAD_AT + payload_len]
            f.write(f"{off + PAYLOAD_AT} {payload_len}\n")
    open(f"{out}/bootlog/base.bin", "wb").write(b"".join(base_rows))
    open(f"{out}/bootlog/pal.bin", "wb").write(b"".join(bytes(c) for c in rgb) + b"\0\0\0" * (256 - len(rgb)))
    shutil.copy(binary, f"{out}/bin/bootlog")
    os.chmod(f"{out}/bin/bootlog", 0o755)
    print(f"zip {len(zraw)} B, {K} slots of {len(frame)} B, payload {payload_len} B, desc {desc.count(chr(10))} lines")


main()
