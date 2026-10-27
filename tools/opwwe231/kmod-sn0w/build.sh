#!/usr/bin/env bash
# build.sh — orchestrator for the sn0w kernel module build.
#
# Phases:
#   setup     — one-shot: build the Podman image sn0w-kbuild
#   fetch     — one-shot: clone the Android Common Kernel + download Clang
#   config    — copy the device /proc/config.gz (host-side) into the kernel
#               source tree and run `make olddefconfig`
#   kernel    — full kernel build (needed first time, generates Module.symvers)
#   module    — build the sn0w .ko out-of-tree
#   push      — adb push the .ko to /sdcard, suggest the insmod incantation
#   all       — setup + fetch + config + kernel + module
#
# Large intermediates live in `.scratchpad-local/akm/` (gitignored).
set -euo pipefail

HERE=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO=$(cd -- "$HERE/../../.." && pwd)
AKM="$REPO/.scratchpad-local/akm"
IMAGE="sn0w-kbuild"

mkdir -p "$AKM"

# Android Common Kernel ("ACK") — same major.minor as the device's running
# kernel. OPPO patch set is proprietary; the device vermagic is
# "5.15.185-android14-11-maybe-dirty" so we aim for a match-or-override.
KERNEL_URL="https://android.googlesource.com/kernel/common"
KERNEL_BRANCH="common-android14-5.15"

# Android Clang r487747c — the exact build the device kernel was compiled
# with (from `uname -v`: "based on r487747c"). darwin-x86 is a Linux prebuilt
# despite the name; AOSP's `prebuilts/clang/host/linux-x86/` is what we want
# in the Linux container.
CLANG_URL="https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+archive/refs/heads/main/clang-r487747c.tar.gz"

cmd_setup() {
    echo "[setup] podman build -t $IMAGE $HERE"
    podman build --platform=linux/arm64 -t "$IMAGE" "$HERE"
}

in_container() {
    podman run --rm -it \
        --platform=linux/arm64 \
        -v "$HERE":/sn0w:z \
        -v "$AKM":/akm:z \
        -w /sn0w \
        "$IMAGE" "$@"
}

cmd_fetch() {
    in_container bash -c '
        set -e
        cd /akm
        if [[ ! -d common ]]; then
            echo "[fetch] cloning ACK (shallow)…"
            git clone --depth 1 --branch '"$KERNEL_BRANCH"' '"$KERNEL_URL"' common
        else
            echo "[fetch] ACK already present at /akm/common"
        fi
        if [[ ! -d clang ]]; then
            echo "[fetch] downloading Clang r487747c…"
            mkdir -p clang
            curl -sSL '"$CLANG_URL"' | tar -xz -C clang
        else
            echo "[fetch] Clang already present at /akm/clang"
        fi
        echo "[fetch] done"
        du -sh common clang
    '
}

cmd_config() {
    local src_config="${1:-$REPO/.scratchpad-local/sn0w-opw2-config.txt}"
    if [[ ! -f "$src_config" ]]; then
        echo "ERROR: dumped device config not found at $src_config" >&2
        echo "       (produce it with: adb shell su -c zcat /proc/config.gz > $src_config)" >&2
        exit 1
    fi
    cp "$src_config" "$AKM/.config.device"
    in_container bash -c '
        set -e
        cp /akm/.config.device /akm/common/.config
        cd /akm/common
        make ARCH=arm64 LLVM=1 olddefconfig
        echo "[config] olddefconfig done; .config written"
        head -40 .config
    '
}

cmd_kernel() {
    in_container bash -c '
        set -e
        export PATH=/akm/clang/bin:$PATH
        cd /akm/common
        make ARCH=arm64 LLVM=1 LLVM_IAS=1 -j$(nproc) Image modules
        echo "[kernel] built; Module.symvers:"
        wc -l Module.symvers
    '
}

cmd_module() {
    in_container bash -c '
        set -e
        export PATH=/akm/clang/bin:$PATH
        cd /akm/common
        make ARCH=arm64 LLVM=1 LLVM_IAS=1 M=/sn0w modules
        echo "[module] done:"
        ls -la /sn0w/sn0w_hello.ko
        /akm/clang/bin/llvm-objcopy --dump-section .modinfo=/tmp/mi /sn0w/sn0w_hello.ko
        tr "\0" "\n" < /tmp/mi
    '
}

cmd_push() {
    local ko="$HERE/sn0w_hello.ko"
    [[ -f "$ko" ]] || { echo "no $ko; run build.sh module first"; exit 1; }
    adb push "$ko" /sdcard/sn0w_hello.ko
    echo
    echo "To load (from Wear OS adb shell, after script-pushing):"
    echo "  adb shell su -c 'insmod /sdcard/sn0w_hello.ko && dmesg | tail -20 | grep sn0w'"
    echo "  adb shell su -c 'rmmod sn0w_hello && dmesg | tail -5 | grep sn0w'"
}

cmd_all() {
    cmd_setup
    cmd_fetch
    cmd_config
    cmd_kernel
    cmd_module
}

case "${1:-}" in
    setup) cmd_setup ;;
    fetch) cmd_fetch ;;
    config) cmd_config "${2:-}" ;;
    kernel) cmd_kernel ;;
    module) cmd_module ;;
    push) cmd_push ;;
    all) cmd_all ;;
    "") sed -n '2,22p' "$0" | sed 's/^# \{0,1\}//' ;;
    *) echo "unknown phase: $1"; exit 2 ;;
esac
