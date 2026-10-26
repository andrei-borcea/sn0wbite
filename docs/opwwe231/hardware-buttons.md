# Hardware buttons, input devices and the power menu

Captured **24 October 2026** on **OPWWE231 A.192** (Android 14). Evidence:
[power button policy excerpt](../../research/opwwe231/2026-10-24-custom-boot/power-button-policy-excerpt.txt).

## Input devices

| Where | Device | Keys |
| --- | --- | --- |
| Android and recovery | `qpnp_pon` | `KEY_VOLUMEDOWN` (114) and `KEY_POWER` (116) |
| Android and recovery | `gpio-keys` | `KEY_F5`, `KEY_WAKEUP`; no event seen from either button |
| Android only | `sec_touchscreen` | touch; **absent in recovery** |
| Android and recovery | `aw-haptic-hv` | haptics, no keys |

In the recovery menu the button that moves down reports 114 and the one that selects reports 116 (see the
[flight recorder](../../research/opwwe231/2026-10-24-custom-boot/recovery-flight-recorder-hardware-runs.txt)). Wear OS maps
114 to `STEM_PRIMARY` and 116 to `POWER` in `qpnp_pon.kl`.

## What the framework does with them

| Gesture | Behaviour |
| --- | --- |
| POWER, short | Close IME or go home (`SHORT_PRESS_POWER_CLOSE_IME_OR_GO_HOME`); the `multifunction_single_shortcut_component` setting launches an activity (the sports app on our unit) |
| POWER, double | `MULTI_PRESS_POWER_LAUNCH_TARGET_ACTIVITY`; `multifunction_double_shortcut_component` (Wallet on our unit) |
| POWER, long | Nothing |
| POWER, very long | **Global actions**, the power menu (`VERY_LONG_PRESS_POWER_GLOBAL_ACTIONS`) |
| Crown press | All apps; double press switches recent app (`navigation_double_shortcut_component`); long press launches the assistant (`navigation_long_press_shortcut_component`) |

The power menu is drawn by `com.oplus.wearable.sysui` (`/system/priv-app/HeyClockworkSysUi/HeyClockworkSysUi.apk`). This build
has no `com.android.systemui` package.

## The sn0wbite power menu (implemented)

Hold POWER for 2.5 s and the sn0wbite menu opens instead of the stock one: four tiles, Restart, Recovery, Fastboot and Power off.
It is the Magisk module `sn0wbite_powermenu` ([release](../../releases/opwwe231/README.md), sources in
[tools/opwwe231/powermenu](../../tools/opwwe231/powermenu/)). Verified on the watch.

How it works:

- **The app is a priv-app with no root.** The module puts the APK in `/system/priv-app/Sn0wPowerMenu/` and a whitelist,
  `privapp-permissions-sn0wbite-powermenu.xml`, in `/system/etc/permissions/`. The app holds `REBOOT`, `RECOVERY` and `SHUTDOWN`
  (all `signature|privileged`), so `PowerManager.reboot(...)` and the shutdown intent work without `su`. This build has
  `ro.control_privapp_permissions=enforce`: a privileged permission missing from the whitelist stops the system server, so the
  whitelist must match the manifest exactly.
- **The stock menu is not controlled by the AOSP setting.** `power_button_very_long_press=0` makes `PhoneWindowManager` report
  `VERY_LONG_PRESS_POWER_NOTHING`, and the stock menu still opens. The OPLUS `ShortcutKeyCtrl` in `services.jar`
  (found and decompiled with REA, [excerpt](../../research/opwwe231/2026-10-24-custom-boot/shortcutkeyctrl-power-long-press-excerpt.txt))
  schedules `mLongPressRunnable` at a fixed 3000 ms after the POWER press, and the key release cancels it.
- **The watcher replaces it.** `powerkey` (static ARM32, started by the module's `service.sh`) reads the `qpnp_pon` device. At 2.5 s
  it starts the menu and injects the key release itself, so the 3 s timer is cancelled before it fires; the release also counts
  as a short press (screen off), so the watcher wakes the screen and shows the menu again. The module also sets the AOSP setting
  to `0` so the framework's own path stays off.
- **Recovery now lands in our recovery.** The module also overrides `/vendor/bin/install-recovery.sh` with a no-op, so Android
  stops rebuilding the stock recovery at every boot (see [Custom recovery](./custom-recovery.md)).

Things that cost time:

- Magisk's `--sqlite` policy write (granting root to the app) is what we avoided; the priv-app route needs no grant.
- After changing a priv-app inside a module, delete `/data/system/package_cache` and reboot: the package manager reuses its scan
  of `/system/priv-app` because an overlay does not change the directory time.
- The installer hook `set_permissions` did not leave the binary executable; `service.sh` runs `chmod 755` itself.
- d8 8.2 crashed with a null pointer on a small activity that captured a `null` string in an anonymous class; the app uses
  named classes and empty strings instead.
- The stock `WearGlobalActions` window is drawn by `com.oplus.wearable.sysui`; patching that APK was not needed.

To get the stock menu back, remove the module and run `settings delete global power_button_very_long_press`.
