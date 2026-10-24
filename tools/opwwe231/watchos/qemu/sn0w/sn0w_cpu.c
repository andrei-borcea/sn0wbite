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
