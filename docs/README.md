# Research documentation

These notes describe the **OnePlus Watch 2 (OPWWE231)** inspected on
**20 October 2026**, running **A.192.260910**. A complete storage dump and a
verified restoration path remain outstanding.

| Topic | Notes | Public evidence |
| --- | --- | --- |
| Device, root and storage access | [ADB and storage](./device-and-storage.md) | [Device JSON](../research/opwwe231/2026-10-20-a192/device.json), [ADB excerpts](../research/opwwe231/2026-10-20-a192/adb-excerpts.txt) |
| Partition inventory and readback | [Partition boundaries](./device-and-storage.md#partition-inventory-boundaries) | [63-partition CSV](../research/opwwe231/2026-10-20-a192/partitions.csv), [fastboot excerpts](../research/opwwe231/2026-10-20-a192/fastboot-excerpts.txt) |
| Installed update and companion app | [OTA and OHealth](./ota-and-ohealth.md) | [Recovery excerpts](../research/opwwe231/2026-10-20-a192/recovery-update-excerpts.txt), [findings JSON](../research/opwwe231/2026-10-20-a192/ota-and-ohealth.json) |
| Brick recovery | [Recovery research](./recovery.md) | Local bootloader observations and linked public reports; no demonstrated EDL restore |
| Apple reference | [watchOS artifact](./watchos-reference.md) | [Package metadata and hash](../research/opwwe231/2026-10-20-a192/watchos-reference.json) |

Read the [public evidence collection](../research/opwwe231/2026-10-20-a192/README.md)
for file provenance and hashes, and the [publication policy](./evidence-policy.md)
for what we retain or remove.

We distinguish device observations, focused static analysis, public third-party
reports and unresolved hypotheses. Historical findings do not establish support
on another build or model.
