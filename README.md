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

Research snapshot: **20 October 2026**.

| Area | Status |
| --- | --- |
| Target | **OnePlus Watch 2 / OPWWE231**, Qualcomm W5 Gen 1 (`monaco`) |
| Installed firmware | Android 14, **A.192.260910**; ARM64 kernel and 32-bit Android userspace |
| Access | ADB shell available; locked bootloader, no root, raw storage reads denied |
| Storage inventory | **63 named physical partitions**; ADB mappings match fastboot sizes and types |
| Complete backup | **Incomplete:** no raw storage image acquired |
| Restoration | **Unverified:** no demonstrated full unbrick procedure for this device/build |
| Apple reference | watchOS **6.3.1 / 17U224**, OTA for **Watch5,4 / N146bAP** |

Recovery logs show an **A.190 → A.192** update that patched `init_boot` and updated
MCU firmware. Neither a successful forced check nor the paired-phone
investigation recovered the matching A.192 package. See the evidence and limits
in the topic notes below.

## Documentation and evidence

| Topic | Read more |
| --- | --- |
| Device identity, ADB, root and storage | [Device and storage notes](./docs/device-and-storage.md) |
| OTA history and paired-phone analysis | [OTA and OHealth](./docs/ota-and-ohealth.md) |
| Recovery, fastboot and EDL | [Recovery research](./docs/recovery.md) |
| Apple package identity and feasibility | [watchOS reference](./docs/watchos-reference.md) |
| Captured output and machine-readable findings | [Public evidence collection](./research/opwwe231/2026-10-20-a192/README.md) |
| Privacy, redactions and provenance | [Evidence publication policy](./docs/evidence-policy.md) |

The [research index](./docs/README.md) connects findings to their evidence.
Public data includes selected ADB/fastboot/recovery output, all 63 partition
mappings, structured observations and checksums. Original acquisitions remain
private. A complete dump and a working method to restore it are still required
before boot-partition experiments.

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
- [ ] Obtain privileged storage reads while preserving the current device state.
- [ ] Acquire and verify complete storage images, including companion-firmware coverage.
- [ ] Establish a documented restoration path for this exact device.
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
