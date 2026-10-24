#!/system/bin/sh
# Give the stock power menu back when the module is removed.
( while [ "$(getprop sys.boot_completed)" != 1 ]; do sleep 2; done; settings delete global power_button_very_long_press ) > /dev/null 2>&1 &
