# watchOS 27 Watch7,11 — exp16: AMFI vnode_check_signature → trust-cache

**Date:** 2026-10-26
**Status:** ✅ **MAJOR PROGRESS**. The adhoc dyld_shared_cache_arm64e is
now accepted by the kernel; `/sbin/launchd` runs, dyld loads libSystem,
IOKit matching advances to serial-port driver instantiation. Next
blocker shifts from AMFI's shared-cache verdict to the Mach-O load-time
validator (OS_REASON_EXEC code 1 = BAD_MACHO) on a later exec in
launchd's startup.
**Base:** `bootkc.before-exp10` (9-patch APFS/md chain)
**Manifest:** `tools/watchos/qemu-sptm/patchkit/manifest-exp16-amfi-trustcache.yaml`
**Patched sha256:** `71af5c041c0dcbe0...` (`bootkc.exp16`)
**Composite sha256 (+ exp14):** `6a1624cb6bb7dda7...` — the LC bypass
alone does **not** move the needle past this new blocker (confirmed by
diff on the boot logs).

## The patch

Two writes, 40 bytes total, no scanner needed.

### 1. The hook (replacing PACIBSP at offset +4 of `_vnode_check_signature`)

```
VA 0xFFFFFFF00893DA34
  orig: 7f2303d5     (PACIBSP)
  new:  515b6214     (B 0xFFFFFFF00A1D4778)
```

`BTI c` at offset 0 is preserved — the function is reached by
`BLRAA` from the MAC-framework dispatcher, and ARMv8.5 BTI requires
the indirect-call target to be a `BTI c` landing pad. exp10 verified
this the hard way (bare `MOV X0,#0 ; RET` entry panics "Kernel BTI
failure").

### 2. The trampoline (36 bytes in the zero-filled `__TEXT_EXEC` pad)

```
VA 0xFFFFFFF00A1D4778  (file 0x31D0778, 14472 bytes of verified zero pad)
  stp  x29, x30, [sp,#-0x10]!       ; preserve caller's PAC-signed LR
  mov  x0,  x3                      ; a4 (output ptr) → arg 1
  adrp x2,  0xFFFFFFF0074B9000      ; ADRL "trust-cache"
  add  x2,  x2,  #0xF46             ;  (verified string at 0xFFFFFFF0074B9F46)
  movz w1,  #1                      ; trust_id = 1 (trust-cache)
  bl   0xFFFFFFF00893BFD0           ; sub_FFFFFFF00893BFD0 (set trust)
  ldp  x29, x30, [sp], #0x10
  movz x0,  #0                      ; return 0
  ret
```

The ADRP/ADD/MOVZ/BL sequence is a *copy of a known-good pattern
inside AMFI itself at VA `0xFFFFFFF00893DE1C`*, which is the branch
`_vnode_check_signature` takes when a signature **is** a genuine
trust cache. We replace the whole decision tree with that path.

## Boot-log evidence

The 4 AMFI "I started" lines are the only AMFI output. Compare with
all pre-exp16 experiments:

| | pre-exp16 (incl. exp10b/11/12/13/15) | exp16 |
|---|---|---|
| `AMFI: '…dyld_shared_cache_arm64e' is adhoc signed.` | ✓ (every run) | **absent** |
| `AMFI: '…dyld_shared_cache_arm64e': unsuitable CT policy 0 for this platform/device, rejecting signature.` | ✓ (sometimes bypassed site-by-site) | **absent** |
| `dyld[1]: result from check_np(): -1, errno 12` | ✓ | **absent** |
| `dyld[1]: Library not loaded: /usr/lib/libSystem.B.dylib` | ✓ | **absent** |
| `panic ... launchd[1] fatal signal 6` (dyld fail) | ✓ | **absent** |
| `Identified Serial Port qemuport0 … at …` (IOKit driver matching advanced) | ✗ | **✓ (NEW)** |
| `panic ... init[1] fatal signal 9 -- namespace 9 code 0x1` (`OS_REASON_EXEC` / `BAD_MACHO`) | ✗ | **✓ (new blocker)** |

So:
- Full-system dyld shared cache registration succeeded.
- `libSystem.B.dylib` loaded via the cache.
- `launchd` renamed itself to `init` and reached further init stages.
- IOKit matching ran to the point of binding the four `qemuport*` serial
  nodes, which happens AFTER launchd has brought up `kextd`-equivalent
  services inside the ramdisk → **userspace init is partially up**.

The new panic is an exec BAD_MACHO on something `init` tries to spawn
with `posix_spawn(POSIX_SPAWN_SETEXEC)` — the SETEXEC flag makes the
spawn replace the calling process, so a BAD_MACHO on the child kills
`init` with the exec reason, which panics the kernel.

## Why the generic "always trust-cache" works

The downstream shared-region creation path only requires the trust
category to be `"trust-cache"` (verified by inspecting
`sub_FFFFFFF009F79CA8`'s callers in IDA). Writing that unconditionally
to `*a4` satisfies the invariant for the dyld shared cache. For every
other code signature validation, the kernel ALSO gets "trust-cache"
back from AMFI — which is permissive but harmless on qemu-sptm (no
real security boundary) and empirically survived every validation up
to the BAD_MACHO site in exec.

**Risk if ever applied to real hardware**: effectively disables AMFI
code-signing gates for every path. Do not deploy this patch anywhere
other than the qemu-sptm bringup.

## Next blocker: BAD_MACHO exec validator

The panic is sourced from `exec_mach_imgact` or an equivalent on this
XNU revision, which rejects a Mach-O being exec'd by init with
`os_reason_create(OS_REASON_EXEC, EXEC_EXIT_REASON_BAD_MACHO)`.
Candidate REA sites for the next session:

- Follow `sub_FFFFFFF009EB5338` (the kernel's "fatal signal to init"
  handler, where the panic string originates) backwards through its
  xref `sub_FFFFFFF009EB5220`, which is the init-killing exit path.
  Also xref'd from `sub_FFFFFFF009A1DAC8`, which is almost certainly
  the exec_mach_imgact-equivalent.
- Grep for the imm16 pair (`MOVZ W0,#9 ; MOVZ W1,#1`) that constructs
  the OS_REASON_EXEC/BAD_MACHO argument pair.
- The matching XNU open-source reference is `bsd/kern/kern_exec.c`
  (`EXEC_EXIT_REASON_BAD_MACHO` is set in several places; the one
  that fires on init-SETEXEC-spawn is the key).

The right patch is likely surgical: identify the specific invariant
that fails for an adhoc Mach-O at exec time (not during the shared
cache load) and either skip the check or coerce the fail path to a
no-op `return 0`.

## Reproducing

```
patchkit.py apply <bootkc.before-exp10> \
  -m tools/watchos/qemu-sptm/patchkit/manifest-exp16-amfi-trustcache.yaml \
  -o bootkc.exp16          # → sha256 71af5c041c0dcbe0…
# boot: expect no AMFI shared-cache reject lines; expect
#       Identified Serial Port qemuport0..3; expect init[1] fatal
#       signal 9 namespace 9 code 0x1 (BAD_MACHO) as the new blocker.
```
