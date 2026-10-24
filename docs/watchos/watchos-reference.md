# watchOS reference and feasibility

Public metadata: [watchOS reference JSON](../../research/opwwe231/2026-10-20-a192/watchos-reference.json).
We publish package identity and the archive digest; no Apple firmware accompanies
this repository.

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
