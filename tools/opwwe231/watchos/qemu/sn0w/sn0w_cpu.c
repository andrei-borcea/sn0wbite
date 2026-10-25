/*
 * sn0w CPU compat layer -- IMPDEF system-register neutralization.
 *
 * Part of the sn0wbite watchOS boot-chain port. See README.md.
 */
#include "qemu/osdep.h"
#include "hw/arm/sn0w_cpu.h"
#include "target/arm/cpregs.h"

void sn0w_cpu_neutralize_impdef(ARMCPU *cpu,
                                const Sn0wImpdefSpace *spaces, int n_spaces)
{
    int i, opc1;

    for (i = 0; i < n_spaces; i++) {
        /*
         * CRm and opc2 can be wildcarded (CP_ANY), but opc1 cannot: QEMU derives
         * the architectural min-EL / permission mask from opc1 *before* it
         * expands the wildcard, so CP_ANY there trips a g_assert_not_reached().
         * Enumerate opc1 0..7 and emit one blanket entry per value.
         *
         * ARM_CP_CONST   = reads return resetvalue (0), writes ignored (RAZ/WI).
         * ARM_CP_OVERRIDE = a later, specific definition of any encoding inside
         *                   this space replaces the blanket one, so a CPU model
         *                   can still implement individual registers for real.
         * PL1_RW is within the permission mask of every opc1 value, so the same
         * access setting is valid for all of them.
         */
        for (opc1 = 0; opc1 <= 7; opc1++) {
            ARMCPRegInfo ri = {
                .name = "SN0W_IMPDEF_RAZWI",
                .cp = CP_REG_ARM64_SYSREG_CP,
                .opc0 = spaces[i].op0,
                .opc1 = opc1,
                .crn = spaces[i].crn,
                .crm = CP_ANY,
                .opc2 = CP_ANY,
                .state = ARM_CP_STATE_AA64,
                .access = PL1_RW,
                .type = ARM_CP_CONST | ARM_CP_OVERRIDE,
                .resetvalue = 0,
            };
            define_one_arm_cp_reg(cpu, &ri);
        }
    }
}

void sn0w_cpu_neutralize_apple_impdef(ARMCPU *cpu)
{
    static const Sn0wImpdefSpace apple_impdef[] = {
        { .op0 = 3, .crn = 15 },
    };
    sn0w_cpu_neutralize_impdef(cpu, apple_impdef, ARRAY_SIZE(apple_impdef));
}

void sn0w_cpu_install_apple_aprr(ARMCPU *cpu)
{
    /*
     * APRR_EL0 (S3_4_C15_C2_0) and APRR_EL1 (S3_4_C15_C2_1): the Apple
     * A11..A13 / S4-S5 "Access Protection Remap Registers", two 16-entry x
     * 4-bit permission-index tables. The A13 QEMU core models only the
     * A14-style SPRR encodings (opc1=6); the Tempest/S5 kernel instead programs
     * these APRR registers (opc1=4), so this core must model the pair.
     *
     * They are REAL read/write state (backed by env->sprr.aprr_el[]), not
     * constants -- the kernel both writes and reads them back:
     *   - pmap protection updates re-read them and panic ("modifying a PPL
     *     mapping" / "creating an xPRR mapping") unless they hold the arch
     *     defaults APRR_EL0 = 0x4545010167670101, APRR_EL1 = 0x4455445566666677.
     *   - the PPL call gate, on entry, writes APRR_EL1 = 0x4455405566666677
     *     (the default with the index-10 nibble toggled to make PPL pages
     *     writable), ISB, then reads it back and panics "ppl_dispatch: failed
     *     due to bad arguments/state" unless the readback matches; it restores
     *     the default on exit. A write-ignored constant fails that readback --
     *     which is why these must be R/W, not ARM_CP_CONST.
     * The default and the PPL-mode value differ only at APRR index 10, which the
     * pmap check exempts (its allowed-index mask is 0x440 = bits 6 and 10), so a
     * single R/W register satisfies both the pmap check and the PPL gate. SPRR-
     * based MMU remapping is never enabled on this core, so the PTE AP/XN/PXN
     * bits keep their architectural meaning regardless. resetvalue seeds the
     * defaults for any reader that runs before the kernel's first write;
     * ARM_CP_OVERRIDE lets these replace the blanket RAZ/WI for their encodings.
     */
    ARMCPRegInfo aprr_el0 = {
        .name = "APRR_EL0",
        .cp = CP_REG_ARM64_SYSREG_CP,
        .opc0 = 3, .opc1 = 4, .crn = 15, .crm = 2, .opc2 = 0,
        .state = ARM_CP_STATE_AA64,
        .access = PL1_RW,
        .type = ARM_CP_OVERRIDE,
        .resetvalue = 0x4545010167670101ULL,
        .fieldoffset = offsetof(CPUARMState, sprr.aprr_el[0]),
    };
    ARMCPRegInfo aprr_el1 = {
        .name = "APRR_EL1",
        .cp = CP_REG_ARM64_SYSREG_CP,
        .opc0 = 3, .opc1 = 4, .crn = 15, .crm = 2, .opc2 = 1,
        .state = ARM_CP_STATE_AA64,
        .access = PL1_RW,
        .type = ARM_CP_OVERRIDE,
        .resetvalue = 0x4455445566666677ULL,
        .fieldoffset = offsetof(CPUARMState, sprr.aprr_el[1]),
    };
    define_one_arm_cp_reg(cpu, &aprr_el0);
    define_one_arm_cp_reg(cpu, &aprr_el1);
}
