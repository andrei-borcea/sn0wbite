# IDA Hex-Rays decompilations

Snapshots of Hex-Rays output for the kernel functions that drive the
bringup patches. Pasted verbatim from IDA so experimental manifests can
cite exact variable names. These are DERIVED artefacts — the ground truth
is the kernelcache binary (`bootkc_watch7_11`, stored outside the repo at
`.scratchpad-local/session-previous/ida_work/`).

| File                           | Function / mechanism                                   |
|--------------------------------|--------------------------------------------------------|
| `amfi_vcs_hexrays.c`           | `_vnode_check_signature` (AMFI) — target of exp16      |
| `amfi_vnode_check.c`           | MACF vnode-check wrapper chain                         |
| `auth_remap.c`                 | sptm auth-remap plumbing (reference only)              |
| `dynamic_trust.c`              | kernel dynamic trust-cache lookup                      |
| `mac_vcs.c`                    | MACF `mac_vnode_check_signature` dispatch              |
| `map_and_slide_setup.c`        | `sub_FFFFFFF009F79CA8` shared_region_map_and_slide — exp15 negative-result target |
| `shared_region_check_np.c`     | shared region sanity check (exp15 upstream)            |
| `pdc_resolver.c`               | stub — ptrauth key resolver fragment                   |

Line numbers inside these files refer to IDA addresses encoded via
`sub_FFFFFFF0XXXXXXXX` names, not to source positions. They are useful
primarily to tie a patch byte at a specific VA back to the C statement
that produced the instruction.
