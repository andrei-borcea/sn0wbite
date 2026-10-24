#!/system/bin/sh
MODDIR=${0%/*}
TARGET=/system/media/bootanimation/bootanimation-plus.zip
chmod 755 "$MODDIR/bin/bootlog" # the installer hook does not always leave it executable
stop bootanim
mount --bind "$MODDIR/system/media/bootanimation.zip" "$TARGET"
# live verbose console: bootlog rewrites frames of that zip while the animation plays (see tools/recovery-menu/bootanim/bootlog.c).
# It exits by itself after boot completes; if it fails the static frames stay on screen. Disable the whole module with: touch $MODDIR/disable
"$MODDIR/bin/bootlog" --module "$MODDIR" > /data/local/tmp/sn0wbite_bootlog.txt 2>&1 &
start bootanim

# verbose boot logging: capture full dmesg + boot logcat on every boot
LOGDIR=/data/local/tmp/sn0wbite_boot_logs
mkdir -p "$LOGDIR"
TS=$(date +%Y%m%d_%H%M%S)
dmesg > "$LOGDIR/dmesg_${TS}.txt" 2>&1 &
