# watchOS 27 on qemu-sptm — Watch7,11 (T8310, Apple S10) bring-up

Boots the stock Apple Watch Series 10 (Watch7,11 / `n218bAP` / Apple S10 "T8310")
kernel+SPTM+TXM+restore ramdisk in [`jprx/qemu-sptm`](https://github.com/jprx/qemu-sptm)
with two tiny changes to [`jprx/darwin-vm`](https://github.com/jprx/darwin-vm)
device-tree fixup script.

This is the second QEMU bring-up in this repo; the earlier Watch5,4 / S5 / T8006
work on an old qemu-t8030 fork lives under
[`tools/watchos/qemu/`](../qemu/).

## Scope

- Target SoC: **Apple S10 (T8310 "Haviland-B")**, same silicon as iPhone S9/S10.
- Target OS: **watchOS 27.0.1 build 24R365** (Restore IPSW,
  `Watch7,11_27.0.1_24R365_Restore.ipsw`, SHA-1
  `e48d731c9219613dc2b985b4095e4459570d7f7d`).
- Base emulator: `jprx/qemu-sptm` + `jprx/darwin-vm` (both unmodified except
  for `dt_fixup-watch7_11.patch` below).
- Result: kernel + SPTM/TXM + launchd + `restored_external` reach the normal
  `enable_usb (standardRestore)` host-wait, which is the furthest a restore
  ramdisk can go without a host restore endpoint.

Not committed: Apple firmware (kernelcache, SPTM, TXM, DeviceTree, ramdisk,
trustcache). You download the IPSW yourself from Apple's CDN and feed it
through the recipe below.

## Why these two patches

Both are in `dt_fixup.py`. Neither touches QEMU C code.

1. **`sgx` child is optional.** Watch SoCs have no AGX GPU node under
   `arm-io/`, so the unconditional `remove_child('sgx')` raises
   `ValueError: child sgx not found`. Wrap it in `if 'sgx' in d['arm-io']`.

2. **Watch dock nodes must be removed.** Top-level `dock-connect`,
   `dock-debug-wireless`, and `dock-0pin` make `AppleDockConnectAIC::start`
   touch MMIO we don't model, panicking as an "Unhandled Everest
   implementation specific error" on the E-core (the data abort is
   mis-attributed through the per-core error status regs).

See `dt_fixup-watch7_11.patch` for the exact diff (ready for upstream to
`jprx/darwin-vm`).

## Repro recipe

```bash
# 0. Prereqs (macOS arm64 host, Xcode CLT)
brew install jq ipsw lzfse ninja gmp nettle pixman glib pkg-config meson

# 1. Clone jprx/qemu-sptm and jprx/darwin-vm
git clone --depth=1 https://github.com/jprx/qemu-sptm.git
git clone --depth=1 https://github.com/jprx/darwin-vm.git

# 2. Apply the Watch7,11 dt_fixup patch
( cd darwin-vm && git apply /path/to/this/dt_fixup-watch7_11.patch )

# 3. Build qemu-sptm (aarch64 only, no GUI)
mkdir -p qemu-sptm/build && cd qemu-sptm/build
../configure --target-list=aarch64-softmmu --disable-pvg --disable-sdl \
             --disable-gtk --disable-cocoa --disable-vnc --disable-werror \
             --disable-docs
ninja -j8 qemu-system-aarch64
cd ../..

# 4. Unwrap the firmware from the Watch7,11 IPSW
IPSW=~/Downloads/Watch7,11_27.0.1_24R365_Restore.ipsw
WORK=./watch7_11-ipsw
mkdir -p $WORK darwin-vm/firmware
cd $WORK
unzip -o $IPSW \
  'kernelcache.release.watch7b' \
  'Firmware/sptm.t8310.release.im4p' \
  'Firmware/txm.watchos.release.im4p' \
  'Firmware/all_flash/DeviceTree.n218bap.im4p' \
  'Firmware/043-15866-581.dmg.trustcache' \
  '043-15866-581.dmg'

FW=../darwin-vm/firmware
ipsw img4 im4p extract kernelcache.release.watch7b             -o $FW/bootkc
ipsw img4 im4p extract Firmware/sptm.t8310.release.im4p        -o $FW/sptm
ipsw img4 im4p extract Firmware/txm.watchos.release.im4p       -o $FW/txm
ipsw img4 im4p extract Firmware/all_flash/DeviceTree.n218bap.im4p -o $FW/dtree.original
ipsw img4 im4p extract 043-15866-581.dmg                        -o $FW/ramdisk.dmg
ipsw img4 im4p extract Firmware/043-15866-581.dmg.trustcache    -o $FW/ramdisk.tc
cd ..

# 5. Fix up the device tree
( cd darwin-vm && ./dt_fixup.py -nvram nvram.bin firmware/dtree.original firmware/dtree )

# 6. Boot
cd darwin-vm
./qemu-sptm/build/qemu-system-aarch64 \
  -M darwin \
  -bootkc firmware/bootkc \
  -dtree  firmware/dtree \
  -sptm   firmware/sptm \
  -txm    firmware/txm \
  -tc     firmware/ramdisk.tc \
  -ramdisk firmware/ramdisk.dmg \
  -args "rd=md0 serial=3 -v -noprogress wdt=-1 wlan-olyhal-abort" \
  -nographic -serial mon:stdio -m 2G
```

Expected boot progression (serial log excerpt):

```
iBoot version: qemu-sptm
Darwin Image4 Extension Version 7.0.0 ... AppleImage4-374~13958 ... RELEASE_ARM64E
AMFI: Booted in a VM
<...dozens of kexts load: Backlight, Credential, Analytics, Serial, LightEmUp, SFRManifest...>
com.apple.xpc.launchd ... Darwin Bootstrapper Version 7.0.0 ... launchd/RELEASE_ARM64E
com.apple.xpc.launchd ... launching: speculative restored_external
restored_external[3] ... CHECKPOINT PROGRESS: START -> presumed_initial_monitor_no_return
executing /sbin/mount_tmpfs /mnt5
CHECKPOINT MONITOR: [0x1180] create_ramdisk
CHECKPOINT NOTICE: Image4 device: AP nonce clearable
CHECKPOINT WARNING: Skipping consuming APNonce because we have no nvram access yet
CHECKPOINT BEGIN/END: umask, setvbuf, update_root_mount, disable_watchdog,
                     set_progress_0, listen_for_log_client, create_listen_socket
CHECKPOINT BEGIN: MAIN:[0x040E] enable_usb
enabling USB (blocking) with description: standardRestore
```

Beyond `enable_usb` the restore workflow expects a host (Finder / `ipsw
restore` / the restore server) over the (emulated) USB endpoint, which this
bring-up does not provide — the ramdisk is doing exactly what it would on
real hardware with no cable plugged in.

Noise, not blockers:
- `AppleSEPManager` endpoint timeout (SEP is stubbed, same as all SPTM targets).
- `waiting for matching IOKit service IONVRAM` repeats forever on a side
  thread. `restored_external` treats it as a tolerated failure and continues.

## Why the kernelcache file is called `release.watch7b`

The "kernel-ext" in Apple's IPSW manifest is `watch7b` for both Watch7,8 and
Watch7,11 (Series 10 42mm / 46mm cellular). `BuildIdentities[*].Info.Variant`
tells you whether a given manifest entry is Erase / Upgrade / Recovery; the
Erase variant for `n218bAP` picks ramdisk `043-15866-581.dmg` + trustcache
`043-15866-581.dmg.trustcache`.

## Status

- Mid 2026-10-26 — first-ever reported watchOS 27 boot on T8310 in
  qemu-sptm, up to the restore USB wait, with these two patches.

## Upstream

The two-hunk patch is in `dt_fixup-watch7_11.patch`, ready for a PR to
`jprx/darwin-vm`. Both hunks are safe for the existing iPhone / Mac targets
(the `sgx` removal becomes a no-op when the child is already there, which it
was before — the `in` check short-circuits trivially; the top-level
`dock-connect*` removals only run if those nodes exist, which they don't on
any iPhone / Mac SoC jprx has tested).
