# boot-watchos payload build — static ARM32 Android executable, matches
# the recovery userspace ABI used by sn0wbite (armv8l / 32-bit userspace on
# a 64-bit kernel, same as sn0wmenu9).

NDK ?= /opt/homebrew/share/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin
CC   := $(NDK)/armv7a-linux-androideabi21-clang
STRIP := $(NDK)/llvm-strip

SRC := src/boot-watchos.c

all: boot-watchos.stripped

boot-watchos: $(SRC)
	$(CC) -Os -Wall -D_FILE_OFFSET_BITS=64 -o $@ $< -lm

boot-watchos.stripped: boot-watchos
	$(STRIP) -o $@ $<
	@printf "boot-watchos.stripped: "
	@wc -c < $@

clean:
	rm -f boot-watchos boot-watchos.stripped

.PHONY: all clean
