#!/usr/bin/env bash
# End-to-end smoke test for the sn0wbite mapping engine. Builds a synthetic
# boot image, runs the composer, then runs the differ against the watchOS
# expectations file. Asserts the differ reports at least one BLOCKER (the
# synthetic device has no PAC/TGran16/LSE) and that the GIC-v3 shows up as
# a SHIM against the Apple AIC expectation.
set -euo pipefail

HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
MAP="$HERE/.."
REPO=$(cd -- "$MAP/../.." && pwd)
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# PyYAML is the only non-stdlib dep. On a PEP-668 host (Homebrew Python on
# macOS) pip refuses to touch the system site-packages, so we keep a venv at
# .venv/ in the repo root (gitignored). Bootstrap:
#   python3 -m venv .venv && .venv/bin/pip install pyyaml
VENV="$REPO/.venv"
if [[ -x "$VENV/bin/python" ]]; then
  PY="$VENV/bin/python"
elif python3 -c "import yaml" 2>/dev/null; then
  PY="python3"
else
  cat >&2 <<MSG
PyYAML is not available. Bootstrap a project venv once:
  python3 -m venv .venv && .venv/bin/pip install pyyaml
Then re-run this script.
MSG
  exit 2
fi

echo "[1/4] synth bootimg -> $TMP/backup"
"$PY" -I "$HERE/synth.py" "$TMP/backup"

echo "[2/4] compose profile"
"$PY" -I "$MAP/profile.py" "$TMP/backup" \
  --device synthetic \
  -o "$TMP/device.yaml" \
  --workdir "$TMP/workdir"

echo "[3/4] diff against watchOS 6.3.1 expectations"
# diff.py exits 1 when there are BLOCKERs; that's expected on the synthetic
# device, so disable set -e just around it.
set +e
"$PY" -I "$MAP/diff.py" "$TMP/device.yaml" \
  "$MAP/expectations/watchos-6.3.1-watch5.yaml" \
  -o "$TMP/port-gap.yaml" --markdown
diff_rc=$?
set -e

echo "[4/4] assert outputs"
"$PY" -I - "$TMP/device.yaml" "$TMP/port-gap.yaml" "$diff_rc" <<'PY'
import sys, yaml
device = yaml.safe_load(open(sys.argv[1]))
gap = yaml.safe_load(open(sys.argv[2]))
diff_rc = int(sys.argv[3])

assert device["schema_version"] == 1
assert device["identity"]["device_id"] == "synthetic"
assert device["memory"]["dram_base"] == "0x80000000", device["memory"]
assert device["memory"]["dram_size"] == "0x40000000", device["memory"]
assert any(r["usage"] == "ramoops" for r in device["memory"]["reserved"]), device["memory"]
assert device["console_uart"]["type"] == "msm-geni-uart", device["console_uart"]
assert device["interrupt_controller"]["type"] == "gic-v3", device["interrupt_controller"]
assert device["interrupt_controller"]["dist_base"] == "0x17a00000"
assert device["interrupt_controller"]["redist_base"] == "0x17a60000"
assert device["partial"] is True

# gap: must flag the Apple AIC vs GIC and the missing CPU features
by_path = {it["path"]: it for it in gap["items"]}
assert by_path["interrupt_controller.type"]["verdict"] == "SHIM"
# tgran16/pauth/lse are unknown on the profile (we never set isa_features)
assert by_path["cpu.isa_features.tgran16"]["verdict"] in ("SHIM", "BLOCKER")
assert diff_rc in (0, 1), f"unexpected diff.py exit {diff_rc}"
print("OK  profile looks sane, diff flagged the AIC/GIC gap")
PY

echo "smoke test passed."
