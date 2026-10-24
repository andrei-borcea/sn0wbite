Build of the v9 boot menu (macOS, Android NDK from Homebrew). Run from this directory.

  NDK=/opt/homebrew/share/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin
  # logo (only when the branding assets change): python3 -I gen_logo9.py ../../../../assets/branding/sn0wbite-avatar-512.png wordmark166x36.png logo9.h
  $NDK/armv7a-linux-androideabi21-clang -static -Os -Wall -D_FILE_OFFSET_BITS=64 -o sn0wmenu9 sn0wmenu9.c -lm
  $NDK/llvm-strip -o sn0wmenu9.stripped sn0wmenu9
  python3 -I pack_menu9.py <stock recovery.img> sn0wmenu9.stripped sn0w-targets.conf <out.img>     # refuses images over the safe size
  python3 -I verify_image.py <stock recovery.img> <out.img> sn0wmenu9.stripped sn0w-targets.conf    # offline diff against the stock image
  ../flash_recovery.sh --check <out.img>                                                            # then, with the user's OK, without --check

UI preview on the host (no device): clang -O1 -o preview9 preview9.c -lm && ./preview9 <0..4> out.ppm && python3 -I ppm2png.py out.ppm
Logic tests on the watch, in Android and without touching anything real (--sim: scratch misc file, scripted keys, no display, no reboot):
  adb push sn0wmenu9.stripped /data/local/tmp/s9/sn0wmenu; adb push test_sim9.sh /data/local/tmp/s9/s9test.sh
  adb shell "su -c 'sh /data/local/tmp/s9/s9test.sh > /data/local/tmp/s9/results.txt 2>&1'"     # ~80 s; T1..T9, read results.txt
  --keylog (sim, real key devices, no actions) logs the real button presses: which physical button is which key code.
Older sources (bootmenu2..6.c, probe7/8.c, ui.h, logo.h, pack_recovery2..4.py, pack_probe7/8.py) are kept for the record.
