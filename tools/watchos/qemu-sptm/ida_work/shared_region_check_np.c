
ulong fcn.fffffff009f77788(int64_t arg1,int64_t arg2)

{
    int64_t iVar1;
    int iVar2;
    uint64_t uVar3;
    uint64_t uVar4;
    uint64_t uVar5;
    int64_t in_x2;
    uint64_t uVar6;
    uint64_t uVar7;
    ulong uVar8;
    uint64_t uStack_70;
    uint64_t uStack_68;
    int64_t iStack_60;
    int64_t iStack_58;
    
    iStack_58 = 0;
    uVar5 = tpidr_el1;
    uVar3 = fcn.fffffff009a43ffc(uVar5);
    iVar1 = *0xfffffff00a38ffe0;
    if (2 < *0xfffffff00a38ffec) {
        if ((*0xfffffff0079bbe58 & 1) == 0) {
            if ((*0xfffffff00a395de4 & 1) == 0) {
                uVar7 = uVar5 | 0xffffff8000000000;
                uVar4 = uVar7;
                if ((uVar5 & 0x80000000000000) == 0) {
                    uVar4 = uVar5 & 0x7ffffffffff;
                }
                if (((uVar4 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar4) ||
                    (*0xfffffff007a0d830 <= uVar4 && uVar4 < *0xfffffff007a0d838)) ||
                   ((*0xfffffff007a0d858 <= uVar4 && (uVar4 < *0xfffffff007a0d860)))) {
                    fcn.fffffff009b3bf1c(uVar4);
                }
                else if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar7)) && (uVar7 < 0xfffffffc00000000)) {
                    fcn.fffffff00998128c(uVar7);
                }
                in_x2 = 0;
                fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18cfb);
            }
            else {
                uVar4 = uVar5 | 0xffffff8000000000;
                if ((uVar5 & 0x80000000000000) == 0) {
                    uVar4 = uVar5 & 0x7ffffffffff;
                }
                if (((*0xfffffff007a0d818 <= uVar4 || uVar4 < *0xfffffff007a0d810) &&
                    (uVar4 < *0xfffffff007a0d830 || *0xfffffff007a0d838 <= uVar4)) &&
                   ((uVar4 < *0xfffffff007a0d858 || (*0xfffffff007a0d860 <= uVar4)))) goto joined_r0xfffffff009f78250;
            }
        }
        else {
            uVar4 = fcn.fffffff009f771e4(uVar5);
            if ((uVar4 & 1) == 0) {
joined_r0xfffffff009f78250:
                if ((((uVar5 >> 0x37 & 1) != 0) && (uVar4 = uVar5 | 0xffffff8000000000, 0xffffffdbffffffff < uVar4)) &&
                   (uVar4 < 0xfffffffc00000000)) {
                    fcn.fffffff00998128c(uVar4);
                }
            }
        }
    }
    iVar1 = 0xffffffff;
    if ((*(uVar3 + 0x6b0) & 4) != 0) {
        iVar1 = -1;
    }
    if (*arg2 == iVar1) {
        *(arg1 + 0x6cc) = 1;
        return 0;
    }
    uVar4 = fcn.fffffff009ad9608(uVar3);
    if (uVar4 == 0) {
        uVar8 = 0x16;
        goto code_r0xfffffff009f77e10;
    }
    if (*arg2 == 0) {
        fcn.fffffff009ae2d58(uVar3,uVar4,in_x2);
        fcn.fffffff009adb4e4(uVar3);
        uVar8 = 0;
    }
    else {
        iVar2 = fcn.fffffff009adb8d0(uVar4,&iStack_58);
        iVar1 = *0xfffffff00a38ffe0;
        if (iVar2 == 0) {
            iVar2 = fcn.fffffff009adc2e0(uVar3,uVar4,iStack_58);
            iVar1 = *0xfffffff00a38ffe0;
            if (iVar2 == 0) {
                iVar2 = fcn.fffffff009adda20(uVar4);
                iVar1 = *0xfffffff00a38ffe0;
                if (iVar2 == 0) {
                    uVar8 = fcn.fffffff009b309b4(&iStack_58,*arg2,8);
                    iVar1 = *0xfffffff00a38ffe0;
                    if ((uVar8 != 0) && (0 < *0xfffffff00a38ffec)) {
                        if (((*0xfffffff0079bbe58 & 1) == 0) && ((*0xfffffff00a395de4 & 1) == 0)) {
                            iVar2 = fcn.fffffff009f771e4(uVar5);
                            uVar3 = uVar5 | 0xffffff8000000000;
                            if (iVar2 == 0) {
                                if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar3)) &&
                                   (uVar3 < 0xfffffffc00000000)) {
                                    fcn.fffffff00998128c(uVar3);
                                }
                            }
                            else {
                                uVar7 = uVar5 & 0x7ffffffffff;
                                if ((uVar5 & 0x80000000000000) != 0) {
                                    uVar7 = uVar3;
                                }
                                fcn.fffffff009b3bf1c(uVar7);
                            }
                            fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18bba);
                        }
                        else {
                            uVar3 = fcn.fffffff009f771e4(uVar5);
                            if (((((uVar3 & 1) == 0) && ((uVar5 >> 0x37 & 1) != 0)) &&
                                (uVar3 = uVar5 | 0xffffff8000000000, 0xffffffdbffffffff < uVar3)) &&
                               (uVar3 < 0xfffffffc00000000)) {
                                fcn.fffffff00998128c(uVar3);
                            }
                        }
                    }
                    goto code_r0xfffffff009f77e08;
                }
                if (0 < *0xfffffff00a38ffec) {
                    if (((*0xfffffff0079bbe58 & 1) != 0) || ((*0xfffffff00a395de4 & 1) != 0))
                    goto code_r0xfffffff009f77b88;
                    iVar2 = fcn.fffffff009f771e4(uVar5);
                    uVar7 = uVar5 | 0xffffff8000000000;
                    if (iVar2 == 0) {
                        if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar7)) && (uVar7 < 0xfffffffc00000000)
                           ) {
                            fcn.fffffff00998128c(uVar7);
                        }
                    }
                    else {
                        uVar6 = uVar5 & 0x7ffffffffff;
                        if ((uVar5 & 0x80000000000000) != 0) {
                            uVar6 = uVar7;
                        }
                        fcn.fffffff009b3bf1c(uVar6);
                    }
                    fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18c10);
                }
            }
            else if (0 < *0xfffffff00a38ffec) {
                if (((*0xfffffff0079bbe58 & 1) != 0) || ((*0xfffffff00a395de4 & 1) != 0)) goto code_r0xfffffff009f77b88;
                iVar2 = fcn.fffffff009f771e4(uVar5);
                uVar7 = uVar5 | 0xffffff8000000000;
                if (iVar2 == 0) {
                    if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar7)) && (uVar7 < 0xfffffffc00000000)) {
                        fcn.fffffff00998128c(uVar7);
                    }
                }
                else {
                    uVar6 = uVar5 & 0x7ffffffffff;
                    if ((uVar5 & 0x80000000000000) != 0) {
                        uVar6 = uVar7;
                    }
                    fcn.fffffff009b3bf1c(uVar6);
                }
                fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18c67);
            }
        }
        else if (0 < *0xfffffff00a38ffec) {
            if ((*0xfffffff0079bbe58 & 1) == 0) {
                if ((*0xfffffff00a395de4 & 1) == 0) {
                    uVar6 = uVar5 | 0xffffff8000000000;
                    uVar7 = uVar6;
                    if ((uVar5 & 0x80000000000000) == 0) {
                        uVar7 = uVar5 & 0x7ffffffffff;
                    }
                    if (((uVar7 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar7) ||
                        (*0xfffffff007a0d830 <= uVar7 && uVar7 < *0xfffffff007a0d838)) ||
                       ((*0xfffffff007a0d858 <= uVar7 && (uVar7 < *0xfffffff007a0d860)))) {
                        fcn.fffffff009b3bf1c(uVar7);
                    }
                    else if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar6)) &&
                            (uVar6 < 0xfffffffc00000000)) {
                        fcn.fffffff00998128c(uVar6);
                    }
                    fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18cc7);
                }
                else {
                    uVar7 = uVar5 | 0xffffff8000000000;
                    if ((uVar5 & 0x80000000000000) == 0) {
                        uVar7 = uVar5 & 0x7ffffffffff;
                    }
                    if (((*0xfffffff007a0d818 <= uVar7 || uVar7 < *0xfffffff007a0d810) &&
                        (uVar7 < *0xfffffff007a0d830 || *0xfffffff007a0d838 <= uVar7)) &&
                       ((uVar7 < *0xfffffff007a0d858 || (*0xfffffff007a0d860 <= uVar7))))
                    goto joined_r0xfffffff009f77b94;
                }
            }
            else {
code_r0xfffffff009f77b88:
                uVar7 = fcn.fffffff009f771e4(uVar5);
                if ((uVar7 & 1) == 0) {
joined_r0xfffffff009f77b94:
                    if ((((uVar5 >> 0x37 & 1) != 0) && (uVar7 = uVar5 | 0xffffff8000000000, 0xffffffdbffffffff < uVar7)
                        ) && (uVar7 < 0xfffffffc00000000)) {
                        fcn.fffffff00998128c(uVar7);
                    }
                }
            }
        }
        if ((*(uVar3 + 0x6b0) >> 2 & 1) != 0) {
            uStack_68 = 0;
            iStack_60 = 0;
            fcn.fffffff009adbdf4(uVar4,&iStack_60,&uStack_68);
            uStack_70 = uStack_68 >> 0x1e & 0x1ffff | iStack_60 << 0x11;
            iVar2 = fcn.fffffff009b309b4(&uStack_70,*arg2,8);
            iVar1 = *0xfffffff00a38ffe0;
            if ((iVar2 != 0) && (0 < *0xfffffff00a38ffec)) {
                if ((*0xfffffff0079bbe58 & 1) == 0) {
                    if ((*0xfffffff00a395de4 & 1) == 0) {
                        uVar7 = uVar5 | 0xffffff8000000000;
                        uVar3 = uVar5 & 0x7ffffffffff;
                        if ((uVar5 & 0x80000000000000) != 0) {
                            uVar3 = uVar7;
                        }
                        if (((uVar3 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar3) ||
                            (*0xfffffff007a0d830 <= uVar3 && uVar3 < *0xfffffff007a0d838)) ||
                           ((*0xfffffff007a0d858 <= uVar3 && (uVar3 < *0xfffffff007a0d860)))) {
                            fcn.fffffff009b3bf1c(uVar3);
                        }
                        else if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar7)) &&
                                (uVar7 < 0xfffffffc00000000)) {
                            fcn.fffffff00998128c(uVar7);
                        }
                        fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18b70);
                    }
                    else {
                        uVar3 = uVar5 & 0x7ffffffffff;
                        if ((uVar5 & 0x80000000000000) != 0) {
                            uVar3 = uVar5 | 0xffffff8000000000;
                        }
                        if (((*0xfffffff007a0d818 <= uVar3 || uVar3 < *0xfffffff007a0d810) &&
                            (uVar3 < *0xfffffff007a0d830 || *0xfffffff007a0d838 <= uVar3)) &&
                           ((uVar3 < *0xfffffff007a0d858 || (*0xfffffff007a0d860 <= uVar3))))
                        goto joined_r0xfffffff009f78280;
                    }
                }
                else {
                    uVar3 = fcn.fffffff009f771e4(uVar5);
                    if ((uVar3 & 1) == 0) {
joined_r0xfffffff009f78280:
                        if ((((uVar5 >> 0x37 & 1) != 0) &&
                            (uVar3 = uVar5 | 0xffffff8000000000, 0xffffffdbffffffff < uVar3)) &&
                           (uVar3 < 0xfffffffc00000000)) {
                            fcn.fffffff00998128c(uVar3);
                        }
                    }
                }
            }
        }
        uVar8 = 0xc;
    }
code_r0xfffffff009f77e08:
    fcn.fffffff009ada788(uVar4);
code_r0xfffffff009f77e10:
    iVar1 = *0xfffffff00a38ffe0;
    if (*0xfffffff00a38ffec < 3) {
        return uVar8;
    }
    if ((*0xfffffff0079bbe58 & 1) == 0) {
        if ((*0xfffffff00a395de4 & 1) == 0) {
            uVar4 = uVar5 | 0xffffff8000000000;
            uVar3 = uVar4;
            if ((uVar5 & 0x80000000000000) == 0) {
                uVar3 = uVar5 & 0x7ffffffffff;
            }
            if (((uVar3 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar3) ||
                (*0xfffffff007a0d830 <= uVar3 && uVar3 < *0xfffffff007a0d838)) ||
               ((*0xfffffff007a0d858 <= uVar3 && (uVar3 < *0xfffffff007a0d860)))) {
                fcn.fffffff009b3bf1c(uVar3);
            }
            else if ((((uVar5 >> 0x37 & 1) != 0) && (0xffffffdbffffffff < uVar4)) && (uVar4 < 0xfffffffc00000000)) {
                fcn.fffffff00998128c(uVar4);
            }
            fcn.fffffff00a0531b8(-0xff8ff8000,iVar1,0,-0xff8f18b26);
            return uVar8;
        }
        uVar3 = uVar5 | 0xffffff8000000000;
        if ((uVar5 & 0x80000000000000) == 0) {
            uVar3 = uVar5 & 0x7ffffffffff;
        }
        if (uVar3 < *0xfffffff007a0d818 && *0xfffffff007a0d810 <= uVar3) {
            return uVar8;
        }
        if (*0xfffffff007a0d830 <= uVar3 && uVar3 < *0xfffffff007a0d838) {
            return uVar8;
        }
        if ((*0xfffffff007a0d858 <= uVar3) && (uVar3 < *0xfffffff007a0d860)) {
            return uVar8;
        }
    }
    else {
        uVar3 = fcn.fffffff009f771e4(uVar5);
        if ((uVar3 & 1) != 0) {
            return uVar8;
        }
    }
    if ((((uVar5 >> 0x37 & 1) != 0) && (uVar5 = uVar5 | 0xffffff8000000000, 0xffffffdbffffffff < uVar5)) &&
       (uVar5 < 0xfffffffc00000000)) {
        fcn.fffffff00998128c(uVar5);
    }
    return uVar8;
}

