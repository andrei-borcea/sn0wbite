#!/system/bin/sh
# service.sh runs late (post boot-complete-ish); capture a logcat snapshot of the full boot sequence
LOGDIR=/data/local/tmp/sn0wbite_boot_logs
mkdir -p "$LOGDIR"
TS=$(date +%Y%m%d_%H%M%S)
( sleep 20; logcat -b all -d > "$LOGDIR/logcat_boot_${TS}.txt" 2>&1 ) &
