# OnePlus Watch 2 boot-chain tools

| Path | Purpose | Notes |
| --- | --- | --- |
| [recovery-menu/src](./recovery-menu/src/BUILD.md) | The recovery boot menu: source, image packer, offline verifier, host preview, simulation tests | [Custom recovery](../../docs/custom-boot/custom-recovery.md) |
| [recovery-menu/*.sh](./recovery-menu/) | `flash_recovery.sh` (size guard, readback, AVB tail check), `restore_stock_recovery.sh`, `enter_menu.sh`, `read_flightlog.sh` | Need `WATCH_ADB_SERIAL`; the flash and restore scripts also need your own recovery dump |
| [recovery-menu/bootanim](./recovery-menu/bootanim/) | The live verbose boot screen: updater (`bootlog.c`), frame builder (`mkbootanim.py`), host renderer and the Magisk module files | [Boot screen](../../docs/custom-boot/boot-screen.md) |
| [powermenu](./powermenu/) | The power menu app (`build.sh`), the key watcher `powerkey.c` and the Magisk module files (`build_module.sh`) | [Hardware buttons](../../docs/device/hardware-buttons.md) |
| [watchos/probe](./watchos/probe/cpuprobe.c) | `cpuprobe`: reads the CPU ID registers from EL0 (static aarch64) | [Bring-up measurements](../../docs/watchos/bring-up-status.md) |
| [watchos/bootargs](./watchos/bootargs/) | `boot_args.h` + `mk_bootargs`: the watchOS kernel entry struct a loader puts in x0 | [Boot chain reconstruction](../../docs/watchos/boot-chain-reconstruction.md) |
| [magisk](./magisk/README.md) | Patch an `init_boot.img` with Magisk on the watch | |

The built images are in [releases/opwwe231](../../releases/opwwe231/README.md). Build with the Android NDK on macOS (`BUILD.md`).
