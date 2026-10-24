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

### Patched: the rtclock timebase panic (kernel patch, bring-up)

Fully traced (gdbstub + REA `ev_690d2eda`): `clock_timebase_init` (`0x…079329bc`)
reads `gPEClockFrequencyInfo.timebase_frequency_hz` (`0x…08c14e98`), whose only
writer in the kernel is `pe_identify_machine`'s `/cpus` walk (`0x…07dd01e8`) —
which a gdbstub race shows runs *after* `clock_timebase_init`. So the global is 0,
the callback gets `{0, 1}`, its `a >= b` check fails, and it panics "invalid
constant 0 / 1". An init-ordering problem, not a missing DT value.

Per the "patch now, clean later" decision, `t8030_patch_kernel` now overwrites the
two instructions at `0x…079329bc` (`adrp x8, gPEClock; ldr x8,[x8,#0xe98]`) with
`movz x8,#0x3600; movk x8,#0x16e,lsl#16` (x8 = 24000000), so `clock_timebase_init`
always sees 24 MHz. Verified: the global now reads `0xd286c008/0xf2a02dc8` and the
rtclock panic is gone. (The DT `timebase-frequency = 24 MHz` on root + cpu nodes
stays — it is the value `pe_identify_machine` stores, just too late to matter.)
**Clean follow-up:** establish the timebase before `clock_config` instead of
patching (see memory).

### Current blocker: `panic("%s: invalid bit index (%u)")` in timer setup

With the timebase patched the boot advances to a new single trap, whose message
(x2) is **`"%s: invalid bit index (%u)"`** (REA `ev_66a48bd7`). The firing site is
`0x…0793df58` (x7 caller = `0x…0793df64`): ARM generic-timer setup that writes
`CNTKCTL_EL1` / `CNTP_CTL_EL0` and validates a "bit index" (`b.hs` -> panic) before
`bfi`-ing it into `CNTKCTL_EL1`. The index (x8, computed just before `0x…0793df20`)
is out of range. Next: find where that bit index comes from (likely the timer IRQ
/ a per-cpu timer vector from the DT or `aic-timebase`) and why it is invalid.
Still upstream of `serial_init` (console silent).

### How to diagnose a boot crash

`-serial file:con.log -D dbg.log -d int,guest_errors,unimp` gives the exception
stream; `-d in_asm` dumps each translated block **once** (this build is
`--disable-capstone`, so blocks print as raw `OBJD-T:` bytes — decode the leading
word, little-endian). A release kernelcache has asserts compiled out, so a missing
DeviceTree node or unset pointer surfaces as a Data Abort to a near-null FAR rather
than a message. Map a runtime virtual address to the kernelcache with REA/rizin
(`s <vaddr>; pd`) — slide is 0 with `kaslr-off=on`.
