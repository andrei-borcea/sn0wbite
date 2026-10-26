# Apple Watch / watchOS tooling

Track 2 code — everything related to the Apple Watch Series 10
(Watch7,11, T8310) and watchOS 27, split between a running virtual
reference platform (QEMU-SPTM) and an in-progress native-target QEMU
adaptation. Not OnePlus-specific — see [`../opwwe231/`](../opwwe231/README.md)
for the OnePlus Watch 2 tooling.

## Virtual reference platform (QEMU-SPTM, running today)

Shipping path to boot `Watch7,11` under watchOS 27 in a QEMU-SPTM fork,
reach userspace, and run user-authored code as `pid 3` under launchd.
Reproduces end-to-end from a public IPSW + a built QEMU-SPTM.

| Path | Purpose |
| --- | --- |
| [qemu-sptm/boot.sh](./qemu-sptm/boot.sh) | Wrapper that launches the `darwin-vm` QEMU-SPTM VM from inside the repo. Points at `.scratchpad-local/` for the firmware blobs. |
| [qemu-sptm/patchkit](./qemu-sptm/patchkit/) | YAML-driven kernelcache patcher. 38 numbered experiment manifests + the `patchkit.py` applier. |
| [qemu-sptm/gdbstub](./qemu-sptm/gdbstub/) | LLDB callback helpers used with `-gdb tcp::1234` plus the one-shot BRK-in-bootkc debugger technique. |
| [qemu-sptm/scripts](./qemu-sptm/scripts/) | Per-experiment Python assemblers (`exp15_asm.py` … `exp29_asm.py`), plus `disasm_range.py` and `parse_dtree.py`. |
| [qemu-sptm/ida_work](./qemu-sptm/ida_work/) | Hex-Rays snapshots of the kernel functions targeted by the patch chain. |
| [qemu-sptm/userspace/sn0shell](./qemu-sptm/userspace/sn0shell/) | The custom user binary: source, build + adhoc-sign script, trust-cache rebuilder, deploy script. 11-command REPL running under launchd on the VM. |
| [qemu-sptm/dt_fixup-watch7_11.patch](./qemu-sptm/dt_fixup-watch7_11.patch) | DeviceTree fixup applied to the restore payload before boot. |

Full narrative is in
[`docs/watchos/full-boot-walkthrough.md`](../../docs/watchos/full-boot-walkthrough.md)
and [`docs/watchos/userspace-shell-recipe.md`](../../docs/watchos/userspace-shell-recipe.md).

## Native-target QEMU adaptation (in progress)

Toward a Watch7,11 ↔ OPWWE231 port. The sn0w CPU layer RAZ/WIs the
Apple IMPDEF register space so a stock QEMU core can run the Apple
kernel; progress is tracked in
[`docs/watchos/boot-chain-port-plan.md`](../../docs/watchos/boot-chain-port-plan.md).

| Path | Purpose | Notes |
| --- | --- | --- |
| [qemu/](./qemu/) | `t8006-adaptation.patch` + the modular **sn0w CPU shim**. Reaches early SMP / idle; blocked on AIC-routed interrupt delivery. | [port plan](../../docs/watchos/boot-chain-port-plan.md) |
| [bootargs/](./bootargs/) | `boot_args.h` + `mk_bootargs` — the watchOS kernel entry struct a loader puts in x0. | [boot chain reconstruction](../../docs/watchos/boot-chain-reconstruction.md) |
| [emu/boot_harness.py](./emu/boot_harness.py) | Unicorn harness: runs the kernelcache reset vector with a synthesized `boot_args`. Validates early boot through MMU-enable. | [boot chain reconstruction](../../docs/watchos/boot-chain-reconstruction.md#testing-the-boot-in-emulation) |
| [port/pac_census.py](./port/pac_census.py) | Capstone census of the 331,911 PAC sites and 82 Apple IMPDEF register sites. | [port plan](../../docs/watchos/boot-chain-port-plan.md) |
| [port/kernel_patch.py](./port/kernel_patch.py) | Bulk PAC-form rewriter (Unicorn path only; QEMU uses the sn0w shim). | — |
| [port/symbolicate.py](./port/symbolicate.py) | Symbol resolver on an unsymbolicated kernelcache. | [qemu build & boot](../../docs/watchos/qemu-build-and-boot.md) |
| [port/rea-r2.sh](./port/rea-r2.sh) | REA + rizin wrapper. | [qemu build & boot](../../docs/watchos/qemu-build-and-boot.md) |
| [probe/cpuprobe.c](./probe/cpuprobe.c) | Reads the physical watch's CPU ID registers from EL0 (static aarch64). | [bring-up measurements](../../docs/watchos/bring-up-status.md) |

## Also under `.scratchpad-local/` (gitignored)

- the extracted Watch7,11 kernelcache (`bootkc_watch7_11` + IDA db),
- all 38 experiment-output kernelcaches (`bootkc.exp10` … `bootkc.exp38`),
- the restore `ramdisk.dmg`, `ramdisk.tc`, `sptm`, `txm`, and
- the built QEMU-SPTM binary.

Backups of the pristine firmware are kept side-by-side so each experiment
is reproducible from scratch.
