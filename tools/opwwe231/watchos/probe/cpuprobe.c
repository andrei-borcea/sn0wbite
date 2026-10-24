/* cpuprobe: what the W5 cores really implement, as seen from EL0 of an aarch64 process (static, NDK aarch64).
 * The arm64 kernel emulates mrs of the ID registers for EL0 when HWCAP_CPUID is set, and may hide fields it does not expose.
 * Prints the registers and the fields that decide whether the watchOS kernel can run: translation granules, atomics, pointer
 * authentication, exception levels. Also dumps the native /proc/cpuinfo feature line (a 32-bit reader only sees the compat view). */
#include <stdio.h>
#include <string.h>
#include <sys/auxv.h>

#define RD(r) ({ unsigned long v_; __asm__ volatile("mrs %0, " #r : "=r"(v_)); v_; })
#define F(v, hi, lo) (((v) >> (lo)) & ((1UL << ((hi) - (lo) + 1)) - 1))

int main(void) {
    unsigned long hw = getauxval(AT_HWCAP);
    printf("AT_HWCAP=0x%lx  CPUID emulation (bit 11): %s\n", hw, hw & (1UL << 11) ? "yes" : "NO");
    FILE *f = fopen("/proc/cpuinfo", "r");
    char l[512];
    while (f && fgets(l, sizeof l, f)) if (!strncmp(l, "Features", 8) || !strncmp(l, "CPU part", 8)) { fputs(l, stdout); if (!strncmp(l, "Features", 8)) break; }
    if (!(hw & (1UL << 11))) return 1;
    unsigned long midr = RD(midr_el1), pfr0 = RD(id_aa64pfr0_el1), isar0 = RD(id_aa64isar0_el1), isar1 = RD(id_aa64isar1_el1);
    unsigned long mmfr0 = RD(id_aa64mmfr0_el1), mmfr1 = RD(id_aa64mmfr1_el1), mmfr2 = RD(id_aa64mmfr2_el1);
    printf("MIDR_EL1        0x%016lx  implementer 0x%lx part 0x%lx variant %lu rev %lu\n", midr, F(midr, 31, 24), F(midr, 15, 4), F(midr, 23, 20), F(midr, 3, 0));
    printf("ID_AA64PFR0     0x%016lx  EL0 %lu EL1 %lu EL2 %lu EL3 %lu FP %lu AdvSIMD %lu GIC %lu\n", pfr0, F(pfr0, 3, 0), F(pfr0, 7, 4), F(pfr0, 11, 8), F(pfr0, 15, 12), F(pfr0, 19, 16), F(pfr0, 23, 20), F(pfr0, 27, 24));
    printf("ID_AA64ISAR0    0x%016lx  AES %lu SHA1 %lu SHA2 %lu CRC32 %lu ATOMIC(LSE) %lu RDM %lu\n", isar0, F(isar0, 7, 4), F(isar0, 11, 8), F(isar0, 15, 12), F(isar0, 19, 16), F(isar0, 23, 20), F(isar0, 31, 28));
    printf("ID_AA64ISAR1    0x%016lx  DPB %lu APA %lu API %lu JSCVT %lu FCMA %lu LRCPC %lu GPA %lu GPI %lu\n", isar1, F(isar1, 3, 0), F(isar1, 7, 4), F(isar1, 11, 8), F(isar1, 15, 12), F(isar1, 19, 16), F(isar1, 23, 20), F(isar1, 27, 24), F(isar1, 31, 28));
    printf("ID_AA64MMFR0    0x%016lx  PARange %lu ASIDBits %lu TGran16 %lu TGran64 %lu TGran4 %lu\n", mmfr0, F(mmfr0, 3, 0), F(mmfr0, 7, 4), F(mmfr0, 23, 20), F(mmfr0, 27, 24), F(mmfr0, 31, 28));
    printf("ID_AA64MMFR1    0x%016lx  HAFDBS %lu VMIDBits %lu VH %lu HPDS %lu PAN %lu\n", mmfr1, F(mmfr1, 3, 0), F(mmfr1, 7, 4), F(mmfr1, 11, 8), F(mmfr1, 15, 12), F(mmfr1, 23, 20));
    printf("ID_AA64MMFR2    0x%016lx  CnP %lu UAO %lu LSM %lu IESB %lu VARange %lu\n", mmfr2, F(mmfr2, 3, 0), F(mmfr2, 7, 4), F(mmfr2, 11, 8), F(mmfr2, 15, 12), F(mmfr2, 19, 16));
    puts("Reading guide: TGran16 0 = 16 KB granule not implemented (1 = yes, 2 = yes with LPA2); ATOMIC 2 = LSE; APA/API/GPA/GPI nonzero = pointer authentication;");
    puts("a field the kernel hides in the emulated register reads as 0, so a zero is only conclusive for fields the kernel documents as visible.");
    return 0;
}
