# Reaching userspace on Watch7,11 (watchOS 27) — sn0shell recipe

**Date:** 2026-10-26
**Status:** confirmed working end-to-end — a user-written binary executes as
`pid 3` under launchd on the QEMU-SPTM Watch7,11 (T8310) VM running watchOS 27.

## TL;DR

```bash
cd tools/watchos/qemu-sptm/userspace/sn0shell
./build.sh    # xcrun --sdk watchos clang -arch arm64 -mwatchos-version-min=10.0
./deploy.sh   # install into ramdisk, refresh trust cache
cd ../../../../../.scratchpad-local/session-previous/sptm/darwin-vm
./run.sh      # QEMU — serial prints "[SN0WBITE] userspace reached!" at pid 3
```

Backups are left as `.scratchpad-local/session-previous/sptm/darwin-vm/firmware/ramdisk.pre_sn0shell.{dmg,tc}`.

## The actual chain

1. **Baseline bootkc**: `bootkc.before-exp10` (the 9-patch APFS/launchctl base,
   `shasum -a 256 = a75d58d6…`). **No AMFI override needed** — the exp16
   trust-cache override chain is NOT required when the user code is already
   listed in the plain ramdisk trust cache.
2. **Compile a watchOS-platform binary**: `-arch arm64 -mwatchos-version-min=10.0`
   produces `LC_BUILD_VERSION platform=4` (watchOS). Avoid `-arch arm64e` for
   first-pass bringups — adhoc-signing arm64e requires wiring up a PAC-aware
   signing path that the stock `codesign -s -` does not have.
3. **Adhoc sign**: `codesign -s - --force --identifier com.sn0wbite.shell sn0shell`.
   Capture the SHA256 cdhash from `codesign -dvvv`.
4. **Trust cache update**: insert `(cdhash, flags=0x0002)` into the ramdisk
   trust cache. Entries are sorted ascending by cdhash (likely binary-searched
   at kernel runtime). Format fully documented in
   `tools/watchos/qemu-sptm/userspace/sn0shell/build-tc.py`.
5. **Install into the ramdisk DMG**: `hdiutil attach -owners off`, drop the
   binary into `/sbin/sn0shell` and a `LaunchDaemon` plist into
   `/System/Library/LaunchDaemons/com.sn0wbite.sn0shell.plist` that keeps the
   binary alive, routes stdio to `/dev/console`.
6. **Boot QEMU** with the baseline bootkc + modified ramdisk + modified TC.
   launchd exec's the binary as pid 3; dyld accepts platform=4 unmodified;
   libSystem resolves out of the shared cache in the ramdisk.

## Why the simpler fixes failed, as documented

| Attempt                                           | Outcome                                                                        |
|---------------------------------------------------|--------------------------------------------------------------------------------|
| Boot stock `/bin/sh` under launchd                | `OS_REASON_DYLD code=0x9` — `/bin/sh` is built for `platform iOS`              |
| Patch `/bin/sh` LC byte + resign                  | Resign appeared to succeed but neighbouring dylibs' code signatures broke (`libswiftDarwin` invalid signature at boot). Root cause unclear; likely DMG metadata was shuffled by `hdiutil`. |
| Patch dyld to accept iOS binaries                 | dyld is adhoc-signed and in the trust cache; byte-level patch would need a resign + TC refresh for dyld too. Not pursued once sn0shell worked. |
| Build a fresh watchOS binary                      | ✅ worked first try                                                            |

## Serial fingerprint

```
com.apple.xpc.launchd  ... (pid/3 [sn0shell]) <Notice>: created
[SN0WBITE] userspace reached! Watch7,11 watchOS 27.
[SN0WBITE] argv0=/sbin/sn0shell
[SN0WBITE] tick
[SN0WBITE] tick
...
```

Captured in `docs/watchos/logs/boot_sn0shell.log` (first 200 lines).

## What this unblocks

Any user-level code can now be driven on the VM: a bigger program that reads
`/dev/console` for input, loads additional dylibs out of the shared cache,
or probes IOKit from the user side. Serial is file-backed via QEMU's
`-serial mon:stdio` — swap to `-serial pty` for interactive stdin.

## Related

- `tools/watchos/qemu-sptm/userspace/sn0shell/README.md` — code-side
  README.
- `docs/watchos/exp16-amfi-trust-cache-breakthrough.md` — the prior IOKit
  milestone (kernel-side).
- Memory: `watchos-27-userspace-shell-reached.md`.
