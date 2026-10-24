#!/system/bin/sh
# v9 menu logic tests in --sim mode (no real misc, display, inputs, reboot). Run as root from /data/local/tmp/s9.
D=/data/local/tmp/s9
cd $D || exit 1
M=./sn0wmenu
rm -rf misc*.bin wd* *.log during_* frames frames10; mkdir frames frames10
cat > pay_ok.sh <<'EOF'
#!/system/bin/sh
dd if=/data/local/tmp/s9/misc.bin bs=1 count=16 2>/dev/null | od -An -c > /data/local/tmp/s9/during_bcb.txt
dd if=/data/local/tmp/s9/misc.bin bs=1 skip=32768 count=16 2>/dev/null | od -An -c > /data/local/tmp/s9/during_mark.txt
echo "hello from the payload"
echo "a long second line that has to wrap in the verbose window of the menu"
sleep 2
exit 0
EOF
printf '#!/system/bin/sh\nexit 3\n' > pay_fail.sh
printf '#!/system/bin/sh\nkill -9 $PPID\nsleep 5\n' > pay_kill.sh
printf '#!/system/bin/sh\nsleep 30\n' > pay_slow.sh
printf '#!/system/bin/sh\ni=1\nwhile [ $i -le 40 ]; do echo "line $i of the chatty payload"; i=$((i+1)); done\nsleep 1\nexit 0\n' > pay_chatty.sh
chmod 755 pay_*.sh
cat > targets.conf <<'EOF'
# test targets
WATCHOS TEST|/data/local/tmp/s9/nopayload
payload ok|/data/local/tmp/s9/pay_ok.sh
payload fail|/data/local/tmp/s9/pay_fail.sh
payload kill|/data/local/tmp/s9/pay_kill.sh
EOF
printf 'slow|/data/local/tmp/s9/pay_slow.sh\n' > targets2.conf
printf 'chatty|/data/local/tmp/s9/pay_chatty.sh\n' > targets3.conf
bcb() { od -An -c $1 | head -1; }
echo "##### T1 wear os"
$M --sim --misc misc1.bin --targets targets.conf --keys "p" --idle 30 2> t1.log; echo "exit=$?"; cat t1.log
echo "##### T2 navigation, payloads, info, shutdown (+ a live deadman that must stay quiet)"
$M --deadman --sim --misc misc.bin --wd wd2 --cap 300 --grace 10 2> dm2.log &
DM=$!
$M --sim --misc misc.bin --targets targets.conf --wd wd2 --dump frames --keys "nppnpnpnnpnnnpnnp" --idle 60 2> t2.log; echo "exit=$?"
kill $DM; wait $DM 2>/dev/null
cat t2.log; echo "--- deadman log"; cat dm2.log
echo "--- during payload ok: bcb / mark"; cat during_bcb.txt during_mark.txt
echo "--- after exit: bcb head / mark"; bcb misc.bin; dd if=misc.bin bs=1 skip=32768 count=16 2>/dev/null | od -An -c
echo "##### T3 payload kills the menu, then the next start"
$M --sim --misc misc3.bin --targets targets.conf --keys "npnnnp" --idle 60 2> t3a.log; echo "exit=$?"; cat t3a.log
echo "--- state left behind"; bcb misc3.bin; dd if=misc3.bin bs=1 skip=32768 count=16 2>/dev/null | od -An -c
$M --sim --misc misc3.bin --targets targets.conf --keys "" --idle 4 2> t3b.log; echo "exit=$?"; cat t3b.log
echo "--- state after the restart"; bcb misc3.bin; dd if=misc3.bin bs=1 skip=32768 count=16 2>/dev/null | od -An -c
echo "##### T4 idle timeout 4 s"
$M --sim --misc misc4.bin --targets targets.conf --keys "" --idle 4 2> t4.log; echo "exit=$?"; cat t4.log
echo "##### T5 deadman vs a frozen menu (BCB set by hand at +1 s must be cleared)"
$M --deadman --sim --misc misc5.bin --wd wd5 --stuck 3 --grace 6 --cap 100 2> dm5.log &
DM=$!
$M --sim --misc misc5.bin --targets targets.conf --wd wd5 --idle 100 --hang 3 --keys "" 2> m5.log &
MM=$!
sleep 1; printf 'boot-recovery' | dd of=misc5.bin bs=1 conv=notrunc 2>/dev/null
i=0; while kill -0 $DM 2>/dev/null && [ $i -lt 20 ]; do sleep 1; i=$((i+1)); done
echo "deadman gone after ~${i}s; alive=$(kill -0 $DM 2>/dev/null && echo yes || echo no)"; kill -9 $MM $DM 2>/dev/null
cat dm5.log; echo "--- bcb after"; bcb misc5.bin
echo "##### T6 deadman without any menu (grace 3)"
timeout 20 $M --deadman --sim --misc misc6.bin --wd wd6 --grace 3 --cap 100 2> dm6.log; echo "exit=$?"; cat dm6.log
echo "##### T7 pinned session: the deadman must stand down"
$M --sim --misc misc7.bin --targets targets.conf --wd wd7 --keys "nnp" --idle 60 2> t7.log; echo "exit=$?"; cat t7.log; echo "wd: $(cat wd7)"
timeout 12 $M --deadman --sim --misc misc7.bin --wd wd7 --stuck 2 --grace 2 --cap 3 2> dm7.log; echo "exit=$? (124 = still alive after 12 s)"; cat dm7.log
echo "##### T8 payload timeout (--payload-max 2)"
$M --sim --misc misc8.bin --targets targets2.conf --keys "npp" --payload-max 2 --idle 8 2> t8.log; echo "exit=$?"; cat t8.log
echo "##### T9 low battery through a fake battery dir"
mkdir -p bdir; echo 10 > bdir/capacity; echo Discharging > bdir/status
timeout 12 $M --deadman --sim --misc misc9.bin --wd wd9 --batt-dir bdir --cap 100 --grace 100 2> dm9a.log; echo "discharging 10%: exit=$? (0 = fired)"; cat dm9a.log
echo Charging > bdir/status
timeout 7 $M --deadman --sim --misc misc9.bin --wd wd9 --batt-dir bdir --cap 100 --grace 100 2> dm9b.log; echo "charging 10%: exit=$? (124 = stayed quiet)"; cat dm9b.log
echo 40 > bdir/capacity; echo Discharging > bdir/status
timeout 7 $M --deadman --sim --misc misc9.bin --wd wd9 --batt-dir bdir --cap 100 --grace 100 2> dm9c.log; echo "discharging 40%: exit=$? (124 = stayed quiet)"; cat dm9c.log
echo "##### T10 payload output: 40 lines reach the window, only the first 30 reach the flight recorder"
$M --sim --misc misc10.bin --targets targets3.conf --dump frames10 --keys "npp" --idle 8 2> t10.log; echo "exit=$?"
echo "stderr '> LINE' lines: $(grep -c '> line' t10.log)   flight recorder '> line' lines: $(dd if=misc10.bin bs=1 skip=36864 count=24008 2>/dev/null | grep -c '> line')"
grep -E "LAUNCH|returned" t10.log
echo "##### T11 missing targets file is reported"
$M --sim --misc misc11.bin --targets /nonexistent.conf --keys "" --idle 3 2> t11.log; grep -E "targets:|ready" t11.log
echo "##### done"; ls frames frames10
