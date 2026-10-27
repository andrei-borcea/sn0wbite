# sn0w-el2-opw2 — bare-metal EL2 micro-hypervisor for the OPW2 watchOS port

A small AArch64 EL2 trap handler that stays below the ported watchOS kernelcache
and emulates the ARMv8.1/8.3 features the Qualcomm W5 Gen 1 cores (Cortex-A53-
derived, ARMv8.0) do not implement. Reuses the architectural model of the
existing QEMU-side shim at [`tools/watchos/qemu/sn0w/`](../qemu/sn0w/) but runs
as real EL2 code on real silicon instead of as QEMU `define_arm_cp_regs`
callbacks.

Companion to the [`patchkit-opw2` v1-v5 chain](../port/patchkit-opw2/):

| chain step | lives in | what it does |
| --- | --- | --- |
| v1 TCR constant | `patchkit-opw2/manifest-v1.yaml` | kernel self-sets 4KB granule |
| v2 bootstrap constants | `patchkit-opw2/manifest-v2.yaml` | kernel builds 4KB-granule tables |
| v3 PAC-key MSRs + reset-vector Apple IMPDEF + APRR_EL1 vector handlers → NOP | `patchkit-opw2/manifest-v3.yaml` | reset vector no longer traps |
| v4 PAC bulk strip | `tools/watchos/port/kernel_patch.py --apply` | 49 k `pacia/braa/retaa/…` → identity |
| **v5 LSE atomics** | **sn0w-el2-opw2 (this directory)** | EL2 traps UNDEF, emulates `ldadd/stadd/cas/swp/…` with `ldxr/stxr` |
| **+ 72 Apple IMPDEF sysregs** | **sn0w-el2-opw2** | EL2 traps `mrs/msr s3_4_c15_*`, returns RAZ/WI or (for APRR) real backing |

## Why EL2

ARMv8-A gives us three mode levels we can realistically use:
- **EL1** — where XNU runs. XNU reads/writes its own system state here. Can't trap its own illegal ops.
- **EL2** — hypervisor level. Can intercept EL1's UNDEF trap (via `HCR_EL2.TGE/HCR_EL2.TID3/HCR_EL2.AMO` etc.), run a handler, resume EL1 after fixing the state.
- **EL3** — secure monitor. Qualcomm firmware owns it; we don't touch it.

The Qualcomm bootloader on this platform already runs code at EL2 (confirmed by
`kvm-arm.mode=protected` in the live kernel cmdline — pKVM requires an EL2
hypervisor). We take over the EL2 slot from the moment stage1 hands off: install
our trap vectors, drop XNU to EL1, let it run. Any instruction A53 can't
decode generates an UNDEF; without us the kernel panics on the first `pacia`.
With us, UNDEF routes to our vectors at EL2, we decode the faulting insn, do
the right thing, and `eret` back.

## Scope

What the shim DOES, in priority order:
1. **LSE atomic emulation** (v5 contract). ~10 k sites across the kernelcache.
   Decode `ldadd/ldclr/ldset/ldeor/stadd/stclr/stset/steor + CAS + CASP + SWP`
   including the acq/rel size and byte/half variants; perform the equivalent
   `ldxr/stxr` loop in EL2 against the EL1 physical address.
2. **Apple IMPDEF sysreg traps** on the `S3_4_C15_*` space (the 72 regs
   catalogued in the port-gap). RAZ/WI by default; selected regs
   (APRR_EL0 = S3_4_C15_C2_0, APRR_EL1 = S3_4_C15_C2_1) get real R/W backing
   per CPU — the kernel's PPL call gate and pmap re-read them. Logic mirrors
   `tools/watchos/qemu/sn0w/sn0w_cpu.c:sn0w_cpu_install_apple_aprr`.
3. **PAC-flavored instructions outside `kernel_patch.py`'s `CODE_SEGS`**
   (the 6 191 the harness still sees after v4). Trap-emulate as NOPs/strips.

What the shim DOES NOT do:
- Virtualize the memory system (no stage-2 translation; EL1 owns its tables).
- Virtualize interrupts (GIC-v3 traffic goes straight to EL1 drivers; our
  AIC shadow lives in XNU's platform-expert layer, not here).
- Virtualize timers (CNTP goes to EL1).

Think of it as "a very narrow EL2 whose only job is instruction emulation",
not "a KVM-style hypervisor".

## Interlock with the deployment chain

```
┌──────────────────────────┐
│ Qualcomm XBL/ABL (EL3+)  │  bootloader, we have no control
└──────────┬───────────────┘
           │ loads slot B (boot.img we built)
┌──────────▼───────────────┐
│ stage1 bootloader (EL2)  │  our arm64 bare-metal payload:
│ (sn0w-el2-opw2/stage1/)  │   - draws Apple logo on framebuffer
│                          │   - reads boot_args/DT from embedded blobs
│                          │   - installs sn0w EL2 trap vectors (THIS DIR)
│                          │   - sets up stage-1 translation at EL2
│                          │   - eret-s to EL1 with x0=boot_args, entry=0xFFFFFFF007DF8538
└──────────┬───────────────┘
┌──────────▼───────────────┐
│ XNU kernelcache (EL1)    │  watchOS 27, port-patched (v1-v4 applied)
│                          │  every UNDEF → trap to EL2 → our handler → eret
└──────────────────────────┘
```

Stage1 and sn0w-el2 are the two sides of the same binary (stage1 contains the
EL2 trap vectors in its payload). They are packaged together, built together.

## Status (2026-10-27)

- Directory scaffolded.
- Architecture described in this README.
- `src/` is empty — implementation pending.
- Design mirrors `tools/watchos/qemu/sn0w/` for the Apple IMPDEF sysreg
  trap table; the LSE emulator is new material (QEMU's `-cpu max` provides
  LSE natively, so there is nothing to crib from the QEMU variant).

## Next steps

1. `src/trap_vectors.S` — the 16-entry EL2 vector table (`vbar_el2`), with
   the Sync EL1→EL2 slot routed to a C dispatcher.
2. `src/dispatch.c` — decode `ESR_EL2.EC` (0x18=MSR/MRS/SYS, 0x25=Data abort
   from lower EL, 0x00=UNKNOWN/UNDEF). For UNKNOWN, read the faulting insn
   at `ELR_EL2`, run the LSE / PAC / Apple-sysreg classifier, dispatch.
3. `src/lse.c` — `ldadd/stadd/cas/swp` emulators keyed on the opcode
   subfields (size, acq/rel, source Rs, address Rn, output Rt). Each uses
   `ldxr/stxr` on the live EL1 address.
4. `src/sysreg_apple.c` — port of `tools/watchos/qemu/sn0w/sn0w_cpu.c`'s
   decode table to bare-metal (no `ARMCPRegInfo`; just a switch on
   `op0:op1:CRn:CRm:op2` → RAZ/WI or backed field).
5. Link into stage1's address space at a known physaddr; stage1 writes
   `vbar_el2` to this table before `eret`.
