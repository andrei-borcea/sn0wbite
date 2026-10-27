#!/usr/bin/env bash
# verify-boot.sh — apply the full patchkit-opw2 chain (v1..vN + the kernel_patch.py
# bulk PAC strip for v4) to a clean kernelcache and run the Unicorn harness on
# the final result. Reaching `msr sctlr_el1` (VA 0xFFFFFFF007DFC3B8) is the
# success criterion.
#
# Usage:
#   verify-boot.sh <kernelcache.macho>              # full chain (v1 v2 v3 v4)
#   verify-boot.sh --up-to v2 <kernelcache.macho>   # stop after v2
#
# Keeps the intermediate .macho files in a tmpdir so a failed step can be
# diffed against the previous one.
set -euo pipefail

HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO=$(cd -- "$HERE/../../../.." && pwd)

UPTO="v4"
if [[ "${1:-}" == "--up-to" ]]; then UPTO="$2"; shift 2; fi
if [[ $# -lt 1 ]]; then
  sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'
  exit 2
fi
KC="$1"

VENV="$REPO/.venv"
if [[ -x "$VENV/bin/python" ]]; then
  PY="$VENV/bin/python"
elif python3 -c "import yaml, unicorn" 2>/dev/null; then
  PY="python3"
else
  cat >&2 <<MSG
Needs PyYAML + Unicorn. Bootstrap once:
  python3 -m venv .venv && .venv/bin/pip install pyyaml unicorn capstone
MSG
  exit 2
fi

TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
PKT="$REPO/tools/watchos/qemu-sptm/patchkit/patchkit.py"
KP="$REPO/tools/watchos/port/kernel_patch.py"
HARN="$REPO/tools/watchos/emu/boot_harness.py"

ver_ge() { [[ "$1" == "$2" ]] || [[ "$(printf '%s\n%s' "$1" "$2" | sort -V | tail -1)" == "$1" ]]; }

CUR="$KC"
for v in v1 v2 v3; do
  ver_ge "$UPTO" "$v" || break
  M="$HERE/manifest-$v.yaml"
  [[ -f "$M" ]] || { echo "missing $M"; exit 1; }
  OUT="$TMP/kc.$v.macho"
  echo "[chain] applying $v -> $(basename "$OUT")"
  "$PY" "$PKT" apply "$CUR" -m "$M" -o "$OUT" | tail -2
  CUR="$OUT"
done

if ver_ge "$UPTO" "v4"; then
  OUT="$TMP/kc.v1v2v3v4.macho"
  echo "[chain] v4 bulk PAC strip -> $(basename "$OUT")"
  "$PY" "$KP" "$CUR" --apply "$OUT" | tail -4
  CUR="$OUT"
fi

# TODO v5 LSE bulk when the transformer is in place

echo
echo "[harness] running Unicorn boot_harness on $(basename "$CUR")"
"$PY" "$HARN" "$CUR" 2>&1 | tail -8
echo
sha256=$(shasum -a 256 "$CUR" | awk '{print $1}')
echo "final sha256: $sha256"
