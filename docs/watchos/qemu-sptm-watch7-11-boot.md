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

### 2026-10-27: deep REA with r2 + r2ghidra

With `r2ghidra`'s `pdg` decompiler the AMFI entry is confirmed as
`_vnode_check_signature` (logs `AMFI: vnode_check_signature called
with platform %d`). Its return value `uVar21` propagates up as the
hook errno. The CT-policy and adhoc print blocks both set an
intermediate `bVar4 = false` which, at the function's do-while, causes
`break` → error exit. In assembly `bVar4 == w22`, reset via
`mov w22, #0` at `0xfffffff00893e5e0` (CT) and `0xfffffff00893e618`
(adhoc).

Patching both to `mov w22, #1` **does** remove the
`"AMFI: code signature validation failed."` log, but the function
still returns a failure value to the shared-region path — `uVar21`
derives from more than `bVar4` (likely `piVar16` state /
`csm_evaluate_image4` result accumulated earlier).

Separately, `shared_region_check_np` was located at
`0xfffffff009f77788` and `vm_shared_region_auth_remap` at
`0xfffffff009adda20` via r2ghidra. Both experiments proved dead ends:
- `nop` the top-level `b.ne` in `shared_region_check_np` at
  `0xfffffff009f77950` so the fast-path always triggers → `check_np`
  returns 0 to dyld, but `/sbin/launchd` crashes with
  `exit reason namespace 2 subcode 0xb` (SIGSEGV) because the kernel
  never set up the task's shared region, so dyld dereferences
  unmapped cache pages.
- Replacing `vm_shared_region_auth_remap` entry with
  `mov x0, #0 ; ret` → no effect: `vm_shared_region_setup` fails
  earlier (in the AMFI `cs_blob_create` chain) so `auth_remap` is
  never reached.

What's needed next is a Ghidra-level trace of where
`cs_blob_create` / `cs_shared_region_create` calls the AMFI validator
for the dyld cache specifically and either patch that single caller
to skip the AMFI call, or patch `/usr/lib/dyld` on the rootfs to not
issue `__shared_region_check_np` at all (and lazy-load dylibs from
the cache file by direct symbol resolution). Both are longer passes.

### 2026-10-27: cs_enforcement_disable + bootarg attempts

The `cs_enforcement_disable=1` kernel bootarg is recognised (`AMFI
init: cs_enforcement disabled by boot-arg`) but AMFI then
counter-panics with `"can't has cs_enforcement_disable" @
AppleMobileFileIntegrity.cpp:5710`. Patching that panic (`bl panic
-> nop` and the terminal `brk -> ret`) lets the bootarg take effect,
after which the kernel enters an infinite loop of
`loadTrustCacheWithExternalManifest: -536870206` **before** the APFS
mount — full-disable is also a dead end.

The `amfi_get_out_of_my_way=1` bootarg used by community iOS VM
projects (vphone-cli, etc.) is silently ignored on watchOS 27.

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

### 2026-10-26: IDA Pro 9.4 Hex-Rays REA of AMFI + shared_region

Loaded the full `bootkc_watch7_11` into IDA Pro 9.4 and drove it via
the `ida-pro-mcp` JSON-RPC server on `127.0.0.1:13337`. Full
Hex-Rays decompile of the two critical functions in
`scratchpad/ida_work/`:

- `amfi_vcs_hexrays.c` — AMFI `vnode_check_signature` MACF hook at
  `0xfffffff00893da30` (0x14c4 bytes).
- `shared_region_check_np.c` — kernel syscall at
  `0xfffffff009f77788` (0xb04 bytes).

**Correction to the earlier r2+r2ghidra model.** The fail flag in
`vnode_check_signature` is NOT `w22` as previously inferred:

| | r2ghidra model (wrong) | Hex-Rays model (correct) |
|---|---|---|
| CT policy reject | `w22 = 0` at `0x893e5e0` | `LODWORD(v31) = 0` at `0x893e5e0` + `v107 = 1` at `0x893e61c` |
| Adhoc / dev-signed reject | `w22 = 0` at `0x893e618` | `LODWORD(v31) = 0` at `0x893e618` (same slot) |
| Actual decision | `w22` late check | `CBZ W22, 0x893e7c8` at `0xfffffff00893e750` |

The single decision is the CBZ at `0xfffffff00893e750`: if the trust
bitmap (W22) is zero, jump to the "AMFI: code signature validation
failed" print and then write `untrusted` (code 10) into the output
`a4` via `sub_FFFFFFF00893BFD0`. Otherwise reach the dynamic trust
assignment via `sub_FFFFFFF00893FF44(..., "dynamic", ...)` at
`0xfffffff00893e79c`. `sub_FFFFFFF00893FF44` turns out to be a thin
OOP-JIT entitlement check (returns 1 for anything without
`com.apple.private.oop-jit.{loader,runner}`), so for
`dyld_shared_cache_arm64e` the AMFI success path is actually
functional.

**Why the previous `w22 = 1` patches "removed the log but didn't fix
anything":** they successfully redirected AMFI to the success path,
AMFI wrote a `dynamic` trust into `*a4`, AMFI returned 0. The
rejection comes afterwards in the VM shared region setup
(`vm_shared_region_create` / `_auth_remap` / `_enter`), where a
`dyld_shared_cache` that AMFI only certified as "dynamic" (not
"trusted-cache") is still refused for mapping. `dyld` then calls
`shared_region_check_np` (syscall 294) and the kernel returns 12
(`ENOMEM`) from the `copyout` error path at
`0xfffffff009f77e04`, because `vm_shared_region_start_address()` /
`vm_shared_update_task()` never populated the region.

**Conclusion.** A one-instruction AMFI patch is not enough; a
multi-site patch chain on the VM shared region creation path would
be needed to make an adhoc shared cache produce a `trusted-cache`
mapping. That is realistically 10–20 more hours of REA on a chain
whose outcome is still only a serial-console shell (`qemu-sptm` has
no DCP/AGX for SpringBoard).

**Deeper REA (2026-10-26, same IDA session).** The function hosting
the real rejection is `shared_region_map_and_slide_setup`
= `sub_FFFFFFF009F79CA8` at `0xfffffff009f79ca8`, 0x27a0 bytes =
1803 Hex-Rays lines. Decompile saved in
`scratchpad/ida_work/map_and_slide_setup.c`. Return-value
distribution:

| Exit value | Meaning | Branch count | Example sites |
|---|---|---|---|
| `v39 = 0` (success) | KERN_SUCCESS | 1 | `L1657 @0x9f7a504` |
| `v39 = 22` (EINVAL) | validation reject | 11 | `L906 @0x9f7b638`, `L1408 @…`, `L1657-adjacent LABEL_563` |
| `v39 = 12` (ENOMEM) | kalloc failure | 1 | `L358 @0x9f7a060` — this is `sub_FFFFFFF0099D1514(…)==NULL`, NOT the adhoc rejection |
| `v39 = 1` (KERN_FAILURE) | Mach fault | 6 | mapping-op failures |
| `v39 = v<other>` | delegated | 7 | nested helpers |

Each of the 11 EINVAL branches guards a different invariant
(mapping range in-bounds, slide-info size, cs-blob consistency,
sr-cache header counts, protection flags, auth remap). Finding
*which one* fires for an adhoc shared cache requires a live trace
(instrumentation of the kernel at each branch, or a step-trace from
QEMU gdbstub). Static-only, the branches cannot be ordered.

**Status of this REA sub-task.** The IDA Hex-Rays contribution is
complete: the AMFI vnode_check_signature flow is fully mapped, the
previous session's `w22`-patch inference is corrected, and the real
blocker is now localised to `sub_FFFFFFF009F79CA8` with a labelled
exit map but without a verified single patch site. A productive
next step is either a kernel-side instrumentation pass (patch every
EINVAL exit to also print a unique marker into the serial log, then
boot and read which marker fires), or the pivot path below.

Three realistic directions:

1. **Custom sysroot (jprx pattern).** Compile a `bash` + `launchd`
   + minimal `libSystem` for watchOS 27 arm64e against the Xcode
   watchOS 27 SDK, side-load it instead of the stock rootfs, and
   skip the shared cache entirely. This is how `jprx/darwin-vm`
   reaches a shell on iPhone 12-17 targets.
2. **Full VM-shared-region patch chain.** Decompile the five
   functions (`_cs_blob_create`, `_cs_shared_region_create`,
   `_vm_shared_region_create`, `_vm_shared_region_auth_remap`,
   `_vm_shared_region_enter`) with Hex-Rays and patch each
   `*a4 = untrusted` write + each trust-level check. Many patches,
   deep understanding of the trust-cache state machine required.
3. **Accept current progress.** `qemu-sptm` already reaches APFS
   mount + `launchd` exec, which is the practical ceiling for a
   kernel-dev sandbox without a GPU emulation. Pivot to the Watch 2
   bringup work (the real target: Snapdragon W5, arm64).
