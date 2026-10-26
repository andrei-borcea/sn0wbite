"""
LLDB script to attach to QEMU gdbstub and investigate init BAD_MACHO.

Usage:
  lldb -s lldb_init_debug.py

Prerequisites:
  qemu-system-aarch64 ... -s -S  (listening on tcp:1234, paused at start)
"""
# ----- lldb commands -----
# connect to QEMU gdbstub
# QEMU exposes an abstract AArch64 target; lldb-arm64e should bind
"""
LLDB script equivalent (text form, used with -o flag):
"""

# This file is a REFERENCE for the commands to run interactively or via `-o`.
#
# COMMAND SEQUENCE:
#
# 1. Launch QEMU in a terminal:
#    cd $PREV/sptm/darwin-vm
#    cp $CUR/bootkc.exp16 firmware/bootkc
#    $PREV/sptm/qemu-sptm/build/qemu-system-aarch64 \
#        -M darwin -bootkc firmware/bootkc -dtree firmware/dtree \
#        -sptm firmware/sptm -txm firmware/txm -tc firmware/ramdisk.tc \
#        -ramdisk <rootfs.dmg> \
#        -args 'rd=md0 serial=3 -v -noprogress wdt=-1 wlan-olyhal-abort' \
#        -nographic -serial file:$CUR/boot_gdb.log -m 20G -display none \
#        -s -S     # gdb on 1234, paused at start
#
# 2. In another terminal, attach lldb:
#    lldb
#    (lldb) gdb-remote 1234
#
# 3. KASLR slide: observed 0x20000000. Runtime address = static + slide.
#    First BAD_MACHO BL (static): 0xFFFFFFF009EA81B4
#    Runtime: 0xFFFFFFF029EA81B4
#
# 4. Set breakpoint:
#    (lldb) breakpoint set --address 0xFFFFFFF029EA81B4
#    (lldb) continue
#
# 5. When hit, inspect:
#    (lldb) register read x0 x1 x2 x3 x4 x19 x30
#    - x0..x3 are args to the containing function (exec_mach_imgact)
#    - x19 is often a saved vp/proc pointer by convention
#    - x30 is caller LR
#
# 6. The vnode pointer should be in one of the arg registers. Try reading:
#    (lldb) memory read -s 8 -c 64 -f x <vp_ptr>
#    - Look for string pointers in the dump; v_name is typically between
#      offsets 0xB0 and 0xC8 in struct vnode.
#    - Once v_name offset is found:
#    (lldb) memory read -c 128 -f s <vp_ptr>+<offset>
#
# 7. The string will reveal WHICH binary init is trying to exec. From there:
#    - If xpcproxy: launchd's SETEXEC on a job's binary
#    - If a dylib: dyld's load of a specific library
#    - If launchd itself: start_init's initial exec
#
# 8. Also useful: break on os_reason_create to see the stack right at panic time:
#    (lldb) breakpoint set --address 0xFFFFFFF029F6DDF4
#    (lldb) breakpoint command add -F "lambda f, bp_loc, dict: f.target.process.selected_thread.StepOutOfFrame() and 0"
#
# EXPECTED OUTCOME:
# One iteration of this gets us the exact binary path. That tells us whether
# the shell path is:
#   (a) Patch a specific adhoc-signed system binary on the rootfs
#   (b) Build a replacement for that specific binary via watchOS SDK
#   (c) Specific kernel-side patch targeting that one exec
#
# All three are "consolidated" fixes vs. our whack-a-mole chain.

print("See script source for the full lldb command sequence.")
print("This file is a reference; actual lldb runs are interactive.")
