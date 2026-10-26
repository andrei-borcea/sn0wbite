#!/usr/bin/env python3
"""Standalone ADT parser — reads Apple Device Tree binary, prints structure.
Used to sanity-check the n218bAP DT against dt_fixup.py's assumptions
BEFORE running the real patch script. No deps on dt_fixup.py."""
import struct
import sys
import math


def round_up_4(i):
    return 4 * math.ceil(i / 4)


class Node:
    def __init__(self):
        self.props = {}  # name -> raw bytes
        self.children = []


def decode(buf, off=0):
    n_props, n_children = struct.unpack_from("<II", buf, off)
    off += 8
    node = Node()
    for _ in range(n_props):
        name = buf[off:off + 32].split(b"\x00", 1)[0].decode("utf-8", "replace")
        plen = struct.unpack_from("<I", buf, off + 32)[0] & 0x7FFFFFFF
        padded = round_up_4(plen)
        val = buf[off + 36:off + 36 + plen]
        node.props[name] = val
        off += 36 + padded
    for _ in range(n_children):
        child, off = decode(buf, off)
        node.children.append(child)
    return node, off


def find_child(node, name):
    for c in node.children:
        if c.props.get("name", b"").split(b"\x00", 1)[0].decode("utf-8", "replace") == name:
            return c
    return None


def walk(node, depth=0, path=""):
    name = node.props.get("name", b"?").split(b"\x00", 1)[0].decode("utf-8", "replace")
    print(f"{'  ' * depth}- {name}")
    for c in node.children:
        walk(c, depth + 1, path + "/" + name)


def main():
    if len(sys.argv) < 2:
        print("usage: parse_dtree.py <dtree> [path]")
        sys.exit(1)
    with open(sys.argv[1], "rb") as f:
        data = f.read()
    root, _ = decode(data)

    # Top-level dump (limited)
    print("=== root ===")
    for k, v in root.props.items():
        if len(v) <= 32 and v.endswith(b"\x00"):
            try:
                s = v.split(b"\x00", 1)[0].decode("utf-8")
                print(f"  {k:30s} = str:{s!r}")
            except UnicodeError:
                print(f"  {k:30s} = {v.hex()}")
        else:
            print(f"  {k:30s} = {len(v)}B {v[:16].hex()}...")

    print("\n=== top-level children ===")
    for c in root.children:
        print(f"  {c.props.get('name', b'?').split(b'\\x00',1)[0].decode('utf-8','replace')}")

    print("\n=== chosen props ===")
    chosen = find_child(root, "chosen")
    if chosen:
        for k, v in chosen.props.items():
            if len(v) == 4:
                print(f"  {k:30s} = u32:0x{struct.unpack('<I', v)[0]:x}")
            elif len(v) == 8:
                print(f"  {k:30s} = u64:0x{struct.unpack('<Q', v)[0]:x}")
            elif len(v) <= 48:
                print(f"  {k:30s} = {len(v)}B {v[:32].hex()}")
            else:
                print(f"  {k:30s} = {len(v)}B ...")

    print("\n=== arm-io children (devices) ===")
    arm_io = find_child(root, "arm-io")
    if arm_io:
        comp = arm_io.props.get("compatible", b"").split(b"\x00", 1)[0].decode("utf-8", "replace")
        soc_gen = arm_io.props.get("soc-generation", b"").split(b"\x00", 1)[0].decode("utf-8", "replace")
        print(f"  arm-io compatible: {comp}")
        print(f"  arm-io soc-generation: {soc_gen}")
        print("  children:")
        for c in arm_io.children[:50]:
            print(f"    - {c.props.get('name', b'?').split(b'\\x00',1)[0].decode('utf-8','replace')}")
        print(f"    (total: {len(arm_io.children)} children)")
        has_dock = find_child(arm_io, "dockchannel-uart")
        has_aic = find_child(arm_io, "aic")
        has_uart0 = find_child(arm_io, "uart0")
        has_sep = find_child(arm_io, "sep")
        print(f"  dockchannel-uart present: {has_dock is not None}")
        print(f"  aic present: {has_aic is not None}")
        print(f"  uart0 present: {has_uart0 is not None}")
        print(f"  sep present: {has_sep is not None}")
        if has_aic:
            aic_comp = has_aic.props.get("compatible", b"").split(b"\x00", 1)[0].decode("utf-8", "replace")
            print(f"  aic compatible: {aic_comp}")

    print("\n=== chosen/lock-regs/amcc check ===")
    chosen = find_child(root, "chosen")
    if chosen:
        lr = find_child(chosen, "lock-regs")
        if lr:
            amcc = find_child(lr, "amcc")
            if amcc:
                print(f"  amcc present, children:")
                for c in amcc.children:
                    n = c.props.get("name", b"?").split(b"\x00", 1)[0].decode("utf-8", "replace")
                    print(f"    - {n}")

    print("\n=== secure-root-prefix at root? ===")
    print(f"  {'yes' if 'secure-root-prefix' in root.props else 'NO (dt_fixup line 285 will fail!)'}")

    print("\n=== root/rtc at root? ===")
    print(f"  rtc child present: {find_child(root, 'rtc') is not None}")


if __name__ == "__main__":
    main()
