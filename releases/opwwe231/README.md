# OnePlus Watch 2 (OPWWE231) releases

Modified images and a module built for **OPWWE231, build A.192.260910**, unlocked bootloader. They are published as-is, for
research. **Flashing can leave the watch unbootable.** Take your own backup of the partitions first, and read
[Custom recovery](../../docs/custom-boot/custom-recovery.md) before flashing the recovery.

| File | Size | SHA-256 | What it is |
| --- | --- | --- | --- |
| `init_boot_magisk-v30.7_arm32.img` | 8,388,608 B | `5dd5976fdf5eb3846f362d16098c170f404359c75572a4312623467a8d1f0e47` | Magisk v30.7 root, patched on the watch with the 32-bit binaries. Built from the A.144 `init_boot` of this device (Google-hosted OTA) and verified to boot the A.192 system. This is the image running on our unit |
| `recovery_sn0wmenu_v9b.img` | 11,767,808 B | `28c157979366cc392d494cb152c63029b98f96c4fb074e23ca4c27b2973ebf2b` | Recovery with the sn0wbite boot menu (Wear OS, dual boot, stock recovery, fastboot, info, power off) and the verbose window. Built from the stock A.192 recovery ramdisk: two rc files changed, `sn0wmenu` and its targets file added, 91 cosmetic files removed |
| `sn0wbite_bootanim_v2.0.zip` | 6,898,425 B | `faa68491a10d8aa505f76010a78268e55ad2d05bfb6b974b39bdba36b27cb450` | Magisk module: static logo with the kernel log scrolling live during Wear OS boot ([how](../../docs/custom-boot/boot-screen.md)) |

Verify with `shasum -a 256 -c SHA256SUMS`.

## Use

```sh
# root (needs the unlocked bootloader; the image is padded to the 8 MiB partition)
adb reboot bootloader && fastboot flash init_boot init_boot_magisk-v30.7_arm32.img && fastboot reboot

# recovery menu: flash right before use, Android restores the stock recovery on its next boot
export WATCH_ADB_SERIAL=<serial> STOCK_RECOVERY_DUMP=<your dd dump of the recovery partition>
tools/opwwe231/recovery-menu/flash_recovery.sh recovery_sn0wmenu_v9b.img && adb reboot recovery
# from fastboot, `fastboot reboot recovery` re-enters the same image

# boot screen: install the zip from the Magisk app, then reboot
```

The recovery must stay below 12,083,200 bytes (its AVB metadata follows the image). To return to stock, flash your own backup
of `init_boot` and `recovery` (`restore_stock_recovery.sh` for the recovery).

## What is not here

- **No bootloader or boot ROM changes.** `abl`, `xbl` and the other boot-chain partitions are untouched on our unit, and we
  have no access to the SoC boot ROM. Nothing to release.
- No stock partition images and no personal data: our backups stay private ([policy](../../docs/evidence-policy.md)).

## Provenance and licenses

The images contain the stock ramdisk of this device's firmware, copyright OnePlus, Google and their suppliers, plus our changes;
they are included for interoperability research and are not endorsed by those companies. Magisk is GPLv3 and its binaries
come unmodified from the official v30.7 APK (SHA-256 `e0d32d21...ebd5`). The project sources under `tools/` are ours.
