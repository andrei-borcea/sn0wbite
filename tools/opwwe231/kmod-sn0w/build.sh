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
# macOS APFS is case-insensitive by default: kernel source files like
# xt_DSCP.c + xt_dscp.c collide and git drops one. We keep the kernel source
# in a Podman NAMED VOLUME (lives in the Linux VM's ext4 — case-sensitive)
# instead of a bind-mount from the Mac filesystem.
AKM_VOLUME="sn0w-akm"
IMAGE="sn0w-kbuild"

# Android Common Kernel ("ACK") — same major.minor as the device's running
# kernel (5.15.185). The device vermagic has "-android14-11-maybe-dirty"; we
# aim for CRC match on core symbols which stay stable across the android14
# release train. `android14-5.15-lts` is the long-term stable snapshot.
KERNEL_URL="https://android.googlesource.com/kernel/common"
KERNEL_BRANCH="android14-5.15-lts"

# Compiler: container ships clang-14 from Ubuntu Jammy. For the hello module
# we rely on it; if MODVERSIONS CRC comparison fails we revisit with the
# exact Android Clang r487747c (device was built with 17.0.2, from uname).
CLANG_URL=""

cmd_setup() {
    echo "[setup] podman build -t $IMAGE $HERE"
    podman build --platform=linux/arm64 -t "$IMAGE" "$HERE"
}

in_container() {
    local tty_flags="-i"
    [[ -t 0 ]] && tty_flags="-it"
    # Ensure the named volume exists (idempotent).
    podman volume inspect "$AKM_VOLUME" >/dev/null 2>&1 || podman volume create "$AKM_VOLUME" >/dev/null
    podman run --rm $tty_flags \
        --platform=linux/arm64 \
        -v "$HERE":/sn0w:z \
        -v "$AKM_VOLUME":/akm \
        -w /sn0w \
        "$IMAGE" "$@"
}

cmd_fetch() {
    in_container bash -c '
        set -e
        cd /akm
        if [[ ! -d common ]]; then
            echo "[fetch] cloning ACK (shallow) '"$KERNEL_BRANCH"'…"
            git clone --depth 1 --branch '"$KERNEL_BRANCH"' '"$KERNEL_URL"' common
        else
            echo "[fetch] ACK already present at /akm/common"
            (cd common && git log -1 --oneline)
        fi
        echo "[fetch] done"
        du -sh common
    '
}

cmd_config() {
    local src_config="${1:-$REPO/.scratchpad-local/sn0w-opw2-config.txt}"
    if [[ ! -f "$src_config" ]]; then
        echo "ERROR: dumped device config not found at $src_config" >&2
        echo "       (produce it with: adb shell su -c zcat /proc/config.gz > $src_config)" >&2
        exit 1
    fi
    # The device .config lives on the Mac side; copy it into the volume via
    # a side-channel bind-mount.
    podman run --rm --platform=linux/arm64 \
        -v "$src_config":/src.config:ro \
        -v "$AKM_VOLUME":/akm \
        "$IMAGE" cp /src.config /akm/.config.device
    in_container bash -c '
        set -e
        cp /akm/.config.device /akm/common/.config
        cd /akm/common
        # Device config references OPPO/Google-internal paths we do not have in
        # a fresh ACK source: abi_symbollist whitelist, signing_key.pem. Disable
        # them — we are not shipping a signed/release build.
        sed -i -E "s/^CONFIG_TRIM_UNUSED_KSYMS=y/# CONFIG_TRIM_UNUSED_KSYMS is not set/" .config
        sed -i -E "s/^CONFIG_UNUSED_KSYMS_WHITELIST=.*/CONFIG_UNUSED_KSYMS_WHITELIST=\"\"/" .config
        sed -i -E "s/^CONFIG_MODULE_SIG_KEY=.*/CONFIG_MODULE_SIG_KEY=\"\"/" .config
        sed -i -E "s/^CONFIG_SYSTEM_TRUSTED_KEYS=.*/CONFIG_SYSTEM_TRUSTED_KEYS=\"\"/" .config
        sed -i -E "s/^CONFIG_SYSTEM_REVOCATION_KEYS=.*/CONFIG_SYSTEM_REVOCATION_KEYS=\"\"/" .config
        # Disable Clang LTO: linking vmlinux with LTO OOMs on 12 GB VMs.
        # We only need Module.symvers for CRC matching, not an optimized Image.
        sed -i -E "s/^CONFIG_LTO_CLANG=.*/# CONFIG_LTO_CLANG is not set/" .config
        sed -i -E "s/^CONFIG_LTO_CLANG_THIN=.*/# CONFIG_LTO_CLANG_THIN is not set/" .config
        sed -i -E "s/^CONFIG_LTO_CLANG_FULL=.*/# CONFIG_LTO_CLANG_FULL is not set/" .config
        sed -i -E "s/^CONFIG_THINLTO=.*/# CONFIG_THINLTO is not set/" .config
        sed -i -E "s/^# CONFIG_LTO_NONE.*/CONFIG_LTO_NONE=y/" .config
        grep -qE "^CONFIG_LTO_NONE=y" .config || echo "CONFIG_LTO_NONE=y" >> .config
        # Also disable CFI (depends on LTO) and PGO.
        sed -i -E "s/^CONFIG_CFI_CLANG=.*/# CONFIG_CFI_CLANG is not set/" .config
        # BTF needs the `pahole` toolchain which the Ubuntu Jammy container
        # does not ship easily. We do not consume BTF info; disable it.
        sed -i -E "s/^CONFIG_DEBUG_INFO_BTF=.*/# CONFIG_DEBUG_INFO_BTF is not set/" .config
        sed -i -E "s/^CONFIG_DEBUG_INFO_BTF_MODULES=.*/# CONFIG_DEBUG_INFO_BTF_MODULES is not set/" .config
        make ARCH=arm64 LLVM=1 olddefconfig
        echo "[config] olddefconfig done; relevant lines:"
        grep -E "^(CONFIG_)?TRIM_UNUSED|^(CONFIG_)?UNUSED_KSYMS|^(CONFIG_)?MODULE_SIG_KEY|^(CONFIG_)?SYSTEM_TRUSTED" .config
    '
}

cmd_kernel() {
    in_container bash -c '
        set -e
        cd /akm/common
        make ARCH=arm64 LLVM=1 LLVM_IAS=1 -j$(nproc) Image modules_prepare
        make ARCH=arm64 LLVM=1 LLVM_IAS=1 -j$(nproc) modules
        echo "[kernel] built; Module.symvers:"
        wc -l Module.symvers
    '
}

cmd_module() {
    in_container bash -c '
        set -e
        cd /akm/common
        make ARCH=arm64 LLVM=1 LLVM_IAS=1 M=/sn0w modules
        echo "[module] done:"
        ls -la /sn0w/sn0w_hello.ko
        llvm-objcopy --dump-section .modinfo=/tmp/mi /sn0w/sn0w_hello.ko
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
