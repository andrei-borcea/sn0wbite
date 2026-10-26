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

1. ✅ **DONE** — PR to `jprx/darwin-vm` with the two-hunk `dt_fixup` patch is
   open.
2. **Optional cleanup**: patch kernelcache to force an early
   `publishResource("IONVRAM")` so the IONVRAM wait loop stops polluting the
   log (cosmetic — the restore flow already treats it as a tolerated
   failure). `IODTNVRAM::init` localized at `0xfffffff00a0ff700`,
   `publishResource` internal wrapper at `0xfffffff00a0a52dc`. The IONVRAM
   pub itself is routed through virtual (`blraa`) calls, so a surgical patch
   needs Ghidra + vtable analysis.
3. **Rootfs boot (attempted 2026-10-26, blocked by XNU md driver)**: the
   full AEA-encrypted rootfs `043-04970-555.dmg.aea` (5.0 GB on disk,
   decrypts to 8.4 GB) now extracts automatically with
   ```
   ipsw fw aea -o rootfs-out/ 043-04970-555.dmg.aea
   ```
   (no fcs-key database needed — `ipsw` fetches it via Apple's WKMS
   endpoint). The decrypted DMG mounts as an Apple APFS container with
   `/Volumes/Orchid24R365.N218bOS` (7.7 GB used, 7.1 GB in `/System`).
   Passing it as `-ramdisk` to qemu-sptm (with `dram-size` bumped to
   `0x400000000` = 16 GB in `dt_fixup`) **does not boot**: the XNU `md`
   driver (`dev_init:299`) prints `block count 741376` for an 8.4 GB
   ramdisk, which is exactly `(0x216A00000 & 0xFFFFFFFF) / 512`, so the
   driver truncates the memory-descriptor length to 32 bits somewhere in
   the chain `md_setup → IOMemoryDescriptor::withPhysicalAddress → …`. The
   APFS `nx_dev_init` then detects the mismatch
   (`container size 8969519104 greater than device size 379584512`) and
   `apfs_vfsop_mountroot` fails with error 92 in a retry loop.

   Candidate patch site:
   `0xfffffff0097b5af4  str w9, [x19, #0x4f0]` writes `block_count` as a
   32-bit value from the W-register populated by `ldur w9, [fp, -0x54]`
   right after a `blraa` through `IOMemoryDescriptor`'s vtable+0x10
   (`md.cpp:299` print format at VA `0xfffffff0078fbfd8`,
   `str w0, [x19, #0x4ec]` at `0x97b5a98` sets `block_size`). The vtable
   method returns `block_count` as a 32-bit out-arg, so the fix needs to
   be upstream of this store; full Ghidra chain analysis is deferred.

### 2026-10-26 later: bypass of the APFS size-mismatch check

The APFS sanity check in `nx_dev_init` that trips the kernel into the
retry loop is at:

```
0xfffffff0098a46b4  ldr x25, [x9, #0x28]     ; APFS container block_count
0xfffffff0098a46b8  mul x8, x25, x20          ; × block_size
0xfffffff0098a46bc  cmp x8, x28                ; vs device_size (u32 truncated)
0xfffffff0098a46c0  b.hi 0xfffffff0098a4870   ; container > device → "corruption"
```

Flipping that one `b.hi` to a `nop` (file offset `0x28a06c0`,
`880d0054` → `1f2003d5`) bypasses the "container size > device size"
sanity gate. With the patch the boot gets four more APFS phases
further:

```
dev_init:299: md0 block count 741376    ← still truncated, cosmetic
nx_mount:1157: md0 initializing cache w/hash_size 8192 and cache size 22528
nx_mount:1482: md0 checkpoint search: largest xid 595, best xid 595 @ 3
nx_mount:1509: md0 stable checkpoint indices: desc 2 data 3
apfs_mount_livefs:29660: md0s1 failed to fetch the extentref tree (error 6)
```

The `ENXIO` on the extent reference tree is the symptom of the same
32-bit truncation deeper in the stack: the extent fetch issues an I/O
beyond `device_size` (the 362 MB cap) and the md driver rejects it. A
real fix needs to patch the block_count population itself, which does
not fit inline in the two existing instruction slots
(`ldur w9, [fp,-0x54] ; str w9, [x19,#0x4f0]`) and requires either a
trampoline into kernel padding or a surgical rewrite of the virtual
method behind `[x19, #0x4e0]+0x10`. Deferred to a future session with
Ghidra available.

   Minimal rootfs variant: the dyld shared cache alone is 3.6 GB and
   indivisible, so trimming the rootfs to under 4 GB (the 32-bit cap)
   requires aggressive `PrivateFrameworks` pruning that would cascade
   dependency failures. Not pursued in this session.

## Status summary (2026-10-26)

- ✅ Boot the watchOS 27 restore ramdisk on T8310 up to
  `enable_usb (standardRestore)` host-wait — upstream PR open.
- ✅ Decrypt the IPSW rootfs AEA via WKMS lookup.
- ⚠️ Boot the decrypted rootfs: blocked on an XNU `md` 32-bit length
  truncation; follow-up ticket to find and patch the upstream cast.
