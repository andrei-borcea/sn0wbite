/* stage1-opw2 linker script.
 *
 * The Qualcomm bootloader loads a Linux-shaped Image to a 2 MB-aligned RAM
 * region (the exact phys addr is picked by the loader; the Image is PIC so
 * long as text_offset=0). We emit a flat binary with everything at offset 0.
 */
ENTRY(_start)

PHDRS {
    load PT_LOAD FLAGS(5);  /* R-X */
}

SECTIONS {
    . = 0;

    .text : {
        KEEP(*(.text.boot))
        *(.text .text.*)
    } :load

    .rodata : ALIGN(8) {
        *(.rodata .rodata.*)
    } :load

    .data : ALIGN(8) {
        *(.data .data.*)
    } :load

    .bss (NOLOAD) : ALIGN(16) {
        *(.bss .bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.ARM.*)
        *(.eh_frame*)
    }
}
