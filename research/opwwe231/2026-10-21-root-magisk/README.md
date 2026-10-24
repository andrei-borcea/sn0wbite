# OPWWE231 A.192: root and boot customization, 21 October 2026

Target: **OnePlus Watch 2**, Android 14, build **A.192.260910**, bootloader
transitioned from **locked** (as recorded in the
[20 October collection](../2026-10-20-a192/README.md)) to **unlocked
(orange)** during this session.

Full narrative: [Root access and boot customization](../../../docs/device/root-and-boot-customization.md).

| File | Contents |
| --- | --- |
| [magisk-architecture-bug-excerpts.txt](./magisk-architecture-bug-excerpts.txt) | The arm64-vs-arm32 Magisk patching bug: device ABI, `file` output on both the broken and fixed `/init`, and the resulting boot failure |
| [boot-patch-on-device-excerpt.txt](./boot-patch-on-device-excerpt.txt) | Full output of Magisk's own `boot_patch.sh`, run unmodified on-device, plus the resulting root confirmation |
| [bootanimation-plus-discovery-excerpt.txt](./bootanimation-plus-discovery-excerpt.txt) | The logcat line identifying this device's proprietary boot animation path |
| [system-backup-manifest.csv](./system-backup-manifest.csv) | Name, size and SHA-256 for all 62 acquired non-`userdata` partitions |
| [provenance.json](./provenance.json) | Private source-file hashes and excerpt derivation notes |
| [SHA256SUMS](./SHA256SUMS) | SHA-256 digests for this public collection |

## What changed on the device

- Bootloader: locked → unlocked (`fastboot flashing unlock`; wipes user data).
- Root: none → working, via Magisk, confirmed with `su -c id` returning `uid=0(root)`.
- `init_boot`: stock A.192 → a validated, genuine A.144 image (same device,
  confirmed via embedded OTA metadata and exact partition-size matches),
  Magisk-patched for root with the correct 32-bit ARM binaries.
- Boot animation: stock → project-branded, via a Magisk module targeting this
  device's actual (non-standard) animation path.
- Storage: a full, SHA-256-verified backup of every non-`userdata` partition
  now exists privately, giving this device a documented restoration path for
  the first time.

## What this does not include

No raw partition image, APK or firmware binary is published here, consistent
with the [evidence policy](../../../docs/evidence-policy.md). The acquired
`.img` files, the Magisk release APK, and the custom boot animation assets
remain in the gitignored `backups/` directory referenced by `provenance.json`.
`userdata` was not acquired at all, by choice.

From this directory, verify the exported files with:

```sh
shasum -a 256 -c SHA256SUMS
```
