# OPWWE231 A.192: custom recovery menu and live boot screen, 24 October 2026

Target: **OnePlus Watch 2**, Android 14, build **A.192.260910**, bootloader unlocked, root through Magisk.

Narrative and design: [Custom recovery](../../../docs/opwwe231/custom-recovery.md),
[Live boot screen](../../../docs/opwwe231/boot-screen.md) and
[Hardware buttons](../../../docs/opwwe231/hardware-buttons.md). The built images are in
[releases/opwwe231](../../../releases/opwwe231/README.md).

| File | Contents |
| --- | --- |
| [recovery-flight-recorder-hardware-runs.txt](./recovery-flight-recorder-hardware-runs.txt) | The menu's own log from the first runs on the watch: display recipe (first modeset fails, second works), key devices and codes, Wear OS and fastboot exits |
| [recovery-simulation-regression.txt](./recovery-simulation-regression.txt) | Menu logic, in-flight dual boot marker, idle exit, watchdog cases and payload output, run in `--sim` mode on the watch |
| [libbootanimation-features.txt](./libbootanimation-features.txt) | What the stock boot animation player can draw, from its strings (REA evidence ID inside) |
| [live-boot-console-excerpts.txt](./live-boot-console-excerpts.txt) | The red/green in-place frame test, player log lines, boot timeline and the first real boot with the console |
| [power-button-policy-excerpt.txt](./power-button-policy-excerpt.txt) | Framework policy for POWER and the crown, the shortcut settings, and the SysUI package |
| [shortcutkeyctrl-power-long-press-excerpt.txt](./shortcutkeyctrl-power-long-press-excerpt.txt) | The OPLUS power key handler in `services.jar`: the fixed 3000 ms timer that opens the stock power menu, decompiled with REA |
| [provenance.json](./provenance.json) | Where each file comes from |
| [SHA256SUMS](./SHA256SUMS) | SHA-256 digests for this collection |
