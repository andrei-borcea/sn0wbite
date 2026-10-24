# sn0wbite power menu

`MenuActivity` and `Tile` are the app (four tiles: Restart, Recovery, Fastboot, Power off). `powerkey.c` is the root watcher that opens it on
a POWER hold. `module/` and `build_module.sh` make the Magisk module `sn0wbite_powermenu`. Design and findings:
[Hardware buttons](../../../docs/device/hardware-buttons.md).

```sh
./build.sh                                                  # Android SDK build-tools 34, signs powermenu.apk with a key generated in out/
./build_module.sh <Magisk-v30.7.apk> sn0wbite_powermenu.zip # needs the Android NDK
```

The module needs the app and the privileged-permission whitelist to match exactly (see the doc). Keep the signing key private: it is the
identity of the priv-app.
