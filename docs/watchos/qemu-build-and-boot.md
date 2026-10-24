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

## Phase 2c: adapting the machine + CPU to T8006 (in progress)

The `t8030` machine did not even start the vCPU on T8006: its device setup
asserted/crashed on DeviceTree differences before the first instruction, and
once that was cleared the CPU trapped on Apple registers the A13 model lacks.
The adaptation ([t8006-adaptation.patch](../../tools/opwwe231/watchos/qemu/t8006-adaptation.patch),
status in its [README](../../tools/opwwe231/watchos/qemu/README.md)) works the chain:

1. **CPU model.** `apple_a13_cpu_create` crashed reading `cluster-id`/`cluster-type`
   (A13 is big.LITTLE; single-cluster Tempest has neither). Made optional. **Done.**
2. **Coprocessors.** The machine asserted in `apple_sart_create` (and would in
   ANS/SMC/SIO/SPMI/…) on T8006 DeviceTree differences. Skipped those creates for a
   minimal boot-to-console machine. **Done.**
3. **nvram.** Skipping the coprocessors removed the `nvram` device (an NVMe
   namespace created inside the SMC/ANS), which `t8030_memory_setup` required.
   Made optional — boot proceeds with an empty nvram proxy, which is not needed to
   reach console. **Done:** machine init now completes and the kernel is launched
   (`g_virt_base 0xfffffff004000000`, `entry 0x…805df8538`, cmdline passed).
4. **Apple IMPDEF registers.** The first guest instruction trapped on
   `msr s3_4_c15_c2_1` (a 2-instruction write thunk at `0xfffffff008b44010`, REA
   `ev_a791894f…`) — an Apple IMPDEF register (`op0=3,CRn=15`) the A13 model does
   not implement, so QEMU raised Undefined Instruction and the CPU spun in its
   vector. Fixed by the modular **[sn0w CPU layer](../../tools/opwwe231/watchos/qemu/sn0w/)**,
   which RAZ/WIs the whole Apple IMPDEF space (the A13's real registers still
   override their own encodings). **Done:** the kernel now runs deep into early
   boot — it sets PAN and performs per-CPU PPL lookups keyed on `MPIDR_EL1 & 0xff`.

### Resolved: `"No cpus found!"` (DeviceTree cpu filter)

Root-caused with the QEMU gdbstub + REA. The `TPIDR_EL1 == 0` Data Abort was a
symptom: the function at `0x…07824328` is `panic_trap_to_debugger`, reached from
`panic("No cpus found!")` (REA `ev_aa85dcf4`). `ml_parse_cpu_topology` (REA
`ev_189cc62f`) enumerated **zero** CPUs: `DTFindNode("/cpus")` succeeded but the
child iterator returned none — the flattened `/cpus` node had `child_node_count = 0`
(confirmed by reading guest memory at the node). The cause: the fork's DeviceTree
filter `macho_dtb_node_process` (`hw/arm/xnu.c`) **removes any node whose
`compatible` is not in the `KEEP_COMP[]` whitelist**, and that list has
`apple,twister/thunder/lightning` (A9/A13) but not **`apple,tempest`** (the S5
core). So both `cpu0`/`cpu1` (compatible `apple,tempest`) were stripped. Fix:
whitelist `apple,tempest` in `KEEP_COMP`. Result: the 2.4 M-trap death spiral is
gone and the kernel runs far past cpu-topology into early per-CPU / pmap init.

### Current blocker: `panic("rtclock timebase_callback: invalid constant")`

The single `0xe7ffdeff` trap at `0x…078241b0` is the kernel's panic-finalizer
(record context via `0x…078241f8`, then the `0xe7ffdeff` halt). gdbstub read of
its args gives the message (x2) = **`"rtclock timebase_callback: invalid constant
%ld / %ld"`** (REA `ev_be8eb31d`). That callback validates a mach-timebase
`{numer, denom}`: it requires **`denom` in `[1,4]`** and `numer >= denom` (24 MHz
-> `125/3`, denom 3 — valid). It receives `{0,0}` and panics.

Root: `gPEClockFrequencyInfo` (at `0x…08c14e90`) is still **all zero** when the
callback runs (confirmed by reading guest memory at the panic). The kernel's
`pe_init` DeviceTree walk reads `"timebase-frequency"` (two readers, REA
`ev_5e856fa1`) into `gPEClockFrequencyInfo.timebase_frequency_hz`
(`0x…08c14e98`), but `rtclock_timebase_callback` fires **before** that walk
populates it. So this is an init-ordering / missing-platform-setup issue, not
just a missing DeviceTree value.

The adaptation now provides `timebase-frequency = 24 MHz` on the DT root and the
cpu nodes (our T8006 DeviceTree ships every per-cpu frequency as 0; iBoot fills
them on real hardware, read as 8-byte values — a 4-byte prop makes the kernel
read 4 garbage high bits). That is correct and necessary (the `pe_init` walk will
read it) but does not by itself clear the panic, because the timebase is consumed
before the walk. **Next step:** find who calls `rtclock_timebase_callback` and
where its `{numer,denom}` comes from that early — likely the timebase must be
established from `boot_args`/the `aic-timebase` device (iBoot's job) before
`rtclock_init`, or an init step we stubbed is the one that normally sets it.

### How to diagnose a boot crash

`-serial file:con.log -D dbg.log -d int,guest_errors,unimp` gives the exception
stream; `-d in_asm` dumps each translated block **once** (this build is
`--disable-capstone`, so blocks print as raw `OBJD-T:` bytes — decode the leading
word, little-endian). A release kernelcache has asserts compiled out, so a missing
DeviceTree node or unset pointer surfaces as a Data Abort to a near-null FAR rather
than a message. Map a runtime virtual address to the kernelcache with REA/rizin
(`s <vaddr>; pd`) — slide is 0 with `kaslr-off=on`.
