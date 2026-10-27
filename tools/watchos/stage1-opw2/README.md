# stage1-opw2

AArch64 bare-metal "stage1" bootloader for the OPW2 watchOS port. The eventual
job: receive control from Qualcomm's XBL/ABL, draw the Apple boot logo, build
a watchOS `boot_args` struct with the Watch7,11 DeviceTree, install the sn0w
EL2 trap vectors ([sn0w-el2-opw2](../sn0w-el2-opw2/)), and `eret` to EL1 at
the kernelcache's `entry0` (0xFFFFFFF007DF8538).

Today's build is the **smoke payload only** — ~140 bytes that write a
`SN0W-LIVE <el>` tag into the ramoops reserved region at 0x71f00000 and then
loop forever in `wfi`. Enough to answer one question:

> Does code I packed into a boot.img and flashed to slot B actually execute
> on this watch, instead of getting rejected by AVB / rebooted by a watchdog
> on the first undefined instruction?

Everything else (the real stage1 with kernelcache handoff) builds on top.

## Build

Needs the Android NDK (same the recovery menu uses).

```
make
```

Produces `stage1.bin` (Linux ARM64 Image-shaped flat binary) and, via the
packer, `stage1-slot-b.img` (Android boot.img v4 ready for
`fastboot flash boot_b`).

```
.venv/bin/python pack-bootimg.py stage1.bin -o stage1-slot-b.img
```

The packer reads the stock boot header from
`backups/oneplus-watch-2/2026-10-21-full-system-backup/boot.img` and copies
its `os_version` + cmdline — the only differences from stock are the kernel
payload and `kernel_size`.

## Smoke protocol (how to run the test on-device)

Preconditions:
- Watch is in Wear OS with Magisk root available (confirmed 2026-10-27).
- `fastboot` CLI on the Mac.
- `backups/oneplus-watch-2/.../recovery.img` is still intact in the local
  tree (fallback for a hard revert).

Steps:

1. **From Wear OS**: trigger fastboot.
   ```
   adb reboot bootloader
   ```

2. **From fastboot**: flash slot B and switch the active slot.
   ```
   fastboot flash boot_b tools/watchos/stage1-opw2/stage1-slot-b.img
   fastboot --set-active=b
   fastboot reboot
   ```

3. **Observe the watch**:
   - If the OnePlus/Wear splash lingers past ~15 s with the backlight on but
     no boot animation → **stage1 is executing** (we are in WFI, the SoC is
     idle; no Linux panic has fired a watchdog reset). Success.
   - If the watch loops the splash every few seconds → the bootloader rejected
     the image (AVB), or stage1 took an exception before `wfi`. Failure.

4. **Revert to Wear OS slot A** regardless of step 3's outcome:
   - Hold POWER + the side key the sn0wbite recovery was built against
     (keycodes 114 / 116 per the recovery notes) until the device resets into
     the sn0wbite menu.
   - Pick FASTBOOT, or from the menu rebooting to `adb reboot bootloader`.
   - From fastboot:
     ```
     fastboot --set-active=a
     fastboot reboot
     ```

The ramoops tag we write at 0x71f00000 (`SN0W-LIVE <el>`, where `<el>` is
ASCII `'1'..'3'` of the exception level stage1 landed in) is not readable from
Wear OS without an in-tree kernel module (`ro.secure=1`, `/dev/mem` disabled)
or a pstore-formatted record. Reading it is a later-session improvement; the
visual signal above is enough to answer the smoke question.

## What this is NOT

- Not the final stage1. No boot_args, no DT, no kernelcache handoff, no logo.
- Not watchOS. We are literally just testing that our custom boot.img boots.
- Not a security claim. Flashing an unsigned boot.img presumes the user
  already crossed the fastboot-unlock boundary.

## Status (2026-10-27)

- `src/entry.S`, `src/stage1.ld`, `Makefile`, `pack-bootimg.py` written.
- Host build verified: `stage1.bin` is 144 bytes, header starts with the
  expected `10 00 00 14 … 41 52 4d 64` (branch to `_entry` + "ARM\x64" magic
  at offset 0x38).
- `stage1-slot-b.img` is 8192 bytes (one page header + one page kernel),
  parsed correctly by `tools/mapping/extractors/bootimg.py`.
- On-device smoke test: **not yet run**.
