# patchkit-opw2 — watchOS kernelcache patches for the OnePlus Watch 2 port

Reproducible, versioned patch chain that adapts the Apple watchOS kernelcache
to run on **OPWWE231** (Qualcomm W5 Gen 1 "monaco", 4× Kryo 2XX Silver cores
= Cortex-A53-based, ARMv8.0-A). Each manifest in this directory is one
root-cause fix; manifests compose, do not drift. The chain is applied in
numerical order to a clean kernelcache using the project's existing runner
[`tools/watchos/qemu-sptm/patchkit/patchkit.py`](../../qemu-sptm/patchkit/patchkit.py).

Target this directory assumes:

- **watchOS 27.0.1 (24R365)**, `kernelcache.release.watch7b`
- Watch7,11 (n218bap, T8310 / S10 SiP) as the Apple-side reference
- Mach-O `__TEXT` at `vmaddr 0xFFFFFFF007004000` / `fileoff 0x0`

For a different kernelcache: run `kc-segmap.py <kernelcache>` to print the
right `va_base` + `file_text_off`, or `kc-segmap.py <kc> --manifest <m> --rewrite`
to splice them in automatically.

## Why these patches

The port-gap report at
[`profiles/opwwe231/port-gap.watchos-6.3.1.yaml`](../../../../profiles/opwwe231/port-gap.watchos-6.3.1.yaml)
(regeneratable for watchOS 27 once we add
`tools/mapping/expectations/watchos-27-watch7_11.yaml`) classifies the deltas
between the Apple expectations and the live OPW2 profile. The ones that need
in-kernelcache byte edits — not an EL2 shim — are:

| gap | manifest | status |
| --- | --- | --- |
| TCR_EL1 TG0/TG1 = 16KB granule (A53 has no TGran16, ARMv8.2+) | **v1 TCR constant 16KB→4KB** | ✅ written |
| Page-table bootstrap uses 16KB stride `0x4000` + 32MB L2 block `0x2000000` | **v2 stride+block constants** | 🔜 planned |
| PAC MSRs (`apia/apib/apda/apdb/apga key lo/hi`) + Apple IMPDEF `APCTL` enables | **v3 PAC key MSR → nop** | 🔜 planned |
| 331k PAC-flavored instructions (`pacia/braa/retaa/…`) across code segments | **v4 PAC bulk strip** (reuses `tools/watchos/port/kernel_patch.py`) | 🔜 planned |
| 10k LSE atomics (`ldadd/stadd/cas…`) across code segments | **v5 LSE bulk strip to ldxr/stxr pairs** | 🔜 planned |
| 82 distinct Apple IMPDEF system registers | **(moved to EL2 shim, not a kernelcache patch)** | — |

Everything in the "EL2 shim" row belongs to `tools/watchos/qemu/sn0w/` (the
existing sn0w compat layer, which already does RAZ/WI for the Apple IMPDEF
space in QEMU). For the HW path we re-mount sn0w into a bare-metal EL2
micro-hypervisor stage; that work is tracked separately.

## Status (2026-10-27)

- **v1 TCR constant 16KB→4KB** — written and byte-verified against
  `.scratchpad-local/session-previous/kernelcache_padded.macho` (SHA256 of
  clean input `07f9d0e0b69c0a8d…`, patched output `86da6d082b67daa3…`,
  exactly 2 byte diffs at file offsets `0x5aeba9` + `0x5aebab`).
  Runtime verification (reaching `msr sctlr_el1` in Unicorn) is still open:
  [`boot_harness.py`](../../emu/boot_harness.py) targets the watchOS 6.3.1
  reset vector; porting its START/MMU_ON for watchOS 27 is the follow-up.
- v2..v5 are design-complete (see table above) but not yet written. Each
  needs its own manifest and own verification. Order matters: v2 before any
  attempt to run the patched kernel through `pmap_bootstrap`.

## Usage

```bash
# resolve segment offsets for your kernelcache (idempotent if they already match)
.venv/bin/python kc-segmap.py <path/to/kernelcache.macho> \
    --manifest manifest-v1.yaml --rewrite

# apply v1 (and whichever vN manifests are ready)
.venv/bin/python ../../qemu-sptm/patchkit/patchkit.py apply \
    <path/to/kernelcache.macho> \
    -m manifest-v1.yaml \
    -o <out.macho>

# verify-boot wrapper: apply + run Unicorn harness in one shot
./verify-boot.sh manifest-v1.yaml <path/to/kernelcache.macho>
```

Byte-level verification against a reference kernelcache (same manifest, same
input → same output) is `patchkit.py verify`:
```bash
.venv/bin/python ../../qemu-sptm/patchkit/patchkit.py verify \
    <output.macho> -m manifest-v1.yaml -r <input.macho>
```

## How this interlocks with the deployment chain

The patched kernelcache alone does not boot on hardware. The deployment side
is:

1. **sn0wbite recovery payload** (`/sbin/boot-watchos`) — launched via the
   DUAL BOOT menu in `tools/opwwe231/recovery-menu/`, inherits that stage's
   killswitches (BCB fail-open, deadman, idle, battery floor, flight
   recorder). Used for diagnostic bring-up where we can tolerate dropping
   back to Wear OS.
2. **Slot B custom boot.img** — final path. Our stage1 bootloader replaces
   the Qualcomm kernel in `boot_b` and chain-loads the patched kernelcache
   with a synthesized `boot_args` (see `tools/watchos/bootargs/`) and the
   real Watch5,4/7,11 DeviceTree. Stage1 brings its own killswitches per
   [[watchos-boot-layer-killswitch-mandate]].

Both deployment surfaces consume the output of this directory as a
block-level artifact; nothing in the stage1 or the recovery payload embeds
Apple firmware.
