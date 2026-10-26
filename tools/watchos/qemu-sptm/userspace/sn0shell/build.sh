#!/bin/bash
# Compile sn0shell as a watchOS-platform adhoc binary.
# Produces LC_BUILD_VERSION platform=4 (watchOS) so dyld on Watch7,11
# watchOS 27 accepts it (unlike iOS-platform binaries like /bin/sh).
#
# Architecture is selected by the ARCH env var:
#   ARCH=arm64   (default) — plain arm64, no PAC
#   ARCH=arm64e            — arm64e, pointer authentication enabled
# arm64e requires watchOS SDK 26+ (shipped since Xcode 15); the kernel accepts
# adhoc arm64e once the cdhash is in the trust cache, same as arm64.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "${SCRIPT_DIR}/_ascii.sh"
OUT="${SCRIPT_DIR}/sn0shell"
ARCH="${ARCH:-arm64}"

case "${ARCH}" in
    arm64|arm64e) ;;
    *) die "ARCH must be arm64 or arm64e, got '${ARCH}'" ;;
esac

banner "build"
divider

step 1 3 "compile with watchOS SDK (${ARCH}, min=10.0)"
SDK="$(xcrun --sdk watchos --show-sdk-path)"
kv "arch" "${ARCH}"
kv "sdk" "${SDK}"
kv "source" "${SCRIPT_DIR}/sn0shell.c"
kv "output" "${OUT}"
xcrun --sdk watchos clang \
  -arch "${ARCH}" \
  -mwatchos-version-min=10.0 \
  -isysroot "${SDK}" \
  -O1 \
  "${SCRIPT_DIR}/sn0shell.c" -o "${OUT}"
ok "compiled $(stat -f%z "${OUT}") bytes"

step 2 3 "adhoc-sign"
codesign -s - --force --identifier com.sn0wbite.shell "${OUT}"
ok "signed as com.sn0wbite.shell"

step 3 3 "verify Mach-O + cdhash"
FILE_INFO=$(file -b "${OUT}" | head -1)
PLATFORM=$(otool -l "${OUT}" | awk '/LC_BUILD_VERSION/,/sdk/{print}' | awk '/platform/{print $2}')
MINOS=$(otool -l "${OUT}" | awk '/LC_BUILD_VERSION/,/sdk/{print}' | awk '/minos/{print $2}')
CDHASH=$(codesign -dvvv "${OUT}" 2>&1 | awk -F= '/^CDHash=/{print $2}')
kv "file" "${FILE_INFO}"
kv "platform" "${PLATFORM}  ${C_DIM}(4 = watchOS)${C_RST}"
kv "minos" "${MINOS}"
kv "cdhash" "${CDHASH}"
ok "mach-o looks healthy"

finalbox "✦ sn0shell built" \
    "binary=${OUT}" \
    "arch=${ARCH}$([ "${ARCH}" = arm64e ] && echo "  (PAC-enabled)")" \
    "platform=watchOS (LC_BUILD_VERSION platform=4)" \
    "cdhash=${CDHASH}" \
    "next step=./deploy.sh"
