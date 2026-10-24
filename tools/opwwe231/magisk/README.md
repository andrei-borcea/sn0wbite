# Magisk on OPWWE231

`patch_init_boot_on_watch.sh` patches an `init_boot.img` on the watch with the **armeabi-v7a** binaries of the official Magisk
v30.7 APK. The scripts Magisk runs (`boot_patch.sh`, `util_functions.sh`) are identical to upstream; what makes the image boot is
the architecture of the binaries. See [Root and boot customization](../../../docs/device/root-and-boot-customization.md) for the
failure the phone-side patch causes. Magisk is GPLv3: <https://github.com/topjohnwu/Magisk>.

The published result is `init_boot_magisk-v30.7_arm32.img` in [releases/opwwe231](../../../releases/opwwe231/README.md).
