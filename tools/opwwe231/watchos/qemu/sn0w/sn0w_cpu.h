/*
 * sn0w CPU compat layer -- public interface.
 *
 * Part of the sn0wbite watchOS boot-chain port. See README.md in this
 * directory for the "mount your own CPU" porting guide.
 */
#ifndef HW_ARM_SN0W_CPU_H
#define HW_ARM_SN0W_CPU_H

#include "cpu.h"

/*
 * An implementation-defined (IMPDEF) AArch64 system-register space, named by
 * its fixed (op0, CRn) pair. Every (opc1, CRm, opc2) encoding inside the pair
 * is considered part of the space.
 *
 *   Apple cores (Cyclone .. Firestorm, incl. the S4/S5 watch SoCs) put their
 *   HID/EHID/ACC/CTRR/PMC/IPI/... registers at op0=3, CRn=15.
 */
typedef struct Sn0wImpdefSpace {
    int op0;
    int crn;
} Sn0wImpdefSpace;

/*
 * Absorb every encoding in each declared IMPDEF space as RAZ/WI (read-as-zero,
 * write-ignore) instead of letting it raise an Undefined Instruction trap.
 *
 * This lets a QEMU core execute a kernel built for a different CPU, which pokes
 * dozens of vendor registers this core never heard of, without wiring every one
 * of them by hand. Registers the core *does* model for real are declared as
 * usual with define_arm_cp_regs() AFTER this call: they transparently override
 * the blanket entry for their own encoding (QEMU keeps the last definition, and
 * the blanket entry carries ARM_CP_OVERRIDE so the real one is allowed to win).
 *
 * Call order therefore matters: neutralize first, then your real table.
 */
void sn0w_cpu_neutralize_impdef(ARMCPU *cpu,
                                const Sn0wImpdefSpace *spaces, int n_spaces);

/* Convenience preset for the Apple IMPDEF space (op0=3, CRn=15). */
void sn0w_cpu_neutralize_apple_impdef(ARMCPU *cpu);

/*
 * Install the Apple APRR permission-remap registers APRR_EL0 (S3_4_C15_C2_0)
 * and APRR_EL1 (S3_4_C15_C2_1) as real read/write state (backed by
 * env->sprr.aprr_el[], seeded to their architectural defaults). An APRR-era
 * kernel (A11..A13 / S4-S5 watch SoCs) both writes these and reads them back
 * -- the pmap consistency check and the PPL call gate's readback both fail
 * against a write-ignored constant -- so they must be genuine R/W, not CONST.
 * The A13 QEMU core only models the A14-style SPRR encodings (opc1=6); this
 * supplies the older APRR pair (opc1=4) the Tempest kernel programs. Call
 * AFTER sn0w_cpu_neutralize_apple_impdef() so these override the blanket
 * RAZ/WI for their two encodings.
 */
void sn0w_cpu_install_apple_aprr(ARMCPU *cpu);

#endif /* HW_ARM_SN0W_CPU_H */
