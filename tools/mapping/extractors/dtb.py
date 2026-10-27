#!/usr/bin/env python3
# Flattened DeviceTree (FDT) extractor for the sn0wbite mapping engine.
#
# Pure-python FDT parser (no libfdt). Walks the struct block, builds an in-memory
# tree, and produces a device-profile fragment covering: cpu clusters, DRAM,
# reserved regions, console UART (resolved via /chosen stdout-path or
# /aliases/serial0), and the primary interrupt controller.
#
# Reference: Devicetree Specification v0.4, section 5.
import argparse
import pathlib
import struct
import sys

FDT_MAGIC = 0xd00dfeed
FDT_BEGIN_NODE = 1
FDT_END_NODE = 2
FDT_PROP = 3
FDT_NOP = 4
FDT_END = 9


def _hex(n: int) -> str:
    return f"0x{n:x}"


class FdtError(ValueError):
    pass


class Node:
    __slots__ = ("name", "props", "children", "parent")

    def __init__(self, name: str, parent: "Node | None" = None):
        self.name = name
        self.props: dict[str, bytes] = {}
        self.children: list[Node] = []
        self.parent = parent

    def path(self) -> str:
        parts, cur = [], self
        while cur is not None and cur.parent is not None:
            parts.append(cur.name)
            cur = cur.parent
        return "/" + "/".join(reversed(parts)) if parts else "/"

    def walk(self):
        yield self
        for c in self.children:
            yield from c.walk()

    def by_path(self, path: str) -> "Node | None":
        if path == "/":
            return self
        cur = self
        for seg in path.strip("/").split("/"):
            found = next((c for c in cur.children if c.name == seg), None)
            if found is None:
                return None
            cur = found
        return cur


def parse_fdt(data: bytes) -> Node:
    """Parse an FDT blob, return the root Node."""
    if len(data) < 40:
        raise FdtError("fdt too small")
    magic, totalsize, off_struct, off_strings, _off_rsv, version, _lcv, _bcp, _ssz, _structsz = \
        struct.unpack_from(">10I", data, 0)
    if magic != FDT_MAGIC:
        raise FdtError(f"bad magic {magic:#x}")
    if version < 16:
        raise FdtError(f"unsupported fdt version {version}")
    if totalsize > len(data):
        raise FdtError("totalsize exceeds buffer")

    strings = data[off_strings:]

    def _cstr_at(buf: bytes, off: int) -> str:
        end = buf.find(b"\x00", off)
        return buf[off:end].decode("utf-8", errors="replace")

    root: Node | None = None
    cur: Node | None = None
    off = off_struct

    while off < len(data):
        (tok,) = struct.unpack_from(">I", data, off)
        off += 4
        if tok == FDT_BEGIN_NODE:
            name = _cstr_at(data, off)
            off += len(name) + 1
            off = (off + 3) & ~3
            node = Node(name, parent=cur)
            if cur is None:
                root = node
            else:
                cur.children.append(node)
            cur = node
        elif tok == FDT_END_NODE:
            if cur is None:
                raise FdtError("END_NODE without open node")
            cur = cur.parent
        elif tok == FDT_PROP:
            plen, nameoff = struct.unpack_from(">II", data, off)
            off += 8
            value = data[off:off + plen]
            off += plen
            off = (off + 3) & ~3
            if cur is None:
                raise FdtError("PROP outside node")
            cur.props[_cstr_at(strings, nameoff)] = value
        elif tok == FDT_NOP:
            continue
        elif tok == FDT_END:
            break
        else:
            raise FdtError(f"bad token {tok:#x} at {off:#x}")

    if root is None:
        raise FdtError("no root node")
    return root


# ------------------------- property decoders -------------------------

def _u32(b: bytes, o: int = 0) -> int:
    return struct.unpack_from(">I", b, o)[0]


def _reg(b: bytes, ac: int, sc: int) -> list[tuple[int, int]]:
    """Decode a reg property given parent #address-cells and #size-cells."""
    out, i = [], 0
    cell = 4
    rec = (ac + sc) * cell
    while i + rec <= len(b):
        addr = 0
        for j in range(ac):
            addr = (addr << 32) | _u32(b, i + j * cell)
        size = 0
        for j in range(sc):
            size = (size << 32) | _u32(b, i + (ac + j) * cell)
        out.append((addr, size))
        i += rec
    return out


def _stringlist(b: bytes) -> list[str]:
    return [s.decode("utf-8", errors="replace") for s in b.split(b"\x00") if s]


def _ac_sc(node: Node) -> tuple[int, int]:
    p = node.parent or node
    ac = _u32(p.props["#address-cells"]) if "#address-cells" in p.props else 2
    sc = _u32(p.props["#size-cells"]) if "#size-cells" in p.props else 1
    return ac, sc


# ------------------------- fragment builders -------------------------

def _cpu_fragment(root: Node) -> dict:
    cpus = root.by_path("/cpus")
    if not cpus:
        return {}
    clusters: dict[str, dict] = {}
    for child in cpus.children:
        if not child.name.startswith("cpu"):
            continue
        compat = _stringlist(child.props.get("compatible", b""))
        key = compat[0] if compat else "unknown"
        c = clusters.setdefault(key, {"compat": key, "cores": 0})
        c["cores"] += 1
    return {"cpu": {"clusters": list(clusters.values())}} if clusters else {}


def _memory_fragment(root: Node) -> dict:
    frag: dict = {}
    for n in root.walk():
        if n.props.get("device_type", b"").rstrip(b"\x00") == b"memory":
            ac, sc = _ac_sc(n)
            regs = _reg(n.props.get("reg", b""), ac, sc)
            if regs:
                frag.setdefault("memory", {})
                frag["memory"]["dram_base"] = _hex(regs[0][0])
                frag["memory"]["dram_size"] = _hex(sum(r[1] for r in regs))
                break
    reserved = root.by_path("/reserved-memory")
    if reserved:
        ac, sc = _ac_sc(reserved)
        items = []
        for c in reserved.children:
            regs = _reg(c.props.get("reg", b""), ac, sc)
            if not regs:
                continue
            compat = _stringlist(c.props.get("compatible", b""))
            items.append({
                "name": c.name,
                "base": _hex(regs[0][0]),
                "size": _hex(regs[0][1]),
                "usage": compat[0] if compat else c.name.split("@", 1)[0],
            })
        if items:
            frag.setdefault("memory", {})["reserved"] = items
    return frag


def _resolve_stdout(root: Node) -> Node | None:
    chosen = root.by_path("/chosen")
    if chosen and "stdout-path" in chosen.props:
        path = chosen.props["stdout-path"].rstrip(b"\x00").decode()
        path = path.split(":", 1)[0]
        if not path.startswith("/"):
            aliases = root.by_path("/aliases")
            if aliases and path in aliases.props:
                path = aliases.props[path].rstrip(b"\x00").decode()
        return root.by_path(path) if path.startswith("/") else None
    aliases = root.by_path("/aliases")
    if aliases and "serial0" in aliases.props:
        path = aliases.props["serial0"].rstrip(b"\x00").decode()
        return root.by_path(path)
    return None


def _uart_fragment(root: Node) -> dict:
    u = _resolve_stdout(root)
    if u is None:
        return {}
    ac, sc = _ac_sc(u)
    regs = _reg(u.props.get("reg", b""), ac, sc)
    compat = _stringlist(u.props.get("compatible", b""))
    frag = {"console_uart": {}}
    if regs:
        frag["console_uart"]["physaddr"] = _hex(regs[0][0])
    if compat:
        frag["console_uart"]["compat"] = compat[0]
        frag["console_uart"]["type"] = compat[0].split(",", 1)[-1]
    return frag


def _ic_fragment(root: Node) -> dict:
    # Find the first node with the `interrupt-controller` property set
    # (it is an empty-valued property; presence is enough).
    for n in root.walk():
        if "interrupt-controller" not in n.props:
            continue
        ac, sc = _ac_sc(n)
        regs = _reg(n.props.get("reg", b""), ac, sc)
        compat = _stringlist(n.props.get("compatible", b""))
        ic: dict = {}
        if compat:
            ic["compat"] = compat[0]
            lower = compat[0].lower()
            if "gic-v3" in lower or "gic,v3" in lower or "arm,gic-v3" in lower:
                ic["type"] = "gic-v3"
            elif "arm,gic" in lower:
                ic["type"] = "gic-v2"
            elif "aic" in lower:
                ic["type"] = "aic"
            else:
                ic["type"] = compat[0]
        if regs:
            if ic.get("type") == "gic-v3" and len(regs) >= 2:
                ic["dist_base"] = _hex(regs[0][0])
                ic["redist_base"] = _hex(regs[1][0])
            else:
                ic["base"] = _hex(regs[0][0])
        return {"interrupt_controller": ic} if ic else {}
    return {}


def to_profile_fragment(root: Node, source: str) -> tuple[dict, dict]:
    """Return (profile_fragment, evidence_fragment) for the given FDT."""
    frag: dict = {}
    for build in (_cpu_fragment, _memory_fragment, _uart_fragment, _ic_fragment):
        for k, v in build(root).items():
            # Shallow-merge dicts; replace scalars/lists.
            if isinstance(v, dict) and isinstance(frag.get(k), dict):
                frag[k].update(v)
            else:
                frag[k] = v

    # Flatten evidence paths for every leaf we produced.
    def _paths(prefix: str, obj):
        if isinstance(obj, dict):
            for k, v in obj.items():
                yield from _paths(f"{prefix}.{k}" if prefix else k, v)
        elif isinstance(obj, list):
            yield prefix
        else:
            yield prefix

    evidence = {
        path: [{"extractor": "extractors.dtb", "source": source}]
        for path in _paths("", frag) if path
    }
    return frag, evidence


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Dump sn0wbite-relevant facts from an FDT.")
    ap.add_argument("dtb", help="path to .dtb")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--tree", action="store_true", help="dump the node tree instead of a fragment")
    args = ap.parse_args(argv)

    data = pathlib.Path(args.dtb).read_bytes()
    root = parse_fdt(data)

    if args.tree:
        def _dump(n: Node, depth: int = 0):
            print("  " * depth + (n.name or "/"))
            for k in sorted(n.props):
                print("  " * (depth + 1) + f"- {k} ({len(n.props[k])}B)")
            for c in n.children:
                _dump(c, depth + 1)
        _dump(root)
        return 0

    frag, _ev = to_profile_fragment(root, args.dtb)
    if args.json:
        import json
        print(json.dumps(frag, indent=2))
    else:
        import pprint
        pprint.pp(frag)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
