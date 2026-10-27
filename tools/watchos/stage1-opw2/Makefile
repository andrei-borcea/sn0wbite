# stage1-opw2 build — AArch64 bare-metal "Linux Image"-shaped payload.
#
# Uses the Android NDK's aarch64 cross toolchain (same NDK the recovery menu
# uses). Produces stage1.bin which is a Linux arm64 Image ready to go into a
# boot.img v4 as the kernel payload.

NDK ?= /opt/homebrew/share/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin
CC   := $(NDK)/aarch64-linux-android21-clang
LD   := $(NDK)/ld.lld
OBJCOPY := $(NDK)/llvm-objcopy

SRC := src/entry.S
LD_SCRIPT := src/stage1.ld

all: stage1.bin

stage1.elf: $(SRC) $(LD_SCRIPT)
	$(CC) -nostdlib -nostartfiles -ffreestanding -Wl,-T,$(LD_SCRIPT) \
	      -fuse-ld=lld -static -o $@ $(SRC)

stage1.bin: stage1.elf
	$(OBJCOPY) -O binary $< $@
	@printf "stage1.bin: "
	@wc -c < $@
	@printf "first 64 bytes (header): "
	@od -An -tx1 -N 64 $@ | tr -d '\n' | sed 's/^ *//'
	@echo

clean:
	rm -f stage1.elf stage1.bin

.PHONY: all clean
