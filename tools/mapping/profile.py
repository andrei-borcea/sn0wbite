#!/usr/bin/env python3
"""profile.py — compose a sn0wbite device profile from a backup directory.

Discovers Android boot/vendor_boot/dtbo images under a backup dir, routes each
to its extractor (currently bootimg + dtb), merges the fragments with an
evidence trail per field, and writes profiles/<device>/device.yaml. Marks the
profile `partial: true` when `super.img`/`super.img.lz4` is absent.

Usage:
  profile.py <backup_dir> -o <out.yaml> --device <slug>

Follows the same soft-PyYAML dependency pattern as tools/watchos/qemu-sptm/
patchkit/patchkit.py.
"""
import argparse
import hashlib
import pathlib
import sys
from datetime import date

try:
    import yaml
except ImportError:
    sys.stderr.write("missing PyYAML — install with: python3 -m pip install pyyaml\n")
    sys.exit(2)

# Make sibling extractors importable whether we're run from the repo root or
# directly as `python3 tools/mapping/profile.py`.
sys.path.insert(0, str(pathlib.Path(__file__).parent))
from extractors import bootimg as ex_bootimg  # noqa: E402
from extractors import dtb as ex_dtb  # noqa: E402


def _sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def _deep_merge(dst: dict, src: dict) -> None:
    """Merge src into dst in place. Dicts are merged recursively; other values
    overwrite. Lists overwrite too — the composer never concatenates lists
    silently because that would hide multi-source disagreement."""
    for k, v in src.items():
        if isinstance(v, dict) and isinstance(dst.get(k), dict):
            _deep_merge(dst[k], v)
        else:
            dst[k] = v


def _merge_evidence(dst: dict, src: dict) -> None:
    for path, entries in src.items():
        dst.setdefault(path, []).extend(entries)


# Partition name hints: filename stem (without slot suffix) → kind.
_KIND_HINTS = {
    "boot": "boot",
    "vendor_boot": "vendor_boot",
    "init_boot": "boot",
    "recovery": "boot",
    "dtbo": "dtbo",
    "vbmeta": "vbmeta",
    "vbmeta_system": "vbmeta",
    "super": "super",
}


def _classify(path: pathlib.Path) -> str | None:
    stem = path.stem.lower()
    # strip trailing _a / _b slot suffixes
    for suf in ("_a", "_b"):
        if stem.endswith(suf):
            stem = stem[:-2]
            break
    return _KIND_HINTS.get(stem)


def _slot(path: pathlib.Path) -> str:
    stem = path.stem.lower()
    if stem.endswith("_a"):
        return "a"
    if stem.endswith("_b"):
        return "b"
    return "none"


def compose(backup_dir: pathlib.Path, device_id: str, workdir: pathlib.Path) -> dict:
    workdir.mkdir(parents=True, exist_ok=True)
    profile: dict = {
        "schema_version": 1,
        "partial": True,
        "identity": {
            "device_id": device_id,
            "backup_date": date.today().isoformat(),
        },
        "partitions": [],
    }
    evidence: dict = {}
    saw_super = False

    for p in sorted(backup_dir.iterdir()):
        if not p.is_file():
            continue
        kind = _classify(p)
        rel = str(p.relative_to(backup_dir))
        entry: dict = {
            "name": p.stem,
            "size": p.stat().st_size,
            "sha256": _sha256(p),
            "slot": _slot(p),
            "type": {"boot": "abootimg", "vendor_boot": "abootimg",
                     "dtbo": "dtbo", "vbmeta": "vbmeta",
                     "super": "super"}.get(kind, "unknown"),
        }
        profile["partitions"].append(entry)

        if kind == "super":
            saw_super = True
            continue  # super parsing is out of scope for M0.1

        if kind in ("boot", "vendor_boot"):
            try:
                data = p.read_bytes()
                hdr = ex_bootimg.parse_bootimg_header(data)
            except ex_bootimg.BootImgError as e:
                sys.stderr.write(f"[{rel}] bootimg: {e}\n")
                continue
            payloads = ex_bootimg.split_payloads(data, hdr, workdir / p.stem)
            frag, ev = ex_bootimg.to_profile_fragment(hdr, rel)
            _deep_merge(profile, frag)
            _merge_evidence(evidence, ev)

            dtb_path = payloads.get("dtb")
            if dtb_path and dtb_path.stat().st_size:
                try:
                    root = ex_dtb.parse_fdt(dtb_path.read_bytes())
                except ex_dtb.FdtError as e:
                    sys.stderr.write(f"[{rel}] dtb (embedded): {e}\n")
                else:
                    dfrag, dev = ex_dtb.to_profile_fragment(
                        root, f"{rel}#dtb")
                    _deep_merge(profile, dfrag)
                    _merge_evidence(evidence, dev)

        elif kind == "dtbo":
            # DTBO files hold overlays, but the primary tree may also be a
            # plain FDT in some vendors. Try to parse; ignore if not FDT.
            try:
                root = ex_dtb.parse_fdt(p.read_bytes())
            except ex_dtb.FdtError:
                continue
            dfrag, dev = ex_dtb.to_profile_fragment(root, rel)
            _deep_merge(profile, dfrag)
            _merge_evidence(evidence, dev)

    profile["partial"] = not saw_super
    if evidence:
        profile["evidence"] = {k: v for k, v in sorted(evidence.items())}
    return profile


def _yaml_dump(obj) -> str:
    # sort_keys=False keeps the authored field order from compose(); add a
    # heading comment so humans know the file is machine-generated.
    body = yaml.safe_dump(obj, sort_keys=False, default_flow_style=False,
                          allow_unicode=True)
    return (
        "# sn0wbite device profile — machine-generated by tools/mapping/profile.py\n"
        "# Schema: ../../tools/mapping/schema/device-profile.schema.yaml\n"
        + body
    )


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("backup_dir", help="directory with the raw .img backups")
    ap.add_argument("--device", required=True,
                    help="stable device slug, e.g. opwwe231")
    ap.add_argument("-o", "--out", required=True,
                    help="output profile path (profiles/<device>/device.yaml)")
    ap.add_argument("--workdir",
                    help="where to splay kernel/ramdisk/dtb payloads "
                         "(default: alongside out)")
    args = ap.parse_args(argv)

    backup = pathlib.Path(args.backup_dir)
    if not backup.is_dir():
        sys.stderr.write(f"not a directory: {backup}\n")
        return 2
    out = pathlib.Path(args.out)
    workdir = pathlib.Path(args.workdir) if args.workdir else out.parent / "_payloads"
    out.parent.mkdir(parents=True, exist_ok=True)

    profile = compose(backup, args.device, workdir)
    out.write_text(_yaml_dump(profile))
    print(f"wrote {out} (partial={profile['partial']}, "
          f"{len(profile['partitions'])} partitions)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
