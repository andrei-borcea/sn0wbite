#!/bin/bash
# Boot wrapper: launches the darwin-vm QEMU-SPTM VM from inside the repo,
# pointing at the firmware + QEMU binary kept under .scratchpad-local/
# (gitignored because they're large and contain Apple IP).
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
VM_DIR="${VM_DIR:-${REPO}/.scratchpad-local/session-previous/sptm/darwin-vm}"

if [[ ! -x "${VM_DIR}/run.sh" ]]; then
    cat >&2 <<EOF
error: VM dir not found at ${VM_DIR}

Expected the darwin-vm checkout + a built QEMU-SPTM. Set VM_DIR
if the firmware lives elsewhere, e.g.:

    VM_DIR=/path/to/darwin-vm $0

See docs/watchos/qemu-build-and-boot.md for the one-time setup.
EOF
    exit 1
fi

cd "${VM_DIR}"
exec ./run.sh "$@"
