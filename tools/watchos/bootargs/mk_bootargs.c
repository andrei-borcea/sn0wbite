/* mk_bootargs: build the boot_args blob the watchOS kernel expects in x0, for a loader that hands off to the kernelcache.
 * This is the hardware-independent part of the boot-chain reconstruction: the kernel's entry contract. It does NOT make the
 * kernel runnable on a non-Apple core by itself (the kernel still programs a 16 KB granule, PAC keys and Apple IMPDEF registers;
 * see docs/watchos/boot-chain-reconstruction.md). It produces the exact structure, so a loader or an emulator harness can use it.
 *
 *   mk_bootargs --phys 0x800000000 --virt 0xfffffff007004000 --memsize 0x40000000 \
 *               --topkernel 0x8xxxxxxxx --dt-phys 0x8yyyyyyyy --dt-len N \
 *               --cmdline "debug=0x8 -v" --out boot_args.bin
 * Writes a little-endian boot_args image. Verify the field offsets against boot_args.h. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boot_args.h"

static unsigned long long u(const char *s) { return strtoull(s, 0, 0); }

int main(int argc, char **argv) {
    boot_args b;
    memset(&b, 0, sizeof b);
    b.Revision = kBootArgsRevision;
    b.Version = kBootArgsVersion;
    const char *out = "boot_args.bin", *cmd = "-v";
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i], *v = i + 1 < argc ? argv[i + 1] : 0;
        if (!strcmp(a, "--phys") && v) b.physBase = u(argv[++i]);
        else if (!strcmp(a, "--virt") && v) b.virtBase = u(argv[++i]);
        else if (!strcmp(a, "--memsize") && v) { b.memSize = u(argv[++i]); b.memSizeActual = b.memSize; }
        else if (!strcmp(a, "--topkernel") && v) b.topOfKernelData = u(argv[++i]);
        else if (!strcmp(a, "--dt-phys") && v) b.deviceTreeP = u(argv[++i]);
        else if (!strcmp(a, "--dt-len") && v) b.deviceTreeLength = (uint32_t)u(argv[++i]);
        else if (!strcmp(a, "--machine") && v) b.machineType = (uint32_t)u(argv[++i]);
        else if (!strcmp(a, "--cmdline") && v) cmd = argv[++i];
        else if (!strcmp(a, "--out") && v) out = argv[++i];
    }
    snprintf(b.CommandLine, sizeof b.CommandLine, "%s", cmd);
    FILE *f = fopen(out, "wb");
    if (!f) { perror("open"); return 1; }
    fwrite(&b, 1, sizeof b, f);
    fclose(f);
    printf("wrote %s (%zu bytes)\n", out, sizeof b);
    printf("  Revision/Version   %u/%u\n", b.Revision, b.Version);
    printf("  virtBase           0x%llx\n", (unsigned long long)b.virtBase);
    printf("  physBase           0x%llx\n", (unsigned long long)b.physBase);
    printf("  memSize            0x%llx\n", (unsigned long long)b.memSize);
    printf("  topOfKernelData    0x%llx\n", (unsigned long long)b.topOfKernelData);
    printf("  deviceTreeP/len    0x%llx / %u\n", (unsigned long long)b.deviceTreeP, b.deviceTreeLength);
    printf("  CommandLine        \"%s\"\n", b.CommandLine);
    return 0;
}
