#!/bin/bash
# Patch an init_boot.img for root with Magisk, on the watch itself.
#
# OPWWE231 has an arm64 kernel and a 32-bit-only (armeabi-v7a) userspace. Magisk run on a paired phone patches with arm64
# binaries, and the resulting image panics the kernel at boot. boot_patch.sh run on the watch with the armeabi-v7a binaries from
# the official APK produces a working image. boot_patch.sh and util_functions.sh are the unmodified upstream scripts.
#
# usage: WATCH_ADB_SERIAL=<serial> patch_init_boot_on_watch.sh Magisk-v30.7.apk init_boot.img out.img
set -eu
APK=${1:?path to the official Magisk-v30.7.apk}
IN=${2:?baseline init_boot.img}
OUT=${3:?output image}
A=${WATCH_ADB_SERIAL:?set WATCH_ADB_SERIAL to the serial shown by adb devices}
EXPECT=e0d32d2123532860f97123d927b1bb86c4e08e6fd8a48bfc6b5bee0afae9ebd5
[ "$(shasum -a 256 "$APK" | awk '{print $1}')" = "$EXPECT" ] || { echo "APK hash mismatch: use the official Magisk v30.7 release"; exit 1; }
T=$(mktemp -d)
for p in "lib/armeabi-v7a/libmagisk.so:magisk" "lib/armeabi-v7a/libmagiskboot.so:magiskboot" "lib/armeabi-v7a/libmagiskinit.so:magiskinit" \
         "lib/armeabi-v7a/libinit-ld.so:init-ld" "assets/stub.apk:stub.apk" "assets/boot_patch.sh:boot_patch.sh" "assets/util_functions.sh:util_functions.sh"; do
  unzip -p "$APK" "${p%%:*}" > "$T/${p##*:}"
done
D=/data/local/tmp/magisk_patch
adb -s "$A" shell "rm -rf $D; mkdir -p $D"
adb -s "$A" push "$T/." "$D/" >/dev/null
adb -s "$A" push "$IN" "$D/init_boot.img" >/dev/null
adb -s "$A" shell "cd $D && chmod 755 magiskboot magiskinit magisk && BOOTMODE=false sh ./boot_patch.sh init_boot.img"
adb -s "$A" pull "$D/new-boot.img" "$OUT"
file "$OUT" | head -1
echo "patched image written to $OUT; flash it with: fastboot flash init_boot $OUT"
