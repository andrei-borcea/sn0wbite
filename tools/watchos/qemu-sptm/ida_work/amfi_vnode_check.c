
//WARNING: Removing unreachable block (ram,0xfffffff00893e66c)

ulong fcn.fffffff00893da34
                (int64_t arg1,uchar noname_1 [16],uchar noname_2 [16],int64_t arg2,int64_t arg3,int64_t arg4,
                int64_t arg5,int64_t arg6,uchar noname_8 [16],int64_t noname_9,int64_t arg_10h)

{
    uint16_t uVar1;
    bool bVar2;
    int64_t *piVar3;
    bool bVar4;
    uchar uVar5;
    int iVar6;
    int iVar7;
    uint32_t uVar8;
    uint32_t uVar9;
    int64_t *arg3_00;
    int64_t iVar10;
    int64_t iVar11;
    uint64_t uVar12;
    int64_t iVar13;
    int64_t *piVar14;
    ulong uVar15;
    int64_t *piVar16;
    uint64_t uVar17;
    char *pcVar18;
    uint8_t uVar19;
    uint32_t uVar20;
    ulong uVar21;
    uint8_t uVar22;
    int64_t *piVar23;
    uint64_t uVar24;
    int64_t in_stack_00000000;
    int64_t in_stack_00000008;
    int64_t iStack_318;
    uint64_t uStack_310;
    uint8_t uStack_2fc;
    int64_t *piStack_2f8;
    int64_t iStack_2e8;
    uint32_t uStack_2d8;
    ulong uStack_2c0;
    int64_t *piStack_2b8;
    ushort uStack_2aa;
    ulong uStack_2a8;
    ulong *puStack_2a0;
    int64_t iStack_298;
    uchar uStack_290;
    ulong uStack_288;
    ulong *puStack_280;
    int64_t iStack_278;
    uchar uStack_270;
    int64_t iStack_268;
    int64_t *piStack_260;
    int64_t iStack_258;
    uchar uStack_250;
    int64_t *piStack_248;
    int64_t **ppiStack_240;
    int64_t iStack_238;
    uchar uStack_230;
    uint64_t uStack_228;
    ulong uStack_220;
    ulong *puStack_218;
    int64_t iStack_210;
    int64_t *piStack_208;
    uint64_t uStack_200;
    ulong uStack_1f0;
    ulong *puStack_1e8;
    int64_t iStack_1e0;
    int64_t *piStack_1d8;
    ulong uStack_1c8;
    ushort uStack_1be;
    int iStack_1bc;
    ulong uStack_1b8;
    ulong uStack_1b0;
    int64_t *piStack_1a8;
    int64_t **ppiStack_1a0;
    int64_t iStack_198;
    ulong uStack_190;
    ulong uStack_188;
    ulong uStack_180;
    ulong uStack_178;
    ulong uStack_170;
    ulong uStack_168;
    ulong uStack_160;
    int64_t *piStack_158;
    ulong uStack_150;
    ulong uStack_148;
    ulong uStack_140;
    ulong uStack_138;
    int64_t **ppiStack_130;
    ulong *puStack_128;
    ulong *puStack_120;
    int64_t **ppiStack_118;
    int64_t *piStack_110;
    ulong *puStack_108;
    ulong *puStack_100;
    int64_t iStack_f8;
    int64_t iStack_f0;
    int64_t *piStack_e8;
    int64_t iStack_e0;
    int64_t iStack_d8;
    ulong uStack_d0;
    ulong uStack_c8;
    ulong uStack_c0;
    ulong uStack_b8;
    ulong uStack_b0;
    ulong uStack_a8;
    ulong uStack_a0;
    ulong uStack_98;
    ulong uStack_90;
    ulong uStack_88;
    int64_t iStack_78;
    int64_t var_68h;
    
    iStack_78 = **0xfffffff007bcf9e0;
    if (**0xfffffff007bcf9f0 != 0) {
        fcn.fffffff008958174(noname_1._0_8_,noname_2._0_8_,"AMFI: vnode_check_signature called with platform %d\n");
    }
    arg3_00 = fcn.fffffff0089581a4(0xfffffff007bcd008);
    piVar23 = arg3_00 & 0xffffffffffff | 0x6712000000000000;
    piVar14 = arg3_00 + 1;
    fcn.fffffff008958564(piVar14,0x401);
    *arg3_00 = -0xff84346e0;
    arg3_00[0x82] = noname_9;
    arg3_00[0x83] = 0;
    uStack_150 = 0;
    iVar6 = fcn.fffffff008958b54(noname_9,"com.apple.mis.warning",0,0,&uStack_150);
    if ((iVar6 == 0x22) || (iVar6 == 0)) {
        if ((*(arg3_00 + 0x81) & 1) == 0) {
            uVar5 = (**(*arg3_00 + 0x10))(arg3_00);
            *(arg3_00 + 0x81) = uVar5;
        }
        fcn.fffffff008958174("Respecting MIS launch warning: %s\n");
    }
    uVar8 = *arg3;
    *arg4 = 0;
    uStack_2aa = 0;
    iVar10 = fcn.fffffff008958704(arg2,&uStack_2aa);
    iVar11 = fcn.fffffff008958714(arg2);
    if ((iVar11 == 0) ||
       (uVar9 = (*(iVar11 + 8) & 0xff00ff00) >> 8 | (*(iVar11 + 8) & 0xff00ff) << 8,
       (uVar9 >> 0x10 | uVar9 << 0x10) >> 10 < 0x81)) {
        uVar22 = 0;
    }
    else {
        uVar22 = *(iVar11 + 0x57) & 1;
    }
    uStack_2c0 = 0;
    uVar12 = fcn.fffffff00893fce8(&piStack_2b8,arg2,arg3_00,&uStack_2c0);
    piVar3 = piStack_2b8;
    iVar11 = in_stack_00000008;
    if ((uVar12 & 1) == 0) {
        pcVar18 = "The signature could not be validated because AMFI could not load its entitlements for validation: %s"
        ;
code_r0xfffffff00893ded8:
        fcn.fffffff00893c038(arg3_00,in_stack_00000000,in_stack_00000008,pcVar18);
        piVar23 = NULL;
        uVar22 = uStack_2fc;
        goto code_r0xfffffff00893dee8;
    }
    uStack_2d8 = uVar8;
    if (piStack_2b8 != NULL) {
        fcn.fffffff008958764(arg2);
        ppiStack_1a0 = &piStack_1a8;
        piStack_1a8 = NULL;
        iStack_198 = *0xfffffff0074bfcd8;
        uStack_190 = CONCAT71(uStack_190._1_7_,1);
        puStack_218 = &uStack_220;
        uStack_220 = 0;
        iStack_210 = *0xfffffff0074bfcd8;
        piStack_208 = piStack_208 & 0xffffffffffffff00;
        uStack_1f0 = 0;
        iStack_1e0 = *0xfffffff0074bfcd8;
        piStack_1d8 = piStack_1d8 & 0xffffffffffffff00;
        piStack_248 = NULL;
        iStack_238 = *0xfffffff0074bfcd8;
        uStack_230 = 0;
        iStack_268 = 0;
        iStack_258 = *0xfffffff0074bfcd8;
        uStack_250 = 0;
        uStack_288 = 0;
        iStack_278 = *0xfffffff0074bfcd8;
        uStack_270 = 0;
        uStack_2a8 = 0;
        iStack_298 = *0xfffffff0074bfcd8;
        uStack_290 = 0;
        uStack_2aa = uStack_2aa & 0xff;
        puStack_2a0 = &uStack_2a8;
        puStack_280 = &uStack_288;
        piStack_260 = &iStack_268;
        ppiStack_240 = &piStack_248;
        puStack_1e8 = &uStack_1f0;
        iVar13 = fcn.fffffff008942ee4(piVar3);
        if (iVar13 != 0) {
            uStack_150 = *0xfffffff007bcf978;
            uStack_148 = *0xfffffff0074bfce0;
            uStack_140 = 0xfffffff008940050;
            iStack_f0 = &uStack_2aa + 1;
            iStack_e0 = in_stack_00000000;
            iStack_d8 = in_stack_00000008;
            uStack_138 = 0xfffffff007bcb940;
            ppiStack_130 = &piStack_1a8;
            puStack_128 = &uStack_220;
            puStack_120 = &uStack_1f0;
            ppiStack_118 = &piStack_248;
            piStack_110 = &iStack_268;
            puStack_108 = &uStack_288;
            puStack_100 = &uStack_2a8;
            iStack_f8 = arg3;
            piStack_e8 = arg3_00;
            fcn.fffffff0089436b8(piVar3,&uStack_150);
            if (((*(puStack_280 + 3) & 1) != 0) && ((*(puStack_2a0 + 3) & 1) != 0)) {
                fcn.fffffff00893c038(arg3_00,in_stack_00000000,in_stack_00000008,-0xff8b45c93);
                *(ppiStack_1a0 + 3) = 0;
            }
        }
        piVar16 = ppiStack_1a0[3];
        fcn.fffffff0089582c4(&uStack_2a8,8);
        fcn.fffffff0089582c4(&uStack_288,8);
        fcn.fffffff0089582c4(&iStack_268,8);
        fcn.fffffff0089582c4(&piStack_248,8);
        fcn.fffffff0089582c4(&uStack_1f0,8);
        fcn.fffffff0089582c4(&uStack_220,8);
        fcn.fffffff0089582c4(&piStack_1a8,8);
        piStack_2f8 = piVar23;
        iStack_2e8 = arg2;
        if ((piVar16 & 1) == 0) goto code_r0xfffffff00893df14;
    }
    if (iVar10 == 0) {
        fcn.fffffff00893c038(arg3_00,in_stack_00000000,in_stack_00000008,-0xff8b460db);
code_r0xfffffff00893df14:
        piVar23 = NULL;
        iVar6 = 0;
        goto code_r0xfffffff00893df1c;
    }
    iVar7 = fcn.fffffff00893bef0(iVar10,uStack_2aa);
    if (iVar7 != 0) {
        *arg3 = *arg3 | 0x4000000;
        iVar6 = fcn.fffffff008934734(0);
        if (iVar6 != 0) {
            *arg3 = *arg3 | 0x2200;
        }
        fcn.fffffff00893bfd0(arg2,1,-0xff8b460ba);
        if ((piVar3 == NULL) || (piVar14 = fcn.fffffff008944374(piVar3), piVar14 == NULL)) {
            piVar14 = fcn.fffffff008958354(0);
        }
        iVar6 = fcn.fffffff00893ff44(arg3_00,arg2,*arg3,piVar14,-0xff8b460ae,in_stack_00000000,in_stack_00000008);
        (**(*piVar14 + 0x28))(piVar14);
        uVar12 = fcn.fffffff008942104(iVar10);
        if ((uVar12 & 1) == 0) {
            piVar23 = NULL;
            goto code_r0xfffffff00893df1c;
        }
        pcVar18 = "Bad things happened.";
        uStack_2fc = uVar22;
        goto code_r0xfffffff00893ded8;
    }
    piVar23 = fcn.fffffff0089581a4(0xfffffff007bcd088);
    uStack_228 = 0;
    uVar12 = fcn.fffffff008958704(arg2,&uStack_228);
    if (uVar12 == 0) {
        fcn.fffffff008957488();
        goto code_r0xfffffff00893ee64;
    }
    uStack_190 = 0;
    iStack_198 = 0;
    uStack_180 = 0;
    uStack_188 = 0;
    uStack_170 = 0;
    uStack_178 = 0;
    uStack_160 = 0;
    uStack_168 = 0;
    piStack_2f8 = piVar3;
    piStack_158 = piVar3;
    piStack_1a8 = arg3_00;
    ppiStack_1a0 = arg2;
    if ((*(arg3_00 + 0x81) & 1) == 0) {
        uVar5 = (**(*arg3_00 + 0x10))(arg3_00);
        *(arg3_00 + 0x81) = uVar5;
    }
    uVar24 = uStack_228 & 0xff;
    piStack_248 = NULL;
    uStack_98 = 0;
    uStack_a0 = 0;
    uStack_88 = 0;
    uStack_90 = 0;
    uStack_b8 = 0;
    uStack_c0 = 0;
    uStack_a8 = 0;
    uStack_b0 = 0;
    iStack_d8 = 0;
    iStack_e0 = 0;
    uStack_c8 = 0;
    uStack_d0 = 0;
    iStack_f8 = 0;
    puStack_100 = NULL;
    piStack_e8 = NULL;
    iStack_f0 = 0;
    ppiStack_118 = NULL;
    puStack_120 = NULL;
    puStack_108 = NULL;
    piStack_110 = NULL;
    uStack_138 = 0;
    uStack_140 = 0;
    puStack_128 = NULL;
    ppiStack_130 = NULL;
    uStack_148 = 0;
    uStack_150 = 0;
    uVar21 = fcn.fffffff0089586d4(arg2);
    uVar15 = fcn.fffffff008958794(arg2);
    iVar10 = fcn.fffffff0089586c4(uVar21,uVar15,0,0xfade0c02);
    if (iVar10 == 0) {
        pcVar18 = "AMFI: \'%s\' has no cd?\n";
    }
    else {
        uVar8 = (*(iVar10 + 4) & 0xff00ff00) >> 8 | (*(iVar10 + 4) & 0xff00ff) << 8;
        uVar8 = uVar8 >> 0x10 | uVar8 << 0x10;
        if (uVar8 < 8) {
            pcVar18 = "AMFI: \'%s\' has short cd (%u)?\n";
        }
        else {
            uVar21 = fcn.fffffff0089586d4(arg2);
            uVar15 = fcn.fffffff008958794(arg2);
            iVar13 = fcn.fffffff0089586c4(uVar21,uVar15,0x10000,0xfade0b01);
            uStack_310 = uVar12;
            if (iVar13 == 0) {
                pcVar18 = "AMFI: \'%s\' has no CMS blob?\n";
            }
            else {
                uVar9 = *(iVar13 + 4);
                uVar20 = (uVar9 & 0xff00ff00) >> 8 | (uVar9 & 0xff00ff) << 8;
                if ((uVar20 >> 0x10 | uVar20 << 0x10) < 8) {
                    pcVar18 = "AMFI: \'%s\' has short cms (%u), bailing out.\n";
                }
                else if (uVar9 == 0x8000000) {
                    piStack_248 = NULL;
                    uVar8 = fcn.fffffff008958744(arg2);
                    if ((uVar8 >> 1 & 1) != 0) {
                        fcn.fffffff008958174("AMFI: \'%s\' is adhoc signed.\n");
                        goto code_r0xfffffff00893e548;
                    }
                    pcVar18 = "AMFI: \'%s\' has zero length cms, but is not adhoc?\n";
                }
                else {
                    uStack_220 = 0;
                    iVar7 = fcn.fffffff008958264(0);
                    uStack_220 = CONCAT71(uStack_220._1_7_,iVar7 != 0);
                    uVar9 = (*(iVar13 + 4) & 0xff00ff00) >> 8 | (*(iVar13 + 4) & 0xff00ff) << 8;
                    iStack_210 = (uVar9 >> 0x10 | uVar9 << 0x10) - 8;
                    piStack_208 = NULL;
                    uStack_200 = 0;
                    puStack_218 = iVar13 + 8;
                    uVar9 = fcn.fffffff00894882c(&uStack_150,&uStack_220);
                    uVar9 = uVar9 >> 8 & 0xff;
                    if (uVar9 == 0) {
                        piVar16 = NULL;
code_r0xfffffff00893e3d4:
                        uVar8 = fcn.fffffff0089489f4(&uStack_150,iVar10,uVar8);
                        if ((uVar8 >> 8 & 0xff) == 0) {
                            iVar13 = fcn.fffffff008958754(arg2);
                            uVar8 = fcn.fffffff008948b50(&uStack_150,uVar12,uVar24,iVar13);
                            uVar8 = uVar8 >> 8 & 0xff;
                            if (uVar8 == 0) {
code_r0xfffffff00893e4b4:
                                fcn.fffffff008948cc0(&uStack_150,&piStack_248);
                                uStack_1f0 = 0;
                                puStack_1e8 = NULL;
                                uVar24 = fcn.fffffff008948c94(&uStack_150,&uStack_1f0);
                                if (((uVar24 & 0xff00) == 0) && (puStack_1e8 != NULL)) {
                                    uVar21 = fcn.fffffff0089585f4();
                                    fcn.fffffff0089585c4(uVar21,puStack_1e8,uStack_1f0,&uStack_178);
                                }
                                if (piVar16 != NULL) {
                                    bVar4 = true;
                                    goto code_r0xfffffff00893e524;
                                }
                                goto code_r0xfffffff00893e548;
                            }
                            if (uVar8 == 0x24) {
                                iVar13 = fcn.fffffff008958714(arg2);
                                if (iVar13 == iVar10) goto code_r0xfffffff00893e4b4;
                                pcVar18 = "AMFI: \'%s\' no hash agility and first code directory is not the best one\n";
                            }
                            else if (uVar8 == 0x28) {
                                pcVar18 = "AMFI: \'%s\' cmsBlobVerifyWithAgilityHash unsupported: unique=%#x\n";
                            }
                            else {
                                pcVar18 = 
                                "AMFI: \'%s\' cmsBlobVerifyWithAgilityHash failed: component=%#x error=%#x unique=%u\n";
                            }
                            fcn.fffffff008958174(pcVar18);
                        }
                        else {
                            fcn.fffffff008958174
                                      ("AMFI: \'%s\' cmsBlobVerify failed: component=%#x error=%#x unique=%u\n");
                            piStack_248 = NULL;
                        }
                        if (piVar16 == NULL) goto code_r0xfffffff00893e260;
                        bVar4 = false;
code_r0xfffffff00893e524:
                        (**(*piVar16 + 0x28))(piVar16);
                        if (!bVar4) goto code_r0xfffffff00893e260;
code_r0xfffffff00893e548:
                        piVar16 = piStack_248;
                        uVar8 = fcn.fffffff008958744(arg2);
                        if ((uVar8 >> 1 & 1) == 0) {
code_r0xfffffff00893e5a8:
                            bVar4 = false;
                        }
                        else {
                            iVar10 = fcn.fffffff008958714(arg2);
                            if (iVar10 == 0) {
                                fcn.fffffff008958174("AMFI: %s: unable to find code directory\n");
                                goto code_r0xfffffff00893e5a8;
                            }
                            uVar8 = (*(iVar10 + 8) & 0xff00ff00) >> 8 | (*(iVar10 + 8) & 0xff00ff) << 8;
                            if ((uVar8 >> 0x10 | uVar8 << 0x10) < 0x20600) {
                                bVar4 = false;
                            }
                            else {
                                bVar4 = *(iVar10 + 0x61) == '\x02';
                            }
                        }
                        if (((piVar16 & 0x18000001ff80) != 0) || (bVar4)) {
                            uVar17 = fcn.fffffff0089588d4();
                            uVar24 = piVar16 & 0x90;
                            if (((uVar17 & 1) != 0) || (uVar24 == 0)) {
                                iStack_318 = in_stack_00000008;
                                if (((((piVar16 & 0x80000001100) == 0) || ((*0xfffffff007bca811 & 1) == 0)) ||
                                    (uVar12 = fcn.fffffff008942104(uVar12), (uVar12 & 1) != 0)) ||
                                   ((iVar6 == 0x22 || (iVar6 == 0)))) {
                                    if (((*0xfffffff007bca811 & 1) == 0) ||
                                       (((iVar7 = fcn.fffffff008958264(0), (piVar16 & 0x100000002200) == 0 ||
                                         (iVar7 == 0)) || ((iVar6 == 0 || (iVar6 == 0x22)))))) {
                                        if (bVar4) {
                                            iVar10 = fcn.fffffff008958714(arg2);
                                            uVar21 = fcn.fffffff0089588c4();
                                            if (iVar10 == 0) {
                                                fcn.fffffff00895744c();
code_r0xfffffff00893eec4:
                                                uVar21 = fcn.fffffff008958c34
                                                                   ("\"amfid is broken. (%s) (%d) (%d) (%d)\" @%s:%d");
                                                return uVar21;
                                            }
                                            if ((*(iVar10 + 0x57) & 1) != 0) {
                                                fcn.fffffff008958764(arg2);
                                                pcVar18 = "AMFI: %s: OOP-JIT signature represents main binary\n";
code_r0xfffffff00893e8d4:
                                                fcn.fffffff008958174(pcVar18);
                                                bVar2 = false;
                                                bVar4 = false;
                                                iStack_2e8 = arg2;
                                                goto code_r0xfffffff00893e62c;
                                            }
                                            if ((piVar3 != NULL) &&
                                               (iVar13 = fcn.fffffff008942ee4(piVar3), iVar13 != 0)) {
                                                fcn.fffffff008958764(arg2);
                                                pcVar18 = "AMFI: %s: OOP-JIT signature has entitlements\n";
                                                goto code_r0xfffffff00893e8d4;
                                            }
                                            uVar8 = (*(iVar10 + 0x68) & 0xff00ff00) >> 8 |
                                                    (*(iVar10 + 0x68) & 0xff00ff) << 8;
                                            if ((uVar8 >> 0x10 | uVar8 << 0x10) < 0x14) {
                                                fcn.fffffff008958764(arg2);
                                                pcVar18 = "AMFI: %s linkage size is too small\n";
                                                goto code_r0xfffffff00893e8d4;
                                            }
                                            uVar1 = *(iVar10 + 0x62);
                                            if (uVar1 == 0) {
                                                fcn.fffffff008958764(arg2);
                                                pcVar18 = "AMFI: %s: invalid OOP-JIT type\n";
                                            }
                                            else {
                                                uVar8 = uVar1 >> 8 | (uVar1 & 0xff00ff) << 8;
                                                if (uVar8 < 3) {
                                                    iVar10 = fcn.fffffff008958864(uVar21);
                                                    if (iVar10 == 0) {
                                                        fcn.fffffff008958174
                                                                  (
                                                            "AMFI: %s: unable to obtain cs_blob on current_proc\n");
                                                    }
                                                    else {
                                                        uVar21 = fcn.fffffff0089587e4(iVar10);
                                                        iVar13 = fcn.fffffff008958384(uVar21,*0xfffffff007bcbbd8);
                                                        if (iVar13 == 0) {
                                                            fcn.fffffff008958764(iVar10);
                                                            pcVar18 = "AMFI: %s: %s has no entitlements\n";
                                                        }
                                                        else {
                                                            iVar6 = fcn.fffffff008943c40
                                                                              (iVar13,-0xff8b45d98,
                                                                               *(uVar8 * 8 + -0xff8434690));
                                                            if (iVar6 == 0) {
                                                                fcn.fffffff008958764(iVar10);
                                                                pcVar18 = 
                                                            "AMFI: %s: unsatisfied entitlements for loading OOP-JIT signature\n"
                                                            ;
                                                            }
                                                            else {
                                                                iVar6 = fcn.fffffff008937804();
                                                                if (iVar6 == 0) {
                                                                    fcn.fffffff008958174
                                                                              ("OOP-JIT Signed Fast Path -> %s\n");
                                                                    *arg3 = *arg3 | 0x4000;
                                                                    *arg4 = 9;
                                                                    fcn.fffffff00893bfd0(arg2,9,-0xff8b45a8a);
                                                                    bVar2 = true;
                                                                    bVar4 = true;
                                                                    goto code_r0xfffffff00893ecc4;
                                                                }
                                                                pcVar18 = 
                                                            "AMFI: %s: oop-jit is disabled by system configuration\n";
                                                            }
                                                        }
                                                        fcn.fffffff008958174(pcVar18);
                                                    }
                                                    bVar4 = false;
                                                    bVar2 = false;
                                                    goto code_r0xfffffff00893ecc4;
                                                }
                                                fcn.fffffff008958764(arg2);
                                                pcVar18 = "AMFI: %s: unsupported OOP-JIT type: %u\n";
                                            }
                                            fcn.fffffff008958174(pcVar18);
                                            bVar2 = false;
                                            bVar4 = false;
code_r0xfffffff00893ecc4:
                                            iStack_2e8 = arg2;
                                            goto code_r0xfffffff00893e62c;
                                        }
                                        *arg3 = *arg3 & 0xf3ffbfff;
                                        if ((*0xfffffff007bca811 & 1) == 0) {
                                            fcn.fffffff008958174
                                                      (
                                                      "AMFI: \'%s\' does not pass kernel-side policy for this model, bailing out.\n"
                                                      );
                                            bVar4 = false;
                                            goto code_r0xfffffff00893e628;
                                        }
                                        piStack_248 = NULL;
                                        iVar6 = fcn.fffffff00893abac(&piStack_248);
                                        if (iVar6 != 0) {
                                            pcVar18 = "StaticPlatformPolicy<%d>: no registered daemon port\n";
                                            goto code_r0xfffffff00893e614;
                                        }
                                        puStack_218 = NULL;
                                        uStack_220 = 0;
                                        piStack_208 = NULL;
                                        iStack_210 = 0;
                                        uStack_1b8 = 0;
                                        uStack_1b0 = 0;
                                        iStack_1bc = 0;
                                        puStack_128 = NULL;
                                        ppiStack_130 = NULL;
                                        ppiStack_118 = NULL;
                                        puStack_120 = NULL;
                                        uStack_148 = 0;
                                        uStack_150 = 0;
                                        uStack_138 = 0;
                                        uStack_140 = 0;
                                        uStack_1be = 0;
                                        uVar21 = fcn.fffffff008958704(arg2,&uStack_1be);
                                        if ((*(arg3_00 + 0x81) & 1) == 0) {
                                            uVar5 = (**(*arg3_00 + 0x10))(arg3_00);
                                            *(arg3_00 + 0x81) = uVar5;
                                        }
                                        piVar3 = piStack_248;
                                        iStack_268 = 0;
                                        uStack_288 = 0;
                                        uStack_2a8 = 0;
                                        uStack_1c8 = 0;
                                        uVar15 = fcn.fffffff0089586f4(arg2);
                                        iVar6 = fcn.fffffff008944994
                                                          (piVar3,piVar14,uVar15,arg5 & 0xffffffff,0,1,4,
                                                           &iStack_1bc,&uStack_1b8 + 4,&uStack_1b8,arg4,
                                                           &uStack_1b0 + 4,&uStack_1b0,piVar23,&uStack_150,
                                                           &uStack_1be + 1,&iStack_268,&uStack_288,&uStack_2a8,
                                                           &uStack_1c8,&uStack_220);
                                        if (0x13 < **0xfffffff007bcf9f0) {
                                            fcn.fffffff008958174
                                                      (
                                                      "callout out to amfid for %s %s, return %d valid: %d isApple: %d\n"
                                                      );
                                        }
                                        uStack_310 = uVar24;
                                        piStack_2f8 = piVar23;
                                        if (iVar6 == -0x134) {
                                            pcVar18 = "StaticPlatformPolicy<%d>: verify_code_directory server is dead\n"
                                            ;
                                        }
                                        else {
                                            if (iVar6 == 0x10004005) {
                                                fcn.fffffff008958174
                                                          (
                                                          "StaticPlatformPolicy<%d>: verify_code_directory interrupted\n"
                                                          );
                                                iStack_2e8 = arg2;
                                                goto code_r0xfffffff00893ebe4;
                                            }
                                            if (iVar6 == 0) {
                                                puStack_1e8 = puStack_218;
                                                uStack_1f0 = uStack_220;
                                                piStack_1d8 = piStack_208;
                                                iStack_1e0 = iStack_210;
                                                iVar6 = fcn.fffffff00893abfc(&uStack_1f0);
                                                if (iVar6 != 0) {
                                                    if (uStack_1b8._4_4_ != 1) goto code_r0xfffffff00893e61c;
                                                    if (uStack_1be._1_1_ == 0x14) {
                                                        if (0x13 < uStack_1be) {
                                                            uVar19 = 0x14;
                                                            goto code_r0xfffffff00893ece8;
                                                        }
                                                    }
                                                    else {
                                                        uVar19 = uStack_1be;
                                                        if (uStack_1be == uStack_1be._1_1_) {
code_r0xfffffff00893ece8:
                                                            iVar6 = fcn.fffffff008958ba4(uVar21,&uStack_150,uVar19);
                                                            iStack_2e8 = arg2;
                                                            if (iVar6 == 0) goto code_r0xfffffff00893ed94;
                                                        }
                                                    }
                                                    fcn.fffffff008958174("%s: Possible race detected. Rejecting.\n");
                                                    fcn.fffffff008958174
                                                              ("StaticPlatformPolicy<%d>: Unable to match identity\n");
                                                    *arg3 = *arg3 & 0xfffffffe;
                                                    fcn.fffffff0089428dc(piStack_158);
                                                    goto code_r0xfffffff00893e61c;
                                                }
                                                pcVar18 = 
                                                "StaticPlatformPolicy<%d>: unable to verify audit token came from amfid\n"
                                                ;
                                            }
                                            else {
                                                pcVar18 = 
                                                "StaticPlatformPolicy<%d>: verify_code_directory returned 0x%x\n";
                                            }
                                        }
                                        fcn.fffffff008958174(pcVar18);
                                        goto code_r0xfffffff00893e61c;
                                    }
                                    fcn.fffffff008958174("QA Hierarchy used -> %s\n");
                                    *arg3 = *arg3 | 0x4000;
                                    pcVar18 = "qa-path-appstore";
                                }
                                else {
                                    fcn.fffffff008958174("App Store Fast Path -> %s\n");
                                    *arg3 = *arg3 | 0x4000;
                                    pcVar18 = "fast-path-appstore";
                                }
                                fcn.fffffff00893bfd0(arg2,4,pcVar18);
                                bVar4 = true;
                                goto code_r0xfffffff00893e628;
                            }
                            pcVar18 = 
                            "AMFI: \'%s\': has developer signature (%#llx) but developer mode is not enabled\n";
code_r0xfffffff00893e614:
                            fcn.fffffff008958174(pcVar18);
                        }
                        else {
                            fcn.fffffff008958174
                                      (
                                      "AMFI: \'%s\': unsuitable CT policy %#llx for this platform/device, rejecting signature.\n"
                                      );
                            uVar24 = piVar16 & 0x10;
                        }
code_r0xfffffff00893e61c:
                        bVar4 = false;
code_r0xfffffff00893e628:
                        bVar2 = true;
                        iStack_2e8 = arg2;
                        goto code_r0xfffffff00893e62c;
                    }
                    if (uVar9 == 0x3f) {
                        piVar16 = fcn.fffffff0089413d0(piVar14,&uStack_220);
                        iStack_318 = iVar13;
                        if (piVar16 != NULL) {
                            iVar7 = fcn.fffffff008958264(0);
                            uStack_200 = *(piVar16 + 0xd);
                            uVar9 = (*(iVar13 + 4) & 0xff00ff00) >> 8 | (*(iVar13 + 4) & 0xff00ff) << 8;
                            iStack_210 = (uVar9 >> 0x10 | uVar9 << 0x10) - 8;
                            uStack_220 = CONCAT71(uStack_220._1_7_,iVar7 != 0);
                            piStack_208 = piVar16 + 5;
                            puStack_218 = iVar13 + 8;
                            uVar9 = fcn.fffffff00894882c(&uStack_150,&uStack_220);
                            if ((uVar9 >> 8 & 0xff) == 0) goto code_r0xfffffff00893e3d4;
                            fcn.fffffff008958174
                                      ("AMFI: \'%s\' cmsBlobInit failed: component=%#x error=%#x unique=%#x\n");
                            bVar4 = false;
                            piStack_248 = NULL;
                            goto code_r0xfffffff00893e524;
                        }
                        pcVar18 = "AMFI: \'%s\' couldn\'t fetch certs\n";
                    }
                    else {
                        pcVar18 = "AMFI: \'%s\' cmsBlobInit failed: component=%#x error=%#x unique=%#x\n";
                    }
                }
            }
        }
    }
    fcn.fffffff008958174(pcVar18);
code_r0xfffffff00893e260:
    fcn.fffffff008958174("AMFI: \'%s\': Unrecoverable CT signature issue, bailing out.\n");
    fcn.fffffff008958174("AMFI: code signature validation failed.\n");
    iStack_2e8 = arg2;
code_r0xfffffff00893e28c:
    fcn.fffffff00893bfd0(iStack_2e8,10,-0xff8b4604d);
    arg2 = iStack_2e8;
    uVar8 = uStack_2d8;
code_r0xfffffff00893dee8:
    iVar6 = 0;
code_r0xfffffff00893df1c:
    do {
        piVar16 = piStack_2b8;
        if (piStack_2b8 == NULL) {
code_r0xfffffff00893df7c:
            if (iVar6 == 0) {
                uVar21 = 1;
            }
            else {
                uVar9 = *arg3;
                if ((uVar9 >> 0x1a & 1) == 0) {
                    iVar6 = fcn.fffffff008936eac();
                    if (iVar6 == 0) {
                        fcn.fffffff00893c038(arg3_00,in_stack_00000000,iVar11,-0xff8b45fc9);
                        uVar21 = 1;
                        goto joined_r0xfffffff00893e1c4;
                    }
                    uVar9 = *arg3;
                    uVar20 = uVar9 >> 0x1a & 1;
                }
                else {
                    uVar20 = 1;
                }
                *arg3 = uVar9 | 0x20000000;
                if ((piVar16 != NULL) && (uVar20 != 0)) {
                    fcn.fffffff008942db0(piVar16);
                }
                fcn.fffffff008932974(piVar16,arg2);
                uVar21 = 0;
            }
joined_r0xfffffff00893e1c4:
            if (piVar16 != NULL) goto code_r0xfffffff00893e018;
        }
        else {
            iVar7 = fcn.fffffff008943214(piStack_2b8,-0xff8b49d75);
            if ((iVar7 == 0) || (uVar12 = fcn.fffffff008943214(piVar16,-0xff8b453ac), (uVar12 & 1) != 0)) {
                iVar7 = fcn.fffffff008943214(piVar16,-0xff8b4a017);
                if ((iVar7 == 0) || (iVar7 = fcn.fffffff0089587c4(arg2), iVar7 == 1)) goto code_r0xfffffff00893df7c;
                pcVar18 = "%s disallowed on non-platform process";
            }
            else {
                pcVar18 = "%s disallowed without %s";
            }
            fcn.fffffff00893c038(arg3_00,in_stack_00000000,iVar11,pcVar18);
            uVar21 = 1;
code_r0xfffffff00893e018:
            (**(*piVar16 + 0x28))(piVar16);
        }
        if (piVar23 != NULL) {
            fcn.fffffff008958114(0xfffffff007bcd0c8,piVar23);
        }
        if ((uVar8 & (*arg3 ^ 0xffffffff) & 0xfffff7fe) == 0) {
            (**(*arg3_00 + 8))(arg3_00);
            if (**0xfffffff007bcf9e0 == iStack_78) {
                return uVar21;
            }
        }
        else {
            fcn.fffffff008957498();
        }
        fcn.fffffff008958514();
code_r0xfffffff00893ed94:
        if (uStack_1b0._4_4_ != 0) goto code_r0xfffffff00893eec4;
        if (uStack_1b0 != 0) {
            *arg3 = *arg3 | 0x40000000;
        }
        if (uStack_1b8 != 0) {
            *arg3 = *arg3 & 0xfffff7ff;
        }
        if (iStack_1bc != 0) {
            *arg3 = *arg3 | 0x4200;
        }
        if ((piVar16 & 0x10000001ee80) == 0) {
code_r0xfffffff00893ee20:
            iVar6 = fcn.fffffff0089587c4(iStack_2e8);
            if (iVar6 == 0) {
                uVar8 = 3;
                if ((piVar16 & 0x20) != 0) {
                    uVar8 = 6;
                }
                uVar12 = uVar8;
                if ((piVar16 & 0xb0) == 0) {
                    if ((piVar16 >> 0x10 & 1) == 0) {
                        if ((piVar16 >> 10 & 1) == 0) {
                            if ((piVar16 & 0xc000) == 0) {
                                uVar8 = 10;
                                if ((piVar16 & 0x180000003300) != 0) {
                                    uVar8 = 4;
                                }
                                uVar12 = uVar8;
                            }
                            else {
                                uVar12 = 2;
                            }
                        }
                        else {
                            fcn.fffffff008958174("%s: Signature meets iphone VPN Prod policy\n");
                            uVar12 = 10;
                        }
                    }
                    else {
code_r0xfffffff00893ee64:
                        uVar12 = 5;
                    }
                }
                fcn.fffffff00893bfd0(iStack_2e8,uVar12,-0xff8b45551);
            }
            bVar4 = true;
        }
        else {
            iStack_198 = iStack_268;
            uStack_190 = uStack_288;
            uStack_188 = uStack_2a8;
            uStack_180 = uStack_1c8;
            iVar6 = fcn.fffffff00893b95c(&piStack_1a8);
            if (iVar6 == 0) goto code_r0xfffffff00893ee20;
code_r0xfffffff00893ebe4:
            bVar4 = false;
        }
        bVar2 = true;
        uVar24 = uStack_310;
        piVar23 = piStack_2f8;
        iVar11 = iStack_318;
code_r0xfffffff00893e62c:
        uVar19 = uVar22 ^ 1;
        if (uVar24 == 0) {
            uVar19 = 1;
        }
        if ((uVar19 & 1) == 0) {
            if (*0xfffffff00a4df938 == 0) {
                iVar10 = 0;
            }
            else {
                iVar10 = *0xfffffff00a4df938;
            }
            fcn.fffffff0089333a4(iVar10);
        }
        arg2 = iStack_2e8;
        uVar8 = uStack_2d8;
        if (!bVar4) break;
        if ((piStack_2b8 == NULL) || (piVar14 = fcn.fffffff008944374(piStack_2b8), piVar14 == NULL)) {
            piVar14 = fcn.fffffff008958354(0);
        }
        iVar6 = fcn.fffffff00893ff44(arg3_00,iStack_2e8,*arg3,piVar14,-0xff8b4607e,in_stack_00000000,iVar11);
        (**(*piVar14 + 0x28))(piVar14);
    } while( true );
    fcn.fffffff008958174("AMFI: code signature validation failed.\n");
    if (bVar2) goto code_r0xfffffff00893e28c;
    fcn.fffffff008958174("AMFI: OOP-JIT validation was unsuccessful.\n");
    fcn.fffffff00893c038(arg3_00,in_stack_00000000,iVar11,-0xff8b46017);
    goto code_r0xfffffff00893dee8;
}

