#!/usr/bin/env python3
"""Build a modified trust cache that adds sn0shell's cdhash to the stock ramdisk.tc.

Trust cache binary format (watchOS 27, discovered via experimentation):
  offset 0:  4B  version        (always 0x00000001 for v1)
  offset 4:  16B UUID
  offset 20: 4B  num_entries    (LE)
  offset 24: N * 22B entries, each:
      20B  cdhash (SHA256 truncated to 20 bytes; the one reported by
           `codesign -dvvv` as "CDHash sha256=...")
      2B   flags (observed: 0x0002 for all entries)
  Entries are sorted ascending by cdhash — the lookup is likely a binary search.

Usage:
  build-tc.py <input.tc> <sn0shell-binary> <output.tc>

The sn0shell binary's cdhash is read via `codesign -dvvv`.
"""
import struct
import subprocess
import sys
from pathlib import Path

ENTRY_SIZE = 22
HEADER_SIZE = 24
DEFAULT_FLAGS = 0x0002


def cdhash_of(binary: Path) -> bytes:
    out = subprocess.run(
        ["codesign", "-dvvv", str(binary)],
        capture_output=True, text=True, check=True
    ).stderr
    for line in out.splitlines():
        if line.startswith("CDHash=") and len(line) == len("CDHash=") + 40:
            return bytes.fromhex(line.split("=", 1)[1])
    raise SystemExit(f"could not parse CDHash from codesign output of {binary}")


def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__.strip())
    tc_in = Path(sys.argv[1])
    binary = Path(sys.argv[2])
    tc_out = Path(sys.argv[3])

    data = tc_in.read_bytes()
    (version,) = struct.unpack("<I", data[0:4])
    (num,) = struct.unpack("<I", data[20:24])
    print(f"[tc] input: version={version} num_entries={num} size={len(data)}")

    new_hash = cdhash_of(binary)
    new_entry = new_hash + struct.pack("<H", DEFAULT_FLAGS)
    assert len(new_entry) == ENTRY_SIZE

    entries = [
        data[HEADER_SIZE + i * ENTRY_SIZE:HEADER_SIZE + (i + 1) * ENTRY_SIZE]
        for i in range(num)
    ]
    if any(e[:20] == new_hash for e in entries):
        print(f"[tc] cdhash {new_hash.hex()} already present, re-writing in place")
    else:
        print(f"[tc] inserting new entry for cdhash {new_hash.hex()}")
        entries.append(new_entry)

    entries.sort(key=lambda e: e[:20])
    out = (
        data[:20]
        + struct.pack("<I", len(entries))
        + b"".join(entries)
        + data[HEADER_SIZE + num * ENTRY_SIZE:]
    )
    tc_out.write_bytes(out)
    print(f"[tc] wrote {tc_out}: version={version} num_entries={len(entries)} size={len(out)}")


if __name__ == "__main__":
    main()
