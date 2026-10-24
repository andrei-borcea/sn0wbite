#!/bin/bash
# Put the verified stock recovery back. Works from Android (root) or from fastboot. Verifies before and after.
set -u
A=${WATCH_ADB_SERIAL:?set WATCH_ADB_SERIAL to the serial shown by adb devices}
IMG=${STOCK_RECOVERY_DUMP:?set STOCK_RECOVERY_DUMP to your own dump of the stock recovery partition}
EXP=${STOCK_RECOVERY_SHA256:?set STOCK_RECOVERY_SHA256 to the sha256 you recorded when you took the dump}
got=$(shasum -a 256 "$IMG" | awk '{print $1}')
[ "$got" = "$EXP" ] || { echo "dump hash mismatch ($got): refusing to flash"; exit 1; }
if adb -s "$A" get-state 2>/dev/null | grep -q device; then
  adb -s "$A" push "$IMG" /data/local/tmp/recovery_stock.img >/dev/null &&
  adb -s "$A" shell 'su -c "dd if=/data/local/tmp/recovery_stock.img of=/dev/block/by-name/recovery bs=4096 conv=notrunc 2>&1 | tail -1; sync"' &&
  echo "readback: $(adb -s "$A" shell 'su -c "sha256sum /dev/block/by-name/recovery"' | awk '{print $1}')"
elif fastboot devices | grep -q .; then
  fastboot flash recovery "$IMG" && fastboot reboot
else
  echo "watch not visible in adb or fastboot"; exit 2
fi
echo "expected: $EXP"
