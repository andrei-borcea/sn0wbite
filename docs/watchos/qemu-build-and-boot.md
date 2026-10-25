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

### Current blocker: interrupt delivery + secondary CPU (the bypassed AIC)

Past PPL/VM init, boot reaches **early SMP / platform init and timer-wait/idle
loops** (`0x…0784aca8`, which reads `cntpct_el0`, arms a deadline via
`0x…0784740c`, and loops on a processor-state field) — but makes no visible
progress and prints no console. Both-CPU sampling (QEMU monitor `info registers
-a`): **CPU0** cycles in these early-init/idle waits, **CPU1 never leaves the reset
vector** (`0x805df8000`). Root cause: we earlier **no-op'd the IODeviceTree
interrupt resolver and skipped the AIC** device, so **no interrupts are delivered**
(no timer tick, no IPI) and the **secondary CPU is never started**. The kernel's
timed waits and scheduler therefore never advance, and `console_init` (further on)
is not reached. 4 GB boot is also slow under TCG. **Next (the real next phase):**
bring up interrupt delivery — model/wire the **AIC** (timer IRQ at minimum, then
IPIs) so timer interrupts fire and CPU1 can be started. This is the clean AIC fix
deferred twice; it is the gate to a live, console-producing boot.

### How to diagnose a boot crash

`-serial file:con.log -D dbg.log -d int,guest_errors,unimp` gives the exception
stream; `-d in_asm` dumps each translated block **once** (this build is
`--disable-capstone`, so blocks print as raw `OBJD-T:` bytes — decode the leading
word, little-endian). A release kernelcache has asserts compiled out, so a missing
DeviceTree node or unset pointer surfaces as a Data Abort to a near-null FAR rather
than a message. Map a runtime virtual address to the kernelcache with REA/rizin
(`s <vaddr>; pd`) — slide is 0 with `kaslr-off=on`.
