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

## Result (25 October 2026, late) — ramdisk mounted, kernel exec'd userland

Passing the OTA's own update ramdisk through `-initrd` and wiring `rd=md0` into
the boot args mounts it as root:

```
BSD root: md0, major 3, minor 0
apfs_vfsop_mountroot: apfs: mountroot called!
apfs_vfsop_mount: unable to root from devvp (root_device): 2       # DMG is HFS+, not APFS
apfs_vfsop_mountroot: apfs: mountroot failed, error: 2
hfs: mounted GraceUpdate17U224.arm64_32SURamDisk on device b(3, 0)
```

After the mount the console goes quiet, which is expected: launchd takes over,
further messages route through ASL/syslog instead of kprintf, and
`com.apple.restored` opens its XPC listener waiting for a paired iPhone that
never arrives. No panic / no `launchd died` → PID 1 is alive. To observe
userland on serial, append `-s` (single-user → launchd execs `/bin/sh` on the
console; with `-serial mon:stdio` an interactive shell appears).

Ramdisk source (OTA layout):

```
~/Downloads/<ota-hash>/AssetData/payload/replace/usr/standalone/update/ramdisk/arm64_32SURamDisk.dmg
```

Boot command:

```
qemu-system-aarch64 -smp 2 \
    -M t8030,trustcache-filename=$O/Firmware/018-09700-014.dmg.trustcache,kaslr-off=on \
    -kernel $O/kernelcache.release.watch5 \
    -dtb $O/Firmware/all_flash/DeviceTree.n146bap.im4p \
    -initrd $O/../payload/replace/usr/standalone/update/ramdisk/arm64_32SURamDisk.dmg \
    -append "debug=0x8 -v serial=3" \
    -serial mon:stdio -display none -no-reboot
```

Next milestones: confirm shell with `-s`; then reconstruct the full rootfs
from `AssetData/payloadv2/` for SpringBoard/UI.

## Result (25 October 2026, mid) — full IOKit up, waits on IOMedia

The kernelcache now boots through the full driver stack and quiesces on the
root block device. Tail of the console (headers only):

```
AppleT8006IO::start: chip-revision: B1
AppleInterruptController::start: _aicVersion = 2 _aicNumExtInts = 0x1c0 _aicNumIPIs = 4
AppleT8006GPIOIC::start: ...
AppleT8006MemCacheController::start: ... (+ tunables stream) ... finished
AppleT8006PlatformErrorHandler::start: SEP_FUSE_CFG map ... AMCC / AMCX / CP / DT0 / CCU0
AppleS8000AES::start: aes-version: 3
AppleARMLightEmUp::start: starting...
AppleS5L8940XI2CController::start: i2c1 ... i2c3 ...
RTBuddy(RTP): start(...)
AppleKeyStore starting / AppleSEPKeyStore::start: _sep_enabled = 1
AppleCredentialManager: start: returning, result = true
AppleSSE::start returning, result = 1
ERROR: AppleMobileApNonce::start: Timeout waiting for AES accelerator service   (expected, no HW)
ERROR: AppleARMPE::getGMTTimeOfDay: RTC did not show up                         (expected, no HW)
apfs_module_start:1689: load: com.apple.filesystems.apfs, v1412.100.68
com.apple.AppleFSCompressionTypeZlib kmod start
IOSurfaceRoot::installMemoryRegions()
Waiting on <dict ...><string>IOMedia</string>...<integer>0x1</integer>...</dict>
```

The kernel is **idle on the root-block-device matching dictionary** — the
expected quiesce point: the OTA bundle does not carry a mountable rootfs. See
[Phase 2c](#phase-2c-adapting-the-machine--cpu-to-t8006-in-progress) below for
the full chain of fixes that got us here.

Next milestone: supply a rootfs (ramdisk or synth image), reach launchd / userland.

## Earlier result (24 October 2026)

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

At that point our watchOS kernelcache loaded into QEMU and the kernel patchfinder ran on it (finding and patching AMFI, the trust
cache, `mac_mount` -- the code-signing bypasses), but no kernel console text appeared yet. Two patterns (`amfi_sha1`,
`trustcache16`) do not match because this is watchOS 6.3.1, not iOS 14; they are AMFI/trust-cache (userland) patches, not fatal to
early boot.

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

### Fixed (complete): the timebase (`gPEClockFrequencyInfo` seed)

Both early-boot panics — `rtclock ... invalid constant 0/1` and the generic-timer
`... invalid bit index` — have the same root: `gPEClockFrequencyInfo.timebase_frequency_hz`
(`0x…08c14e98`, `__DATA,__common`) is read by 8 early consumers but written only by
`pe_identify_machine`'s `/cpus` walk, which (gdbstub-confirmed) runs after them, so
each early reader sees 0. On real hardware iBoot has already populated it.

The complete fix is one line in `t8030_patch_kernel`: seed the global in guest RAM
at load time —
`*(uint64_t *)vtop_static(0xFFFFFFF008C14E98) = 24000000;`.
The kernel does not re-zero `__common` (verified: the value survives to the first
reader), so this single write makes **all 8 readers** see the real 24 MHz and the
system timer run correctly — no per-reader code patching, no whack-a-mole. (The DT
also carries `timebase-frequency = 24 MHz` for `pe_identify_machine`'s own use.)
Result: the boot clears both timebase panics and advances into IOKit.

### Blocker (bypassed): IOKit interrupt-controller resolution (AIC not registered) — DECOMPILED

The Data Abort (FAR 0) at `0x…07d2b9dc` is IODeviceTree interrupt resolution. The
crashing function (deep C++, all PAC-signed vcalls) walks devices and resolves each
one's `interrupt-parent`/`interrupts` against the collection of **registered
interrupt controllers** (a singleton at `0x…08c796a8`), matching by phandle. The
key string in hand is `x24 = "interrupt-controller"` (`0x…072c96fb`) and the target
phandle being resolved is `x19 = 0x10` — which is exactly the **AIC** (`/arm-io/aic`,
compatible `aic,1`, `AAPL,phandle = 0x10`, has the `interrupt-controller` property;
the node survives the DT filter). The search finds no controller for phandle `0x10`
(the collection is empty / the AIC is not in it), returns NULL, and the code
dereferences it (`mov x28, 0; ... ; ldr x8, [x28]`).

So the AIC is **not registered as an interrupt controller** at the point a device
resolves its interrupt-parent. On real hardware / the reference iOS boot the
platform expert registers the primary controller early. Candidates for our minimal
machine: this resolution runs before the AIC driver matches/registers (ordering),
or our stubbed platform does not drive the AIC registration. (Our DT has 5
interrupt-controller nodes — aic 0x10, gpio 0x23 `gpio,t8006`, spmi 0x26, aop-gpio
0x4f, smc-pmu 0x55; note `gpio,t8006` is **not** in `KEEP_COMP`, so the gpio ICs are
filtered out — a device referencing them would also fail, a second thing to fix.)

**Next:** determine why the AIC is absent from the registry — find where interrupt
controllers register into `0x…08c796a8` and whether the AIC's registration runs
before this resolution; then register it early (platform/DT) or fix the ordering.
Also add the T8006 device compatibles (e.g. `gpio,t8006`) to `KEEP_COMP` as needed,
in step with modelling/stubbing those devices. Console is still upstream.

**Update (decompiled + bypassed):** the function is `IODeviceTreeAlloc` (`0x…07d2a78c`,
2-phase: register controllers with `#interrupt-cells`+`AAPL,phandle`, then resolve
each node's interrupts via `0x…07d2b5b4`); gdbstub shows phase-1 registers **no**
controller (the add never fires), so the resolver NULL-derefs on the empty arrays
for the AIC phandle `0x10`. Why phase-1 skips the AIC is still open (DFS reach vs
the add's inner validity, or the primary controller is meant to be pre-registered
by the IOPlatformExpert). Per "patch now, clean later", `t8030_patch_kernel` now
no-ops the resolver (`movz w0,#1; ret`) to get past the NULL-deref -- interrupts are
not routed by this.

**Why phase-1 skips the AIC (resolved by re-decompile):** phase-1 adds a controller
to the resolver arrays only if, walking the `AAPL,phandle` property's attached-object
list (`prop->method_0x38()` then `->next`), one entry is **`&gIOPlatform`**
(`DAT_…08c12620`). That attachment is created by **`registerInterruptController`**,
which the controller's **driver** (`AppleInterruptController`) calls when it starts,
or the platform expert calls for the primary controller (the AIC is the only IC with
no `interrupt-parent`; it is marked `IOPrimaryInterruptController`). Our minimal boot
runs no IOKit driver matching and no platform-expert primary registration, so
`gIOPlatform` is never attached to phandle `0x10` → phase-1 skips the AIC. The DT
itself is correct (the `/arm-io/aic` node has `#interrupt-cells` + `AAPL,phandle=0x10`
+ `interrupt-controller`). **Consequence for the clean fix:** registering the AIC for
real means running the interrupt-controller driver/registration path, not just
force-adding the bare DT node (a driverless node would have none of the controller
methods the resolver and device drivers call). That is a deep IOKit subsystem, so the
practical route is to first identify exactly what the later bootstrap thread blocks on
(which needs a rebuilt symbol map — the kernelcache is stripped, `nsyms = 0`) and then
satisfy that specific wait.

### pmap PPL/xPRR consistency check (APRR) — RESOLVED

Past the resolver no-op, boot hit `panic("%s: modifying a PPL mapping pte_p=%p
pmap=%p prot=%d options=%u, pte=0x%llx, tmplate=0x%llx")` in `__PPLTEXT`
(`0x…08b22b14`). Decompiling the containing function (`0x…08b22650`, a pmap
protection-update over a VA range; REA `ev_87531d34`) showed the panic is a
**consistency check**, not a real protection fault: on every mapping it reads the
Apple **APRR** permission-remap registers `APRR_EL0` (`S3_4_C15_C2_0`) and
`APRR_EL1` (`S3_4_C15_C2_1`) and panics ("modifying a PPL mapping" / "creating an
xPRR mapping") unless they still hold the architectural defaults
`0x4545010167670101` / `0x4455445566666677` the kernel wrote at bring-up. Our sn0w
blanket RAZ/WI made them read **0**, so the check tripped on the first mapping
(gdbstub: `x0` = the format string, `x2` = `pte_p` in the physmap aperture). The
A13 core models only the A14-style **SPRR** encodings (`opc1=6`), not the A13-era
**APRR** pair (`opc1=4`) the S5/Tempest kernel uses — which is also why
`s3_4_c15_c2_1` originally trapped to Undefined Instruction.

**Fix (root-cause, in the sn0w layer):** `sn0w_cpu_install_apple_aprr()` defines
`APRR_EL0`/`APRR_EL1` to return those defaults, overriding the blanket RAZ/WI for
just those two encodings (SPRR-based MMU remapping is never enabled on this core,
so the PTE AP/XN/PXN bits keep their architectural meaning — no remap table is
consulted). The pmap panic is gone and boot advances through pmap.

### ppl_dispatch: the PPL call gate's APRR readback — RESOLVED

Past the pmap check, boot hit `panic("ppl_dispatch: failed due to bad
arguments/state")` (fmt `0x…08b83130`, via the panic trampoline `0x…08b1af1c`).
`ppl_dispatch` is a trampoline table (`0x…077f00a8`: `mov x15,<routine>; b
0x…07df6444`) into a common gate `0x…07df6444`. When PPL is locked down
(`__PPLDATA+0x188 != 0`) the gate jumps to the `__PPLTEXT` entry `0x…08b33fe0`,
which — note, **no `genter`**; this S5 kernel has no GXF instructions — enters PPL
mode purely by **writing `APRR_EL1` (`S3_4_C15_C2_1`) to the PPL-writable value
`0x4455405566666677`**, `isb`, then **reading it back and comparing**; a mismatch
takes the `x15=2` path that panics. Our first APRR fix made `APRR_EL1` an
`ARM_CP_CONST` (writes ignored), so the readback returned the default and the gate
failed. Fix: the APRR registers are now **real read/write** state
(`CPUARMState.sprr.aprr_el[]`, `sn0w_cpu_install_apple_aprr`), so the gate's write
sticks and the readback matches. The default and the PPL-mode value differ only at
APRR index 10, which the pmap check exempts (mask `0x440`), so one R/W register
satisfies both. ppl_dispatch now succeeds and PPL routines execute.

### APRR MMU permission-remap (PPL page RO write-fault loop) — RESOLVED (bring-up grant)

With PPL routines running, boot spun on a **Data Abort loop** (no panic): ESR
`0x9600004f` = write **permission fault, level 3**, FAR in the physmap aperture
(`0xffffffe066808000` / `…c000`), faulting PC `0x…07df30e0`/`…3108` inside
`bzero`/`dc zva` (`stp x1,x1,[x0]`, `x1=0`) — PPL zeroing a freshly allocated
page-table page. The abort vector retried the same instruction → endless loop.
Root cause: PPL maps its protected pages read-only, non-exec at EL1
(`AP[2:1]=0b10`, `XN=1` → xPRR indices 10/11) and relies on the **APRR permission
remap** to make them writable from inside PPL. The A13 QEMU core implements remap
only for the A14-style **SPRR** path (`ptw.c pte_to_sprr_prot_is_guarded`, gated by
`arm_is_sprr_enabled`), never for **APRR**, so translation used the raw RO bits.

Instrumenting `get_S1prot` showed the fault pages are index 10 (`ap=2,xn=1,pxn=0`)
with `sprr_en=0`, and — crucially — that the PPL-mode `APRR_EL1`/`APRR_EL0` tables
leave indices 10/11 **non-writable** (attr 0/R). So the real write-grant is **not**
a simple `APRR_EL1[idx]` lookup; it is the "PPL mode" CPU state itself. Fix
(bring-up): in `get_S1prot`, grant EL1 data R/W to EL1-read-only, non-executable
pages (`!is_user && ap==2 && xn`). PPL maps nothing else that way and nothing
outside PPL writes those pages, so boot behaves as on hardware while only relaxing
the (emulation-irrelevant) PPL write-protection. No TLB-flush/mode tracking needed.
Result: the loop is gone, PPL writes succeed, boot runs through PPL and VM init.
(A faithful per-index APRR remap remains a TODO.)

### Physical-timer FIQ delivery — FIXED

Past PPL/VM init, boot reaches the **scheduler** and runs the **idle thread**
(`0x…0784aca8`: reads `cntpct_el0`, arms a deadline via `0x…0784740c`, saves
context through `0x…07dfc920` → `0x…07dfd260`, loops on a processor-state field).
It made no visible progress because the **timer interrupt was never delivered**:
`apple_a13.c` wired only the **virtual** generic timer (`GTIMER_VIRT`) to the CPU
FIQ, but the watchOS S5 kernel drives the **physical** timer (`CNTP`,
`cntp_ctl_el0`; gdbstub showed `CNTP_CTL_EL0 = 0x5` = enabled, unmasked, ISTATUS=1,
and `CNTV_CTL_EL0 = 0`). Fix: wire `GTIMER_PHYS` into the FIQ OR-gate as well.
Verified: a breakpoint on the FIQ vector (`VBAR_EL1+0x300 = 0x…07df4100`, with
`VBAR_EL1 = 0x…07df3e00`) now **fires** — the timer FIQ is taken.

### Current blocker: the bootstrap thread does not progress (no console yet)

With the timer FIQ delivered, CPU0 still sits in the **idle thread** and prints no
console. Both-CPU sampling (QEMU monitor `info registers -a`) shows CPU0 cycling in
the idle/scheduler loop and **CPU1 parked at the reset vector** (`0x805df8000`) —
but this is **not** a CPU1 wait: `-smp 1` stalls identically. So the scheduler is
up and the idle thread runs, but the **bootstrap thread** (which would run
`console_init`, IOKit, and the rest) never makes progress — it is blocked and
nothing makes it runnable, so the idle loop spins forever and no console banner is
printed.

**Pinpointed (via a rebuilt symbol map).** The kernelcache is stripped
(`nm` → 0 symbols), so [`tools/.../port/symbolicate.py`](../../tools/opwwe231/watchos/port/symbolicate.py)
rebuilds an address→name map from `__func__` strings (a C-identifier `__cstring`
referenced by exactly one function names it): **6278 functions named**. An
`assert_wait` tracer (lldb breakpoint callback at `assert_wait = 0x…07844d6c`,
caller symbolicated via the map) then showed a boot thread calling `assert_wait`
**~200×** from the **same** site, all from the function at **`0x…0784802c`** on the
same event token (its own address, `adr x16,0x…0784802c; paciza`). That function
spins: `ldrb w8,[0x…08bcb550]; cmp w8,1; b.eq done; … assert_wait; thread_block` —
it **waits for the byte flag at `0x…08bcb550` to become 1**, which never happens, so
it blocks, is re-woken (timer), re-checks, re-waits, forever. The flag's only setter
is `0x…0784bc54` (store at `0x…0784be64`), gated by a counter/threshold
(`[0x…08b547cc]` vs `[0x…08bcb310]`) and a lock (`ldxr`/`casa`/`wfe` on
`0x…08c57470`) — a synchronization/rendezvous primitive that never reaches its
condition on our minimal machine. The functions are called through PAC'd vtables
and reference no strings, so they are not named yet. Runtime values at the stall
(gdbstub): flag `[0x…08bcb550] = 0`, `[0x…08b547cc] = 2`, threshold
`[0x…08bcb310] = 1` → `2 < 1` is false, so `do_work` (which stores the flag) never
runs. The counter-writer computes `popcount` of a mask at `0x…08b547c8`, but that
word read `0xffffffff` (a `-1`/"none" sentinel, popcount 32 ≠ 2), so it is **not** a
clean 2-CPU mask and the struct is not fully understood. The threshold is a
widely-read global (~15 readers) set by `0x…0783dd14`. **Next (focused RE):**
cross-reference these global offsets (`0x…08bcb300/310/518/548/550`,
`0x…08b547c8/cc`, lock `0x…08c57470`) against XNU open source to name this
primitive (likely `osfmk/kern` or `osfmk/arm64`), which reveals who should set the
flag, then supply it — a stubbed-coprocessor completion (RTC/SMC/AOP mailbox), an
AIC-routed device IRQ, or a second CPU (the DT has two `apple,tempest` nodes and
CPU1 never leaves the reset vector). 4 GB boot is also slow under TCG.

### DockChannel serial console + the `debug-enabled` gate — MODELED (console still deferred)

To try to *see* the boot log, the machine now models the **Apple DockChannel UART**,
which is the watchOS S5 serial console (not the Samsung `uart0`). `t8030.c`
`t8030_create_dockchannel()` maps the two register windows from the
`/arm-io/dockchannel-uart` DT node (`reg = [0x3d128000, 0x10000, 0x3d10c000,
0x4000]`) with the XNU-6153 pexpert layout (`pe_serial.c`/`dockchannel.h`): TX
`WDATA1 = +0x4004`, TX-free-space `WSTAT = +0x4014` (always reports space so
`serial_init`'s `while(!tr0())` exits), RX empty. `xnu.c` **keeps** the
`dockchannel-uart` DT node (it was being stripped by `REM_NAMES`), DockChannel
takes `serial_hd(0)` and the Samsung UARTs shift to `serial_hd(i+1)`.

Real bug fixed on the way: the DockChannel windows sit **inside** the broad
`pmgr-unk-reg-18` placeholder range (the `pmgr` DT `reg` covers
`soc+0x3d100000..+0x3d1a0000`), which **shadowed** the device — `info mtree -f`
showed only the placeholder. Mapping DockChannel with
`memory_region_add_subregion_overlap(..., 1)` (priority 1) makes the real device
win for its exact sub-ranges, the same technique as the boot-display vram mapping.

Also set `/chosen debug-enabled = 1` in the DT. A production-fused unit leaves it
`0`, which makes `PE_i_can_has_debugger()` return false and **suppresses the serial
console and kprintf entirely** — the gate that keeps a release kernelcache silent.
iBoot sets it on a development-fused unit; we provide the same contract.

### DECISIVE finding: the kernel does ZERO arm-io MMIO — blocked before IOKit

Instrumenting every SoC MMIO handler (PMGR reg + unk, AIC read/write, DockChannel)
with counters shows the kernel performs **no arm-io MMIO of any kind** across the
whole run — not one PMGR read/write, not one AIC access, not one serial byte — even
with the DockChannel device correctly mapped (confirmed on top via `info mtree`) and
`debug-enabled = 1`. Yet CPU0 runs the **idle loop healthily** (both-CPU PC sampling:
CPU0 cycles `0x…0784adb8 ↔ 0x…0784add8`, i.e. it wakes from WFI — the physical-timer
FIQ is being delivered — arms the next deadline via `0x…0784740c`, re-idles; CPU1
stays at the reset vector `0x805df8000`, never started because the kernel never
writes the PMGR cpu-start reg, but `-smp 1` is identical so CPU1 is not the blocker).

This **reframes** the stall: reaching the scheduler idle loop means `serial_init`
and `sched_init` ran, but the **bootstrap thread is blocked before any device I/O**,
i.e. before IOKit device matching. On this SoC the early console is deferred, so
until the bootstrap thread unblocks and IOKit attaches the serial driver, **no
console appears regardless** — the DockChannel model + `debug-enabled` are necessary
groundwork but cannot produce output on their own. The kernel's `pram`/embedded
panic-log region (`0x8ffeb0000`) is all-zero during normal boot (it is a panic-only
mirror), so it is not a shortcut to the boot log either. This is consistent with the
`assert_wait` analysis above: the bootstrap thread waits on a synchronization
primitive (`flag 0x…08bcb550`) that never reaches its condition on the minimal
machine. **Next:** identify what the bootstrap thread waits on in early
`kernel_bootstrap` (post `sched_init`, pre IOKit) and supply it.

### REFINED (decisive): the stall is an IOKit matching deadlock on `waitForService`

A `thread_block` tracer over the QEMU gdbstub (lldb breakpoint at
`thread_block = 0x…07845d18`, manual `x29` frame-chain unwind, symbolicated
against the rebuilt map) settles the question. The kernel is **not** slow and
**not** pre-IOKit — it is **in IOKit matching** and deadlocked:

- It boots normally and spins up ~24 kernel threads (`sched_maintenance_thread`,
  `mapping_replenish`, the `thread_call` pool, VM threads, …) that block/wake
  via the timer FIQ. After ~145 `thread_block` events the system **plateaus**:
  zero new call sites appear; only the periodic maintenance thread
  (continuation `0x…0078402c`, the known red herring) keeps cycling.
- The blocked worker threads' backtraces land in the **IOKit matching machinery**
  (`0x…07d3edd8` → `0x…07d3bba4` → `0x…07d3c0e0` = `_IOServiceJob`/
  `doServiceMatch`/`probeCandidates`, `IOService::startCandidate` at
  `0x…07e62024` in **AppleARMPlatform**, through `SystemPowerProfileOverrideDict`).
  Each ends in a **`waitForService`-style wait** (`0x…07d4a8c8`: installs a
  matching notification on IOKit's service globals and blocks on a semaphore
  until a matching provider is published), then `thread_block`.

So several drivers (AppleARMPlatform among them) call `waitForService` /
`waitForMatchingService` during their `start`/`probe` and block **before they
touch any hardware** — which is exactly why there is zero arm-io MMIO. The
awaited provider services never publish because their backing devices were
**skipped** (coprocessors) or the **primary interrupt controller was never
registered** (the IODeviceTree resolver is no-op'd). IOKit matching therefore
deadlocks and boot never reaches the console handoff / root mount.

This is the concrete form of OPTION 3. **Next:** decompile the blocked drivers'
`start` (AppleARMPlatform `0x…07e62024`; the driver at `0x…078c596c`; the one at
`0x…07cfea70`) to read the exact matching dictionaries / service keys they wait
on, then publish those services — register the primary AIC properly (so
interrupt-driven matching proceeds) and model/stub the awaited coprocessor
services so they match. Tooling: `thread_block` tracer + fp-chain unwinder in
scratch (`tblock_trace.py`, `unwind_block.py`); PAC-strip a stacked return addr
with `(x & 0xffffffffff) | 0xfffffff000000000`, never a bare 48-bit mask.

### RE tooling: REA driven by radare2

REA's analysis providers are Ghidra/Hopper/IDA/Kuna (no radare2 slot), but its
rizin-backed commands (`execute-rizin-command`, `debug-with-rizin`) resolve the
`rizin` executable from `PATH`. [`tools/.../port/rea-r2.sh`](../../tools/opwwe231/watchos/port/rea-r2.sh)
shims `rizin` → `r2` on `PATH` so those commands run **radare2** instead (it
accepts REA's `-N -q -c` invocation and the core `aa`/`aar`/`axt`/`af`/`pdf`/`pd`
commands, and loads this fileset kernelcache natively with arm64e fixups). Use
it like `rea-r2.sh execute-rizin-command <kc> '<r2 commands>'`.

### BREAKTHROUGH: console works, the AIC registers, the core deadlock is broken

The entire blind-hang / IOKit-matching-deadlock that blocked every prior session
had **one** root cause: the fork's DeviceTree filter (`xnu.c macho_dtb_node_process`)
removes any node whose `compatible` is not in `KEEP_COMP`, and the list carried
only `arm-io,s8000` / `arm-io,t8030` — **not** `arm-io,t8006`. So the whole
`/arm-io` subtree (the AIC, PMGR, GPIO, I2C, the IOP coprocessors, **and the
DockChannel console node**) was being stripped from the DT the kernel sees. With
no AIC node, `gIODTPHandles` was empty and the IODeviceTree interrupt resolver
NULL-dereffed on the CPUs' `interrupt-parent` phandle `0x10` (the reason the
resolver no-op bandaid was needed); with no `dockchannel-uart` node the serial
console never came up (the whole reason we were blind).

- **Fix 1 — `KEEP_COMP` (xnu.c):** add the T8006/T8002 compatibles
  (`arm-io,t8006`, `pmgr1,t8006`, `gpio,t8006`, `i2c,t8006`, `wdt,t8006`,
  `aic,t8006`, `dockchannel,t8002`, `dockchannel-uart`, `mcc,t8006`,
  `error-handler,t8006`). `/arm-io` and its children now survive → **the
  DockChannel console produces output** (we can finally read the kernel/panic log)
  and boot reaches `AppleInterruptController::start`.
- **Fix 2 — ipid-mask (t8030.c `t8030_create_aic`):** pad the aic node's
  `ipid-mask` from 52 → 56 bytes. `apple_aic_create` rounds `numEIR` up to an even
  14 (`(len/4+1)&~1`), so the modelled AIC advertises 14 EIRs / 448 IRQs via
  `rAIC_INFO`, and `AppleInterruptController::start` then demands a `numEIR*4 = 56`
  byte mask ("length is 52 but should be 56", `AppleInterruptControllerV1.cpp:128`).
  → **the AIC driver starts and registers** (`_aicVersion=2`,
  `_aicNumExtInts=0x1c0`, `_aicNumIPIs=4`). The interrupt-controller deadlock is
  **broken**.

Boot now runs visibly through corecrypto/FIPS (all PASSED), AppleCredentialManager,
Darwin Image4, `AppleT8006IO::start` (chip-revision B1), and the AIC, then stops
at the next blocker: **`ApplePMGR::initDriver` (`ApplePMGR.cpp:860`, "REQUIRE
failed: `_optionalBridges & (0x1 << i)`")** — a T8006 PMGR bridge-config mismatch
(the pmgr node has 24 `reg` bridges; a mandatory one is absent from the driver's
mapped set). Debugging is now fast: run, read `con.log`, see the exact panic
string with `file:line`. Tooling for the kernelcache RE is driven through
radare2 (`tools/.../port/rea-r2.sh`, with `pdg`/`pdd`) plus the Apple open-source
`xnu-6153.141.1` for struct/mechanism reference.

### How to diagnose a boot crash

`-serial file:con.log -D dbg.log -d int,guest_errors,unimp` gives the exception
stream; `-d in_asm` dumps each translated block **once** (this build is
`--disable-capstone`, so blocks print as raw `OBJD-T:` bytes — decode the leading
word, little-endian). A release kernelcache has asserts compiled out, so a missing
DeviceTree node or unset pointer surfaces as a Data Abort to a near-null FAR rather
than a message. Map a runtime virtual address to the kernelcache with REA/rizin
(`s <vaddr>; pd`) — slide is 0 with `kaslr-off=on`.
