# sn0w compat layer

A thin, reusable shim for running an OS kernel built for one CPU on a QEMU core
that is *not* that CPU. It lives beside the `qemu-t8030` fork and is mounted into
it by [`../t8006-adaptation.patch`](../t8006-adaptation.patch); the sources here
are the canonical, human-edited copies.

The goal is that adding support for a different SoC/CPU is a small, obvious edit
in one place — "mount your processor here" — rather than scattered hacks across
the machine and CPU models.

## Why this exists

A kernel like watchOS / XNU hits the CPU with dozens of **implementation-defined
(IMPDEF) system registers** the vendor invented (Apple's HID/EHID/ACC/CTRR/PMC/…
all live at `op0=3, CRn=15`). A stock QEMU core has never heard of them, so the
first `msr`/`mrs` to one raises an *Undefined Instruction* trap and the kernel
dies in its early exception vector. The census for our target found **82
distinct** such registers (see
`research/opwwe231/2026-10-24-watchos-boot/pac-and-register-census.txt`).

Wiring 82 registers by hand — and rediscovering them one boot-crash at a time —
is the wrong unit of work. This layer absorbs a whole IMPDEF *space* at once.

## What it does

`sn0w_cpu_neutralize_impdef(cpu, spaces, n)` registers, for each declared
`(op0, CRn)` space, a wildcard **RAZ/WI** definition (`ARM_CP_CONST` — reads
return 0, writes are ignored) covering every `(opc1, CRm, opc2)` encoding inside
it. The entry carries `ARM_CP_OVERRIDE`, so any register the CPU models **for
real** — defined the normal way with `define_arm_cp_regs()` *after* this call —
transparently replaces the blanket entry for its own encoding.

So the contract is just an ordering rule:

```c
sn0w_cpu_neutralize_apple_impdef(cpu);              /* 1. blanket the space  */
define_arm_cp_regs(cpu, my_real_impdef_regs);       /* 2. real ones override */
```

(One subtlety handled inside: QEMU derives a register's architectural
permission mask from `opc1` *before* it expands wildcards, so `opc1` cannot be
`CP_ANY`; the helper enumerates `opc1` 0..7 and wildcards only `CRm`/`opc2`.)

## Mount your own CPU

1. Model the registers you care about in a normal `ARMCPRegInfo[]` table, exactly
   as `apple_a13.c` already does (give the ones with real side effects real
   read/write functions or backing storage).
2. Declare the IMPDEF `(op0, CRn)` space(s) your target's kernel pokes. For any
   Apple core that is simply `{ op0 = 3, CRn = 15 }` — use the
   `sn0w_cpu_neutralize_apple_impdef()` preset. For a non-Apple vendor, pass your
   own `Sn0wImpdefSpace[]` to `sn0w_cpu_neutralize_impdef()`.
3. Call the neutralizer at the top of your CPU's cpreg setup, before your real
   table (see the one-line hook in `apple_a13_add_cpregs()`).

That is the whole extension surface for the CPU side.

## Scope and honesty

- **RAZ/WI is a bring-up tool, not emulation.** A register the kernel *writes
  then reads back* will read 0, not the written value, and a status register the
  kernel reads expecting a hardware-defined value will read 0. If a boot diverges
  because of that, promote that specific encoding to a real definition in your
  CPU table (step 1) — the override makes it win. Most IMPDEF traffic at boot is
  one-way config writes, for which RAZ/WI is correct and enough.
  - The S5 port's first such case is the **APRR** permission-remap pair
    `APRR_EL0`/`APRR_EL1` (`S3_4_C15_C2_0`/`_1`): XNU's pmap re-reads them on
    *every* protection update and panics ("modifying a PPL mapping") unless they
    still hold the arch defaults it wrote at bring-up. Under blanket RAZ/WI they
    read 0 and the check trips on the first mapping. `sn0w_cpu_install_apple_aprr()`
    promotes exactly those two encodings to constants returning
    `0x4545010167670101` / `0x4455445566666677`, so the check passes. (The A13
    core only models the A14-style **SPRR** encodings at `opc1=6`; APRR is the
    older `opc1=4` pair the Tempest kernel uses.)
- This is the **QEMU full-system path**, which runs FEAT_PAuth and the 16 KB
  granule natively, so the kernel's 331,911 PAC sites need **no** patching here.
  The separate Unicorn harness (no PAC) is where the bulk PAC-strip applies.
- Current files cover the CPU register space. The machine-side tweaks (skip the
  coprocessors we do not model yet, make nvram optional) still live as edits in
  `t8030.c` in the adaptation patch; folding those into a `Sn0wSocProfile` is the
  natural next module.

## Files

| File | Role |
| --- | --- |
| `sn0w_cpu.h` | public interface: `Sn0wImpdefSpace`, the neutralizer calls, `sn0w_cpu_install_apple_aprr()` |
| `sn0w_cpu.c` | the RAZ/WI wildcard registration + the Apple APRR promotion |

Mounted into the fork at `hw/arm/sn0w_cpu.c` and `include/hw/arm/sn0w_cpu.h`
(+ one line in `hw/arm/meson.build`, one `#include` and two calls —
`sn0w_cpu_neutralize_apple_impdef()` then `sn0w_cpu_install_apple_aprr()` — in
`hw/arm/apple_a13.c`) — all carried by the adaptation patch.
