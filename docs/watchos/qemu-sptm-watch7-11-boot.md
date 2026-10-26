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

### 2026-10-26 later: full APFS mount of the rootfs

Multiple surgical kernel patches get the watchOS 27 APFS rootfs
(8.4 GB) past the XNU `/dev/md` driver's 32-bit length truncation and
all the way to `apfs_log_op_with_proc: md0s1 mount-complete volume
Orchid24R365.N218bOS`, with `launchd` exec'ing on top. The chain:

| # | VA (patch) | Original | New | Effect |
|---|---|---|---|---|
| 1 | `0x98a46c0` | `880d0054` (`b.hi`) | `1f2003d5` (`nop`) | bypass APFS "container > device" sanity |
| 2 | `0x97b5ab4` | `608602f9` (`str x0, [x19,#0x508]`) | `7733b317` (`b 0x8482890`) | redirect block_count store to trampoline |
| 3 | `0x8482890` (16 B pad) | 16× `nop` | `movz w0,#0x5000; movk w0,#0x010B,lsl#16; str x0,[x19,#0x508]; b 0x97b5ab8` | hard-code block_count = `0x010B5000` (8.4 GB / 512) |
| 4 | `0x9b6058c` | `82020054` (`b.hi`) | `1f2003d5` (`nop`) | bypass `mdevstrategy` 32-bit `cmp x19,x9` where `w9 = *(u32*)(dev_handle+8) << 12` |
| 5 | `0x9b60598` | `69000054` (`b.ls`) | `03000014` (`b`) | force success-path on the next 32-bit remaining-length check (no truncation of IO count) |
| 6 | `0x981fc74` | `69020054` (`b.ls`) | `1f2003d5` (`nop`) | bypass md "start outside device" check |
| 7 | `0x981fc88` | `e9050054` (`b.ls`) | `2f000014` (`b`) | force md "range outside device" always-success |
| 8 | `0x9b60634` | `200c0054` (`b.eq`) | `1f2003d5` (`nop`) | mdevstrategy "sink not mapped" bypass |
| 9 | `0x9b606ec` | `77070034` (`cbz`) | `1f2003d5` (`nop`) | mdevstrategy "source not mapped" bypass |

(All file offsets are `0xE28000 + (VA - 0xFFFFFFF007E2C000)`; the
kernel's `__TEXT_EXEC` segment.)

With this chain:

```
dev_init:299: md0 device_handle block size 512 block count 17518592
nx_mount:1157/1482/1509 (checkpoint xid 595, descs/data OK)
handle_mount:893: md0s1 vol-uuid: 204D04D5-ED03-4EA7-9576-132B49C01372 block size: 4096 block count: 2189824 (features 8.0.12)
handle_mount:906: md0s1 setting dev block size to 4096 from 512
apfs_log_op_with_proc:3279: md0s1 mount-complete volume Orchid24R365.N218bOS
dyld[1]: ignite() returned 8; ignition disabled
...
```

### 2026-10-26 even later: `cs_enforcement_disable=1` bootarg path explored

The `cs_enforcement_disable=1` kernel bootarg is **recognised** by
watchOS 27 (`_initializeAppleMobileFileIntegrity(): cs_enforcement
disabled by boot-arg` appears in the log), but AMFI then calls
`panic("can't has cs_enforcement_disable" @AppleMobileFileIntegrity.cpp:5710)`
to actively kill the boot. The panic sits in a tiny helper function at
`0xfffffff0089573ec..0x895741c`:

```asm
0x8957400  adrp x8, "AppleMobileFileIntegrity.cpp"
0x8957404  add  x8, x8, #0x3fe
0x8957408  mov  w9, #0x164e            ; line 5710
0x895740c  stp  x8, x9, [sp]
0x8957410  adrp x0, "can't has cs_enforcement_disable" (0xfffffff0074b9b66)
0x8957414  add  x0, x0, #0xb66
0x8957418  bl   panic                   ; patched -> nop
0x895741c  bti  c
...
0x8957428  brk  #0xbffc                 ; patched -> ret
```

Flipping the `bl panic` to a `nop` and the terminal `brk` to a `ret`
(file offsets `0x1953418` and `0x1953428`, bytes `07060094 -> 1f2003d5`
and `80ff37d4 -> c0035fd6`) lets `cs_enforcement_disable=1` take
effect without the counter-panic.

**Result**: after the panic bypass + `cs_enforcement_disable=1`, the
early kernel init enters an **infinite loop** of:

```
trustCacheIsDuplicateImg4: unable to extract img4 module: 0x06 | 0x25 | 1
loadTrustCacheWithExternalManifest: unable to load trust cache: -536870206
```

(spamming hundreds of thousands of lines in ~90 s, long before the
APFS mount). Disabling `cs_enforcement` apparently makes some trust
cache loader lose its gating and retry forever; it never progresses
past that loop. So `cs_enforcement_disable=1` is a dead end on its
own.

The `amfi_get_out_of_my_way=1` bootarg (used by community iOS VM
projects like vphone-cli) is **silently ignored** on watchOS 27 — the
AMFI reject of the dyld cache still fires with it set.

See the extended bootarg attempt in `boot_rootfs_amfi3.log` and the
baseline (no cs_enforcement_disable) still reaching APFS mount in
`boot_rootfs_amfi4.log`.

### Next blocker: AMFI rejects the dyld shared cache

Beyond the APFS mount, `/sbin/launchd` execs, `dyld` runs, mmaps the
stock watchOS `/System/Library/Caches/com.apple.dyld/dyld_shared_cache_arm64e`,
and tries to register its signature with the kernel. AMFI rejects:

```
AMFI: '/System/Library/Caches/com.apple.dyld/dyld_shared_cache_arm64e' is adhoc signed.
AMFI: '/System/Library/Caches/com.apple.dyld/dyld_shared_cache_arm64e': unsuitable CT policy 0 for this platform/device, rejecting signature.
AMFI: code signature validation failed.
dyld[1]: result from check_np(): -1, errno 12  (ENOMEM)
dyld[1]: Library not loaded: /usr/lib/libSystem.B.dylib  (no such file, no dyld cache)
panic: launchd[1] fatal signal 6
```

The AMFI validation function lives at `0xfffffff00893da34` (pacibsp
entry) with a main return at `0xfffffff00893f1d0` (retab); it is a
large function with many branches to error-print blocks for both the
"CT policy" and "adhoc signed" cases. Experiments:
- Flipping the inner `b.ne 0x893e5ec` (CT policy error gate) to an
  unconditional `b` suppresses the "unsuitable CT policy" log but
  leaves the "adhoc signed" branch intact (the function returns a
  failure code anyway and `check_np` still errors ENOMEM).
- Rewriting the function entry to `mov x0, #0 ; ret` breaks earlier
  AMFI consumers (APFS trustcache validation during mount), so the
  function cannot be nulled wholesale.
- Rewriting only the final `mov x0, x19` at `0x893f1b4` to
  `mov x0, #0` is not enough — the shared-region syscall path reads
  an AMFI-populated cache-descriptor struct and still errors out.

The real fix needs full Ghidra-level analysis of the AMFI code-signing
validator plus the shared-region registration path (`vm/shared_region.c`
→ `cs_blob_create` → AMFI `csm_` monitor hooks) to find and patch the
exact decision point for the "shared cache acceptable" verdict.
Deferred to a future session with full Ghidra available.

### Progress table

| Phase | Status |
|---|---|
| iBoot → XNU handoff | ✅ |
| SPTM/TXM init | ✅ |
| XNU + IOKit kexts | ✅ |
| **APFS rootfs mount** (8.4 GB, with the 9-patch chain above) | ✅ **NEW** |
| `/sbin/launchd` exec | ✅ |
| dyld load | partial — reaches shared cache registration |
| AMFI accept dyld cache | ❌ blocker — needs deeper REA |
| `launchd` complete bootstrap | ❌ gated on dyld cache |
| SpringBoard | far future |

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
