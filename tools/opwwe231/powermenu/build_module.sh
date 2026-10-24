#!/bin/bash
# Build the sn0wbite_powermenu Magisk module zip: the app as a priv-app, its permission whitelist and the key watcher.
# Needs ./build.sh done (powermenu.apk), the Android NDK, and the official Magisk APK for the installer script.
# usage: build_module.sh <Magisk-v30.7.apk> <out zip>
set -eu
MAGISK_APK=$(cd "$(dirname "${1:?path to the official Magisk-v30.7.apk}")" && pwd)/$(basename "$1")
OUT=$(cd "$(dirname "${2:?output zip}")" && pwd)/$(basename "$2")
cd "$(dirname "$0")"
NDK=${NDK:-/opt/homebrew/share/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin}
T=$(mktemp -d)
cp -R module/. "$T/"
mkdir -p "$T/bin" "$T/system/priv-app/Sn0wPowerMenu" "$T/META-INF/com/google/android"
"$NDK/armv7a-linux-androideabi21-clang" -static -Os -Wall -o "$T/bin/powerkey" powerkey.c
"$NDK/llvm-strip" "$T/bin/powerkey"
cp powermenu.apk "$T/system/priv-app/Sn0wPowerMenu/Sn0wPowerMenu.apk"
unzip -p "$MAGISK_APK" assets/module_installer.sh > "$T/META-INF/com/google/android/update-binary"
echo "#MAGISK" > "$T/META-INF/com/google/android/updater-script"
rm -f "$OUT"
(cd "$T" && zip -q -r -X "$OUT" .)
unzip -l "$OUT" | tail -12
shasum -a 256 "$OUT"
