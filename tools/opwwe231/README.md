# OnePlus Watch 2 boot-chain tools

| Path | Purpose | Notes |
| --- | --- | --- |
| [recovery-menu/src](./recovery-menu/src/BUILD.md) | The recovery boot menu: source, image packer, offline verifier, host preview, simulation tests | [Custom recovery](../../docs/custom-boot/custom-recovery.md) |
| [recovery-menu/*.sh](./recovery-menu/) | `flash_recovery.sh` (size guard, readback, AVB tail check), `restore_stock_recovery.sh`, `enter_menu.sh`, `read_flightlog.sh` | Need `WATCH_ADB_SERIAL`; the flash and restore scripts also need your own recovery dump |
| [recovery-menu/bootanim](./recovery-menu/bootanim/) | The live verbose boot screen: updater (`bootlog.c`), frame builder (`mkbootanim.py`), host renderer and the Magisk module files | [Boot screen](../../docs/custom-boot/boot-screen.md) |
| [magisk](./magisk/README.md) | Patch an `init_boot.img` with Magisk on the watch | |

The built images are in [releases/opwwe231](../../releases/opwwe231/README.md). Build with the Android NDK on macOS (`BUILD.md`).
