#!/bin/bash
# Safe flash of a custom recovery image from Android (root). Usage: flash_recovery.sh [--check] <image>
#  - refuses images that would reach the AVB metadata stored after the stock image (see docs/custom-recovery-and-boot-screen.md)
#  - verifies the transfer, the write, and that everything after the image still equals the verified backup
set -u
CHECK=0; [ "${1:-}" = "--check" ] && { CHECK=1; shift; }
IMG=${1:?usage: flash_recovery.sh [--check] <image>}
A=${WATCH_ADB_SERIAL:?set WATCH_ADB_SERIAL to the serial shown by adb devices}
BACKUP=${STOCK_RECOVERY_DUMP:?set STOCK_RECOVERY_DUMP to your own full dump of the recovery partition (dd of /dev/block/by-name/recovery), taken before the first flash}
LIMIT=12083200; MARGIN=65536
SZ=$(stat -f %z "$IMG" 2>/dev/null || stat -c %s "$IMG")
[ $((SZ % 4096)) -eq 0 ] || { echo "REFUSED: size $SZ is not a multiple of 4096"; exit 1; }
[ "$SZ" -le $((LIMIT - MARGIN)) ] || { echo "REFUSED: $SZ B exceeds the safe size $((LIMIT - MARGIN)) B"; exit 1; }
BLK=$((SZ / 4096))
adb -s "$A" get-state 2>/dev/null | grep -q device || { echo "watch not in Android/adb"; exit 2; }
HOST=$(shasum -a 256 "$IMG" | awk '{print $1}')
TAIL_BAK=$(dd if="$BACKUP" bs=4096 skip=$BLK 2>/dev/null | shasum -a 256 | awk '{print $1}')
tail_dev() { adb -s "$A" shell "su -c 'dd if=/dev/block/by-name/recovery bs=4096 skip=$BLK 2>/dev/null | sha256sum'" | awk '{print $1}'; }
echo "image $SZ B ($BLK blocks)  sha256 $HOST"
PRE=$(tail_dev); echo "tail before : $PRE"; echo "tail backup : $TAIL_BAK"
[ "$PRE" = "$TAIL_BAK" ] || { echo "ABORT: partition tail differs from the verified backup; restore the stock image first"; exit 3; }
[ $CHECK -eq 1 ] && { echo "check ok: nothing written"; exit 0; }
adb -s "$A" push "$IMG" /data/local/tmp/recovery_flash.img >/dev/null
DEV=$(adb -s "$A" shell 'sha256sum /data/local/tmp/recovery_flash.img' | awk '{print $1}')
[ "$DEV" = "$HOST" ] || { echo "ABORT: transfer mismatch, nothing written"; exit 4; }
adb -s "$A" shell 'su -c "dd if=/data/local/tmp/recovery_flash.img of=/dev/block/by-name/recovery bs=4096 conv=notrunc 2>&1 | tail -1; sync"'
HEAD=$(adb -s "$A" shell "su -c 'dd if=/dev/block/by-name/recovery bs=4096 count=$BLK 2>/dev/null | sha256sum'" | awk '{print $1}')
POST=$(tail_dev)
echo "head readback: $HEAD"; echo "tail after   : $POST"
if [ "$HEAD" = "$HOST" ] && [ "$POST" = "$TAIL_BAK" ]; then echo "RESULT: OK image written and AVB metadata untouched"
else echo "RESULT: MISMATCH -> run restore_stock_recovery.sh"; exit 5; fi
