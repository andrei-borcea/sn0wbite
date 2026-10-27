#!/usr/bin/env python3
"""diff.py — compare a sn0wbite device profile against an expectations file.

Produces a declarative port-gap report: for each rule in the expectations YAML,
look up the matching path in the profile, apply the `compare:` operator, emit
a verdict and the attached remediation hint. Writes a YAML report; with
`--markdown` also writes a human-readable summary alongside.

Usage:
  diff.py <device.yaml> <expectations.yaml> -o <port-gap.yaml> [--markdown]
"""
import argparse
import pathlib
import sys

try:
    import yaml
except ImportError:
    sys.stderr.write("missing PyYAML — install with: python3 -m pip install pyyaml\n")
    sys.exit(2)

VERDICTS = ("MATCH", "SHIM", "BLOCKER")


def _get_path(obj, dotted: str):
    """Walk `obj` by dotted path. Returns (found, value). None values count as
    present; only a missing key counts as absent."""
    cur = obj
    for seg in dotted.split("."):
        if seg.startswith("_"):
            # paths starting with "_" are reserved for expectations-side notes
            # (not expected in device profiles). Treat as absent silently.
            return False, None
        if not isinstance(cur, dict) or seg not in cur:
            return False, None
        cur = cur[seg]
    return True, cur


def _apply(rule: dict, profile: dict) -> dict:
    path = rule["path"]
    op = rule.get("compare", "equal")
    found, actual = _get_path(profile, path)
    remediation = rule.get("remediation")
    note = rule.get("note")

    item: dict = {
        "path": path,
        "compare": op,
        "expected": rule.get("value"),
        "actual": actual,
    }
    if remediation is not None:
        item["remediation"] = remediation
    if note is not None:
        item["note"] = note

    if op == "note":
        item["verdict"] = "MATCH"
        item["actual"] = rule.get("value")
        return item

    if op == "present":
        if found and actual is not None:
            item["verdict"] = "MATCH"
        else:
            item["verdict"] = rule.get("verdict_on_absent", "SHIM")
        return item

    if op == "feature":
        # feature rules are rooted at cpu.isa_features.<key> — follow suit.
        found, actual = _get_path(profile, path)
        item["actual"] = actual
        if actual is True:
            item["verdict"] = "MATCH"
        elif actual is False:
            item["verdict"] = rule.get("verdict_on_absent", "BLOCKER")
        else:  # None / missing
            item["verdict"] = rule.get("verdict_on_absent", "SHIM")
            if not found:
                item["unknown"] = True
        return item

    if op == "equal":
        if not found:
            item["verdict"] = rule.get("verdict_on_absent", "SHIM")
        elif actual == rule.get("value"):
            item["verdict"] = "MATCH"
        else:
            item["verdict"] = rule.get("verdict_on_mismatch", "SHIM")
        return item

    item["verdict"] = "SHIM"
    item["note"] = f"unknown compare op: {op}"
    return item


def diff(profile: dict, expectations: dict) -> dict:
    items = [_apply(rule, profile) for rule in expectations.get("rules", [])]
    counts = {v: sum(1 for it in items if it["verdict"] == v) for v in VERDICTS}
    return {
        "schema_version": 1,
        "expectations_id": expectations.get("expectations_id"),
        "target_os": expectations.get("target_os"),
        "target_device": expectations.get("target_device"),
        "device_id": profile.get("identity", {}).get("device_id"),
        "profile_partial": profile.get("partial", True),
        "summary": counts,
        "items": items,
    }


def _render_markdown(report: dict) -> str:
    lines = [
        f"# port-gap: {report.get('device_id') or '?'} → {report.get('expectations_id') or '?'}",
        "",
        f"Target: **{report.get('target_os')}** on {report.get('target_device')}  ",
        f"Profile partial: `{report.get('profile_partial')}`",
        "",
        "## Summary",
        "",
        "| Verdict | Count |",
        "| --- | ---: |",
    ]
    for v in VERDICTS:
        lines.append(f"| {v} | {report['summary'].get(v, 0)} |")
    lines += ["", "## Items", ""]
    for it in report["items"]:
        verdict = it["verdict"]
        badge = {"MATCH": "✅", "SHIM": "🟡", "BLOCKER": "⛔"}.get(verdict, "·")
        lines.append(f"### {badge} `{it['path']}` — {verdict}")
        lines.append("")
        lines.append(f"- compare: `{it['compare']}`")
        if "expected" in it and it["expected"] is not None:
            lines.append(f"- expected: `{it['expected']}`")
        if "actual" in it:
            lines.append(f"- actual: `{it.get('actual')}`")
        rem = it.get("remediation")
        if rem:
            lines.append(f"- remediation: **{rem.get('kind')}**"
                         + (f" → `{rem['target']}`" if rem.get("target") else ""))
            if rem.get("note"):
                lines.append(f"  > {rem['note'].strip()}")
        if it.get("note"):
            lines.append(f"- note: {it['note'].strip()}")
        lines.append("")
    return "\n".join(lines)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("profile")
    ap.add_argument("expectations")
    ap.add_argument("-o", "--out", required=True)
    ap.add_argument("--markdown", action="store_true",
                    help="also write a .md summary alongside the YAML")
    args = ap.parse_args(argv)

    profile = yaml.safe_load(pathlib.Path(args.profile).read_text())
    expect = yaml.safe_load(pathlib.Path(args.expectations).read_text())
    report = diff(profile, expect)

    out = pathlib.Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(
        "# sn0wbite port-gap report — machine-generated by tools/mapping/diff.py\n"
        + yaml.safe_dump(report, sort_keys=False, allow_unicode=True))
    if args.markdown:
        md = out.with_suffix(".md")
        md.write_text(_render_markdown(report))
        print(f"wrote {out} and {md}")
    else:
        print(f"wrote {out}")

    counts = report["summary"]
    print(f"  MATCH {counts.get('MATCH', 0)}  "
          f"SHIM {counts.get('SHIM', 0)}  "
          f"BLOCKER {counts.get('BLOCKER', 0)}")
    return 1 if counts.get("BLOCKER", 0) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
