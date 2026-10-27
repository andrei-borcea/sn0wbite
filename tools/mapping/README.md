# sn0wbite mapping engine

Device-agnostic hardware profiler for the sn0wbite port. Given a backup of a
wearable's stock partitions, it emits a declarative *device profile*
(YAML) describing CPU, memory, UART, interrupt controller, timers and
partitions as derived from stock firmware. A separate *expectations* file (one
per target OS build) describes what the port needs; a declarative differ
produces a *port-gap* report with `MATCH` / `SHIM` / `BLOCKER` verdicts and
remediation hints.

Agnostic to device: another watch is "drop a backup, run the two commands,
read the port-gap." No code change in the engine for a new target.

## Pipeline

```
┌──────────────┐   acquire/*.sh    ┌──────────────┐   profile.py    ┌───────────────────────┐
│ real device  │ ──────────────▶   │ backups/<d>/ │ ───────────▶    │ profiles/<d>/device.yaml │
└──────────────┘   adb exec-out    │  *.img       │                 └───────────────┬───────┘
                                   └──────────────┘                                 │
                                                                                    │ diff.py
                                                                                    ▼
                                   ┌───────────────────┐             ┌──────────────────────────────┐
                                   │ expectations/*.yaml│ ──────────▶ │ profiles/<d>/port-gap.*.yaml │
                                   │ (per target OS)    │             │ + .md                        │
                                   └───────────────────┘             └──────────────────────────────┘
```

## Layout

```
tools/mapping/
  schema/device-profile.schema.yaml       JSON Schema for a device profile
  extractors/
    bootimg.py                            Android boot_img v0..v4 + vendor_boot v3/v4
    dtb.py                                Pure-python FDT parser (no libfdt)
  expectations/
    watchos-6.3.1-watch5.yaml             Apple expectations for the Watch5,4 target
  acquire/
    opwwe231.sh                           Pulls OPW2 partitions over adb (read-only)
  testdata/
    synth.py                              Synthetic boot_a.img for the smoke test
    smoke.sh                              End-to-end offline pipeline test
  profile.py                              Composer (backup dir → device.yaml)
  diff.py                                 Differ (device.yaml + expectations → port-gap.yaml)
```

Everything the engine writes about a device lives under:
- `backups/<device>/<date>/` — **gitignored**, raw partition dumps + `SHA256SUMS`
- `profiles/<device>/` — **committed**, YAML facts and port-gap reports

## Setup (once)

PyYAML is the only non-stdlib dependency. On a PEP-668 host (Homebrew Python)
pip refuses to touch the system site-packages, so bootstrap a repo-local venv:

```bash
python3 -m venv .venv
.venv/bin/pip install pyyaml
```

The scripts prefer `./.venv/bin/python` when it exists and fall back to
`python3` otherwise.

## Usage — OnePlus Watch 2 (OPWWE231)

1. Reboot the watch into the **sn0wbite recovery** (`adb reboot recovery`
   from Wear OS, or hold the side key per `tools/opwwe231/recovery-menu/`).
2. From the host:

   ```bash
   tools/mapping/acquire/opwwe231.sh                 # read-only, streams via adb exec-out
   .venv/bin/python tools/mapping/profile.py \
       backups/opwwe231/$(date -u +%F) \
       --device opwwe231 \
       -o profiles/opwwe231/device.yaml

   .venv/bin/python tools/mapping/diff.py \
       profiles/opwwe231/device.yaml \
       tools/mapping/expectations/watchos-6.3.1-watch5.yaml \
       -o profiles/opwwe231/port-gap.watchos-6.3.1.yaml --markdown
   ```

The acquire script **never writes to any device partition**: it only runs
`dd if=/dev/block/by-name/<part>` through `adb exec-out`. Modem, data,
persist, and userdata are deliberately excluded.

## Adding another device

1. Dump its partitions (any method; one `.img` per partition under
   `backups/<new-device>/<date>/`).
2. Run `profile.py` with `--device <slug>`.
3. Run `diff.py` against the same expectations file.

If a new partition type (`<something>_a.img`) needs parsing, add an extractor
module under `extractors/` emitting a *profile fragment* and route it from
`profile.py`. The schema stays the same.

## Adding another target OS

Write a new `expectations/<os-ver>-<device>.yaml`. Rules use these compare
operators (see `diff.py`):

| op       | matches when                                                     |
| :------- | :--------------------------------------------------------------- |
| `equal`  | profile value is exactly `value`                                 |
| `present`| the field exists (non-null)                                      |
| `feature`| `cpu.isa_features.<key>` is `true` (BLOCKER if false, SHIM if unknown) |
| `note`   | informational only — always `MATCH`                              |

Each rule can override the default verdict with `verdict_on_mismatch:` or
`verdict_on_absent:` set to `MATCH`, `SHIM`, or `BLOCKER`.

## Smoke test

Builds a synthetic Android v2 boot image with an embedded FDT, runs the full
pipeline against it, asserts that: the DRAM/ramoops/UART/GIC facts round-trip
through the composer and that the differ flags the AIC-vs-GIC mismatch against
Apple expectations.

```bash
bash tools/mapping/testdata/smoke.sh
```

Expected final line: `smoke test passed.`

## What this is not

- Not a firmware extractor: no decryption, no AVB bypass, no proprietary
  decompression. Encrypted partitions (`aboot`, `sep-firmware`) are hashed
  and recorded as `type: encrypted`; the engine does not try to look inside.
- Not a reverse-engineering tool: all facts come from parsing published
  formats (bootimg, FDT) and from the hand-curated expectations file. REA /
  Ghidra work lives elsewhere.
- Not a flasher: the acquire scripts and the engine never write to a
  device partition.
