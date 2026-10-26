# Scripts

Helper scripts used during the Watch7,11 watchOS 27 bringup. They're not
reusable libraries — each one is a one-shot computation from a specific
investigation, kept here so the manifests in `../patchkit/` can be
regenerated from decompiled code snippets rather than just byte strings.

| Script             | What it computes                                               |
|--------------------|----------------------------------------------------------------|
| `exp15_asm.py`     | entry marker (exp15, negative result)                          |
| `exp16_asm.py`     | AMFI trust-cache trampoline bytes (`exp16` breakthrough)       |
| `exp21_asm.py`     | combined trust-cache + platform bits                           |
| `exp24_asm.py`     | OS_REASON filter                                               |
| `exp27_asm.py`     | OS_REASON logger (first draft, bug fixed by exp28/29)          |
| `exp28_asm.py`     | logger with fixed printf wrapper call sequence                 |
| `exp29_asm.py`     | logger with stack-arg Darwin AArch64 variadic ABI              |
| `disasm_range.py`  | mini wrapper to dump instructions over a VA window             |
| `parse_dtree.py`   | parse Apple DeviceTree binary blobs (for `dt_fixup-watch7_11.patch`) |

Each `expNN_asm.py` prints out the final `orig`/`new` hex strings that the
corresponding `manifest-expNN-*.yaml` consumes. If you edit the computed
sequence (e.g. change a branch target), rerun the script and paste the
output into the manifest.
