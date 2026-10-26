# Full boot walkthrough — from zero to userspace shell

For when you want to drive the whole thing yourself end-to-end. Covers
one-time setup, the per-run commands, and the expected serial output at
each stage.

## Prerequisites (one-time)

1. **Mac with Xcode installed**. Verify the watchOS SDK is present:
   ```bash
   xcrun --sdk watchos --show-sdk-path
   # /Applications/Xcode.app/Contents/Developer/Platforms/WatchOS.platform/Developer/SDKs/WatchOS26.2.sdk
   ```
2. **Watch Series 10 restore IPSW** (`Watch7,11_26.1_23S357_Restore.ipsw`
   or compatible). Obtain from Apple's public restore URLs — never
   redistributed from this repo.
3. **QEMU-SPTM build** at `.scratchpad-local/session-previous/sptm/qemu-sptm/build/qemu-system-aarch64`.
   If missing, follow `docs/watchos/qemu-build-and-boot.md`. ~576 MB when
   built.
4. **Firmware extracted** from the IPSW into
   `.scratchpad-local/session-previous/sptm/darwin-vm/firmware/`:
   `bootkc` (+ `bootkc.before-exp10` baseline), `dtree`, `ramdisk.dmg`,
   `ramdisk.tc`, `sptm`, `txm`. The restore workflow is in
   `.scratchpad-local/session-previous/sptm/darwin-vm/get_files.sh`.
5. **About 45 GB free disk** — kernelcache experiments produce ~54 MB
   each, ramdisk DMGs are ~134 MB.

## Per-run — the three-step boot

```bash
cd /Users/andreiborcea/Work/Personal/sn0wbite

# 1. Build sn0shell (watchOS-platform arm64, adhoc-signed)
tools/watchos/qemu-sptm/userspace/sn0shell/build.sh

# 2. Deploy into the ramdisk + refresh the trust cache
tools/watchos/qemu-sptm/userspace/sn0shell/deploy.sh

# 3. Boot the VM
tools/watchos/qemu-sptm/boot.sh
```

That's it. The three scripts are idempotent — rerun any of them without
cleaning up and you'll land in the same state.

## What you'll see on the serial console

### T+0 to T+3s — early kernel

```
HW BOOT CONFIG :: boot-manifest-hash: ...
AppleEffaceableNor: Initializing
AppleARMPMU: Supported CPU: Olympus-A ...
IOReturn IOPlatformExpertRegisterPlatformDevice ...
```

### T+3 to T+6s — IOKit matching, launchd starts

```
Identified Serial Port qemuport0
Identified Serial Port qemuport1
Identified Serial Port qemuport2
Identified Serial Port qemuport3
com.apple.xpc.launchd ... <Notice>: launchd 2500 ...
```

If you see `OS_REASON_EXEC ... BAD_MACHO` here instead, the trust cache is
stale — rerun `deploy.sh`.

### T+6s — sn0shell runs (the moment)

```
com.apple.xpc.launchd ... (pid/3 [sn0shell]) <Notice>: created
com.apple.xpc.launchd ... (system/com.sn0wbite.sn0shell [3]) <Notice>: service state: spawning
AMFI: Launch Constraint Violation (not enforcing), error info: ...
com.apple.xpc.launchd ... (system/com.sn0wbite.sn0shell [3]) <Notice>: service state: running

[SN0WBITE] userspace reached! Watch7,11 watchOS 27.
[SN0WBITE] argv0=/sbin/sn0shell
[SN0WBITE] tick
[SN0WBITE] tick
[SN0WBITE] tick
```

The "Launch Constraint Violation" line is informational (`not enforcing`) —
sn0shell was launched as a system service but has no constraint plist. It's
not a failure.

### T+66s — sn0shell exits normally

After 60 ticks (`sleep 1` per tick) the program returns 0. launchd's
`KeepAlive=true` restarts it, so the loop repeats every 10 s. Exit QEMU
with `Ctrl+A x`.

## Rollback

```bash
cd .scratchpad-local/session-previous/sptm/darwin-vm/firmware
cp ramdisk.pre_sn0shell.dmg ramdisk.dmg
cp ramdisk.pre_sn0shell.tc  ramdisk.tc
```

`deploy.sh` creates these backups once on the first run. Rerunning
`deploy.sh` after rollback starts from the pre-sn0shell state again.

## Writing your own payload

Edit `tools/watchos/qemu-sptm/userspace/sn0shell/sn0shell.c`.
Keep the LC_BUILD_VERSION platform=4 by building through `build.sh` (the
compiler flags are what produce the watchOS LC). Then:

```bash
./build.sh    # rebuild sn0shell with your edits
./deploy.sh   # TC gets automatically rebuilt with the new cdhash
./boot.sh     # verify
```

The trust cache rebuilder (`build-tc.py`) does the right thing when the
cdhash changes — it replaces the entry in-place and keeps the sort order.

## Known caveats

- `-serial mon:stdio` is non-interactive by default. For a REPL-style
  payload, edit `run.sh` to use `-serial pty` and connect with `screen` on
  the pty it reports.
- `-arch arm64e` support needs a PAC-aware signing path. Not implemented
  yet — stick to `arm64` until needed.
- The experiment chain (manifests `exp10` … `exp38`) is NOT needed for
  the userspace path. Those are kernel-side investigations kept for
  provenance. `boot.sh` on the clean baseline `bootkc.before-exp10` is
  enough once the ramdisk + TC are in place.

## When something breaks

Collect the first 500 lines of the serial after boot — most problems show
up in the launchd event stream:

```bash
tools/watchos/qemu-sptm/boot.sh 2>&1 | tee boot.log &
sleep 20
pkill -f qemu-system-aarch64
head -500 boot.log
```

Then grep for `OS_REASON`, `AMFI:`, `code = `, and the sn0shell marker.
Common states:

| Line                                     | Diagnosis                                                    |
|------------------------------------------|--------------------------------------------------------------|
| `OS_REASON_EXEC code=0x1 BAD_MACHO`      | Trust cache doesn't contain sn0shell's cdhash. Rerun `deploy.sh`. |
| `OS_REASON_DYLD code=0x9 ... platform iOS` | You pointed at `/bin/sh` instead of sn0shell. Fix the plist's `Program` key. |
| `code signature invalid` for a dylib     | The DMG was mounted writable and `codesign` ran on a shared library — restore from backup, rerun `deploy.sh`. |
| No `[SN0WBITE]` line within 30 s         | QEMU hung before userspace. Check `boot.log` for `panic(` or `SPTM`. |
