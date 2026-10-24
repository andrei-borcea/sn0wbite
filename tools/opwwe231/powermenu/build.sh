#!/bin/bash
# Build and sign powermenu.apk with the Android SDK build tools (brew install --cask android-commandlinetools, then
# sdkmanager "build-tools;34.0.0" "platforms;android-34"). The signing key is generated once in out/ and is not published.
set -eu
cd "$(dirname "$0")"
SDK=${ANDROID_SDK_ROOT:-/opt/homebrew/share/android-commandlinetools}
BT=$SDK/build-tools/34.0.0
JAR=$SDK/platforms/android-34/android.jar
rm -rf out/classes out/*.apk out/classes.dex; mkdir -p out/classes
javac --release 8 -Xlint:-options -g:none -cp "$JAR" -d out/classes src/com/sn0wbite/powermenu/*.java
"$BT/d8" --lib "$JAR" --min-api 30 --output out out/classes/com/sn0wbite/powermenu/*.class
"$BT/aapt2" link -o out/base.apk -I "$JAR" --manifest AndroidManifest.xml --min-sdk-version 30 --target-sdk-version 34
(cd out && zip -q -j base.apk classes.dex)
"$BT/zipalign" -f 4 out/base.apk out/aligned.apk
[ -f out/debug.keystore ] || keytool -genkeypair -keystore out/debug.keystore -storepass sn0wbite -keypass sn0wbite -alias sn0wbite -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=sn0wbite" >/dev/null 2>&1
"$BT/apksigner" sign --ks out/debug.keystore --ks-pass pass:sn0wbite --key-pass pass:sn0wbite --out powermenu.apk out/aligned.apk
"$BT/apksigner" verify powermenu.apk && ls -l powermenu.apk
