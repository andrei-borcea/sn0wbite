# Root access and boot customization on OPWWE231

Research date: **21 October 2026**. Target: **OPWWE231 A.192**, bootloader state
transitioned from locked to **unlocked (orange)** during this session.

This closes two open items from the [project roadmap](../README.md#roadmap):
privileged storage reads, and a documented restoration path for this device.

## Summary

Starting from the locked, non-rooted state recorded in the
[20 October evidence collection](../research/opwwe231/2026-10-20-a192/README.md),
we:

1. Unlocked the bootloader (`fastboot flashing unlock`), which wipes user data.
2. Flashed a genuine `init_boot.img` for this device extracted from a public
   Google-hosted OTA package, validated it boots the current A.192 system, and
   kept it as a local restoration baseline.
3. Patched that image for root with Magisk, hit and diagnosed a real
   architecture bug, fixed it, and confirmed root.
4. Took a full backup of every non-`userdata` partition.
5. Replaced the device's boot animation and added persistent verbose boot
   logging, discovering along the way that this device does not use the
   standard AOSP boot animation path.

## Source of the `init_boot.img` baseline

No public OPWWE231 package exists for the installed version, A.192 (see
[OTA and OHealth](./ota-and-ohealth.md)). The oldest confirmed match for this
exact model is a full OTA Google hosts for A.192's predecessor chain:

- `https://android.googleapis.com/packages/ota-api/package/690ef27e9b28ab76add006e2c35b9fed045fa6da.zip`
- Metadata inside the package: `post-build=OnePlus/OPWWE231/OPWWE231:13/TWR7.231113.001.OPWWE231_11_A.144.250623/01:user/release-keys`,
  `pre-device=OPWWE231`. This confirms the package targets this exact device,
  not a related model.
- `init_boot.img` extracted from this package: 8,388,608 bytes, matching the
  live device's `fastboot getvar partition-size:init_boot` (`0x800000`) exactly.
  `boot.img` and `vendor_boot.img` sizes also matched their live partitions.

We did not download or use any third-party firmware mirror for this device;
every OWWE231-labeled package we found during the OTA investigation turned out
to be a different product (OnePlus Watch 2R or OPPO Watch X), confirmed by
cross-checking model/project codes, and was not used.

## Validating a cross-version `init_boot.img` before flashing

A.144 is Android 13; the installed system is Android 14. Before touching the
device we established, with the bootloader still locked:

- The partition is a split GKI `init_boot` (header v4, `kernel_size=0`),
  meaning only the ramdisk differs between versions; `boot.img` was never
  modified.
- The `init` binary inside the stock ramdisk is unmodified AOSP
  (`system/core/init/first_stage_init.cpp`, `first_stage_mount.cpp` source
  paths are present in its strings) that reads `fstab.*` from the *currently
  installed* vendor/system partitions at boot, not from anything baked into
  the ramdisk itself.
- `init_boot`, `boot` and `vendor_boot` partition sizes are identical between
  A.144 and the live A.192 device (checked live via `fastboot getvar
  partition-size:<name>`), so the OS upgrade did not resize them.

This is evidence of compatibility, not proof; only a boot test resolves actual
forward compatibility. With the bootloader unlocked, we flashed the stock,
unmodified A.144 `init_boot.img` and the device booted the full A.192 system
normally. One non-fatal crash of a short-lived `init`-named subcontext process
appeared in logcat at the "Boot completed" boundary; it did not block boot and
was not reproduced afterward.

## The Magisk architecture bug

The Magisk app's **Select and Patch a File** flow, run on the paired phone
(arm64), produced an `init_boot.img` whose `/init` replacement
(`magiskinit`) was **ELF 64-bit aarch64**. This device's platform is
**32-bit ARM only** (`ro.product.cpu.abi=armeabi-v7a`), confirmed independently
with `file` and with [REA](https://github.com) static analysis.

Flashing that image produced a hard kernel panic equivalent
(`Attempted to kill init!`) immediately after the bootloader logo, with no
adb/fastboot reachable in the normal way — recoverable only through the
bootloader's own `EDL mode` / `Reboot` recovery menu, which exposed `adb` in
`recovery` state, from which `adb reboot bootloader` restored fastboot access.
The stock A.144 `init_boot.img` was reflashed to recover.

Root cause: `init_boot.img` on a split-GKI device carries **no kernel**
(`kernel_size=0`), which is normally what Magisk's packer inspects to pick an
architecture. With nothing to inspect, the architecture decision silently fell
back to the host device the app was running on (the phone), not the target
watch.

**Fix**: run Magisk's own patcher directly *on the watch*, so host and target
are the same device. We extracted the `armeabi-v7a` (32-bit) `magiskboot`,
`magiskinit`, `magisk`, `init-ld`, `stub.apk`, `boot_patch.sh` and
`util_functions.sh` from the official Magisk release APK, pushed them to
`/data/local/tmp`, and ran `boot_patch.sh` unmodified via `adb shell`. The
resulting `/init` was confirmed **32-bit ARM**, matching the device, and the
patched `init_boot.img` booted successfully with working root.

This exact bug and fix are specific to **split-`init_boot` devices whose only
supported ABI is 32-bit ARM** patched from a 64-bit host; most phones are
arm64 and never hit this path.

### Credit

The patching logic is [Magisk](https://github.com/topjohnwu/Magisk) by
topjohnwu and contributors, GPL-3.0. We used the official `v30.7` release APK
unmodified; `boot_patch.sh` was run exactly as shipped, with no logic changes
beyond supplying the correct architecture's binaries explicitly instead of
letting the app choose. No other third party provided the `init_boot.img`
baseline; it came directly from Google's own OTA hosting for this model (see
above).

## Granting root to the shell

On first use, Magisk's superuser grant prompt (`SuRequestActivity`) did not
render usably on this device's small round display (its layout assumes a
rectangular phone screen, and its default 10-second auto-deny timeout expired
before the prompt could be reliably used). We instead granted root to the
`Shell` entry directly from the Magisk app's **Superuser** tab, which is
reachable and usable on this screen. `su -c id` subsequently returned
`uid=0(root)`.

## Full system backup

With root, we dumped every partition **except `userdata`** (the user's own
data; scoped out, see [evidence policy](./evidence-policy.md)) directly from
`/dev/block/by-name/<partition>`, **62 partitions in total**, each verified
with SHA-256. The stored copies live only in the gitignored `backups/`
directory, not in `research/`.

The first attempt streamed every partition in one pass over `adb exec-out`.
Two problems surfaced, both caught by checking sizes against independently
known values rather than trusting a clean exit code:

- The largest partition, `super` (physical size 5,368,709,120 bytes, confirmed
  with `blockdev --getsize64`), came back truncated to 3,942,929,920 bytes with
  no error from the stream.
- The loop itself stopped after `super` with exit code 0, silently skipping
  the remaining 9 partitions (`toolsfv`, `tz`, `uefisecapp`, `uefivarstore`,
  `vbmeta`, `vbmeta_system`, `vendor_boot`, `xbl`, `xbl_config`); no error was
  logged for this either.

We re-acquired all 10 affected partitions by writing each to the device's own
storage with `dd` first, then `adb pull`, instead of streaming through `adb
exec-out` directly; every file then matched its `fastboot
getvar`/`blockdev --getsize64` expected size exactly. The remaining 52
partitions matched their expected sizes on the first attempt.

**This is a documented restoration path for this exact device**: the stock,
validated `init_boot.img`, plus full dumps of every other non-`userdata`
partition, are held privately with checksums, flashable with `fastboot flash
<partition> <file>` from an unlocked bootloader.

## Boot animation and verbose logging

We built a Magisk module to replace the boot animation with a project-branded
one and to capture verbose boot logs. The first two attempts silently failed:
both a pure Magisk magic-mount overlay and an early `post-fs-data.sh`
bind-mount left the stock animation playing, even though the replaced file's
hash was confirmably correct on the booted system afterward.

Tracing `logcat` for `BootAnimation` resolved it:

```text
D BootAnimation: Playing files = /system/media/bootanimation/bootanimation-plus.zip/part1, ...
```

This device does not use the standard AOSP `/system/media/bootanimation.zip`
path at all. OnePlus/OPPO's customized `BootAnimation` reads from
**`/system/media/bootanimation/bootanimation-plus.zip`**, alongside sibling
files `bootanimation-op.zip`, `rbootanimation-op.zip`, `rbootanimation-plus.zip`
and `wbootanimation.zip` in the same directory — a proprietary override point,
not documented in AOSP. Binding our replacement onto the correct path, with an
explicit `stop bootanim` / `start bootanim` in `post-fs-data.sh` to avoid a
startup race against the service's own early trigger, produced the expected
result on the next boot.

This is a reusable finding for any `OPWWE231` (and plausibly sibling
Oppo/OnePlus Wear devices sharing this `BootAnimation` customization) boot
animation project: **target `bootanimation-plus.zip`, not
`bootanimation.zip`.**

The pre-kernel static logo (drawn by the `abl` bootloader before Android
starts) is a separate, earlier boot stage. The device's `splash` partition is
unused (all-zero on this build), so that logo is most likely compiled into
`abl.elf` itself. We did not attempt to change it: unlike `init_boot`,
corrupting the bootloader binary has no fastboot-level recovery path.

"Verbose" boot output has no on-screen text console on this device (no
framebuffer console path to the round display); the practical equivalent we
implemented is a Magisk module script that saves `dmesg` and a windowed
`logcat -b all` snapshot to persistent storage on every boot, now readable
because of root (previously `dmesg` returned `Permission denied` even on an
unlocked, non-rooted bootloader).

## What this unblocks

- Privileged storage reads (`adb shell su -c dd ...`) now work for any
  partition, closing that roadmap item.
- A validated, checksummed restoration path exists for this exact device and
  build.
- Root enables on-device tooling (Frida, direct partition inspection,
  `dmesg`) for the project's next phase: watchOS kernel analysis and
  controlled payload execution.
