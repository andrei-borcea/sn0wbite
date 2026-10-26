
ulong fcn.fffffff009adda20(int64_t arg1)

{
    uint64_t arg1_00;
    bool bVar1;
    int iVar2;
    int64_t iVar3;
    int64_t *piVar4;
    int64_t arg1_01;
    uint64_t uVar5;
    uint16_t uVar6;
    uint64_t uVar7;
    uint64_t uVar8;
    int64_t iVar9;
    uint64_t *arg1_02;
    int64_t arg5;
    uint16_t uVar10;
    int64_t iVar11;
    ulong uVar12;
    uint in_wzr;
    uchar in_q7 [16];
    int64_t in_stack_fffffffffffffea0;
    int32_t in_stack_fffffffffffffea8;
    ulong uStack_100;
    ulong uStack_f8;
    int64_t iStack_f0;
    int64_t iStack_e8;
    ulong uStack_e0;
    uint64_t uStack_d8;
    ulong uStack_d0;
    uint64_t uStack_c8;
    ulong uStack_98;
    ulong uStack_90;
    ulong uStack_88;
    ulong uStack_80;
    ulong uStack_78;
    
    arg1_00 = tpidr_el1;
    iVar3 = fcn.fffffff009a43ffc(arg1_00);
    piVar4 = iVar3 + 0x28;
    iVar9 = *piVar4;
    if ((((iVar9 == 0) || (iVar9 == 0)) || (iVar9 = *(iVar9 + 0x58), iVar9 == 0)) ||
       ((iVar9 == 0 || ((*(iVar9 + 0x72) & 1) != 0)))) {
        bVar1 = true;
    }
    else {
        bVar1 = false;
    }
    uVar5 = *(arg1_00 + 0x4d8);
    if (*0xfffffff00a42dc58 != 0) {
        fcn.fffffff0099d6bb8(0xfffffff00a42dc50,arg1_00,0);
        uVar5 = *0xfffffff00a42dc58;
    }
    *0xfffffff00a42dc58 = uVar5;
    uVar12 = *0xfffffff0070099a0;
    arg1_02 = arg1 + 0x80;
    uVar5 = *arg1_02;
    while (uVar5 != 0) {
        uStack_100 = 0xfffffff00a40bc30;
        uStack_f8 = uVar12;
        iStack_f0 = -0xff6622010;
        iStack_e8 = -0xff8698828;
        uStack_e0 = 0xfffffff00a42dc50;
        uStack_98 = 0xfffffff00a40bc30;
        uStack_90 = uVar12;
        uStack_88 = 0xfffffff0099de020;
        uStack_80 = 0xfffffff0079677f8;
        uStack_78 = 0xfffffff00a42dc50;
        fcn.fffffff0099dd810(arg1_02,uVar5,0xc0000000,0,&uStack_100,&uStack_98);
        uVar5 = *arg1_02;
    }
    *arg1_02 = arg1_00;
    uVar5 = *0xfffffff00a42dc58;
    iVar2 = *0xfffffff00a42dc58;
    if (*0xfffffff00a42dc58 == *(arg1_00 + 0x4d8)) {
        *0xfffffff00a42dc58 = CONCAT44(*0xfffffff00a42dc5c,in_wzr);
    }
    if (iVar2 == *(arg1_00 + 0x4d8)) {
        iVar2 = *(arg1 + 0x98);
        iVar9 = *0xfffffff00a38ffe0;
    }
    else {
        fcn.fffffff0099d76ec(0xfffffff00a42dc50,arg1_00,uVar5 & 0xffffffff);
        iVar2 = *(arg1 + 0x98);
        iVar9 = *0xfffffff00a38ffe0;
    }
    *0xfffffff00a38ffe0 = iVar9;
    if (iVar2 != 0) {
        if ((*(iVar3 + 0x41c) & 1) != 0) {
            if (*0xfffffff00a42dc58 == 0) {
                *0xfffffff00a42dc58 = *(arg1_00 + 0x4d8);
            }
            else {
                fcn.fffffff0099d6bb8(0xfffffff00a42dc50,arg1_00,0);
            }
            *arg1_02 = 0;
            fcn.fffffff0099de948(arg1_02,0,2,0,0);
            uVar5 = *0xfffffff00a42dc58;
            iVar2 = *0xfffffff00a42dc58;
            if (*0xfffffff00a42dc58 == *(arg1_00 + 0x4d8)) {
                *0xfffffff00a42dc58 = CONCAT44(*0xfffffff00a42dc5c,in_wzr);
            }
            if (iVar2 == *(arg1_00 + 0x4d8)) {
                return 0;
            }
            fcn.fffffff0099d76ec(0xfffffff00a42dc50,arg1_00,uVar5 & 0xffffffff);
            return 0;
        }
        if ((*(*(*piVar4 + 0x58) + 0x73) & 1) == 0) {
            uVar10 = 0;
        }
        else {
            uVar10 = *(iVar3 + 0x694) >> 1 & 1;
        }
        if (*(arg1 + 0x20) == 0x100000c) {
            uVar6 = (*0xfffffff0079ab440 != 0 && *0xfffffff0079ab444 != 0) && *0xfffffff0079ab4d4 != 0;
        }
        else {
            uVar6 = 0;
        }
        if (bVar1) {
            arg5 = 0;
        }
        else {
            arg5 = *(iVar3 + 0x350);
        }
        if (*(**(arg1 + 0xa0) + 0x58) != arg5 || (uVar6 & (uVar10 ^ 1)) != 0) {
            if (2 < *0xfffffff00a38ffec) {
                if ((*0xfffffff0079bbe58 & 1) == 0) {
                    if ((*0xfffffff00a395de4 & 1) == 0) {
                        uVar7 = arg1_00 | 0xffffff8000000000;
                        uVar5 = uVar7;
                        if ((arg1_00 & 0x80000000000000) == 0) {
                            uVar5 = arg1_00 & 0x7ffffffffff;
                        }
                        if (((uVar5 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar5) ||
                            (*0xfffffff007a0d830 <= uVar5 && uVar5 < *0xfffffff007a0d838)) ||
                           ((*0xfffffff007a0d858 <= uVar5 && (uVar5 < *0xfffffff007a0d860)))) {
                            fcn.fffffff009b3bf1c(uVar5);
                        }
                        else if ((((arg1_00 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar7)) &&
                                (uVar7 < 0xfffffffc00000000)) {
                            fcn.fffffff00998128c(uVar7);
                        }
                        fcn.fffffff00a0531b8(-0xff8ff8000,iVar9,0,-0xff8f45674);
                    }
                    else {
                        uVar5 = arg1_00 | 0xffffff8000000000;
                        if ((arg1_00 & 0x80000000000000) == 0) {
                            uVar5 = arg1_00 & 0x7ffffffffff;
                        }
                        if (((*0xfffffff007a0d818 <= uVar5 || uVar5 < *0xfffffff007a0d810) &&
                            (uVar5 < *0xfffffff007a0d830 || *0xfffffff007a0d838 <= uVar5)) &&
                           ((uVar5 < *0xfffffff007a0d858 || (*0xfffffff007a0d860 <= uVar5))))
                        goto joined_r0xfffffff009ade144;
                    }
                }
                else {
                    uVar5 = fcn.fffffff009ad959c(arg1_00);
                    if ((uVar5 & 1) == 0) {
joined_r0xfffffff009ade144:
                        if ((((arg1_00 >> 0x37 & 1) != 0) &&
                            (uVar5 = arg1_00 | 0xffffff8000000000, 0xffffffdbffffffff < uVar5)) &&
                           (uVar5 < 0xfffffffc00000000)) {
                            fcn.fffffff00998128c(uVar5);
                        }
                    }
                }
            }
            if (*(arg1 + 0x98) != 0) {
                uVar5 = 0;
                while( true ) {
                    iVar9 = *(*(arg1 + 0xa0) + uVar5 * 8);
                    iVar11 = *(iVar9 + 0x48);
                    iVar9 = fcn.fffffff009ae5214(*(iVar11 + 0x30),*(iVar11 + 0x20),iVar11,iVar9,arg5);
                    if (iVar9 == 0) break;
                    arg1_01 = fcn.fffffff009ad9b3c(arg1);
                    uStack_d8 = 0;
                    uStack_e0 = 0;
                    uStack_c8 = 0;
                    uStack_d0 = 0;
                    uStack_f8 = 0;
                    uStack_100 = 0;
                    iStack_e8 = 0;
                    iStack_f0 = 0;
                    iVar2 = fcn.fffffff009ade150(arg1_01,ZEXT816(0),&uStack_100);
                    if (iVar2 == 0) {
                        if (iStack_e8 - iStack_f0 == *(iVar11 + 0x28) - *(iVar11 + 0x20)) {
                            uVar7 = 0x20000004081;
                            if ((uStack_c8 & 0x600) != 0 || *(arg1 + 0x20) == 0x1000007) {
                                uVar7 = 0x20000004001;
                            }
                            uVar8 = (uStack_c8 & 0x80) << 5;
                            if (uVar10 == 0) {
                                uVar8 = 0;
                            }
                            uStack_98 = *(iVar11 + 0x18);
                            iVar11 = *piVar4;
                            if (iVar11 == 0) {
                                iVar11 = 0;
                            }
                            uVar12 = fcn.fffffff009a8bfb8
                                               (iVar11,&uStack_98,iStack_e8 - iStack_f0,0,uVar7 | uVar8,iVar9,0,1
                                                ,in_q7,iVar11,0,0,in_stack_fffffffffffffea0,
                                                in_stack_fffffffffffffea8);
                            bVar1 = uVar12 == 0;
                        }
                        else {
                            bVar1 = false;
                            uVar12 = 5;
                        }
                    }
                    else {
                        bVar1 = false;
                        uVar12 = 1;
                    }
                    uVar7 = uStack_d8 & 0xfffffffffffffffc;
                    if ((uStack_d8 & 3) != 0) {
                        uVar7 = 0xfffffff00a42d1c0;
                    }
                    if (uVar7 != 0) {
                        fcn.fffffff009aaa4d4(uVar7);
                    }
                    (**(*(iVar9 + 8) + 8))(iVar9);
                    if ((!bVar1) || (uVar5 = uVar5 + 1, *(arg1 + 0x98) <= uVar5)) goto code_r0xfffffff009ade078;
                }
                uVar12 = 5;
                goto code_r0xfffffff009ade078;
            }
        }
    }
    uVar12 = 0;
code_r0xfffffff009ade078:
    *(iVar3 + 0x41c) = 1;
    if (*0xfffffff00a42dc58 == 0) {
        *0xfffffff00a42dc58 = *(arg1_00 + 0x4d8);
    }
    else {
        fcn.fffffff0099d6bb8(0xfffffff00a42dc50,arg1_00,0);
    }
    *arg1_02 = 0;
    fcn.fffffff0099de948(arg1_02,0,2,0,0);
    uVar5 = *0xfffffff00a42dc58;
    iVar2 = *0xfffffff00a42dc58;
    if (*0xfffffff00a42dc58 == *(arg1_00 + 0x4d8)) {
        *0xfffffff00a42dc58 = CONCAT44(*0xfffffff00a42dc5c,in_wzr);
    }
    if (iVar2 != *(arg1_00 + 0x4d8)) {
        fcn.fffffff0099d76ec(0xfffffff00a42dc50,arg1_00,uVar5 & 0xffffffff);
    }
    return uVar12;
}

