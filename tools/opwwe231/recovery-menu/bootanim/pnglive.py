"""Fixed-size palette PNG with stored (uncompressed) deflate blocks, and a stored-entry bootanimation zip builder.
The PNG size depends only on the dimensions, so the pixel data of a frame can be rewritten in place inside the zip."""
import struct, zlib, zipfile

def _chunk(t, d): return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)

def idat_payload(rows):
    """zlib stream with stored blocks for the filtered scanlines (filter byte 0 + one index per pixel)."""
    raw = b"".join(b"\0" + r for r in rows)
    out = bytearray(b"\x78\x01")
    for i in range(0, len(raw), 65535):
        blk = raw[i:i + 65535]
        out += bytes([1 if i + 65535 >= len(raw) else 0]) + struct.pack("<HH", len(blk), len(blk) ^ 0xffff) + blk
    out += struct.pack(">I", zlib.adler32(raw) & 0xffffffff)
    return bytes(out)

def png(w, h, rows, palette):
    plte = b"".join(bytes(c) for c in palette) + b"\0\0\0" * (256 - len(palette))
    return (b"\x89PNG\r\n\x1a\n" + _chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 3, 0, 0, 0)) + _chunk(b"PLTE", plte)
            + _chunk(b"IDAT", idat_payload(rows)) + _chunk(b"IEND", b""))

def build_zip(path, desc, frames):
    """frames: {entry name: png bytes}; returns {name: (data offset, length)} for in-place updates."""
    with zipfile.ZipFile(path, "w", zipfile.ZIP_STORED) as z:
        z.writestr("desc.txt", desc)
        for n, b in frames.items(): z.writestr(n, b)
    idx = {}
    raw = open(path, "rb").read()
    with zipfile.ZipFile(path) as z:
        for zi in z.infolist():
            if zi.filename == "desc.txt": continue
            o = zi.header_offset
            nl, el = struct.unpack_from("<HH", raw, o + 26)
            idx[zi.filename] = (o + 30 + nl + el, zi.file_size)
    return idx
