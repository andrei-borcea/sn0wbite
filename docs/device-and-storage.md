# Watch 2: device identity, ADB and storage

Inspection date: **20 October 2026**. Target: **OnePlus Watch 2, OPWWE231 A.192**.

Public evidence: [device properties](../research/opwwe231/2026-10-20-a192/device.json),
[ADB excerpts](../research/opwwe231/2026-10-20-a192/adb-excerpts.txt),
[fastboot excerpts](../research/opwwe231/2026-10-20-a192/fastboot-excerpts.txt),
and [all 63 partition mappings and sizes](../research/opwwe231/2026-10-20-a192/partitions.csv).

## Device baseline

We use the observed model and installed build to select firmware. Images and
instructions for other models, including `OPWWE234` and `OPWWE251`, need separate
compatibility evidence before they can apply here.

| Component | Observed baseline |
| --- | --- |
| Device | OnePlus Watch 2, `OPWWE231`; OHealth reports BLACK STEEL |
| Main platform | Qualcomm Snapdragon W5 Gen 1 / `monaco` |
| Android | 14 / SDK 34 |
| Build | `OPWWE231_11_A.192.260910` |
| Security patch | `2026-08-01` |
| Architecture | ARM64 kernel, 32-bit Android userspace (`armeabi-v7a`, `armeabi`) |
| Boot security | Locked bootloader, verified boot green, SELinux enforcing |
| ADB privileges | UID 2000 (`shell`); production build, no usable `su` |
| Storage | eMMC `mmcblk0`, 63 named physical partitions; separate `boot0` and `boot1` areas |

Bootloader metadata confirms these partition sizes. Their contents on the
installed A.192 build remain unread:

| Partition | Size | Role documented in historical Watch 2 research |
| --- | --- | --- |
| `boot` | 96 MiB | ARM64 kernel |
| `init_boot` | 8 MiB | Generic initial ramdisk |
| `vendor_boot` | 96 MiB | Vendor ramdisk and device-specific boot data |
| `recovery` | 68 MiB | Recovery ramdisk |

The [Watch 2 porting notes](https://wiki.postmarketos.org/wiki/OnePlus_Watch_2_%28oneplus-opwwe231%29)
describe boot image header v4. We still need the current images to verify their
layout against this firmware.

## Backup and root investigation

The immediate objective is to preserve the watch's existing state before
changing boot partitions. Current access does not allow a complete acquisition:

- `adb root` rejects the production build. The shell cannot read the eMMC block
  devices, protected OTA cache, or tested logical block devices.
- Fastboot is reachable through ADB. An attempted `fetch init_boot` failed
  because the device does not support the required fetch capability. The
  resulting empty file contains no image data.
- Magisk would require a compatible stock image and a supported way to boot or
  flash the patched image. We have neither a verified A.192 image nor a root
  method that preserves the current user state. Bootloader unlocking entails a
  factory reset.

A complete acquisition must cover the main eMMC area, both GPT copies,
unallocated sectors, and `boot0`/`boot1`, with measured lengths and SHA-256
digests. Writable partitions need a coherent snapshot. Companion MCU/RTOS
storage requires separate investigation; secure storage, RPMB and device-bound
keys may limit what we can copy and restore.

We must verify the writing and recovery mechanism as well as the dump. A set of
readable images alone does not establish that we can restore a bricked device.

## Partition inventory boundaries

We joined the ADB by-name mappings with fastboot-reported sizes and types by
exact partition name. All 63 names match between those sources. The CSV includes
partition numbers, block paths and byte sizes. We have not read the GPT sectors,
so it contains no inferred start offsets or sector map. The eMMC hardware areas
`boot0` and `boot1` are separate from the 63 numbered partitions; their lengths
remain unknown.

The earlier investigation entered fastboot and returned to Android. We performed
this publication offline using saved evidence. We did not unlock the bootloader,
flash partitions or erase the device.

## Compare identity and access over ADB

Use the locally connected watch's selector in `WATCH_ADB_SERIAL`. These examples
query selected properties through an already authorized ADB connection:

```sh
adb -s "$WATCH_ADB_SERIAL" shell getprop ro.product.model
adb -s "$WATCH_ADB_SERIAL" shell getprop ro.build.fingerprint
adb -s "$WATCH_ADB_SERIAL" shell getprop ro.build.version.security_patch
adb -s "$WATCH_ADB_SERIAL" shell getprop ro.product.cpu.abilist
adb -s "$WATCH_ADB_SERIAL" shell getprop ro.boot.verifiedbootstate
adb -s "$WATCH_ADB_SERIAL" shell id
adb -s "$WATCH_ADB_SERIAL" shell getenforce
adb -s "$WATCH_ADB_SERIAL" shell 'command -v su'
adb -s "$WATCH_ADB_SERIAL" shell ls -l /dev/block/by-name
```

The [captured ADB errors](../research/opwwe231/2026-10-20-a192/adb-excerpts.txt)
show the historical failed root request and storage reads. The excerpts preserve
source line numbers. They do not constitute a complete interactive session.

See [recovery research](./recovery.md) for the remaining restoration requirements
and the [evidence policy](./evidence-policy.md) for publication boundaries.
