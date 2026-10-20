# OTA history and OHealth

Inspection date: **20 October 2026**. Target: **OPWWE231 A.192**.

Public evidence: [selected recovery output](../research/opwwe231/2026-10-20-a192/recovery-update-excerpts.txt)
and [OTA/OHealth findings](../research/opwwe231/2026-10-20-a192/ota-and-ohealth.json).

## Update investigation

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

## Static analysis and runtime observations

We used focused `apkanalyzer` disassembly of the copied applications to trace
the relevant paths. These findings describe the inspected versions; class names
and behavior can change in later releases.

| Component | Inspected version | Relevant path |
| --- | --- | --- |
| Watch Google Play services | `26.40.32 (264032086)` | `UpdateDownloadHandler` passes the update URL to a watch-side HTTP downloader and local file provider. |
| Phone OHealth | `4.62.7_9b9749e_260920` (`46207`) | `OTAUpdateActivity`, `OTACheckManager` and `MeTabAdapter` distinguish an explicit refresh, background checks and the Device-tab entry. |

| Check path | `forceRefresh` | `autoDownload` | Evidence boundary |
| --- | --- | --- | --- |
| Internal OHealth OTA page | `true` | `false` | Static code; no supported entry or execution established for this watch. |
| Background resume/connection check | `false` | `true` | Static code with preference, connection and network guards; restarting the app is not a proven check-only operation. |

The Device-tab update entry has type `40`; More has type `41`. The captured
phone UI omitted type `40`, consistent with the Wear OS gate in the inspected
code. We found no registered OTA deep link for this path. Static page discovery
does not establish that the paired device can invoke it.

The private cache path follows
`/data/user/0/com.heytap.health.international/files/device_ota/<MD5(URL)>.bin`.
This describes a filename algorithm, not a recovered firmware URL. Direct access
was denied and `run-as` rejected the production application.

## Historical phone-log coverage

Offline analysis covered 48 closed main/transport log files from 19 October.
We decoded 102,758 valid JSON records, retained 39 OTA-related records locally,
and found no firmware URL. Of 580 frames, 572 had complete gzip CRC validation;
eight were partial. Four proprietary OHealth checks reported no new version.
Those responses do not describe the separate Google Wear OS OTA offer.

The public export contains aggregate counts and technical conclusions. Original
log records, usage times, health data and decoder key material remain private.

The recovery excerpt includes an intermediate checksum warning together with the
subsequent patch result and successful verification. Its `5b7df291` value is only
an image checksum prefix; it cannot identify the OTA ZIP or reconstruct its URL.
