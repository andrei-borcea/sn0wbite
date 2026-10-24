#!/bin/bash
# Read the sn0wbite recovery flight recorder from misc (offset 0x9000). Watch must be in Android with root.
# Usage: read_flightlog.sh [--clear]
A=${WATCH_ADB_SERIAL:?set WATCH_ADB_SERIAL to the serial shown by adb devices}
adb -s "$A" exec-out 'su -c "dd if=/dev/block/by-name/misc bs=1 skip=36864 count=24008 2>/dev/null"' | python3 -I -c '
import struct, sys
b = sys.stdin.buffer.read()
if len(b) < 8 or b[:4] != b"SN0L":
    print("(flight recorder empty: the recovery menu never ran, or the log was cleared)"); sys.exit(0)
n = struct.unpack("<I", b[4:8])[0]
sys.stdout.write(b[8:8 + n].decode("latin1"))'
if [ "$1" = "--clear" ]; then
  adb -s "$A" shell 'su -c "dd if=/dev/zero of=/dev/block/by-name/misc bs=1 seek=36864 count=8 conv=notrunc 2>/dev/null; sync"' && echo "(flight recorder cleared)"
fi
