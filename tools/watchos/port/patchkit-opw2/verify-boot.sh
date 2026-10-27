#!/usr/bin/env bash
# verify-boot.sh — apply the current patchkit-opw2 manifest to a clean kernelcache
# and run the Unicorn harness on the result. Success criterion: the kernel reaches
# `msr sctlr_el1` (the MMU-enable site, VA 0xFFFFFFF007DFC3B8) without halting on
# the TCR constant load.
#
# Usage: verify-boot.sh <manifest.yaml> <kernelcache.macho>
set -euo pipefail

HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO=$(cd -- "$HERE/../../../.." && pwd)

if [[ $# -lt 2 ]]; then
  sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'
  exit 2
fi
MANIFEST="$1"
KC="$2"

VENV="$REPO/.venv"
if [[ -x "$VENV/bin/python" ]]; then
  PY="$VENV/bin/python"
elif python3 -c "import yaml, unicorn" 2>/dev/null; then
  PY="python3"
else
  cat >&2 <<MSG
Needs PyYAML and Unicorn. Bootstrap once:
  python3 -m venv .venv && .venv/bin/pip install pyyaml unicorn
MSG
  exit 2
fi

echo "[1/3] resolving segment offsets from $KC"
"$PY" "$HERE/kc-segmap.py" "$KC" --manifest "$MANIFEST" --rewrite

TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
PATCHED="$TMP/kc.patched.macho"

echo "[2/3] applying $MANIFEST to produce $PATCHED"
"$PY" "$REPO/tools/watchos/qemu-sptm/patchkit/patchkit.py" apply "$KC" \
  -m "$MANIFEST" -o "$PATCHED"

echo "[3/3] running Unicorn harness"
"$PY" "$REPO/tools/watchos/emu/boot_harness.py" "$PATCHED"

echo "verify-boot: OK"
