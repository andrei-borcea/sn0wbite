#!/usr/bin/env python3
"""patchkit.py — apply reproducible binary patches to a watchOS bootkc kernelcache.

Reads a YAML manifest describing patches by virtual address + original bytes +
replacement bytes, applies them to a clean kernelcache, and (optionally) scans
a function range to transform every instance of a pattern (bulk scanner).

Usage:
  patchkit.py apply <bootkc.orig> -m manifest.yaml -o <bootkc.patched>
  patchkit.py verify <bootkc.patched> -m manifest.yaml -r <bootkc.orig>
  patchkit.py dump   <bootkc> --va <addr> --length <n>

VA ↔ file-offset translation assumes a single __text window; override via
manifest `va_base` / `file_text_off` if those differ for another kernelcache.
"""
import argparse, hashlib, pathlib, re, sys

try:
    import yaml
except ImportError:
    sys.stderr.write("missing PyYAML — install with: python3 -m pip install pyyaml\n")
    sys.exit(2)


DEFAULT_VA_BASE = 0xFFFFFFF007E2C000
DEFAULT_FILE_TEXT_OFF = 0xE28000


def parse_int(v):
    if isinstance(v, int):
        return v
    s = str(v).strip()
    return int(s, 0)


def va_to_offset(va, va_base, file_text_off):
    return file_text_off + (va - va_base)


def load_manifest(path):
    with open(path) as f:
        m = yaml.safe_load(f)
    m.setdefault("va_base", DEFAULT_VA_BASE)
    m.setdefault("file_text_off", DEFAULT_FILE_TEXT_OFF)
    m["va_base"] = parse_int(m["va_base"])
    m["file_text_off"] = parse_int(m["file_text_off"])
    m.setdefault("patches", [])
    m.setdefault("scanners", [])
    return m


def apply_direct_patch(data, patch, va_base, file_text_off, verify=True):
    va = parse_int(patch["va"])
    off = va_to_offset(va, va_base, file_text_off)
    orig = bytes.fromhex(patch["orig"].replace(" ", ""))
    new = bytes.fromhex(patch["new"].replace(" ", ""))
    if len(orig) != len(new):
        raise ValueError(f"patch {patch['name']!r}: orig/new length mismatch "
                         f"({len(orig)} vs {len(new)})")
    if verify:
        actual = bytes(data[off:off + len(orig)])
        if actual != orig:
            raise ValueError(
                f"patch {patch['name']!r}: orig byte mismatch at VA 0x{va:X} "
                f"(file_off 0x{off:X})\n  expected: {orig.hex()}\n  found:    {actual.hex()}"
            )
    data[off:off + len(new)] = new
    return off, len(new), va


# ---- scanner support (bulk-pattern transforms within a function range) ----
#
# Scanner spec in YAML:
#   - name: bulk-einval-to-success
#     function_va: 0xFFFFFFF009F79CA8
#     function_size: 0x27A0
#     find:    "52 ** 02 c0"      # ** = wildcard byte; here matches MOV W<n>, #22 (any dst register in low 16)
#     replace: "52 KEEP(1) 00 00" # KEEP(idx) copies byte at `idx` of the matched slice; literal hex kept
#
# Byte directives in find:
#   ** → wildcard (match any byte)
#   XX → literal
#
# Byte directives in replace:
#   XX       → literal
#   KEEP(n)  → copy byte at index `n` of the matched slice (0-based)
#
# The implementation decodes ARM64 MOV immediate instructions in a general way
# via helpers below; but the YAML string form keeps patches auditable.


def compile_find_pattern(spec):
    toks = spec.replace(",", " ").split()
    pat = bytearray()
    mask = bytearray()
    for t in toks:
        if t == "**":
            pat.append(0)
            mask.append(0)
        else:
            pat.append(int(t, 16))
            mask.append(0xFF)
    return bytes(pat), bytes(mask)


def compile_replace_pattern(spec, match_len):
    """Returns a function (matched_bytes) -> bytes_to_write."""
    toks = re.findall(r"KEEP\(\s*\d+\s*\)|[0-9A-Fa-f]{2}", spec.replace(",", " "))
    plan = []  # list of ("lit", int) or ("keep", int)
    for t in toks:
        m = re.match(r"KEEP\(\s*(\d+)\s*\)", t)
        if m:
            idx = int(m.group(1))
            if idx >= match_len:
                raise ValueError(f"KEEP({idx}) out of range (match len {match_len})")
            plan.append(("keep", idx))
        else:
            plan.append(("lit", int(t, 16)))
    if len(plan) != match_len:
        raise ValueError(
            f"replace length {len(plan)} != find length {match_len} (replace must be in-place)"
        )

    def build(matched):
        out = bytearray()
        for kind, v in plan:
            if kind == "lit":
                out.append(v)
            else:
                out.append(matched[v])
        return bytes(out)

    return build


def apply_scanner(data, scan, va_base, file_text_off):
    func_va = parse_int(scan["function_va"])
    func_size = parse_int(scan["function_size"])
    f_off = va_to_offset(func_va, va_base, file_text_off)
    pat, mask = compile_find_pattern(scan["find"])
    build = compile_replace_pattern(scan["replace"], len(pat))

    hits = []
    i = f_off
    end = f_off + func_size - len(pat) + 1
    step = scan.get("step", 4)  # ARM64 instructions are 4-byte aligned
    while i < end:
        ok = True
        for k in range(len(pat)):
            if (data[i + k] & mask[k]) != pat[k]:
                ok = False
                break
        if ok:
            matched = bytes(data[i:i + len(pat)])
            replacement = build(matched)
            if replacement != matched:  # only count real changes
                hits.append((i, matched, replacement))
                data[i:i + len(replacement)] = replacement
            i += step
        else:
            i += step
    return hits


# ---- commands ----

def cmd_apply(args):
    data = bytearray(pathlib.Path(args.input).read_bytes())
    manifest = load_manifest(args.manifest)
    vb = manifest["va_base"]
    ft = manifest["file_text_off"]

    applied = 0
    for patch in manifest["patches"]:
        if patch.get("disabled"):
            print(f"[-] {patch['name']}: disabled, skipping")
            continue
        off, n, va = apply_direct_patch(data, patch, vb, ft, verify=not args.no_verify)
        print(f"[+] {patch['name']}: {n}B @ file_off 0x{off:X} VA 0x{va:X}")
        applied += 1

    for scan in manifest["scanners"]:
        if scan.get("disabled"):
            print(f"[-] scanner {scan['name']}: disabled, skipping")
            continue
        hits = apply_scanner(data, scan, vb, ft)
        print(f"[+] scanner {scan['name']}: {len(hits)} hit(s) in function VA 0x{parse_int(scan['function_va']):X}")
        for off, m, r in hits:
            va_hit = off - ft + vb
            print(f"    @0x{va_hit:X} file_off 0x{off:X}: {m.hex()} -> {r.hex()}")
        applied += len(hits)

    pathlib.Path(args.output).write_bytes(data)
    sha = hashlib.sha256(data).hexdigest()
    print(f"wrote {args.output} ({len(data)} bytes)  sha256={sha[:16]}...")
    print(f"total modifications: {applied}")
    return 0


def cmd_verify(args):
    """Reproducibility check: apply manifest to <reference> and compare to <input>."""
    data_cur = pathlib.Path(args.input).read_bytes()
    data_ref = bytearray(pathlib.Path(args.reference).read_bytes())
    if len(data_cur) != len(data_ref):
        print(f"size mismatch: input={len(data_cur)}, reference={len(data_ref)}")
        return 1

    manifest = load_manifest(args.manifest)
    vb = manifest["va_base"]
    ft = manifest["file_text_off"]

    for patch in manifest["patches"]:
        if patch.get("disabled"):
            continue
        apply_direct_patch(data_ref, patch, vb, ft, verify=True)
    for scan in manifest["scanners"]:
        if scan.get("disabled"):
            continue
        apply_scanner(data_ref, scan, vb, ft)

    if bytes(data_ref) == data_cur:
        sha = hashlib.sha256(data_cur).hexdigest()
        print(f"OK: manifest applied to {args.reference} reproduces {args.input}")
        print(f"    sha256={sha[:16]}...")
        return 0
    # find first diff
    mismatches = 0
    for i, (a, b) in enumerate(zip(data_ref, data_cur)):
        if a != b:
            if mismatches < 10:
                va = i - ft + vb
                print(f"  file_off 0x{i:X} VA 0x{va:X}: manifest-produces 0x{a:02X}, input-has 0x{b:02X}")
            mismatches += 1
    print(f"MISMATCH: {mismatches} byte(s) differ")
    return 1


def cmd_dump(args):
    data = pathlib.Path(args.input).read_bytes()
    manifest_va_base = DEFAULT_VA_BASE
    manifest_ft_off = DEFAULT_FILE_TEXT_OFF
    if args.manifest:
        m = load_manifest(args.manifest)
        manifest_va_base = m["va_base"]
        manifest_ft_off = m["file_text_off"]
    va = parse_int(args.va)
    off = va_to_offset(va, manifest_va_base, manifest_ft_off)
    n = parse_int(args.length)
    chunk = data[off:off + n]
    print(f"VA 0x{va:X}  file_off 0x{off:X}  len {n}")
    print(chunk.hex())
    return 0


def main():
    ap = argparse.ArgumentParser(prog="patchkit.py")
    sub = ap.add_subparsers(dest="cmd", required=True)

    a = sub.add_parser("apply", help="apply manifest patches to a clean kernelcache")
    a.add_argument("input")
    a.add_argument("-m", "--manifest", required=True)
    a.add_argument("-o", "--output", required=True)
    a.add_argument("--no-verify", action="store_true",
                   help="skip orig-byte check (use when applying over an already-patched file)")
    a.set_defaults(func=cmd_apply)

    v = sub.add_parser("verify", help="verify an input matches manifest applied to reference")
    v.add_argument("input")
    v.add_argument("-m", "--manifest", required=True)
    v.add_argument("-r", "--reference", required=True)
    v.set_defaults(func=cmd_verify)

    d = sub.add_parser("dump", help="dump bytes at VA")
    d.add_argument("input")
    d.add_argument("--va", required=True)
    d.add_argument("--length", default="16")
    d.add_argument("-m", "--manifest", default=None,
                   help="use va_base/file_text_off from this manifest")
    d.set_defaults(func=cmd_dump)

    args = ap.parse_args()
    return args.func(args) or 0


if __name__ == "__main__":
    sys.exit(main())
