# CLAUDE.md — agent guidance for this repository

Guidance for Claude Code (and compatible coding agents) when working on
**sn0wbite**. The top-level [README.md](./README.md) is the human-facing
intro; this file is operational.

## The project in one paragraph

Two tracks. **Track 1** is OnePlus Watch 2 (OPWWE231) modding: custom
recovery, boot menu, power menu, boot screen, Magisk root. **Track 2** is
a QEMU-SPTM bringup of a virtual Apple Watch Series 10 (Watch7,11) running
watchOS 27, used as the reference platform for a native port. On
2026-10-26 Track 2 reached userspace exec on the VM; a native watchOS boot
on the physical OnePlus hardware remains open.

## Where things live

```
docs/opwwe231/           Track 1 narrative docs
docs/watchos/            Track 2 narrative docs (incl. experiment history)
tools/opwwe231/          Track 1 code (OnePlus-specific: magisk, powermenu, recovery-menu)
tools/watchos/           Track 2 code (Apple-side: qemu-sptm/, qemu/, port/, probe/, emu/, bootargs/)
tools/ota/               cross-track: reconstructed OPPO OTA client
research/opwwe231/       dated evidence collections
releases/opwwe231/       flashable images + SHA256SUMS
.scratchpad-local/       large binaries (gitignored): firmware, kernelcache experiments, DMGs
backups/                 private device acquisitions (gitignored)
```

Do not mix the two tracks in the same path. If a change touches Apple
watchOS internals it goes under `tools/watchos/` and `docs/watchos/`.
OnePlus-specific work goes under `tools/opwwe231/` and `docs/opwwe231/`.

## Conventions

- **Attribution.** Commits end with
  `Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>`. PRs end with
  the Claude Code sign-off line. If the user's own instructions (memory,
  CLAUDE.md itself) say otherwise, those win.
- **Confirm before flashing.** Analysis tools install freely; anything
  that writes to a real device (`fastboot flash`, `dd` to a partition, OTA
  trigger) stops and asks the user first.
- **Reproducible patches, not whack-a-mole.** On Track 2, patches must fix
  the root cause with a numbered, versioned manifest (see
  `tools/watchos/qemu-sptm/patchkit/`). If 2–3 iterations of the same
  patch keep shifting the panic without closing it, step back and plan a
  different approach rather than add a fourth variant.
- **Use REA for RE.** Reverse-engineering tasks go through the REA
  CLI/skill with Evidence IDs, not ad-hoc rizin/python one-liners. The
  Ghidra kernelcache recipe (pad past EOF, `GHIDRA_HEADLESS_MAXMEM=16G`,
  snapshot after analysis) is already captured in the user's memory.
- **Consult XNU source.** For kernel struct layouts and mechanisms,
  consult `apple-oss-distributions/xnu` tag `xnu-6153.141.1` before
  decompiling from scratch. Closed kexts (ApplePMGR, AMFI runtime bits)
  need Hex-Rays; snapshots live in `tools/watchos/qemu-sptm/ida_work/`.

## Non-obvious invariants

1. **`.scratchpad-local/` is gitignored, large, and essential at runtime.**
   `tools/watchos/qemu-sptm/boot.sh` resolves the firmware dir to
   `.scratchpad-local/session-previous/sptm/darwin-vm/firmware`. If that
   path moves, update the script together.
2. **The exp16 chain is not required for userspace.** The exp10..exp38
   kernel-patch experiments were investigations. The current path to a
   working userspace shell uses the plain baseline `bootkc.before-exp10`
   (SHA256 `a75d58d6…`) with the user binary's cdhash inserted into the
   ramdisk trust cache. Don't re-apply the whole chain by reflex.
3. **Trust cache format.** 4-byte version `01 00 00 00` + 16-byte UUID +
   4-byte num_entries + N × 22 bytes (20 B SHA-256-truncated cdhash + 2 B
   flags, flags = 0x0002 for all stock entries, sorted ascending by
   cdhash). Rebuilder:
   `tools/watchos/qemu-sptm/userspace/sn0shell/build-tc.py`.
4. **watchOS SDK marks mach host APIs unavailable.** Use `__asm` aliasing
   to call `_host_info` / `_host_statistics64` / `_host_processor_info` by
   their real symbol names; libSystem exports them. See `sn0shell.c` for
   the pattern.
5. **arm64e needs no special signing path.** `codesign -s - --force` on an
   arm64e Mach-O produces an adhoc signature that the kernel accepts once
   the cdhash is in the trust cache — same as arm64.

## When editing, prefer…

- Editing an existing file to creating a new one (project rule).
- Shell + Python inside this tree rather than new dependencies. The repo
  has no package manager; what works is `bash`, `python3`, Xcode's
  toolchain, `codesign`, `hdiutil`, `plutil`, and the QEMU-SPTM build.
- Keeping file-level patches in the patchkit YAML format rather than
  one-off scripts — they are already versioned and the rebuilder reads
  from them.

## When writing docs

- Narrative docs go in `docs/<track>/`. Evidence (raw logs, JSON dumps,
  CSVs) goes in `research/opwwe231/<date>-<topic>/`.
- Use the status legend from the top-level README (✅ 🟡 🧪 🔜) consistently.
- Call out what's a verified device observation vs. a hypothesis.
- Logs under `docs/watchos/logs/` should be trimmed to the first ~200
  lines — the full-run logs stay in `.scratchpad-local/`.

## Memory files (user's auto-memory)

Several non-obvious invariants live in the user's auto-memory at
`~/.claude-smartness/projects/-Users-andreiborcea-Work-Personal-sn0wbite/memory/`.
Key ones to be aware of when planning:
- `feedback-use-rea-for-reverse-engineering.md`
- `feedback-build-patch-chain-reproducibly.md`
- `feedback-consolidated-not-whack-a-mole.md`
- `watchos-27-qemu-boot-reached.md`
- `watchos-27-userspace-shell-reached.md`

Reply to the user in **Italian** by default (they're Italian). Technical
identifiers (`sn0shell`, `cdhash`, `patchkit`, …) stay in English.
