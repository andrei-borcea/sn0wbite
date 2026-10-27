#!/usr/bin/env bash
# opwwe231.sh — pull stock partitions from an OPW2 running the sn0wbite custom
# recovery (adb root available). Streams each partition via `adb exec-out dd`
# straight to a file on the host — nothing is written on the device, nothing on
# the device is flashed, modified, or deleted.
#
# Usage:
#   acquire/opwwe231.sh                 # default partitions into backups/opwwe231/<date>/
#   acquire/opwwe231.sh --dry-run       # print the plan, do not run adb
#   acquire/opwwe231.sh --include-super # also pull super (2-5 GB, slow)
#   acquire/opwwe231.sh -o /other/dir   # custom destination
set -euo pipefail

HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO=$(cd -- "$HERE/../../.." && pwd)
DATE=$(date -u +%Y-%m-%d)
OUT_DEFAULT="$REPO/backups/opwwe231/$DATE"

# Base set: both slots of the boot chain and AVB metadata. Keep this minimal
# and explicit — no wildcards. Modem/persist/userdata/data are omitted on
# purpose: not needed for the port, and they may hold personal data.
PARTS=(
  boot_a boot_b
  vendor_boot_a vendor_boot_b
  init_boot_a init_boot_b
  dtbo_a dtbo_b
  vbmeta_a vbmeta_b
  vbmeta_system_a vbmeta_system_b
  recovery_a recovery_b
)
SUPER=(super)

dry=0
include_super=0
out="$OUT_DEFAULT"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dry-run)        dry=1; shift;;
    --include-super)  include_super=1; shift;;
    -o|--out)         out="$2"; shift 2;;
    -h|--help)
      sed -n '2,15p' "$0" | sed 's/^# \{0,1\}//'
      exit 0;;
    *) echo "unknown arg: $1" >&2; exit 2;;
  esac
done

if [[ $include_super -eq 1 ]]; then
  PARTS+=("${SUPER[@]}")
fi

echo "destination: $out"
echo "partitions:  ${PARTS[*]}"
echo

if [[ $dry -eq 1 ]]; then
  for p in "${PARTS[@]}"; do
    echo "would stream: /dev/block/by-name/$p -> $out/$p.img"
  done
  echo
  echo "SHA256SUMS would be written to $out/SHA256SUMS"
  exit 0
fi

# Preflight: adb must be reachable and running as root (sn0wbite recovery does
# this by default). Refuse to run from stock Wear OS where adb is unprivileged.
if ! command -v adb >/dev/null 2>&1; then
  echo "adb not found in PATH" >&2
  exit 1
fi
adb_user=$(adb shell id -un 2>/dev/null || true)
if [[ "$adb_user" != "root" ]]; then
  echo "adb shell runs as '$adb_user', expected 'root'." >&2
  echo "Reboot into sn0wbite recovery first (RECOVERY STOCK or sn0wmenu9)." >&2
  exit 1
fi

# Resolve each name under /dev/block/by-name once, so we fail fast if a
# partition is missing on this particular build.
missing=()
for p in "${PARTS[@]}"; do
  if ! adb shell "test -e /dev/block/by-name/$p" 2>/dev/null; then
    missing+=("$p")
  fi
done
if (( ${#missing[@]} )); then
  echo "device has no such partitions: ${missing[*]}" >&2
  echo "(not fatal — those will be skipped)" >&2
fi

mkdir -p "$out"
: > "$out/SHA256SUMS"

for p in "${PARTS[@]}"; do
  if printf '%s\n' "${missing[@]:-}" | grep -qx "$p"; then
    continue
  fi
  dst="$out/$p.img"
  echo "pulling $p -> $dst"
  # exec-out: binary-safe. bs=4M to keep throughput up; 2>/dev/null kills the
  # dd summary line that would otherwise land in the stream.
  adb exec-out "dd if=/dev/block/by-name/$p bs=4M 2>/dev/null" > "$dst"
  sz=$(wc -c < "$dst" | tr -d ' ')
  if [[ "$sz" == "0" ]]; then
    echo "  warning: 0 bytes read; removing $dst" >&2
    rm -f "$dst"
    continue
  fi
  # On macOS use shasum, on Linux prefer sha256sum.
  if command -v sha256sum >/dev/null 2>&1; then
    (cd "$out" && sha256sum "$p.img") | tee -a "$out/SHA256SUMS"
  else
    (cd "$out" && shasum -a 256 "$p.img") | tee -a "$out/SHA256SUMS"
  fi
done

echo
echo "done. feed this directory to tools/mapping/profile.py:"
echo "  python3 $REPO/tools/mapping/profile.py $out \\"
echo "    --device opwwe231 \\"
echo "    -o $REPO/profiles/opwwe231/device.yaml"
