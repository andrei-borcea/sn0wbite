# Reconstructing the watchOS boot chain

Goal: map the watchOS 6.3.1 (`17U224`, Watch5,4 / `n146bap`, Apple S5 / T8006) boot chain component by component, and
record the exact interface the watchOS kernel requires at entry, so a loader on our hardware (or an emulation harness) can
reproduce it. Decompilation done with the REA skill (Ghidra/rizin) on the artifacts extracted from the supplied OTA.
Measurements on our hardware are in [bring-up status](./bring-up-status.md); the ISA survey is in
[boot and ISA analysis](./watchos-boot-and-isa-analysis.md). No Apple firmware is committed; this note publishes findings only.

## What the OTA contains, and what is decompilable

| Stage | File | State | Decompilable |
| --- | --- | --- | --- |
| SecureROM (boot ROM) | on-die | not in the OTA | no |
| LLB | `all_flash/LLB.n146b.RELEASE.im4p` | IM4P `illb`, no KBAG, high-entropy payload | **no (encrypted)** |
| iBoot | `all_flash/iBoot.n146b.RELEASE.im4p` | IM4P `ibot` `iBoot-5540.144.2`, encrypted payload | **no (encrypted)** |
| iBSS / iBEC (DFU) | `dfu/iBSS…`, `dfu/iBEC…` | IM4P, encrypted | **no (encrypted)** |
| SEP | `all_flash/sep-firmware.n146b.RELEASE.im4p` | IM4P, encrypted | **no (encrypted)** |
| DeviceTree | `all_flash/DeviceTree.n146bap.im4p` | IM4P `dtre`, **plaintext** | **yes** (already extracted) |
| kernelcache | `kernelcache.release.watch5` | Mach-O `kernelcache`, **plaintext**, 30 MB | **yes** |

The iBoot family payloads carry no `KBAG` tag but are high-entropy with no compression magic: AES-encrypted with keys derived
on the SoC from the fused GID key, which the OTA does not contain. So the Apple boot loaders cannot be read. The two artifacts
we can study are the **kernelcache** and the **DeviceTree**, and together they fully define the kernel's entry contract, which is
what a replacement loader has to satisfy.

## The kernel entry contract

From the kernelcache reset vector (REA on the extracted Mach-O, base `0xfffffff007004000`; `entry0` at `0xfffffff007df8538`
branches to `start` at `0xfffffff007dfc06c`; evidence `ev_e5cf5c62…`, `ev_35dab065…`, `ev_d1fa7004…`).

At entry the kernel expects:

- **EL1**, MMU off, caches off. `start` itself does `msr daifset, 0xf` and `msr oslar_el1, xzr`.
- **`x0` = physical pointer to a `boot_args` struct.** `start` does `mov x20, x0` and then reads:

  | Load | Offset | boot_args field |
  | --- | --- | --- |
  | `ldr x22,[x20,#0x08]` | 0x08 | `virtBase` |
  | `ldr x23,[x20,#0x10]` | 0x10 | `physBase` |
  | `ldr x24,[x20,#0x18]` | 0x18 | `memSize` |
  | `ldr x25,[x20,#0x20]` | 0x20 | `topOfKernelData` (scratch base for the kernel's own page tables) |
  | `ldr x26,[x20,#0x2d0]` | 0x2d0 | an arm64e field used by the early PAC/rebase path (name unconfirmed) |

  The full layout is in [`boot_args.h`](../../tools/watchos/bootargs/boot_args.h); `deviceTreeP` is at 0x60,
  `CommandLine` at 0x6c. [`mk_bootargs`](../../tools/watchos/bootargs/mk_bootargs.c) emits this struct.

- The kernel **builds its own page tables** at `topOfKernelData` in **16 KB-granule** form: the table stride is `4, lsl 12`
  (0x4000 = 16 KB) and the block mappings are `0x2000000` (32 MB, the 16 KB-granule L2 block size), then
  `ttbr1_el1 = topOfKernelData + 0x4000`.
- It then programs **`TCR_EL1 = 0x000800226519a519`** (a constant at `0xfffffff0075b2ba8`): TG0 and TG1 = **16 KB granule**,
  T0SZ = T1SZ = 25 (**39-bit** VA each), IPS = 2 (40-bit PA). This is unconditional — there is no 4 KB path.

So a loader that wants this exact kernel to run has to: load the kernelcache at `physBase`, place a flattened Apple DeviceTree
and a `boot_args` in memory, set `x0`, drop to EL1 with MMU/caches off, and branch to the kernel's physical entry.

## Boot-time Apple-isms the reset vector executes (the patch catalog for a non-Apple core)

`start` runs these unconditionally before the first C function, so on any core that lacks them they trap or mis-execute. To run
this kernel on the Kryo (A53-class) cores these are the sites to neutralize (confirmed absent on our cores by
[`cpuprobe`](../../tools/watchos/probe/cpuprobe.c): API/APA/GPA/GPI = 0, LSE = 0):

| Site in `start` | Instruction | Requires | On Kryo A53 |
| --- | --- | --- | --- |
| `msr tcr_el1, x1` (16 KB) | TCR TG0/TG1=16 KB | FEAT_TGran16 | likely UNDEF-maps / faults (confirm `ID_AA64MMFR0.TGran16` at EL1) |
| `msr apiakeylo/hi_el1`, `apib*`, `apda*`, `apdb*`, `apga*` | PAC key load | FEAT_PAuth | **UNDEF** |
| `mrs/msr s3_4_c15_c0_4` (+`orr 1`,`orr 4`) | Apple APCTL (enable PAC) | Apple IMPDEF | **UNDEF** |
| `msr s3_4_c15_c1_0/1`, `s3_0_c15_c4_0/1`, `s3_0_c15_c1_0` | Apple IMPDEF CPU regs | Apple IMPDEF | **UNDEF** |
| `msr sctlr_el1, x0` with Apple bits | SCTLR EnIA/EnIB + Apple | FEAT_PAuth | the PAC enable bits have no effect / fault |

Beyond `start`, the ISA survey counted about 270,000 PAC and 10,200 LSE sites across the kernelcache and 72 distinct Apple
IMPDEF registers ([boot and ISA analysis](./watchos-boot-and-isa-analysis.md)). A native bring-up has to force 4 KB in TCR and
rebuild the page-table bootstrap, and NOP or trap-emulate every PAC and Apple-IMPDEF site.

## Component map: watchOS stage → our side

| watchOS stage | What it does | Our side |
| --- | --- | --- |
| SecureROM → LLB → iBoot | verify and load images, build `boot_args` + DeviceTree, hand off at EL1 | a loader we write: the Qualcomm ABL can't chain this, so a payload in `boot`/`recovery` that stages the kernelcache, synthesizes `boot_args` (`mk_bootargs`) and the DeviceTree, then branches to the kernel |
| DeviceTree (Apple) | describes AIC, 13 DART IOMMUs, 7 ASC coprocessors, PMGR, display, GPU, ANE | we have the real one extracted; the kernel platform code reads it. Our SoC has none of these blocks, so platform drivers must be replaced or stubbed |
| kernelcache `start` | 16 KB page tables, PAC, Apple sysregs, MMU on | patch catalog above |
| kernelcache `arm_init` → `kernel_bootstrap` | consume `boot_args`, `PE_init_platform`, pmap, VM, scheduler | next decompilation target: confirm the full `boot_args` tail (offset 0x2d0) and the DeviceTree handoff |
| platform drivers (AIC/PMGR/DART/ANS/display) | Apple-SoC specific | replace with Qualcomm equivalents, or run under emulation for study |

## Testing the boot in emulation

A generic ARMv8.3 core (QEMU/Unicorn) has the features the real Kryo lacks (PAC, 16 KB granule, LSE), so it is where the
*software* half of the boot chain is tested before any hardware work. [`emu/boot_harness.py`](../../tools/watchos/emu/boot_harness.py)
(Unicorn) loads the kernelcache segments at their addresses, puts a synthesized `boot_args` in `x0`, and runs the reset vector,
emulating the Apple IMPDEF and PAC registers live (the patch catalog applied at runtime: `msr` recorded, `mrs` returns the last
written value, `retaa/retab` -> `ret`).

Result (stage 1): the reset vector **reaches the MMU-enable instruction** (`msr sctlr_el1`, `0xfffffff007dfc3b8`). On the way it
reads every `boot_args` field we supply (virtBase, physBase, memSize, topOfKernelData, the 0x2d0 field), writes 8258 entries into
the 16 KB page tables at `topOfKernelData`, and sets TCR/TTBR/MAIR. The only things it needs are **16 Apple/PAC register sites**,
all in the reset vector: the ten PAC key registers (`S3_0_C2_C1_0..C3_1` = APIA/APIB/APDA/APDB/APGA key lo/hi) and Apple security
registers (`S3_4_C15_C0_4` APCTL, `S3_4_C15_C1_0/1`, `S3_4_C15_C2_0/1`). This validates the boot_args handoff and the 16 KB
page-table bootstrap we reverse-engineered: given a correct `boot_args`, the watchOS kernel's early boot runs on a non-Apple core
once those 16 sites are neutralized.

Result (stage 2, `--continue`): keeping Unicorn flat (the harness NOPs the `msr sctlr_el1` MMU-enable, so with slide 0 the kernel's
virtual addresses equal the mapped memory), pre-patching the PAC instructions in the executable segments to NOP/stripped branches
(55,280 sites -- the port's PAC patch, applied in-memory), loading the real Apple DeviceTree at `deviceTreeP`, and stubbing the
remaining ~24 Apple IMPDEF registers, the kernel runs **~6 million instructions** of the C boot path: past the arm64e pointer-rebase
loop (the kernelcache re-signing its own pointers) and into successive early-init loops. Each new stop is the next thing to
provide. The technique (PAC-strip + Apple-register stub + flat identity memory + real DeviceTree) is exactly the native port's
patch set, validated to run deep into boot in emulation.

The remaining frontier is the platform: once the kernel reaches for AIC/PMGR/timer MMIO and the SEP/ANS coprocessors, those need
device models. That is the point where a QEMU full-system machine takes over. The full patch scope is in the
[census](../../research/opwwe231/2026-10-24-watchos-boot/pac-and-register-census.txt) (331,911 PAC sites, 82 Apple IMPDEF
registers); the platform spec is the [T8006 device map](./platform-device-map.md); the phased route to the UI is the
[boot-chain port plan](./boot-chain-port-plan.md).

**QEMU implements PAC and the 16 KB granule natively** (`-cpu max`), so on the QEMU path the 331,911 PAC sites do not need
patching at all -- only the Apple IMPDEF registers and the device models do. The PAC-strip tooling here is for the Unicorn
(no-PAC) harness. This is why the full-system QEMU route, not a 331k-site static patch, is the vehicle toward userland and the UI.

## Tools and next steps

- [`tools/watchos/bootargs/`](../../tools/watchos/bootargs/) — `boot_args.h` (the struct, offsets anchored to
  the kernel's own loads) and `mk_bootargs.c` (emits the blob a loader puts in `x0`). Verified: the emitted offsets match the
  reset vector's loads exactly.
- [`tools/watchos/probe/cpuprobe.c`](../../tools/watchos/probe/cpuprobe.c) — reads the core ID registers.
- [`tools/watchos/emu/boot_harness.py`](../../tools/watchos/emu/boot_harness.py) — runs the reset vector in Unicorn with a synthesized `boot_args`; validates the early boot to MMU-enable.
- Next REA targets: `arm_init` and `PE_init_platform` (full `boot_args` tail and the DeviceTree parse), then the pmap bootstrap
  (to compare the Apple physical memory map against our DRAM base `0x40000000`), and the AIC/PMGR drivers.

Evidence IDs (REA, on the extracted kernelcache): `ev_6d3c47aa…` (identity/entry), `ev_e5cf5c62…` (reset vector, boot_args
loads, page tables), `ev_35dab065…` (PAC keys, Apple sysregs, SCTLR), `ev_d1fa7004…` (TCR constant).
