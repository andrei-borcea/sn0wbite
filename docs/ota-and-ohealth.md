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

### Follow-up probe, 20 October 2026

- `/cache` is mounted from `mmcblk0p16` (208 MB total, 7.1 MB used). A full OTA
  ZIP cannot fit, and the `/cache/update.zip` used for A.190 → A.192 is no longer
  occupying space. The shell user still cannot list the directory, so this is
  inferred from `df`, not from a listing.
- The A.190 → A.192 update was a patch-based package (`Patching init_boot image`).
  Even if recovered, it would be an incremental, not a complete firmware image.
- `ro.build.version.ota` is `OPWWE231_11.A.192_1920_202609102221`; region mark
  `EUEX`. These are search keys, not download locations.
- `dumpsys system_update` returns nothing; logcat holds only current-session
  GMS `SystemUpdate` events and no update URL.
- The phone's public OHealth directory (`Android/data/.../files`) is empty.
- Web searches for the OTA string and A.192 found no package or URL, only
  coverage of earlier builds.

### Two separate update paths (static analysis, 20 October 2026)

| | Phone-proxied path (OHealth, OPPO/HeyTap protocol) | Watch path (Google Play services) |
| --- | --- | --- |
| Who downloads | **Phone**: `OTADownloadManager` → `OTAFileDownloader` (OkHttp), checks `md5`, then `OTADeviceTransManager` pushes it to the device over the OPLUS link service. | **Watch**: `com.google.android.gms` `SystemUpdateService` / `SystemUpdateWearableActivity` (26.40.32) downloads over HTTP. |
| Server query | `POST https://ifota-term-{eu,us,in}.coloros.com/post/Query_Update` (and `Query_Description`); host chosen from the account region by `pk.c`. Request and `resps` reply are encrypted JSON (`OppoOtaUtils`, `rf.a`). | Google check-in / OTA API; the URL appears only when an update is offered. |
| Result carries | `OTAVersion`: `firmwareUrl`, `md5`, `otaVersion`, `size`, `summary`, `forceUpdate`. | `android.googleapis.com/packages/ota-api/package/<hash>.zip`. |
| Request fields | `otaVersion`, `romVersion`, `productName`, device unique id, `mode`, `otaPkgType`, `androidVersion`, `colorOSVersion`, `registrationId`, `trackRegion`, `uRegion`, `language`. | Not analyzed. |
| Applies to Watch 2 | Not established. Four historical OHealth checks returned "no new version", but the Wear OS entry (type 40) is gated out of the UI. | Yes: the watch has no OEM OTA app. Only `HeyMcuLinkService` (MCU) exists beside GMS. |

The server returns a `firmwareUrl` only when it considers the reported version
outdated, so a current device receives no URL on either path. Phone-side files
land in OHealth's private storage, which is not readable without root.

The watch is on **Wi-Fi** (default network `WIFI`, validated), not on a
Bluetooth proxy. GMS therefore downloads directly; the phone is not in that data
path.

### Reproduced `Query_Update` request, 20 October 2026

We re-implemented the app's AES-CTR scheme (key/IV derived from a random
15-character token plus a 10-entry constant table embedded in the app; see
[`oppo_ota_query.py`](../oppo_ota_query.py))
and sent a real `Query_Update` request to `https://ifota-term-eu.coloros.com/post/Query_Update`
with the watch's own current version strings (`otaVersion` =
`ro.build.version.ota`, `romVersion` = `A.192`). The `imei` field must be a
non-empty placeholder or the server replies "imei is null" without running the
check; it does not appear to validate the value against a real device.

The server accepted the request (HTTP 200) and our decryption of its `resps`
field reproduced valid JSON on the first attempt, confirming the reconstructed
cipher. The decrypted result for the watch's real, current version:

```json
{"modules":[{"checkFailReason":"no new version","reminderType":0,"otaPkgType":1}],"msg":"SUCCEED","resultCode":1}
```

No `down_url` is returned, consistent with the Google-side result: a device
already on the latest known build is not offered a download link by this
channel either.

### Declaring an older version changes nothing

Using [`ota_check_menu.py`](../ota_check_menu.py),
we also sent two requests that declared `romVersion=A.190` (the watch's
previous build, per the recovery log) instead of its real current `A.192`:
once with `otaVersion` still containing the real build's `A.192` token (a
mistake caught by the tool's consistency check), and once with `otaVersion`
corrected to contain `A.190` (counter and timestamp left as the real current
build's, since neither is known for A.190; see the tool's own derivation
notes). All three requests — real A.192, and both A.190 variants — returned
the same result: HTTP 200, `msg:"SUCCEED"`, `checkFailReason:"no new version"`.

The server never returned a format or validation error for any version we
declared. Combined with the earlier static-analysis finding that the OTA
page's normal (type 40) entry is excluded from OHealth's UI for this Wear OS
device, the more likely explanation is that this endpoint does not carry a
firmware catalog for `OPWWE231` at all, and answers "no new version" as a
safe default for any product/version it does not recognize — not that A.192
is genuinely the latest entry in some catalog it does hold. We cannot fully
rule out that the exact `otaVersion` format matters (the `1920` token and the
timestamp's real meaning are still unconfirmed), but three consistent
negative results across different declared states make that explanation
less likely than "this path doesn't serve the Watch at all."

We did not try further variations (different build-counter guesses, a
completely different `otaVersion` shape, or a different `productName`/`otaPrefix`
token). Declaring a false device state to the vendor's server is already a
different action from an honest self-check, and the pattern above gives
diminishing reason to expect a different outcome from more guessing along
the same lines.

### Forcing a check through the real app and the real watch, 20 October 2026

We looked for a way to force a real check — not a synthetic one — on both
sides, using only the access already available (no root, locked bootloader).

**OHealth, direct activity launch — blocked.** `OTAUpdateActivity` and the
other OTA activities are declared without `android:exported` and without an
intent filter, so they default to not exported. Launching the real update
screen directly with `adb shell am start -n
com.heytap.health.international/com.heytap.health.device.ota.OTAUpdateActivity`
is rejected:
```
java.lang.SecurityException: Permission Denial: ... not exported from uid 10379
```
This holds even for the shell user on this production build; there is no adb
bypass for a non-exported activity here.

**OHealth, triggered by a real reconnect — confirmed negative, with real
device data.** `OTACheckManager` implements the device manager's connection
callback (`onConnect(mac, isReset, isOobeFinish)`); on a normal reconnect it
calls into the same `checkDeviceVersion` path we had been probing manually,
but using the real paired device's live `DMProto.ConnectDeviceInfo` (real
`deviceSku`, `deviceModel`, `deviceSoftVersion`, `deviceOtaVersion`), not our
guesses. We forced a reconnect with `adb shell svc bluetooth disable` then
`enable` on the phone — an ordinary, reversible action a user causes just by
walking out of Bluetooth range — and captured logcat:
```
OTACheckManager: Health_onConnect isReset: false, isOobeFinish: true: AC:*:9D:86
OTAVersionManager: Health_requestVersion fail with msg: no new version
OTACheckManager: Health_checkDeviceVersion--> there is no newer version from server
```
This is the real app, with its real internal state, making a real
`OppoOtaApiService` call and getting the same answer as our own reconstructed
client. It is the strongest confirmation we have that this channel has
nothing to offer for the watch's current, genuine state — not a property of
our guesses.

**Watch, Google system update screen — reachable and does a live check.**
`com.google.android.gms/.update.SystemUpdateWearableActivity` is registered
for the public `android.settings.SYSTEM_UPDATE_SETTINGS` action (the same
action Settings itself sends), so it opens with no permission error:
```
adb shell am start -a android.settings.SYSTEM_UPDATE_SETTINGS
```
Opening it fires a real Google Checkin call (`CheckinApiChimeraService
onGetService`) and reports `systemUpdateStatus=(0x000), percent=(-1.0)` —
idle, nothing to install. We confirmed this both visually (once the watch
was off its charging cradle, which otherwise shows a full-screen charging
animation over any foreground app) and in logcat:
```
L'orologio è aggiornato
Versione di Android: 14
Aggiornamento sicurezza Android: 1 agosto 2026
```
matching `ro.build.version.security_patch` exactly.

**The check is live, not cached.** Force-stopping GMS and reopening the
screen shows a real status transition and two full round trips, not a
single cached read:
```
GservicesProvider: Update with URI .../gservices/override   (x2)
Checkin: [CheckinApiChimeraService] onGetService              (x2)
SystemUpdate: systemUpdateStatus=(-1), percent=(0.0)          <- checking
SystemUpdate: systemUpdateStatus=(0x000), percent=(-1.0)      <- idle, nothing to install
```
We also tried proxying this traffic through a local mitmproxy instance
(system-wide `settings put global http_proxy`, plus the CA certificate
pushed to the watch) to read the actual request/response. CA certificate
installation proved unreliable to complete through adb alone on this
device's small-screen UI, and standard Android logcat at this verbosity
never logs the real destination host for this check even once the activity
and gservices processes are correctly identified — consistent with the
`UpdateUrlLogFormatter` redaction logic found in static analysis. We did
not pursue a full TLS interception further; Google's own checkin endpoints
commonly use certificate pinning that would defeat a device-installed CA
regardless of whether installation had succeeded.

**Why a genuine rollback isn't something we can produce from here.** Both
checks above decide based on the watch's *real, current* state — A.192. To
honestly test what the server offers a watch on an older build, the watch
itself would need to report an older build, which means either:
- flashing an actually older firmware image — circular, since recovering
  that image is the original goal and we don't have one; or
- intercepting and rewriting the Bluetooth link between the watch and the
  phone so the app receives a falsified `ConnectDeviceInfo` — a materially
  larger and separate reverse-engineering effort (defeating or sitting
  between two paired, authenticated devices' own protocol), not attempted.

Neither is possible with the current access (OEM unlock enabled, bootloader
still locked, no root). Short of one of those, every path available to us —
synthetic requests and the real app/real watch alike — reports the same,
consistent answer.

### What the watch itself does with an update (static analysis, 20 October 2026)

We reverse engineered `com.google.android.gms` 26.40.32, pulled from the
watch, to trace the real download/install/logging path. Selected evidence:
[`tools/../watch-update-mechanism-excerpts.txt`](../research/opwwe231/2026-10-20-a192/watch-update-mechanism-excerpts.txt).

- **Download target.** `UpdateDownloadHandler` writes the fetched package
  locally as `update.zip` (or `update_s.zip` for a streaming variant). This
  matches every recorded recovery log, which installs from
  `/cache/update.zip`.
- **Where an offered URL is stored.** `UpdateInstallationController`
  persists it under the key `control.installation.current_update_url`, and
  logs any change with `"Update URL changed from \"%s\" to \"%s\"."`.
- **The log line is filtered through a redaction helper
  (`UpdateUrlLogFormatter`).** Decompiled logic: if a resolved feature flag
  is off, the log shows the real, complete URL; if it's on, the log shows
  only `"..." + the last up-to-6 characters` of the URL's filename. We did
  not read that flag's live value, so whether this build's logcat would ever
  show a real update URL in full, or only a short masked fragment, is
  unresolved from static analysis alone.
- **A separate, already-verified, unredacted channel exists.** Independent
  of that log line, GMS also exposes the same kind of value as a plain
  `gservices` content-provider setting (`update_url`), which is not run
  through any log formatter. A small custom `BroadcastReceiver` app
  (`dev.sn0wbite.otareader`, source and build already in
  `backups/oneplus-watch-2/2026-10-20-inspection/init_boot/ota-reader-app/`,
  already built, installed, queried and uninstalled) reads it directly:
  ```
  content://com.google.android.gsf.gservices  key="update_url"
  -> {"update_url":null,"rows":1}
  ```
  `null` today, consistent with no update currently offered. This is the
  reliable way to capture a real, complete URL automatically the moment
  Google does offer one — it sidesteps the log-redaction question entirely,
  since it reads GMS's own stored value rather than a formatted log line.
- **Historical install count.** The watch's recovery logs record **11**
  past OTA installations (`last_log`, `last_log.1` through `last_log.10`),
  every one via `/cache/update.zip` — the same mechanism as the documented
  A.190 → A.192 update, not a one-off.

None of this changes the outcome already established: the watch is
currently on the latest build either system knows about, so there is
nothing queued to download. What it adds is the exact mechanism, and a
tested, working, unobtrusive way to catch the real URL automatically for
whatever build comes after A.192.

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

## Re-deriving the Query_Update request fields with jadx, 20 October 2026

After installing a proper decompiler (jadx-headless-mcp 0.7.1, fetched from
its upstream release and checksum-verified, configured via `REA_JADX_MCP_JAR`
for REA's Android tools), we re-read `e0.f` -- the exact method that builds
the OHealth request body -- as real Java pseudocode instead of smali. This
corrected several field assumptions our earlier smali-only reading had made:

| Field | What we assumed | What the decompiled code actually does |
| --- | --- | --- |
| `imei` | A 15-digit placeholder satisfying a "non-empty" check | `ig.w0.buildByMac(deviceBtMac).getDeviceUniqueId()` -- looks up the paired device's locally cached record and returns either `UserDeviceInfo.getRm()` (region mark; this watch's `ro.vendor.oplus.regionmark` reads `EUEX`) or `.getDeviceSn()` (a device serial), chosen by a capability flag. Never a telephony IMEI. |
| `romVersion` | The short `"A.192"` form | `deviceSoftVersion`, a value sourced from the Bluetooth DM-protocol link to the watch, with no confirmed 1:1 `getprop` equivalent |
| `otaPrefix` / `productName` | The device model, `"OPWWE231"` | The device model directly, **unless** `DeviceModel.B6()` is true, in which case it's `OppoOtaUtils.e(deviceSku, deviceModel)` instead (which only remaps two unrelated hardcoded SKUs; any other `deviceSku`, including a real one different from the public model name, passes through unchanged) |
| `otaVersion` | `ro.build.version.ota` directly | `deviceOtaVersion` directly, **unless** the same `B6()` flag is true, in which case it's a composed string `"{otaPrefix}_{deviceHardVersion}.{deviceSoftVersion}"` |

`B6()` resolves to `a6() || o6()`, each of which reads a Kotlin `by lazy`
boolean field. We traced as far as the constructor and found no visible
initializer for either lazy delegate in the decompiled method body --
Kotlin property-delegate initializers are not always exposed this way by
JADX. This remains unresolved by static analysis.

`oppo_ota_query.py` now documents all of this in its module docstring and
defaults `--imei` to `EUEX` (the regionmark, our best-grounded guess for
`Rm`) instead of a meaningless placeholder digit string.

**This does not change the standing conclusion.** The real app's own live,
reconnect-triggered check (already documented above) used these exact real
fields automatically, correct or not, and got "no new version" -- the same
answer our reconstruction gets regardless of which branch of `B6()` applies.
The corrections matter for anyone extending this tool or re-deriving the
protocol from scratch, not for whether this channel has anything to offer.
