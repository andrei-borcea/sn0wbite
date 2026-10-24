# Research documentation

These notes describe the **OnePlus Watch 2 (OPWWE231)** inspected from
**20 October 2026**, running **A.192.260910**. As of **24 October 2026** the
bootloader is unlocked, root works, a full non-`userdata` backup with a restoration path exists, and the watch runs a
custom recovery menu and a live verbose boot screen.

## Device and access

| Topic | Notes | Public evidence |
| --- | --- | --- |
| Device, root and storage access | [ADB and storage](./device/device-and-storage.md) | [Device JSON](../research/opwwe231/2026-10-20-a192/device.json), [ADB excerpts](../research/opwwe231/2026-10-20-a192/adb-excerpts.txt) |
| Partition inventory and readback | [Partition boundaries](./device/device-and-storage.md#partition-inventory-boundaries) | [63-partition CSV](../research/opwwe231/2026-10-20-a192/partitions.csv), [fastboot excerpts](../research/opwwe231/2026-10-20-a192/fastboot-excerpts.txt) |
| Installed update and companion app | [OTA and OHealth](./device/ota-and-ohealth.md) | [Recovery excerpts](../research/opwwe231/2026-10-20-a192/recovery-update-excerpts.txt), [findings JSON](../research/opwwe231/2026-10-20-a192/ota-and-ohealth.json) |
| Brick recovery (stock recovery, fastboot, EDL) | [Recovery research](./device/recovery.md) | Local bootloader observations and linked public reports |
| Root, Magisk and boot customization | [Root and boot customization](./device/root-and-boot-customization.md) | [21 October collection](../research/opwwe231/2026-10-21-root-magisk/README.md) |
| Buttons, input devices, the replacement power menu | [Hardware buttons](./device/hardware-buttons.md) | [Policy excerpt](../research/opwwe231/2026-10-24-custom-boot/power-button-policy-excerpt.txt), [ShortcutKeyCtrl excerpt](../research/opwwe231/2026-10-24-custom-boot/shortcutkeyctrl-power-long-press-excerpt.txt) |

## Custom boot chain

| Topic | Notes | Public evidence |
| --- | --- | --- |
| Custom recovery, boot menu, dual boot slot, rules learned | [Custom recovery](./custom-boot/custom-recovery.md) | [Flight recorder](../research/opwwe231/2026-10-24-custom-boot/recovery-flight-recorder-hardware-runs.txt), [simulation regression](../research/opwwe231/2026-10-24-custom-boot/recovery-simulation-regression.txt) |
| Live verbose boot screen | [Boot screen](./custom-boot/boot-screen.md) | [Player strings](../research/opwwe231/2026-10-24-custom-boot/libbootanimation-features.txt), [frame test and logs](../research/opwwe231/2026-10-24-custom-boot/live-boot-console-excerpts.txt) |
| Built images and module | [Releases](../releases/opwwe231/README.md) | SHA-256 sums in the release folder |
| Sources and scripts | [tools/opwwe231](../tools/opwwe231/README.md) | |

## watchOS

| Topic | Notes | Public evidence |
| --- | --- | --- |
| Apple reference | [watchOS artifact](./watchos/watchos-reference.md) | [Package metadata and hash](../research/opwwe231/2026-10-20-a192/watchos-reference.json) |
| Boot chain, instruction requirements and port feasibility | [Boot and ISA analysis](./watchos/watchos-boot-and-isa-analysis.md) | REA evidence IDs listed in the note |

Read the [public evidence collections](../research/README.md) for file provenance and hashes, and the
[publication policy](./evidence-policy.md) for what we retain, remove or release.

We distinguish device observations, focused static analysis, public third-party
reports and unresolved hypotheses. Historical findings do not establish support
on another build or model.
