#!/bin/bash
# Reboot into recovery (one-shot). Watch must be in Android. v9 and later need nothing reset first: the menu zeroes the BCB itself
# as its first action. Run it right after flash_recovery.sh (Android rebuilds the stock recovery at its next boot).
A=${WATCH_ADB_SERIAL:?set WATCH_ADB_SERIAL to the serial shown by adb devices}
adb -s "$A" reboot recovery
