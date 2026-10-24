# Building the QEMU fork and booting the watchOS kernelcache

Phase 2b/2c. The vehicle toward the UI is a QEMU full-system machine with Apple device models. We build the `qemu-t8030` fork
(reference, not committed) and boot our watchOS kernelcache on its `t8030` machine, feeding our real T8006 DeviceTree -- the
machine reads the SoC base and every device address from the DeviceTree, so our devices land at our addresses.

## Build (macOS, Apple Silicon)

```sh
git clone --depth 1 https://github.com/TrungNguyen1909/qemu-t8030.git && cd qemu-t8030
git tag -a v6.2.0 -m t8030                         # the version script needs an annotated tag (shallow clone has none)
brew install lzfse meson ninja                     # lzfse decompresses the kernelcache; the rest build QEMU
./configure --target-list=aarch64-softmmu --disable-docs --disable-gtk --disable-sdl --disable-vnc \
  --disable-werror --disable-capstone --disable-slirp --disable-tools --disable-guest-agent --enable-cocoa \
  --extra-cflags=-I/opt/homebrew/opt/lzfse/include --extra-ldflags=-L/opt/homebrew/opt/lzfse/lib
make -j4
codesign --sign - --force build/qemu-system-aarch64-unsigned
cp build/qemu-system-aarch64-unsigned build/qemu-system-aarch64
```

Result: `qemu-system-aarch64` 7.1.0 with the `t8030` and `s8000` machines. `-cpu apple-a13` provides PAC, GXF and the Apple IMPDEF
registers, so the 331,911 PAC sites run natively -- no patch.

## Boot our kernelcache

```sh
O=<extracted OTA>/AssetData/boot
qemu-system-aarch64 -M t8030,trustcache-filename=$O/Firmware/018-09700-014.dmg.trustcache,kaslr-off=on \
  -kernel $O/kernelcache.release.watch5 -dtb $O/Firmware/all_flash/DeviceTree.n146bap.im4p \
  -append "debug=0x8 -v serial=3" -serial mon:stdio -display none
```

`-kernel` takes the raw IM4P kernelcache (QEMU unwraps IM4P + lzfse); `-dtb` takes the raw IM4P DeviceTree (payload type `dtre`).

## Result (24 October 2026)

```
Loading watchOS 6.3...
kernel_low: 0xfffffff004000000
kernel_high: 0xfffffff008d838a8
kpf_amfi_callback: Found AMFI (Leaf)
kpf_amfi_callback: Found lookup_in_trust_cache_module @ 0xfffffff00793e728
KPF: Found mac_mount
kpf_amfi_callback: Found AMFI (Routine)
KPF: Found AppleKeyStoreUserClient::handleUserClientCommandGated
```

So **our watchOS kernelcache loads into QEMU, the kernel patchfinder runs on it** (finding and patching AMFI, the trust cache,
`mac_mount` -- the code-signing bypasses), and the machine initializes from our DeviceTree. The CPU then runs with no unhandled
instructions and no guest errors -- but no kernel console text appears yet. Two patterns (`amfi_sha1`, `trustcache16`) do not match
because this is watchOS 6.3.1, not iOS 14; they are AMFI/trust-cache (userland) patches, not fatal to early boot.

## Next (console output, phase 2c)

The watch (S5) uses the **`dockchannel-uart`** for the debug serial, not the Samsung `uart0` the `t8030` machine routes to stdio.
The kernel is almost certainly running but printing to a channel QEMU does not surface. The next steps:

1. Route the console: wire `dockchannel-uart` (or point the kernel at `uart0`) to `-serial stdio`.
2. Build a `hw/arm/t8006.c` machine from `t8030.c` with the Tempest CPU and our [device map](./platform-device-map.md), trimming
   the T8030-only coprocessors so no device-create asserts.
3. Update the two missing patchfinder patterns for 17U224.

See the [port plan](./boot-chain-port-plan.md) for the full phased route to the UI.
