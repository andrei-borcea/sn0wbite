# sn0shell — first custom userspace binary on Watch7,11 (watchOS 27)

A minimal watchOS-platform arm64 program used to prove the full userspace exec
path works on the QEMU-SPTM Watch7,11 (T8310) virtual machine running
watchOS 27. On 2026-10-26 it executed as `pid 3` under launchd and printed
`[SN0WBITE] userspace reached!` on the serial console.

## Why this exists

The last blocker before userspace was dyld rejecting `/bin/sh` from the
restore ramdisk: it is built with `LC_VERSION_MIN_IPHONEOS`, so dyld on
watchOS 27 reports `OS_REASON_DYLD code=0x9` ("main executable '/bin/sh'
built for platform iOS") and exits. Modifying `/bin/sh` in place broke the
code signatures of neighbouring dylibs.

Root-cause fix: supply a program whose `LC_BUILD_VERSION` already names
watchOS. Then everything downstream (trust cache validation, dyld platform
check, libSystem load) works unmodified.

## Pieces

| File                | Role                                               |
|---------------------|----------------------------------------------------|
| `sn0shell.c`        | Minimal source: write marker to stdout/stderr, loop with 1s nanosleep so we can read the serial for a minute. |
| `build.sh`          | Compile + adhoc-sign. Uses `xcrun --sdk watchos clang -mwatchos-version-min=10.0`. |
| `build-tc.py`       | Rebuild the ramdisk trust cache (`ramdisk.tc`), inserting the sn0shell cdhash in sorted order. Format documented in the script header. |
| `deploy.sh`         | Install the binary + plist into the ramdisk DMG and run `build-tc.py` to refresh the trust cache. Writes backups `ramdisk.pre_sn0shell.{dmg,tc}`. |
| `ramdisk.sn0.tc`    | Pre-built trust cache from the breakthrough run (version=1, 447 entries, with sn0shell's cdhash `3e59e4eba7dbebcb069f9fe861b0295ba093cbe7` at offset 0x8b0). |

## End-to-end recipe

```bash
./build.sh                             # produce ./sn0shell + adhoc signature
./deploy.sh                            # install into firmware dir + refresh TC
cd ../../../../../.scratchpad-local/session-previous/sptm/darwin-vm
./run.sh                               # QEMU boots the VM
```

Expected serial output ~6s after boot:

```
com.apple.xpc.launchd ... (pid/3 [sn0shell]) <Notice>: created
[SN0WBITE] userspace reached! Watch7,11 watchOS 27.
[SN0WBITE] argv0=/sbin/sn0shell
[SN0WBITE] tick
[SN0WBITE] tick
...
```

Exit QEMU: `Ctrl+A` then `x`. Restore the stock ramdisk with the
`ramdisk.pre_sn0shell.{dmg,tc}` backups that `deploy.sh` wrote.

## What this configuration does and doesn't need

- **Baseline bootkc**: `bootkc.before-exp10` (the 9-patch APFS/launchctl base),
  SHA256 `a75d58d6455d2895f17eb14c11fc901c6e50119bbb714e7b7b66fff74676402a`.
  No AMFI override needed — plain trust cache carries sn0shell's cdhash.
- **Ramdisk**: writable APFS DMG from the IPSW `restore_payload` with the
  binary and plist installed in place.
- **Trust cache**: version-1 format, 22-byte entries. Entries sorted ascending
  by cdhash (lookup is likely binary search). Flags observed `0x0002` on all
  stock entries.

## To write different user code

Edit `sn0shell.c`, rerun `build.sh`, rerun `deploy.sh`. The TC rebuild script
is idempotent — it replaces the entry if a cdhash is already present. Keep
`-arch arm64`, not `arm64e`, until a PAC-aware signing path is set up.
