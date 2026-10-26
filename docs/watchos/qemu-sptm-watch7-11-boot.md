# watchOS 27 on qemu-sptm — Watch7,11 (T8310) boot milestone

**Date:** 2026-10-26
**Target:** `Watch7,11` / `n218bAP` / Apple S10 "T8310" ("Haviland-B")
**OS:** watchOS 27.0.1 build `24R365`
  (`Watch7,11_27.0.1_24R365_Restore.ipsw`, SHA-1 `e48d731c9219613dc2b985b4095e4459570d7f7d`)
**Base:** [`jprx/qemu-sptm`](https://github.com/jprx/qemu-sptm) +
  [`jprx/darwin-vm`](https://github.com/jprx/darwin-vm)
**Patch surface:** two hunks in `dt_fixup.py`, no C changes in qemu-sptm
  (see `tools/opwwe231/watchos/qemu-sptm/dt_fixup-watch7_11.patch`)

## Result

Kernel boots. SPTM + TXM load. IOKit stack and dozens of kexts come up.
`launchd` reaches PID 1 and spawns `restored_external`. The restore ramdisk
runs its workflow (mount tmpfs → create virtual ramdisk → log socket → set
progress → disable watchdog) and stops at `enable_usb (standardRestore)`,
which is the normal "waiting for host" state the restore ramdisk reaches on
real hardware without a cable.

## Boot checkpoints reached

| Phase | Reached |
|---|---|
| iBoot → XNU handoff | ✅ |
| SPTM/TXM running | ✅ |
| XNU 13432.1.9 init | ✅ |
| `AMFI: Booted in a VM` | ✅ |
| IOKit services + kexts (Backlight, CredentialManager, Serial, Analytics, SFRManifest, LightEmUp, …) | ✅ |
| `launchd` PID 1 | ✅ |
| `restored_external` spawn (PID 3, 5) | ✅ |
| `mount_tmpfs /mnt5` | ✅ |
| `create_ramdisk` | ✅ |
| `set_progress_0`, `listen_for_log_client`, `create_listen_socket` | ✅ |
| `update_root_mount`, `disable_watchdog` | ✅ |
| `enable_usb (standardRestore)` | ⏸️ blocked (expects host USB endpoint) |

## Non-blockers (noise)

- `AppleSEPManager` endpoint timeout (SEP stubbed, same for every SPTM
  target in jprx's setup).
- `waiting for matching IOKit service IONVRAM` loops on a side thread.
  `restored_external` treats it as a tolerated failure and progresses:

  ```
  [00:00:10.0844-GMT] CHECKPOINT WARNING:(FAILURE:-1) Skipping consuming
    APNonce becauase we have no nvram access yet
  ```

## The two patches

```python
# fixup_sptm — sgx child optional (Watch has no AGX GPU)
if 'sgx' in d['arm-io']:
    d['arm-io'].remove_child('sgx')

# fixup — remove top-level dock-connect family (Watch-specific)
for name in ('dock-connect', 'dock-debug-wireless', 'dock-0pin'):
    if name in d:
        d.remove_child(name)
```

Without the second patch, `AppleDockConnectAIC::start` touches MMIO we don't
model and panics as *"Unhandled Everest implementation specific error"* on
the E-core (the data abort is mis-attributed through the per-core error
status regs `lsu_err_sts` / `mmu_err_sts` / `llc_err_sts`).

## Serial-log excerpt (last run, 100 s)

```
iBoot version: qemu-sptm
Darwin Image4 Extension Version 7.0.0: AppleImage4-374~13958
AppleImage4: Booted in a VM
...
com.apple.xpc.launchd ... Darwin Bootstrapper Version 7.0.0 ... launchd/RELEASE_ARM64E
com.apple.xpc.launchd ... launching: speculative restored_external
restored_external[3] ... CHECKPOINT PROGRESS: START -> presumed_initial_monitor_no_return
executing /sbin/mount_tmpfs /mnt5
[00:00:08.0149] CHECKPOINT MONITOR: [0x1180] create_ramdisk
[00:00:10.0834] CHECKPOINT NOTICE: Image4 device: AP nonce clearable
[00:00:10.0844] CHECKPOINT WARNING:(FAILURE:-1) Skipping consuming APNonce ...
[00:00:47.0279] CHECKPOINT BEGIN: MAIN:[0x0404] update_root_mount
[00:00:47.0282] CHECKPOINT BEGIN: MAIN:[0x0405] disable_watchdog
[00:00:47.0285] CHECKPOINT END:   MAIN:[0x0405] disable_watchdog
[00:00:47.0287] CHECKPOINT BEGIN: MAIN:[0x040E] enable_usb
enabling USB (blocking) with description: standardRestore
```

## Reproducing

Full recipe in `tools/opwwe231/watchos/qemu-sptm/README.md`.

## Status vs. earlier Watch5,4 / T8006 bring-up

The earlier `qemu-t8030`-fork Watch5,4 port (see
`tools/opwwe231/watchos/qemu/`, `docs/watchos/qemu-build-and-boot.md`,
commit `405d36d`) reached its ramdisk mount on an archived 2022 fork of the
qemu-t8030 tree. The new Watch7,11 / T8310 bring-up on `qemu-sptm` goes
through a modern SPTM/TXM-aware kernel on 2026-vintage upstream QEMU, and is
the correct base going forward (iPhone 12 → 17 and M1 → M5 Macs are already
tested by `jprx/darwin-vm` on the same stack, so Watch7,11 is the first
Watch to land there).

## Next

1. Open a PR to `jprx/darwin-vm` with the two-hunk patch
   (`dt_fixup-watch7_11.patch`).
2. Optional cleanup: patch kernelcache to force an early
   `publishResource("IONVRAM")` so the IONVRAM wait loop stops polluting the
   log (cosmetic — the restore flow already treats it as a tolerated
   failure).
3. Larger: swap the restore ramdisk for the full decrypted rootfs
   (`043-04970-555.dmg.aea`) to let launchd come up in a normal user-space
   context rather than restore mode. Requires the watchOS 27.0.1 24R365
   fcs-key for AEA decryption.
