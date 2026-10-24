# OPWWE porting target: watchOS 6.3.1 boot-chain analysis, 24 October 2026

Reverse-engineering evidence for booting the watchOS 6.3.1 (`17U224`, Watch5,4 / Apple S5 / T8006) kernel in emulation. Narrative:
[boot chain reconstruction](../../../docs/watchos/boot-chain-reconstruction.md), [platform device map](../../../docs/watchos/platform-device-map.md),
[port plan](../../../docs/watchos/boot-chain-port-plan.md). No Apple firmware is published; these are measurements and counts.

| File | Contents |
| --- | --- |
| [pac-and-register-census.txt](./pac-and-register-census.txt) | Capstone census of the kernelcache: 331,911 PAC sites and 82 distinct Apple IMPDEF registers (the port patch scope) |
| [t8006-platform-device-map.txt](./t8006-platform-device-map.txt) | The platform MMIO map from the DeviceTree (AIC, PMGR, timer, UART, coprocessors, display) -- the QEMU machine spec |
| [provenance.json](./provenance.json) | Where each file comes from |
| [SHA256SUMS](./SHA256SUMS) | Digests for this collection |
