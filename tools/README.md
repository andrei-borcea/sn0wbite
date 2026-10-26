# Tools

| Directory | Track | What it is |
| --- | --- | --- |
| [opwwe231/](./opwwe231/README.md) | OnePlus Watch 2 | Custom recovery + boot menu, replacement power menu, live verbose boot screen, Magisk patch helper. OnePlus-specific. |
| [watchos/](./watchos/README.md) | Apple Watch | watchOS 27 bringup under QEMU-SPTM: patchkit, gdbstub helpers, Hex-Rays snapshots, the `sn0shell` userspace binary + its build/deploy chain. Also the native-target QEMU adaptation (`qemu/`), early-boot Unicorn harness (`emu/`), PAC/Apple-register census + patcher (`port/`), `cpuprobe` (`probe/`), and the kernel `boot_args` builder (`bootargs/`). |
| [ota/](./ota/) | Cross-track | OPPO/HeyTap OTA client reconstructed from OHealth static analysis. See [OTA and OHealth](../docs/opwwe231/ota-and-ohealth.md). |

See the top-level [README](../README.md) for the two-track overview and
the status tables.
