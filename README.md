<p align="center">
  <img src="./assets/branding/sn0wbite-banner-1280x400.png" alt="sn0wbite: green ASCII apple logo and project wordmark. One bite is all it takes." width="1280">
</p>

# sn0wbite

**sn0wbite** is an experimental reverse engineering project investigating
whether watchOS can boot on the **OnePlus Watch 2 (OPWWE231)**.

The first boot milestone is repeatable execution of the watchOS kernel, with
traces showing the execution stage reached. A usable interface and working
peripherals come later. We have not implemented a loader or demonstrated a
watchOS boot; native compatibility remains unproven.

## Current findings

Research snapshot: **20 October 2026**, updated **21 October 2026**.

| Area | Status |
| --- | --- |
| Target | **OnePlus Watch 2 / OPWWE231**, Qualcomm W5 Gen 1 (`monaco`) |
| Installed firmware | Android 14, **A.192.260910**; ARM64-capable kernel, **32-bit-only (`armeabi-v7a`) Android userspace** |
| Access | **Root via Magisk, confirmed working.** Bootloader **unlocked**; raw storage reads now work for all partitions |
| Storage inventory | **63 named physical partitions**; ADB mappings match fastboot sizes and types |
| Complete backup | **62 of 63 non-`userdata` partitions acquired and SHA-256 verified**; `userdata` excluded by choice |
| Restoration | **Verified**: a documented, checksummed restore path exists for this exact device, demonstrated live during a real recovery |
| Apple reference | watchOS **6.3.1 / 17U224**, OTA for **Watch5,4 / N146bAP** |

Recovery logs show an **A.190 → A.192** update that patched `init_boot` and updated
MCU firmware. Neither a successful forced check nor the paired-phone
investigation recovered the matching A.192 package. See the evidence and limits
in the topic notes below.

Root was achieved by patching a genuine, version-mismatched `init_boot.img`
(sourced from Google's own OTA hosting for this exact device, not a
third-party mirror) with [Magisk](https://github.com/topjohnwu/Magisk),
diagnosing and fixing a real arm64/arm32 architecture bug specific to this
device's split-GKI, 32-bit-only platform along the way. See
[Root and boot customization](./docs/root-and-boot-customization.md).

## Documentation and evidence

| Topic | Read more |
| --- | --- |
| Device identity, ADB, root and storage | [Device and storage notes](./docs/device-and-storage.md) |
| OTA history and paired-phone analysis | [OTA and OHealth](./docs/ota-and-ohealth.md) |
| Recovery, fastboot and EDL | [Recovery research](./docs/recovery.md) |
| Root, Magisk and boot customization | [Root and boot customization](./docs/root-and-boot-customization.md) |
| Apple package identity and feasibility | [watchOS reference](./docs/watchos-reference.md) |
| Captured output and machine-readable findings | [20 October collection](./research/opwwe231/2026-10-20-a192/README.md), [21 October collection](./research/opwwe231/2026-10-21-root-magisk/README.md) |
| Privacy, redactions and provenance | [Evidence publication policy](./docs/evidence-policy.md) |

The [research index](./docs/README.md) connects findings to their evidence.
Public data includes selected ADB/fastboot/recovery output, all 63 partition
mappings, structured observations and checksums. Original acquisitions remain
private. A complete non-`userdata` dump and a verified restoration method now
exist privately, clearing the way for boot-partition experiments.

## Repository layout

```text
assets/branding/   Project logo, banner, avatars, icons and social preview
docs/             Research notes, sources and publication policy
research/         Reviewed evidence organized by device and collection
backups/          Private local acquisitions; ignored by Git
```

## Roadmap

- [x] Identify the attached watch and record its installed firmware and security state.
- [x] Inventory partition names and bootloader-reported sizes.
- [x] Identify the supplied watchOS reference package and record its archive hash.
- [x] Obtain privileged storage reads while preserving the current device state.
- [x] Acquire and verify complete storage images, including companion-firmware coverage. *(all 62 non-`userdata` partitions, including `super`, whose vendor filesystem carries the MCU companion firmware; `userdata` intentionally excluded)*
- [x] Establish a documented restoration path for this exact device.
- [ ] Analyze watchOS boot stages, kernel format and Apple hardware dependencies.
- [ ] Establish controlled payload execution and diagnostic output on the watch.
- [ ] Demonstrate repeatable watchOS kernel execution with traces and reproduction steps.
- [ ] Bring up the interface, peripherals and power management as kernel progress allows.

## Brand assets

The [brand directory](./assets/branding/README.md) contains the supplied logos,
banner and social preview in their original formats.

## Disclaimer

This project is for educational and research purposes. It has no affiliation
with Apple Inc. or OnePlus. watchOS is a trademark of Apple Inc.

Modifying or flashing firmware can erase data or brick the device.
Use at your own risk.
