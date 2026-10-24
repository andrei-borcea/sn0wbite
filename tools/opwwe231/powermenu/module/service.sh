#!/system/bin/sh
MODDIR=${0%/*}
chmod 755 "$MODDIR/bin/powerkey" # the installer hook does not always leave it executable
# The stock very-long-press (global actions) menu is turned off; bin/powerkey opens the sn0wbite menu on a 2.5 s hold instead.
# To get the stock menu back: settings delete global power_button_very_long_press (and remove or disable this module).
(
  while [ "$(getprop sys.boot_completed)" != 1 ]; do sleep 2; done
  settings put global power_button_very_long_press 0
  exec "$MODDIR/bin/powerkey" --hold 2500
) > /data/local/tmp/sn0wbite_powerkey.txt 2>&1 &
