# Live verbose boot screen for OPWWE231

Built **24 October 2026**. Module: `sn0wbite_bootanim` v2.0, flashable zip in
[releases/opwwe231](../../releases/opwwe231/README.md). Sources:
[tools/opwwe231/recovery-menu/bootanim](../../tools/opwwe231/recovery-menu/bootanim/). Evidence:
[24 October collection](../../research/opwwe231/2026-10-24-custom-boot/README.md).

![Boot screen](../images/bootscreen_v2.png)

During Wear OS boot the watch shows a small logo and, under it, the kernel log scrolling live, in the style of a verbose boot.
The logo is static; only the text moves. It runs from about 14 s after power-on, when the boot animation starts, until boot
completes (about 25 s on our unit). The first 14 s are the bootloader's own logo and cannot be changed this way.

## The stock player cannot draw live text

`/system/bin/bootanimation` is a 13 KB launcher; the player is `/system/lib/libbootanimation.so`, AOSP's `BootAnimation` with
OPLUS changes ([strings](../../research/opwwe231/2026-10-24-custom-boot/libbootanimation-features.txt), REA evidence
`ev_087f97f036dfce33ee9bd83f36ce5c6c767d54be7a8614f064b8a95d47653459`). It draws frames from a zip, an optional clock and a
shutdown progress percentage. It reads `/system/media/bootanimation/bootanimation-plus.zip`, not the AOSP path.

## The technique: rewrite frames while they play

The player decodes each frame of a part with count 1 when it shows it, and it maps the zip file. Bytes changed inside the zip
just before a frame is shown therefore change the screen. We checked this on the real player: frames that were red turned green
mid-animation after a `dd conv=notrunc` over the file.

`mkbootanim.py` builds an animation of one ring of K=30 frames (466x466, 8-bit palette, stored PNG, so every frame has the same
size) replayed by 40 `p 1 0 live` parts at 10 fps; the last part loops. `bootlog` (static ARM32, started by the module's
`post-fs-data.sh` before it restarts the player) keeps the last kernel lines from `/dev/kmsg` and, about 1.2 s before each slot is
shown, rewrites that slot: it renders the text, recomputes the zlib Adler-32 and the IDAT CRC and overwrites the pixel data in
place. It exits 4 s after `sys.boot_completed`. If it fails, the static frames stay on screen and boot is unaffected.

**Sync.** The player logs `Playing files = <zip>/live` when part 0 starts. `bootlog` polls `logcat -d -v epoch -b main -s
BootAnimation:D` every 250 ms for the first such line. A following `logcat` child buffers its pipe and is not used; the log
has no `ShownTiming start` line for zip animations; and the buffer is flooded by one `mIsWristBandAnimation` line per frame.

**Layout.** The drawing code is shared with the recovery menu (`src/font9.h`) and runs on the host too (`render_base`), so the
design can be previewed without the watch.

## Gotchas

- A zip under `/data/local/tmp` has label `shell_data_file`; the player cannot read it (`No animation file`, falls back to
  the Android logo). Module files are `system_file`.
- A shell user cannot delete files that carry a different label. Clean test directories as root.
- To replay the animation at runtime for tests: set `service.bootanim.exit` to 0, bind-mount the test zip over the target, and
  `start bootanim`; end it with `service.bootanim.exit` 1 and `umount`.
- Never pipe a compiler into `head`: it can kill the build before the link step.

## Install and roll back

Install `sn0wbite_bootanim_v2.0.zip` from the Magisk app (or copy the module directory into `/data/adb/modules/`). To go back:
copy the previous module over it and reboot, or `touch /data/adb/modules/sn0wbite_bootanim/disable`. The module logs to
`/data/local/tmp/sn0wbite_bootlog.txt` and keeps the verbose boot logging of v1 in `/data/local/tmp/sn0wbite_boot_logs/`.
