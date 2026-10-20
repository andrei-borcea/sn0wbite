<p align="center">
  <img src="./sn0wbite-banner-1280x400.png" alt="sn0wbite: green ASCII apple logo and project wordmark. One bite is all it takes." width="1280">
</p>

# sn0wbite

**sn0wbite** is an experimental reverse engineering project investigating
whether watchOS can boot on the **OnePlus Watch 2**. The first boot milestone is
repeatable execution of the watchOS kernel, with diagnostic traces showing the
execution stage reached. A usable interface and working peripherals come later.

## Research status

**Device and firmware investigation, as of 20 October 2026.** Native watchOS
compatibility remains unproven. The repository contains documentation and brand
assets; we have not implemented a loader or demonstrated a watchOS boot.

| Area | Findings |
| --- | --- |
| Device identity | ADB and OHealth identify **Watch 2, OPWWE231**. The initial Watch 2R identification was incorrect. |
| Installed firmware | Android 14, build **OPWWE231_11_A.192.260910**. |
| Storage access | ADB shell works; root and raw block-device reads are unavailable. |
| Complete backup | **Incomplete.** We have acquired diagnostics and metadata, but no raw storage image. |
| Restoration | **Unverified.** Public EDL candidates do not establish a working unbrick procedure for this device. |
| Apple reference | watchOS **6.3.1 / 17U224**, OTA for **Watch5,4 / N146bAP**. |

## Device baseline

We use the observed model and installed build to select firmware. Instructions
and images for Watch 2R (`OPWWE234`) or Watch 3 (`OPWWE251`) need separate
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

## OTA and companion-phone findings

Recovery logs identify the most recent update as **A.190.260521 → A.192.260910**.
The updater used `/cache/update.zip`, patched `init_boot`, and completed an MCU
firmware update. Permission errors prevent checking whether the ZIP remains in
the cache. We have recovered the installation log, not the package.

A native forced update check completed without returning a download URL.
Targeted Google update-protocol requests and public archive searches also failed
to recover A.192. These results do not establish that the package is unavailable
through all OEM or account-specific channels.

We inspected the paired phone's **OHealth 4.62.7** application and selected local
logs:

- The watch's Google Play services manage the Wear OS HTTP download and its
  destination file. The phone can provide network connectivity over Bluetooth.
- OHealth contains an internal OTA page with a forced-refresh option, but its
  normal UI excludes that entry for this Wear OS device. We have not established
  a supported way to invoke it and download the installed build again.
- Selected historical logs contained no firmware URL. The phone also denies
  access to OHealth's private OTA cache, so its contents remain unknown.

Connecting the phone has not provided privileged access to the watch's storage
or a complete firmware backup.

### Public firmware candidates

| Candidate | What we verified | Remaining limits |
| --- | --- | --- |
| [Google-hosted full A.144 OTA](https://android.googleapis.com/packages/ota-api/package/690ef27e9b28ab76add006e2c35b9fed045fa6da.zip) | Metadata read from the actual ZIP identifies Android 13 / A.144. | Older than the installed firmware; no validated downgrade path. We did not download the full archive. |
| [Historical OPWWE231 dump](https://dumps.tadiphone.dev/dumps/oneplus/opwwe231/-/tree/monaco-user-13-TWR7.231113.001-941-release-keys?ref_type=heads) | Android 13 branch; repository lists `boot.img`, `init_boot.img` and `vendor_boot.img`. | Useful for study; neither an A.192 image set nor a complete backup of this watch. |
| [Service firmware listing, OWWE231 A.117](https://romprovider.com/oneplus-watch-2-owwe231-firmware-stock-rom/) | The page lists an EDL package and requires a OnePlus Service Center ID. | Package contents, internal model identity, loader acceptance and rollback compatibility remain unverified. |
| [QLM OPWWE231 OTA listing](https://qlmflasherpro.com/index.php?a=downloads&b=file&id=23272) | The listing names OPWWE231 and advertises a 1.61 GB OTA. Download requires login. | No actual build or package hash available from the listing; archive contents uninspected. |

An older `boot.img` or `init_boot.img` is not interchangeable with the current
build. An incremental OTA may also require the exact source image to reconstruct
its target. The `OWWE231` service-package name alone neither proves nor rules
out compatibility with `OPWWE231`; that requires internal metadata.

## Recovery and brick research

We have not found a verified public full-restoration procedure for **OPWWE231
A.192**. Recovery options depend on which boot stages still work:

| Device state | Possible route | Evidence and constraints |
| --- | --- | --- |
| Frozen software | Hold Home and Multifunction together for 12 seconds. | The [manufacturer manual](https://fccid.io/2ABZ2-OPWWE231/User-Manual/Users-Manual-7123415.pdf) documents a forced restart. This does not reinstall firmware. |
| Fastboot reachable | Restore compatible original images if the bootloader permits writing. | The current bootloader is locked. We have no validated complete image set or restoration recipe. |
| Android and recovery unavailable | Investigate Qualcomm EDL service recovery. | Device-specific entry, accepted programmer, authentication, firmware compatibility and successful restoration remain unverified. |

The author of the [OPWWE231 porting notes](https://wiki.postmarketos.org/wiki/OnePlus_Watch_2_%28oneplus-opwwe231%29)
reports a brick after flashing a broken recovery, with no ADB or known manual
route back to fastboot. The same author reports `fastboot boot` as unsupported.
This is a historical individual report; current A.192 behavior needs separate
verification.

We excluded apparent successes that concerned a paired phone, Watch 2R, or
Watch 3. A public “Watch 2 Firehose” filename also does not establish a usable
programmer: we have not validated the downloaded candidate's format, hardware
compatibility or acceptance by the watch.

A factory reset erases user data without reinstalling corrupted boot partitions.
If local recovery proves unavailable, [OnePlus support](https://www.oneplus.com/it/support/repair)
must assess service eligibility and repair options. The repair page warns that
service erases data.

## watchOS reference and feasibility

We identified the supplied Apple artifact from its package manifests:

| Field | Value |
| --- | --- |
| watchOS version | `6.3.1` |
| Build | `17U224` |
| Supported device | `Watch5,4` |
| Hardware model | `N146bAP` |
| Package type | `com.apple.MobileAsset.SoftwareUpdate`, update OTA |

Archive SHA-256:

```text
56f2e12ad0eb9ff45858786d151d9eda9667f9c2fab26d8af961b1cbafb24f84
```

This artifact is an Apple-device update package. We have not established that
its payload is self-contained or verified all archive CRCs and Apple signatures.

A shared ARM instruction set and sufficient RAM do not establish watchOS
compatibility. Research still needs to address Apple boot verification, kernel
platform assumptions, memory layout, interrupts, timers and device drivers.
Display, touch, sensors, storage and companion-processor communication all need
hardware support. We have no demonstrated execution path from the Qualcomm boot
chain into this watchOS kernel.

## Roadmap

- [x] Identify the attached watch and record its installed firmware and security state.
- [x] Inventory partition names and bootloader-reported sizes.
- [x] Identify the supplied watchOS reference package and record its archive hash.
- [ ] Obtain privileged storage reads while preserving the current device state.
- [ ] Acquire and verify complete storage images, including companion-firmware coverage.
- [ ] Establish a documented restoration path for this exact device.
- [ ] Analyze watchOS boot stages, kernel format and Apple hardware dependencies.
- [ ] Establish controlled payload execution and diagnostic output on the watch.
- [ ] Demonstrate repeatable watchOS kernel execution with traces and reproduction steps.
- [ ] Bring up the interface, peripherals and power management as kernel progress allows.

## Evidence and reproducibility

Local acquisition records include tool output, manifests, package analysis and
SHA-256 inventories. They remain under the ignored `backups/` directory because
device acquisitions can contain personal data and device-specific secrets. This
README publishes findings and public sources without redistributing those logs
or proprietary firmware.

For each new finding, record the model, build, acquisition method, tool version
and supporting evidence. Keep device observations distinct from historical
reports and hypotheses. Boot experiments must identify image hashes, payload
source revision and the last confirmed execution stage.

## Brand assets

The header uses the supplied project banner. Other formats:
[vector logo](./sn0wbite-logo.svg), [vector mark](./sn0wbite-mark.svg),
[PNG logo](./sn0wbite-logo-800.png), [avatar](./sn0wbite-avatar-512.png),
and [social preview](./sn0wbite-social-preview-1280x640.png).

## Disclaimer

This project is for educational and research purposes. It has no affiliation
with Apple Inc. or OnePlus. watchOS is a trademark of Apple Inc.

Modifying or flashing firmware can erase data or brick the device.
Use at your own risk.
