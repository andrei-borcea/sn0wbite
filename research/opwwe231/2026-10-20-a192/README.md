# OPWWE231 A.192: evidence collected on 20 October 2026

Target: **OnePlus Watch 2**, Android 14, build **A.192.260910**.
We selected these records from saved acquisitions; publishing them required no
new device commands. Earlier investigation included an authorized fastboot
reboot, a rejected ADB root request and a native OTA check.

| File | Contents |
| --- | --- |
| [device.json](./device.json) | Selected identity, tool version, access and backup-status fields |
| [partitions.csv](./partitions.csv) | All 63 physical partition mappings, with fastboot sizes and types |
| [adb-excerpts.txt](./adb-excerpts.txt) | Failed root request and denied storage/cache reads |
| [fastboot-excerpts.txt](./fastboot-excerpts.txt) | Selected bootloader metadata and failed `fetch init_boot`; serial redacted |
| [recovery-update-excerpts.txt](./recovery-update-excerpts.txt) | A.190 → A.192 update, `init_boot` patch and MCU result |
| [ota-and-ohealth.json](./ota-and-ohealth.json) | Forced-check status, companion-app analysis and aggregate log coverage |
| [watchos-reference.json](./watchos-reference.json) | Apple OTA package identity, archive hash and analysis limits |
| [provenance.json](./provenance.json) | Private source-file hashes, field selection and excerpt transformations |
| [SHA256SUMS](./SHA256SUMS) | SHA-256 digests for this public collection |

Excerpts retain original source line numbers; omitted spans are not adjacent
capture output. The fastboot serial is replaced with an explicit placeholder.
The CSV joins names from by-name links to bootloader sizes; it is not a GPT
sector dump and includes no inferred partition offsets. The separate eMMC
hardware areas `boot0`/`boot1` have not been acquired or measured.

**No raw storage image or complete backup accompanies this collection.
Restoration remains unverified.** See [research notes](../../../docs/README.md)
and the [evidence policy](../../../docs/evidence-policy.md) for interpretation.

From this directory, verify the exported files with:

```sh
shasum -a 256 -c SHA256SUMS
```
