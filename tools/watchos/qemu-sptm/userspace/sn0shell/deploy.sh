#!/bin/bash
# Deploy sn0shell into the QEMU firmware dir:
#   1. install the binary into the ramdisk DMG at /sbin/sn0shell
#   2. install the LaunchDaemon plist pointing to it
#   3. rebuild ramdisk.tc with sn0shell's cdhash inserted
#
# Expects the firmware dir to already contain the baseline ramdisk.dmg + ramdisk.tc
# (from the restore_payload of the IPSW), and a built sn0shell next to this script.
#
# Env:
#   FW_DIR  — QEMU firmware dir. default: $REPO/.scratchpad-local/session-previous/sptm/darwin-vm/firmware
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "${SCRIPT_DIR}/_ascii.sh"
REPO="$(cd "${SCRIPT_DIR}/../../../../.." && pwd)"
FW_DIR="${FW_DIR:-${REPO}/.scratchpad-local/session-previous/sptm/darwin-vm/firmware}"

banner "deploy"
divider

[[ -f "${SCRIPT_DIR}/sn0shell" ]]           || die "sn0shell binary not built — run build.sh first"
[[ -f "${FW_DIR}/ramdisk.dmg" ]]            || die "${FW_DIR}/ramdisk.dmg not found"
[[ -f "${FW_DIR}/ramdisk.tc"  ]]            || die "${FW_DIR}/ramdisk.tc not found"
kv "firmware" "${FW_DIR}"
kv "binary"   "${SCRIPT_DIR}/sn0shell"

step 1 5 "backup pristine ramdisk.{dmg,tc}"
if [[ -f "${FW_DIR}/ramdisk.pre_sn0shell.dmg" ]]; then
    info "ramdisk.pre_sn0shell.dmg already exists — keeping"
else
    cp "${FW_DIR}/ramdisk.dmg" "${FW_DIR}/ramdisk.pre_sn0shell.dmg"
    ok "ramdisk.pre_sn0shell.dmg written"
fi
if [[ -f "${FW_DIR}/ramdisk.pre_sn0shell.tc" ]]; then
    info "ramdisk.pre_sn0shell.tc already exists — keeping"
else
    cp "${FW_DIR}/ramdisk.tc" "${FW_DIR}/ramdisk.pre_sn0shell.tc"
    ok "ramdisk.pre_sn0shell.tc written"
fi

step 2 5 "clone pristine DMG → working ramdisk.dmg"
WORK_DMG="${FW_DIR}/ramdisk.dmg"
cp -c "${FW_DIR}/ramdisk.pre_sn0shell.dmg" "${WORK_DMG}"
ok "fresh working copy ready"

step 3 5 "mount DMG with ownership preservation (needs sudo)"
warn "launchd rejects plists not owned by root:wheel — mounting with -owners on"
MOUNT=$(sudo hdiutil attach -owners on -nobrowse "${WORK_DMG}" | tail -1 | awk '{print $NF}')
trap 'sudo hdiutil detach "${MOUNT}" >/dev/null 2>&1 || true' EXIT
kv "mountpoint" "${MOUNT}"

info "installing /sbin/sn0shell (root:wheel 0755)"
sudo install -o 0 -g 0 -m 0755 "${SCRIPT_DIR}/sn0shell" "${MOUNT}/sbin/sn0shell"

PLIST="${MOUNT}/System/Library/LaunchDaemons/com.sn0wbite.sn0shell.plist"
info "writing ${PLIST#${MOUNT}} (root:wheel 0644)"
sudo tee "${PLIST}" > /dev/null <<'PL'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>KeepAlive</key><true/>
	<key>Label</key><string>com.sn0wbite.sn0shell</string>
	<key>ProcessType</key><string>Interactive</string>
	<key>Program</key><string>/sbin/sn0shell</string>
	<key>RunAtLoad</key><true/>
	<key>StandardErrorPath</key><string>/dev/console</string>
	<key>StandardInPath</key><string>/dev/console</string>
	<key>StandardOutPath</key><string>/dev/console</string>
</dict>
</plist>
PL
sudo chown 0:0 "${PLIST}"
sudo chmod 0644 "${PLIST}"

sudo rm -f "${MOUNT}/System/Library/LaunchDaemons/com.jprx.bash.plist"
ok "ramdisk patched: /sbin/sn0shell + com.sn0wbite.sn0shell.plist"

sudo hdiutil detach "${MOUNT}" >/dev/null
trap - EXIT

step 4 5 "rebuild trust cache with sn0shell cdhash"
python3 "${SCRIPT_DIR}/build-tc.py" \
    "${FW_DIR}/ramdisk.pre_sn0shell.tc" \
    "${SCRIPT_DIR}/sn0shell" \
    "${FW_DIR}/ramdisk.tc" | sed "s/^/    /"
ok "ramdisk.tc refreshed"

step 5 5 "summary"
CDHASH=$(codesign -dvvv "${SCRIPT_DIR}/sn0shell" 2>&1 | awk -F= '/^CDHash=/{print $2}')

finalbox "✦ sn0shell deployed" \
    "ramdisk=${FW_DIR}/ramdisk.dmg" \
    "trustcache=${FW_DIR}/ramdisk.tc" \
    "cdhash=${CDHASH}" \
    "backups=ramdisk.pre_sn0shell.{dmg,tc}" \
    "next step=tools/watchos/qemu-sptm/boot.sh"
