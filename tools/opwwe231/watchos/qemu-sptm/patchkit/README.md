# patchkit

Reproducible binary patcher for watchOS `bootkc` kernelcaches used in the
`qemu-sptm` watchOS bring-up work.

The current manual patch chain for Watch7,11 (T8310) running watchOS 27.0.1
build 24R365 lives in `manifest-watch7_11-27.0.1.yaml`. Applying that manifest
to a clean `bootkc` extracted from the stock IPSW produces the same bootkc that
reaches APFS mount + launchd exec in qemu-sptm.

## Why

Hand-patching the bootkc with `dd`/hexedit does not scale past a handful of
sites. Future work (bulk EINVAL→success on
`shared_region_map_and_slide_setup`, AMFI prolog stub, dyld shared-cache
bypasses) will add many more patches that must stay auditable and
regenerable across kernelcache rebuilds.

## Install

```
python3 -m pip install pyyaml
```

## Use

Apply a manifest to a clean kernelcache:

```
./patchkit.py apply /path/to/bootkc.orig \
    -m manifest-watch7_11-27.0.1.yaml \
    -o /path/to/bootkc.patched
```

Verify an already-patched kernelcache matches what the manifest produces when
applied to a clean reference (reproducibility check):

```
./patchkit.py verify /path/to/bootkc \
    -m manifest-watch7_11-27.0.1.yaml \
    -r /path/to/bootkc.orig
```

Dump bytes at a VA for inspection:

```
./patchkit.py dump /path/to/bootkc --va 0xFFFFFFF00893DA30 --length 32 \
    -m manifest-watch7_11-27.0.1.yaml
```

## Manifest format

```yaml
va_base:       0xFFFFFFF007E2C000   # __text segment VA base
file_text_off: 0xE28000             # file offset of __text start

patches:
  - name: apfs-container-sanity-nop
    effect: human-readable description of what this patch does and why
    va:    0xFFFFFFF0098A46C0       # virtual address to patch
    orig:  "880d0054"               # expected bytes at `va` (verified before apply)
    new:   "1f2003d5"               # replacement bytes (same length as orig)
    disabled: false                  # optional, default false

scanners:
  - name: bulk-einval-to-success
    function_va:   0xFFFFFFF009F79CA8
    function_size: 0x27A0
    find:    "52 ** 02 c0"          # ** = wildcard byte
    replace: "52 KEEP(1) 00 00"     # literal hex, or KEEP(n) to copy byte n of the match
    step:    4                       # stride (default 4 = ARM64 instruction size)
    disabled: false
```

Scanners rewrite every matching instance in the given function range in-place.
`find` and `replace` must be the same length. `KEEP(n)` in `replace` preserves
the n-th byte of the matched slice (useful for keeping register-field bits
unchanged while flipping an immediate).

## VA ↔ file offset

For a Watch7,11 bootkc extracted from the stock IPSW:

- `__text` starts at VA `0xFFFFFFF007E2C000`
- `__text` starts at file offset `0xE28000`
- `file_offset(va) = (va - 0xFFFFFFF007E2C000) + 0xE28000`

If you point the manifest at a different device/kernel, override `va_base` and
`file_text_off`.

## Workflow for adding a new patch

1. Reverse-engineer the target site (IDA + Hex-Rays recommended; the project's
   working database at `scratchpad/ida_work/` has the known decompiles).
2. Decide the replacement instructions. Encode them (either by hand or by
   assembling with llvm-mc / `rasm2`).
3. Add a new entry to the manifest with `name`, `effect`, `va`, `orig`, `new`.
4. Run `patchkit.py apply` on the clean bootkc.orig → produces the new patched
   bootkc.
5. Re-sign/pack and boot in qemu-sptm.
6. Observe the serial log; if the boot progresses further, commit the manifest.
   If it panics or stalls, mark the new patch `disabled: true` and iterate.

Keep the manifest additive. A new patch bundle (e.g. an experimental AMFI
stub) can live in its own file alongside the reference manifest and be applied
with `-m manifest-amfi-stub-experiment.yaml` on top of the already-patched
kernel; use `--no-verify` when stacking on an already-patched input.
