/* stage1-opw2 smoke payload — minimal AArch64 "I ran on slot B" proof-of-life.
 *
 * Shape: standard Linux ARM64 kernel image (Documentation/arm64/booting.rst),
 * so the Qualcomm bootloader's existing kernel load path accepts it unchanged.
 * The bootloader loads this image to any 2 MB-aligned RAM region (text_offset
 * = 0 → entry at image start), hands us x0=DTB-phys and jumps to _start.
 *
 * What this build does:
 *   1. Writes the ASCII tag "SN0W-LIVE" + the exception level we booted at
 *      (b'1'..b'3') into the OPW2 ramoops reserved region at 0x71f00000.
 *      The region is confirmed in profiles/opwwe231/device.yaml as 0x80000
 *      bytes starting at 0x71f00000; Linux's pstore driver ignores arbitrary
 *      bytes outside its ring record format, but the region content survives
 *      a warm reboot — we read it from Wear OS slot A afterwards via /dev/mem
 *      (ro.secure=1 blocks it) OR by cat /sys/fs/pstore if we embed a valid
 *      pstore record (follow-up).
 *   2. Enters a low-power WFI loop. No display, no interrupts, no clocks.
 *
 * Recovery contract: if this image hangs, the user holds POWER + <side key>
 * at a cold restart to force the bootloader into recovery. From sn0wbite
 * recovery `adb reboot bootloader; fastboot --set-active=a; fastboot reboot`
 * returns the device to Wear OS slot A. No persistent misc/BCB writes here
 * (bare metal cannot talk to UFS without a driver; left for v2).
 */

    .section .text.boot, "ax"
    .globl  _start
    .align  3

/* ---- Linux ARM64 Image header (64 bytes). Entry at offset 0. ---- */
_start:
    b       _entry                   /* 0x00 */
    .long   0                        /* 0x04: reserved */
    .quad   0x00000000               /* 0x08: text_offset (entry at image start) */
    .quad   __image_end - _start     /* 0x10: image_size */
    .quad   0x8                      /* 0x18: flags = little-endian, relocatable */
    .quad   0                        /* 0x20: reserved */
    .quad   0                        /* 0x28: reserved */
    .quad   0                        /* 0x30: reserved */
    .ascii  "ARM\x64"                /* 0x38: magic */
    .long   0                        /* 0x3C: reserved */

/* ---- entry ---- */
_entry:
    /* x0 = DT phys addr (bootloader-provided), x1..x3 = 0.
     * We're almost certainly at EL2 on this device (kvm-arm.mode=protected in
     * the live kernel cmdline), with MMU off, DAIF probably masked. */

    /* Compute RAMOOPS_BASE = 0x71f00000 in x4.
     * 0x71f00000 = (0x71f0) << 16. */
    movz    x4, #0x71f0, lsl #16

    /* Write "SN0W-LIVE" (9 bytes) + 1-byte EL marker, 10 bytes total. */
    adr     x5, _tag
    mov     x6, #10
1:  ldrb    w7, [x5], #1
    strb    w7, [x4], #1
    subs    x6, x6, #1
    b.ne    1b

    /* Append '0'+EL to the tag. CurrentEL is bits [3:2] of CurrentEL_EL1. */
    mrs     x7, CurrentEL
    lsr     x7, x7, #2
    and     x7, x7, #3
    add     w7, w7, #0x30            /* ASCII '0'..'3' */
    strb    w7, [x4]

    dsb     sy
    isb

    /* Low-power hang. Any reset triggers bootloader; user forces recovery. */
2:  wfi
    b       2b

    .section .rodata
_tag:
    .ascii  "SN0W-LIVE "              /* 10 bytes incl. trailing space */
    .align  3

    .section .bss
    .align  12
__image_end:
