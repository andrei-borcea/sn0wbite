/* XNU arm64 boot_args, as the watchOS 6.3.1 (17U224, Watch5,4 / T8006 Apple S5) kernelcache consumes it.
 *
 * Field offsets 0x00..0x20 are read directly by the kernel reset vector (REA on the extracted kernelcache,
 * evidence ev_e5cf5c62...): `mov x20,x0` saves the boot_args pointer, then
 *   ldr x22,[x20,#0x08]  virtBase
 *   ldr x23,[x20,#0x10]  physBase
 *   ldr x24,[x20,#0x18]  memSize
 *   ldr x25,[x20,#0x20]  topOfKernelData  (scratch base where the kernel builds its own 16 KB page tables)
 *   ldr x26,[x20,#0x2d0] an arm64e boot_args field consumed by the early PAC/rebase path (name unconfirmed)
 * The rest (Video, machineType, deviceTreeP, CommandLine, bootFlags) follow the long-stable XNU layout.
 * The kernel then programs TCR_EL1 = 0x000800226519a519 (16 KB granule TTBR0+TTBR1, 39-bit VA, 40-bit PA). */
#ifndef SN0W_BOOT_ARGS_H
#define SN0W_BOOT_ARGS_H
#include <stdint.h>

#define BOOT_LINE_LENGTH 256
#define kBootArgsRevision 2
#define kBootArgsVersion 2

typedef struct {
    uint64_t v_baseAddr;   /* 0x28 framebuffer physical base */
    uint64_t v_display;    /* 0x30 */
    uint64_t v_rowBytes;   /* 0x38 */
    uint64_t v_width;      /* 0x40 */
    uint64_t v_height;     /* 0x48 */
    uint64_t v_depth;      /* 0x50 */
} boot_video;

typedef struct boot_args {
    uint16_t Revision;            /* 0x00 */
    uint16_t Version;             /* 0x02 */
    uint32_t _pad0;               /* 0x04 */
    uint64_t virtBase;            /* 0x08 kernel virtual base */
    uint64_t physBase;            /* 0x10 kernel physical base */
    uint64_t memSize;             /* 0x18 DRAM size the kernel may use */
    uint64_t topOfKernelData;     /* 0x20 first free phys page above the kernel: page-table scratch */
    boot_video Video;             /* 0x28 */
    uint32_t machineType;         /* 0x58 */
    uint32_t _pad1;               /* 0x5c */
    uint64_t deviceTreeP;         /* 0x60 physical pointer to the flattened Apple DeviceTree */
    uint32_t deviceTreeLength;    /* 0x68 */
    char     CommandLine[BOOT_LINE_LENGTH]; /* 0x6c */
    uint64_t bootFlags;           /* 0x170 */
    uint64_t memSizeActual;       /* 0x178 */
    /* arm64e builds add fields up to at least 0x2d0; left opaque until arm_init is decompiled */
    uint8_t  _tail[0x2d8 - 0x180];
} boot_args;

#endif
