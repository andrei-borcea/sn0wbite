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

## Adding Recovery and Fastboot to the running watch

Open work. The stock menu cannot be extended through settings: `power_button_very_long_press` only chooses between nothing and
global actions. Two routes were identified:

1. A small root app with a menu (Recovery, Fastboot, Restart, Power off), bound to a gesture through the shortcut settings above.
   It needs the Android SDK build tools to compile, which are not installed on the build machine, and Magisk `su` granted to
   the app through the policy database, because Magisk's prompt does not render usably on this display.
2. Patching `HeyClockworkSysUi.apk` so global actions opens our menu. This replaces the stock behaviour completely but a mistake
   leaves the watch without its interface.
