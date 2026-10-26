/* line: 0, address: 0xfffffff00893da30 */ __int64 __fastcall sub_FFFFFFF00893DA30(
/* line: 1 */         __int64 a1,
/* line: 2 */         __int64 a2,
/* line: 3 */         __int64 a3,
/* line: 4 */         _QWORD *a4,
/* line: 5 */         int *a5,
/* line: 6 */         _DWORD *a6,
/* line: 7 */         unsigned int a7,
/* line: 8 */         int a8,
/* line: 9 */         __int128 a9)
/* line: 10 */ {
/* line: 11 */   __int64 v13; // x20
/* line: 12 */   int v14; // w0
/* line: 13 */   int v15; // w22
/* line: 14 */   int v16; // w23
/* line: 15 */   __int64 v17; // x24
/* line: 16 */   __int64 v18; // x0
/* line: 17 */   char v19; // w25
/* line: 18 */   __int64 v20; // x26
/* line: 19 */   __int64 v21; // x25
/* line: 20 */   __int64 v22; // x28
/* line: 21 */   char v23; // w19
/* line: 22 */   __int64 v24; // x0
/* line: 23 */   __int64 v25; // x28
/* line: 24 */   __int64 v26; // x22
/* line: 25 */   int v27; // w25
/* line: 26 */   const char *v28; // x3
/* line: 27 */   __int64 v29; // x0
/* line: 28 */   __int64 v30; // x1
/* line: 29 */   __int64 v31; // x22
/* line: 30 */   int *v32; // x19
/* line: 31 */   int v33; // w8
/* line: 32 */   BOOL v34; // w9
/* line: 33 */   __int64 v35; // x21
/* line: 34 */   const char *v36; // x3
/* line: 35 */   __int64 v37; // x0
/* line: 36 */   __int64 v39; // x0
/* line: 37 */   __int64 v40; // x23
/* line: 38 */   __int128 v41; // q0
/* line: 39 */   char v42; // w0
/* line: 40 */   __int64 v43; // x27
/* line: 41 */   __int64 v44; // x22
/* line: 42 */   __int64 v45; // x0
/* line: 43 */   __int64 v46; // x0
/* line: 44 */   __int64 v47; // x22
/* line: 45 */   __int64 v48; // x21
/* line: 46 */   __int64 v49; // x23
/* line: 47 */   __int64 v50; // x0
/* line: 48 */   __int64 v51; // x0
/* line: 49 */   __int64 v52; // x23
/* line: 50 */   unsigned int v53; // w8
/* line: 51 */   __int64 v54; // x19
/* line: 52 */   __int64 v55; // x8
/* line: 53 */   int v56; // w0
/* line: 54 */   __int64 v57; // x0
/* line: 55 */   unsigned int *v58; // x23
/* line: 56 */   bool v59; // w8
/* line: 57 */   __int64 v60; // x10
/* line: 58 */   __int64 v61; // x9
/* line: 59 */   unsigned int v62; // w0
/* line: 60 */   char v63; // w19
/* line: 61 */   unsigned int v64; // w0
/* line: 62 */   __int64 v65; // x0
/* line: 63 */   int v66; // w0
/* line: 64 */   __int64 v67; // x0
/* line: 65 */   __int64 v68; // x0
/* line: 66 */   int v69; // w27
/* line: 67 */   BOOL v70; // w23
/* line: 68 */   char v71; // w8
/* line: 69 */   __int64 v72; // x19
/* line: 70 */   char v73; // w8
/* line: 71 */   __int64 v74; // x0
/* line: 72 */   int v75; // w0
/* line: 73 */   __int64 v76; // x28
/* line: 74 */   __int64 v77; // x0
/* line: 75 */   __int64 v78; // x22
/* line: 76 */   __int64 v79; // x22
/* line: 77 */   __int64 v80; // x5
/* line: 78 */   __int64 v81; // x1
/* line: 79 */   const char *v82; // x2
/* line: 80 */   __int64 v83; // x28
/* line: 81 */   __int64 v84; // x23
/* line: 82 */   __int64 v85; // x0
/* line: 83 */   int v86; // w0
/* line: 84 */   int v87; // w26
/* line: 85 */   __int64 v88; // x2
/* line: 86 */   __int64 v89; // x0
/* line: 87 */   __int64 v90; // x25
/* line: 88 */   unsigned int v91; // w19
/* line: 89 */   __int64 v92; // x0
/* line: 90 */   __int64 v93; // x0
/* line: 91 */   __int64 v94; // x0
/* line: 92 */   __int64 v95; // x1
/* line: 93 */   __int64 v96; // x0
/* line: 94 */   __int64 v97; // [xsp+68h] [xbp-308h]
/* line: 95 */   __int64 v98; // [xsp+70h] [xbp-300h]
/* line: 96 */   _DWORD *v99; // [xsp+78h] [xbp-2F8h]
/* line: 97 */   char v100; // [xsp+84h] [xbp-2ECh]
/* line: 98 */   __int64 v101; // [xsp+88h] [xbp-2E8h]
/* line: 99 */   const char *v102; // [xsp+90h] [xbp-2E0h]
/* line: 100 */   _QWORD *v103; // [xsp+98h] [xbp-2D8h]
/* line: 101 */   int v105; // [xsp+A8h] [xbp-2C8h]
/* line: 102 */   int v106; // [xsp+ACh] [xbp-2C4h]
/* line: 103 */   int v107; // [xsp+ACh] [xbp-2C4h]
/* line: 104 */   int *v108; // [xsp+B8h] [xbp-2B8h]
/* line: 105 */   __int64 v109; // [xsp+C0h] [xbp-2B0h] BYREF
/* line: 106 */   __int64 v110; // [xsp+C8h] [xbp-2A8h] BYREF
/* line: 107 */   __int16 v111; // [xsp+D6h] [xbp-29Ah] BYREF
/* line: 108 */   __int64 v112; // [xsp+D8h] [xbp-298h] BYREF
/* line: 109 */   __int64 *v113; // [xsp+E0h] [xbp-290h]
/* line: 110 */   __int64 v114; // [xsp+E8h] [xbp-288h]
/* line: 111 */   char v115; // [xsp+F0h] [xbp-280h]
/* line: 112 */   __int64 v116; // [xsp+F8h] [xbp-278h] BYREF
/* line: 113 */   __int64 *v117; // [xsp+100h] [xbp-270h]
/* line: 114 */   __int64 v118; // [xsp+108h] [xbp-268h]
/* line: 115 */   char v119; // [xsp+110h] [xbp-260h]
/* line: 116 */   _QWORD v120[3]; // [xsp+118h] [xbp-258h] BYREF
/* line: 117 */   char v121; // [xsp+130h] [xbp-240h]
/* line: 118 */   _QWORD v122[3]; // [xsp+138h] [xbp-238h] BYREF
/* line: 119 */   char v123; // [xsp+150h] [xbp-220h]
/* line: 120 */   __int64 v124; // [xsp+158h] [xbp-218h] BYREF
/* line: 121 */   __int128 v125; // [xsp+160h] [xbp-210h] BYREF
/* line: 122 */   __int128 v126; // [xsp+170h] [xbp-200h]
/* line: 123 */   __int64 v127; // [xsp+180h] [xbp-1F0h]
/* line: 124 */   __int128 v128; // [xsp+190h] [xbp-1E0h] BYREF
/* line: 125 */   __int128 v129; // [xsp+1A0h] [xbp-1D0h]
/* line: 126 */   __int64 v130; // [xsp+1B8h] [xbp-1B8h] BYREF
/* line: 127 */   __int16 v131; // [xsp+1C2h] [xbp-1AEh] BYREF
/* line: 128 */   int v132; // [xsp+1C4h] [xbp-1ACh] BYREF
/* line: 129 */   __int64 v133; // [xsp+1C8h] [xbp-1A8h] BYREF
/* line: 130 */   __int64 v134; // [xsp+1D0h] [xbp-1A0h] BYREF
/* line: 131 */   __int64 v135; // [xsp+1D8h] [xbp-198h] BYREF
/* line: 132 */   __int64 *v136; // [xsp+1E0h] [xbp-190h]
/* line: 133 */   __int128 v137; // [xsp+1E8h] [xbp-188h]
/* line: 134 */   __int128 v138; // [xsp+1F8h] [xbp-178h]
/* line: 135 */   _BYTE v139[32]; // [xsp+208h] [xbp-168h] BYREF
/* line: 136 */   __int64 v140; // [xsp+228h] [xbp-148h]
/* line: 137 */   __int128 v141; // [xsp+230h] [xbp-140h] BYREF
/* line: 138 */   __int128 v142; // [xsp+240h] [xbp-130h]
/* line: 139 */   __int128 v143; // [xsp+250h] [xbp-120h]
/* line: 140 */   __int128 v144; // [xsp+260h] [xbp-110h]
/* line: 141 */   __int128 v145; // [xsp+270h] [xbp-100h]
/* line: 142 */   __int128 v146; // [xsp+280h] [xbp-F0h]
/* line: 143 */   __int128 v147; // [xsp+290h] [xbp-E0h]
/* line: 144 */   __int128 v148; // [xsp+2A0h] [xbp-D0h]
/* line: 145 */   __int128 v149; // [xsp+2B0h] [xbp-C0h]
/* line: 146 */   __int128 v150; // [xsp+2C0h] [xbp-B0h]
/* line: 147 */   __int128 v151; // [xsp+2D0h] [xbp-A0h]
/* line: 148 */   __int128 v152; // [xsp+2E0h] [xbp-90h]
/* line: 149 */   __int128 v153; // [xsp+2F0h] [xbp-80h]
/* line: 150 */   __int64 v154; // [xsp+308h] [xbp-68h]
/* line: 151 */ 
/* line: 152, address: 0xfffffff00893da78 */   v154 = qword_FFFFFFF00A3A8000;
/* line: 153 */   if ( dword_FFFFFFF00A407338 != 0 )
/* line: 154 */     sub_FFFFFFF008958174(a1: "AMFI: vnode_check_signature called with platform %d\n", a8);
/* line: 155, address: 0xfffffff00893daa8 */   v13 = sub_FFFFFFF0089581A4(a1: &unk_FFFFFFF007BCD008);
/* line: 156, address: 0xfffffff00893dac0 */   sub_FFFFFFF008958564(a1: v13 + 8, a2: 1025);
/* line: 157, address: 0xfffffff00893dadc */   *(_QWORD *)v13 = off_FFFFFFF007BCB920;
/* line: 158, address: 0xfffffff00893dae0 */   *(_QWORD *)(v13 + 1040) = a1;
/* line: 159, address: 0xfffffff00893dae4 */   *(_QWORD *)(v13 + 1048) = 0;
/* line: 160, address: 0xfffffff00893dae8 */   *(_QWORD *)&v141 = 0;
/* line: 161, address: 0xfffffff00893db04 */   v14 = sub_FFFFFFF008958B54(a1, a2: "com.apple.mis.warning", a3: 0, a4: 0, a5: &v141);
/* line: 162, address: 0xfffffff00893db08 */   v15 = v14;
/* line: 163 */   if ( v14 == 34 || v14 == 0 )
/* line: 164 */   {
/* line: 165, address: 0xfffffff00893db1c */     if ( (*(_BYTE *)(v13 + 1032) & 1) == 0 )
/* line: 166, address: 0xfffffff00893db38 */       *(_BYTE *)(v13 + 1032) = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v13 + 16LL))(a1: v13);
/* line: 167 */     sub_FFFFFFF008958174(a1: "Respecting MIS launch warning: %s\n", (const char *)(v13 + 8));
/* line: 168 */   }
/* line: 169, address: 0xfffffff00893db4c */   v106 = v15;
/* line: 170, address: 0xfffffff00893db50 */   v16 = *a5;
/* line: 171, address: 0xfffffff00893db54 */   *a6 = 0;
/* line: 172, address: 0xfffffff00893db58 */   v111 = 0;
/* line: 173, address: 0xfffffff00893db68 */   v17 = sub_FFFFFFF008958704(a1: a4, a2: &v111);
/* line: 174, address: 0xfffffff00893db70 */   v18 = sub_FFFFFFF008958714(a1: a4);
/* line: 175, address: 0xfffffff00893db74 */   v108 = a5;
/* line: 176, address: 0xfffffff00893db8c */   if ( v18 != 0 && bswap32(*(_DWORD *)(v18 + 8)) >> 10 >= 0x81 )
/* line: 177, address: 0xfffffff00893db94 */     v19 = *(_BYTE *)(v18 + 87) & 1;
/* line: 178 */   else
/* line: 179, address: 0xfffffff00893db9c */     v19 = 0;
/* line: 180, address: 0xfffffff00893dba0 */   v20 = *((_QWORD *)&a9 + 1);
/* line: 181, address: 0xfffffff00893dba4 */   v109 = 0;
/* line: 182 */   if ( (sub_FFFFFFF00893FCE8(a1: &v110, a2: a4, a3: v13, a4: &v109) & 1) == 0 )
/* line: 183 */   {
/* line: 184 */     v28 = "The signature could not be validated because AMFI could not load its entitlements for validation: %s";
/* line: 185, address: 0xfffffff00893ded0 */     v29 = v13;
/* line: 186, address: 0xfffffff00893ded4 */     v30 = a9;
/* line: 187, address: 0xfffffff00893ded4 */     goto LABEL_29;
/* line: 188 */   }
/* line: 189, address: 0xfffffff00893dbc4 */   v99 = a6;
/* line: 190, address: 0xfffffff00893dbc8 */   v102 = (const char *)(v13 + 8);
/* line: 191, address: 0xfffffff00893dbcc */   v100 = v19;
/* line: 192, address: 0xfffffff00893dbd0 */   v105 = v16;
/* line: 193, address: 0xfffffff00893dbd4 */   v21 = v110;
/* line: 194, address: 0xfffffff00893dbd8 */   if ( v110 != 0 )
/* line: 195 */   {
/* line: 196, address: 0xfffffff00893dbdc */     v22 = *((_QWORD *)&a9 + 1);
/* line: 197, address: 0xfffffff00893dbe0 */     v101 = v13 & 0xFFFFFFFFFFFFLL | 0x6712000000000000LL;
/* line: 198, address: 0xfffffff00893dbe8 */     v103 = a4;
/* line: 199, address: 0xfffffff00893dbf0 */     sub_FFFFFFF008958764(a1: a4);
/* line: 200, address: 0xfffffff00893dbf8 */     v135 = 0;
/* line: 201, address: 0xfffffff00893dbf8 */     v136 = &v135;
/* line: 202, address: 0xfffffff00893dc04 */     *(_QWORD *)&v137 = 0x2000000000LL;
/* line: 203, address: 0xfffffff00893dc0c */     BYTE8(v137) = 1;
/* line: 204, address: 0xfffffff00893dc14 */     *(_QWORD *)&v125 = 0;
/* line: 205, address: 0xfffffff00893dc14 */     *((_QWORD *)&v125 + 1) = &v125;
/* line: 206, address: 0xfffffff00893dc18 */     *(_QWORD *)&v126 = 0x2000000000LL;
/* line: 207, address: 0xfffffff00893dc1c */     BYTE8(v126) = 0;
/* line: 208, address: 0xfffffff00893dc24 */     *(_QWORD *)&v128 = 0;
/* line: 209, address: 0xfffffff00893dc24 */     *((_QWORD *)&v128 + 1) = &v128;
/* line: 210, address: 0xfffffff00893dc28 */     *(_QWORD *)&v129 = 0x2000000000LL;
/* line: 211, address: 0xfffffff00893dc2c */     BYTE8(v129) = 0;
/* line: 212, address: 0xfffffff00893dc34 */     v122[0] = 0;
/* line: 213, address: 0xfffffff00893dc34 */     v122[1] = v122;
/* line: 214, address: 0xfffffff00893dc38 */     v122[2] = 0x2000000000LL;
/* line: 215, address: 0xfffffff00893dc3c */     v123 = 0;
/* line: 216, address: 0xfffffff00893dc48 */     v120[0] = 0;
/* line: 217, address: 0xfffffff00893dc48 */     v120[1] = v120;
/* line: 218, address: 0xfffffff00893dc4c */     v120[2] = 0x2000000000LL;
/* line: 219, address: 0xfffffff00893dc50 */     v121 = 0;
/* line: 220, address: 0xfffffff00893dc58 */     v116 = 0;
/* line: 221, address: 0xfffffff00893dc58 */     v117 = &v116;
/* line: 222, address: 0xfffffff00893dc5c */     v118 = 0x2000000000LL;
/* line: 223, address: 0xfffffff00893dc60 */     v119 = 0;
/* line: 224, address: 0xfffffff00893dc68 */     v112 = 0;
/* line: 225, address: 0xfffffff00893dc68 */     v113 = &v112;
/* line: 226, address: 0xfffffff00893dc6c */     v114 = 0x2000000000LL;
/* line: 227, address: 0xfffffff00893dc70 */     v115 = 0;
/* line: 228, address: 0xfffffff00893dc74 */     HIBYTE(v111) = 0;
/* line: 229, address: 0xfffffff00893dc7c */     if ( sub_FFFFFFF008942EE4(a1: v21) != 0 )
/* line: 230 */     {
/* line: 231, address: 0xfffffff00893dc98 */       *(_QWORD *)&v141 = &unk_FFFFFFF00A40BC30;
/* line: 232, address: 0xfffffff00893dca4 */       *((_QWORD *)&v141 + 1) = 1107296256;
/* line: 233, address: 0xfffffff00893dcb8 */       *(_QWORD *)&v142 = sub_FFFFFFF008940050;
/* line: 234, address: 0xfffffff00893dcd8 */       *((_QWORD *)&v146 + 1) = v108;
/* line: 235, address: 0xfffffff00893dcdc */       *(_QWORD *)&v147 = (char *)&v111 + 1;
/* line: 236, address: 0xfffffff00893dce0 */       *((_QWORD *)&v147 + 1) = v13;
/* line: 237, address: 0xfffffff00893dce8 */       v148 = a9;
/* line: 238, address: 0xfffffff00893dcf0 */       *((_QWORD *)&v142 + 1) = &unk_FFFFFFF007BCB940;
/* line: 239, address: 0xfffffff00893dcf8 */       *(_QWORD *)&v143 = &v135;
/* line: 240, address: 0xfffffff00893dd00 */       *((_QWORD *)&v143 + 1) = &v125;
/* line: 241, address: 0xfffffff00893dd04 */       *(_QWORD *)&v144 = &v128;
/* line: 242, address: 0xfffffff00893dd08 */       *((_QWORD *)&v144 + 1) = v122;
/* line: 243, address: 0xfffffff00893dd0c */       *(_QWORD *)&v145 = v120;
/* line: 244, address: 0xfffffff00893dd10 */       *((_QWORD *)&v145 + 1) = &v116;
/* line: 245, address: 0xfffffff00893dd14 */       *(_QWORD *)&v146 = &v112;
/* line: 246, address: 0xfffffff00893dd20 */       sub_FFFFFFF0089436B8(a1: v21, a2: &v141);
/* line: 247, address: 0xfffffff00893dd38 */       if ( (v117[3] & 1) != 0 && (v113[3] & 1) != 0 )
/* line: 248 */       {
/* line: 249, address: 0xfffffff00893dd50 */         sub_FFFFFFF00893C038(
/* line: 250 */           a1: v13,
/* line: 251 */           a2: a9,
/* line: 252 */           a3: *((_QWORD *)&a9 + 1),
/* line: 253 */           a4: "Cannot be both OOP-JIT loader and runner simultaneously");
/* line: 254, address: 0xfffffff00893dd58 */         *((_BYTE *)v136 + 24) = 0;
/* line: 255 */       }
/* line: 256 */     }
/* line: 257, address: 0xfffffff00893dd60 */     v23 = *((_BYTE *)v136 + 24);
/* line: 258, address: 0xfffffff00893dd6c */     sub_FFFFFFF0089582C4(a1: &v112, a2: 8);
/* line: 259, address: 0xfffffff00893dd78 */     sub_FFFFFFF0089582C4(a1: &v116, a2: 8);
/* line: 260, address: 0xfffffff00893dd84 */     sub_FFFFFFF0089582C4(a1: v120, a2: 8);
/* line: 261, address: 0xfffffff00893dd90 */     sub_FFFFFFF0089582C4(a1: v122, a2: 8);
/* line: 262, address: 0xfffffff00893dd9c */     sub_FFFFFFF0089582C4(a1: &v128, a2: 8);
/* line: 263, address: 0xfffffff00893dda8 */     sub_FFFFFFF0089582C4(a1: &v125, a2: 8);
/* line: 264, address: 0xfffffff00893ddb4 */     v24 = sub_FFFFFFF0089582C4(a1: &v135, a2: 8);
/* line: 265, address: 0xfffffff00893ddc4 */     if ( (v23 & 1) == 0 )
/* line: 266 */     {
/* line: 267, address: 0xfffffff00893def0 */       v17 = 0;
/* line: 268 */ LABEL_33:
/* line: 269, address: 0xfffffff00893df14 */       v27 = 0;
/* line: 270, address: 0xfffffff00893df14 */       goto LABEL_34;
/* line: 271 */     }
/* line: 272 */   }
/* line: 273 */   if ( v17 == 0 )
/* line: 274 */   {
/* line: 275, address: 0xfffffff00893df08 */     v22 = *((_QWORD *)&a9 + 1);
/* line: 276 */     v24 = sub_FFFFFFF00893C038(a1: v13, a2: a9, a3: *((_QWORD *)&a9 + 1), a4: "Internal Error: No cdhash found.");
/* line: 277, address: 0xfffffff00893df10 */     goto LABEL_33;
/* line: 278 */   }
/* line: 279, address: 0xfffffff00893ddd8 */   v25 = a9;
/* line: 280 */   if ( (unsigned int)sub_FFFFFFF00893BEF0(a1: v17, a2: (unsigned __int8)v111) == 0 )
/* line: 281 */   {
/* line: 282, address: 0xfffffff00893e0c0 */     v17 = sub_FFFFFFF0089581A4(a1: &unk_FFFFFFF007BCD088);
/* line: 283, address: 0xfffffff00893e0c4 */     v124 = 0;
/* line: 284, address: 0xfffffff00893e0d0 */     v39 = sub_FFFFFFF008958704(a1: a4, a2: &v124);
/* line: 285, address: 0xfffffff00893e0d4 */     if ( v39 == 0 )
/* line: 286, address: 0xfffffff00893ee60 */       sub_FFFFFFF008957488();
/* line: 287, address: 0xfffffff00893e0d8 */     v40 = v39;
/* line: 288, address: 0xfffffff00893e0e0 */     v135 = v13;
/* line: 289, address: 0xfffffff00893e0e0 */     v136 = a4;
/* line: 290, address: 0xfffffff00893e0e4 */     v41 = 0u;
/* line: 291, address: 0xfffffff00893e0e8 */     v137 = 0u;
/* line: 292, address: 0xfffffff00893e0e8 */     v138 = 0u;
/* line: 293, address: 0xfffffff00893e0ec */     memset(v139, 0, sizeof(v139));
/* line: 294, address: 0xfffffff00893e0f0 */     v101 = v21;
/* line: 295, address: 0xfffffff00893e0f4 */     v140 = v21;
/* line: 296, address: 0xfffffff00893e0fc */     if ( (*(_BYTE *)(v13 + 1032) & 1) == 0 )
/* line: 297 */     {
/* line: 298, address: 0xfffffff00893e114 */       v42 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v13 + 16LL))(a1: v13);
/* line: 299, address: 0xfffffff00893e118 */       v41 = 0u;
/* line: 300, address: 0xfffffff00893e11c */       *(_BYTE *)(v13 + 1032) = v42;
/* line: 301 */     }
/* line: 302, address: 0xfffffff00893e12c */     v43 = (unsigned __int8)v124;
/* line: 303, address: 0xfffffff00893e130 */     v122[0] = 0;
/* line: 304, address: 0xfffffff00893e134 */     v152 = v41;
/* line: 305, address: 0xfffffff00893e134 */     v153 = v41;
/* line: 306, address: 0xfffffff00893e138 */     v150 = v41;
/* line: 307, address: 0xfffffff00893e138 */     v151 = v41;
/* line: 308, address: 0xfffffff00893e13c */     v148 = v41;
/* line: 309, address: 0xfffffff00893e13c */     v149 = v41;
/* line: 310, address: 0xfffffff00893e140 */     v146 = v41;
/* line: 311, address: 0xfffffff00893e140 */     v147 = v41;
/* line: 312, address: 0xfffffff00893e144 */     v144 = v41;
/* line: 313, address: 0xfffffff00893e144 */     v145 = v41;
/* line: 314, address: 0xfffffff00893e148 */     v142 = v41;
/* line: 315, address: 0xfffffff00893e148 */     v143 = v41;
/* line: 316, address: 0xfffffff00893e14c */     v141 = v41;
/* line: 317, address: 0xfffffff00893e158 */     v44 = sub_FFFFFFF0089586D4(a1: a4);
/* line: 318, address: 0xfffffff00893e160 */     v45 = sub_FFFFFFF008958794(a1: a4);
/* line: 319, address: 0xfffffff00893e174 */     v46 = sub_FFFFFFF0089586C4(a1: v44, a2: v45, a3: 0, a4: 4208856066LL);
/* line: 320, address: 0xfffffff00893e178 */     v103 = a4;
/* line: 321 */     if ( v46 == 0 )
/* line: 322 */     {
/* line: 323 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' has no cd?\n");
/* line: 324, address: 0xfffffff00893e1e8 */       goto LABEL_74;
/* line: 325 */     }
/* line: 326, address: 0xfffffff00893e180 */     v47 = v46;
/* line: 327, address: 0xfffffff00893e188 */     v48 = bswap32(*(_DWORD *)(v46 + 4));
/* line: 328 */     if ( (unsigned int)v48 <= 7 )
/* line: 329 */     {
/* line: 330 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' has short cd (%u)?\n");
/* line: 331 */ LABEL_74:
/* line: 332 */       sub_FFFFFFF008958174(a1: "AMFI: '%s': Unrecoverable CT signature issue, bailing out.\n", v102);
/* line: 333 */       sub_FFFFFFF008958174(a1: "AMFI: code signature validation failed.\n");
/* line: 334, address: 0xfffffff00893e280 */       a4 = v103;
/* line: 335, address: 0xfffffff00893e284 */       v16 = v105;
/* line: 336, address: 0xfffffff00893e284 */       goto LABEL_75;
/* line: 337 */     }
/* line: 338, address: 0xfffffff00893e1ec */     v98 = v40;
/* line: 339, address: 0xfffffff00893e1fc */     v49 = sub_FFFFFFF0089586D4(a1: v103);
/* line: 340, address: 0xfffffff00893e204 */     v50 = sub_FFFFFFF008958794(a1: v103);
/* line: 341, address: 0xfffffff00893e21c */     v51 = sub_FFFFFFF0089586C4(a1: v49, a2: v50, a3: 0x10000, a4: 4208855809LL);
/* line: 342 */     if ( v51 == 0 )
/* line: 343 */     {
/* line: 344 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' has no CMS blob?\n");
/* line: 345, address: 0xfffffff00893e25c */       goto LABEL_74;
/* line: 346 */     }
/* line: 347, address: 0xfffffff00893e224 */     v52 = v51;
/* line: 348, address: 0xfffffff00893e228 */     v53 = *(_DWORD *)(v51 + 4);
/* line: 349 */     if ( bswap32(v53) <= 7 )
/* line: 350 */     {
/* line: 351 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' has short cms (%u), bailing out.\n");
/* line: 352, address: 0xfffffff00893e248 */       goto LABEL_74;
/* line: 353 */     }
/* line: 354 */     if ( v53 == 0x8000000 )
/* line: 355 */     {
/* line: 356, address: 0xfffffff00893e2b4 */       v122[0] = 0;
/* line: 357 */       if ( (sub_FFFFFFF008958744(a1: v103) & 2) == 0 )
/* line: 358 */       {
/* line: 359 */         sub_FFFFFFF008958174(a1: "AMFI: '%s' has zero length cms, but is not adhoc?\n");
/* line: 360, address: 0xfffffff00893e2d4 */         goto LABEL_74;
/* line: 361 */       }
/* line: 362 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' is adhoc signed.\n", v102);
/* line: 363 */ LABEL_105:
/* line: 364, address: 0xfffffff00893e548 */       v31 = v122[0];
/* line: 365 */       if ( (sub_FFFFFFF008958744(a1: v103) & 2) != 0 )
/* line: 366 */       {
/* line: 367, address: 0xfffffff00893e560 */         v68 = sub_FFFFFFF008958714(a1: v103);
/* line: 368 */         if ( v68 != 0 )
/* line: 369 */         {
/* line: 370, address: 0xfffffff00893e580 */           v69 = v105;
/* line: 371, address: 0xfffffff00893e810 */           v70 = bswap32(*(_DWORD *)(v68 + 8)) >= 0x20600 && *(unsigned __int8 *)(v68 + 97) == 2;
/* line: 372 */ LABEL_111:
/* line: 373 */           if ( (v31 & 0x18000001FF80LL) == 0 && !v70 )
/* line: 374 */           {
/* line: 375, address: 0xfffffff00893e5c4 */             v16 = v69;
/* line: 376 */             sub_FFFFFFF008958174(
/* line: 377 */               a1: "AMFI: '%s': unsuitable CT policy %#llx for this platform/device, rejecting signature.\n",
/* line: 378 */               v102,
/* line: 379 */               v31);
/* line: 380, address: 0xfffffff00893e5dc */             v71 = v31;
/* line: 381, address: 0xfffffff00893e5e0 */             LODWORD(v31) = 0;
/* line: 382, address: 0xfffffff00893e5e4 */             v72 = v71 & 0x10;
/* line: 383 */ LABEL_118:
/* line: 384, address: 0xfffffff00893e61c */             v107 = 1;
/* line: 385, address: 0xfffffff00893e624 */             a4 = v103;
/* line: 386, address: 0xfffffff00893e624 */             goto LABEL_119;
/* line: 387 */           }
/* line: 388, address: 0xfffffff00893e5f4 */           v72 = (unsigned __int8)v31 & 0x90;
/* line: 389 */           if ( (sub_FFFFFFF0089588D4() & 1) == 0 && (v31 & 0x90) != 0 )
/* line: 390 */           {
/* line: 391, address: 0xfffffff00893e600 */             v16 = v69;
/* line: 392 */             sub_FFFFFFF008958174(a1: "AMFI: '%s': has developer signature (%#llx) but developer mode is not enabled\n");
/* line: 393 */ LABEL_117:
/* line: 394, address: 0xfffffff00893e618 */             LODWORD(v31) = 0;
/* line: 395, address: 0xfffffff00893e618 */             goto LABEL_118;
/* line: 396 */           }
/* line: 397, address: 0xfffffff00893e678 */           v97 = *((_QWORD *)&a9 + 1);
/* line: 398 */           if ( (v31 & 0x80000001100LL) != 0 && (sub_FFFFFFF008942104(a1: v98) & 1) == 0 && v106 != 34 && v106 != 0 )
/* line: 399 */           {
/* line: 400, address: 0xfffffff00893e8f0 */             v16 = v69;
/* line: 401, address: 0xfffffff00893e904 */             sub_FFFFFFF008958174(a1: "App Store Fast Path -> %s\n", v102);
/* line: 402, address: 0xfffffff00893e914 */             *v108 |= 0x4000u;
/* line: 403, address: 0xfffffff00893e918 */             v82 = "fast-path-appstore";
/* line: 404 */           }
/* line: 405 */           else
/* line: 406 */           {
/* line: 407, address: 0xfffffff00893e6c4 */             v75 = sub_FFFFFFF008958264(a1: 0);
/* line: 408 */             if ( (v31 & 0x100000002200LL) == 0 || v75 == 0 || v106 == 0 || v106 == 34 )
/* line: 409 */             {
/* line: 410 */               if ( v70 )
/* line: 411 */               {
/* line: 412, address: 0xfffffff00893e700 */                 v76 = sub_FFFFFFF008958714(a1: v103);
/* line: 413, address: 0xfffffff00893e704 */                 v77 = sub_FFFFFFF0089588C4();
/* line: 414, address: 0xfffffff00893e708 */                 if ( v76 == 0 )
/* line: 415, address: 0xfffffff00893eec0 */                   sub_FFFFFFF00895744C(a1: v77);
/* line: 416, address: 0xfffffff00893e70c */                 v78 = v77;
/* line: 417, address: 0xfffffff00893e718 */                 v16 = v69;
/* line: 418 */                 if ( (*(_BYTE *)(v76 + 87) & 1) != 0 )
/* line: 419 */                 {
/* line: 420, address: 0xfffffff00893e890 */                   a4 = v103;
/* line: 421, address: 0xfffffff00893e898 */                   sub_FFFFFFF008958764(a1: v103);
/* line: 422 */                   sub_FFFFFFF008958174(a1: "AMFI: %s: OOP-JIT signature represents main binary\n");
/* line: 423, address: 0xfffffff00893e8a8 */                   goto LABEL_153;
/* line: 424 */                 }
/* line: 425 */                 if ( v21 != 0 && sub_FFFFFFF008942EE4(a1: v21) != 0 )
/* line: 426 */                 {
/* line: 427, address: 0xfffffff00893e72c */                   a4 = v103;
/* line: 428, address: 0xfffffff00893e734 */                   sub_FFFFFFF008958764(a1: v103);
/* line: 429 */                   sub_FFFFFFF008958174(a1: "AMFI: %s: OOP-JIT signature has entitlements\n");
/* line: 430 */ LABEL_153:
/* line: 431, address: 0xfffffff00893e8d8 */                   v107 = 0;
/* line: 432, address: 0xfffffff00893e8dc */                   LODWORD(v31) = 0;
/* line: 433, address: 0xfffffff00893e8e8 */                   v25 = a9;
/* line: 434, address: 0xfffffff00893e8ec */                   goto LABEL_119;
/* line: 435 */                 }
/* line: 436 */                 if ( bswap32(*(_DWORD *)(v76 + 104)) <= 0x13 )
/* line: 437 */                 {
/* line: 438, address: 0xfffffff00893e8bc */                   a4 = v103;
/* line: 439, address: 0xfffffff00893e8c4 */                   sub_FFFFFFF008958764(a1: v103);
/* line: 440 */                   sub_FFFFFFF008958174(a1: "AMFI: %s linkage size is too small\n");
/* line: 441, address: 0xfffffff00893e8d4 */                   goto LABEL_153;
/* line: 442 */                 }
/* line: 443, address: 0xfffffff00893eb08 */                 v90 = v72;
/* line: 444 */                 if ( *(_WORD *)(v76 + 98) != 0 )
/* line: 445 */                 {
/* line: 446, address: 0xfffffff00893eb14 */                   v91 = __rev16(*(unsigned __int16 *)(v76 + 98));
/* line: 447 */                   if ( v91 < 3 )
/* line: 448 */                   {
/* line: 449, address: 0xfffffff00893eb6c */                     v92 = sub_FFFFFFF008958864(a1: v78);
/* line: 450, address: 0xfffffff00893eb70 */                     v31 = v92;
/* line: 451 */                     if ( v92 != 0 )
/* line: 452 */                     {
/* line: 453, address: 0xfffffff00893eb7c */                       v93 = sub_FFFFFFF0089587E4(a1: v92);
/* line: 454, address: 0xfffffff00893eb8c */                       v94 = sub_FFFFFFF008958384(a1: v93, a2: &unk_FFFFFFF00A4DF9B0);
/* line: 455 */                       if ( v94 != 0 )
/* line: 456 */                       {
/* line: 457 */                         if ( (unsigned int)sub_FFFFFFF008943C40(
/* line: 458 */                                              a1: v94,
/* line: 459 */                                              a2: "com.apple.private.oop-jit.loader",
/* line: 460 */                                              a3: qword_FFFFFFF007BCB970[v91]) != 0 )
/* line: 461 */                         {
/* line: 462, address: 0xfffffff00893ebb4 */                           if ( (unsigned int)sub_FFFFFFF008937804() == 0 )
/* line: 463 */                           {
/* line: 464, address: 0xfffffff00893ed44 */                             sub_FFFFFFF008958174(a1: "OOP-JIT Signed Fast Path -> %s\n", v102);
/* line: 465, address: 0xfffffff00893ed54 */                             *v108 |= 0x4000u;
/* line: 466, address: 0xfffffff00893ed60 */                             *v99 = 9;
/* line: 467, address: 0xfffffff00893ed6c */                             a4 = v103;
/* line: 468, address: 0xfffffff00893ed78 */                             sub_FFFFFFF00893BFD0(a1: v103, a2: 9, a3: "oop-jit");
/* line: 469, address: 0xfffffff00893ed80 */                             v107 = 1;
/* line: 470, address: 0xfffffff00893ed84 */                             LODWORD(v31) = 1;
/* line: 471, address: 0xfffffff00893ed88 */                             goto LABEL_191;
/* line: 472 */                           }
/* line: 473 */                           sub_FFFFFFF008958174(a1: "AMFI: %s: oop-jit is disabled by system configuration\n");
/* line: 474 */                         }
/* line: 475 */                         else
/* line: 476 */                         {
/* line: 477, address: 0xfffffff00893eca0 */                           sub_FFFFFFF008958764(a1: v31);
/* line: 478 */                           sub_FFFFFFF008958174(a1: "AMFI: %s: unsatisfied entitlements for loading OOP-JIT signature\n");
/* line: 479 */                         }
/* line: 480 */                       }
/* line: 481 */                       else
/* line: 482 */                       {
/* line: 483, address: 0xfffffff00893ec80 */                         sub_FFFFFFF008958764(a1: v31);
/* line: 484 */                         sub_FFFFFFF008958174(a1: "AMFI: %s: %s has no entitlements\n");
/* line: 485 */                       }
/* line: 486, address: 0xfffffff00893ecb4 */                       v107 = 0;
/* line: 487, address: 0xfffffff00893ecb8 */                       LODWORD(v31) = 0;
/* line: 488 */                     }
/* line: 489 */                     else
/* line: 490 */                     {
/* line: 491 */                       sub_FFFFFFF008958174(
/* line: 492 */                         a1: "AMFI: %s: unable to obtain cs_blob on current_proc\n",
/* line: 493 */                         "_validateOOPJit");
/* line: 494, address: 0xfffffff00893ec74 */                       v107 = 0;
/* line: 495 */                     }
/* line: 496, address: 0xfffffff00893ecc0 */                     a4 = v103;
/* line: 497 */ LABEL_191:
/* line: 498, address: 0xfffffff00893ecc4 */                     v16 = v105;
/* line: 499, address: 0xfffffff00893eccc */                     v25 = a9;
/* line: 500, address: 0xfffffff00893ecd0 */                     v72 = v90;
/* line: 501, address: 0xfffffff00893ecd4 */                     goto LABEL_119;
/* line: 502 */                   }
/* line: 503, address: 0xfffffff00893eb20 */                   a4 = v103;
/* line: 504, address: 0xfffffff00893eb28 */                   sub_FFFFFFF008958764(a1: v103);
/* line: 505 */                   sub_FFFFFFF008958174(a1: "AMFI: %s: unsupported OOP-JIT type: %u\n");
/* line: 506 */                 }
/* line: 507 */                 else
/* line: 508 */                 {
/* line: 509, address: 0xfffffff00893eb3c */                   a4 = v103;
/* line: 510, address: 0xfffffff00893eb44 */                   sub_FFFFFFF008958764(a1: v103);
/* line: 511 */                   sub_FFFFFFF008958174(a1: "AMFI: %s: invalid OOP-JIT type\n");
/* line: 512 */                 }
/* line: 513, address: 0xfffffff00893eb58 */                 v107 = 0;
/* line: 514, address: 0xfffffff00893eb5c */                 LODWORD(v31) = 0;
/* line: 515, address: 0xfffffff00893eb64 */                 goto LABEL_191;
/* line: 516 */               }
/* line: 517, address: 0xfffffff00893e82c */               *v108 &= 0xF3FFBFFF;
/* line: 518, address: 0xfffffff00893e838 */               v122[0] = 0;
/* line: 519 */               if ( (unsigned int)sub_FFFFFFF00893ABAC(a1: v122) != 0 )
/* line: 520 */               {
/* line: 521, address: 0xfffffff00893e84c */                 v16 = v69;
/* line: 522 */                 sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: no registered daemon port\n");
/* line: 523, address: 0xfffffff00893e860 */                 goto LABEL_117;
/* line: 524 */               }
/* line: 525, address: 0xfffffff00893e924 */               v98 = (unsigned __int8)v31 & 0x90;
/* line: 526, address: 0xfffffff00893e92c */               v125 = 0u;
/* line: 527, address: 0xfffffff00893e92c */               v126 = 0u;
/* line: 528, address: 0xfffffff00893e930 */               v133 = 0;
/* line: 529, address: 0xfffffff00893e930 */               v134 = 0;
/* line: 530, address: 0xfffffff00893e934 */               v132 = 0;
/* line: 531, address: 0xfffffff00893e938 */               v143 = 0u;
/* line: 532, address: 0xfffffff00893e938 */               v144 = 0u;
/* line: 533, address: 0xfffffff00893e93c */               v141 = 0u;
/* line: 534, address: 0xfffffff00893e93c */               v142 = 0u;
/* line: 535, address: 0xfffffff00893e940 */               v131 = 0;
/* line: 536, address: 0xfffffff00893e950 */               v83 = sub_FFFFFFF008958704(a1: v103, a2: &v131);
/* line: 537, address: 0xfffffff00893e958 */               if ( (*(_BYTE *)(v13 + 1032) & 1) == 0 )
/* line: 538, address: 0xfffffff00893e974 */                 *(_BYTE *)(v13 + 1032) = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v13 + 16LL))(a1: v13);
/* line: 539, address: 0xfffffff00893e978 */               v120[0] = 0;
/* line: 540, address: 0xfffffff00893e97c */               v116 = 0;
/* line: 541, address: 0xfffffff00893e980 */               v112 = 0;
/* line: 542, address: 0xfffffff00893e984 */               v130 = 0;
/* line: 543, address: 0xfffffff00893e988 */               v84 = v122[0];
/* line: 544, address: 0xfffffff00893e990 */               v85 = sub_FFFFFFF0089586F4(a1: v103);
/* line: 545, address: 0xfffffff00893e9c8 */               v101 = v17;
/* line: 546, address: 0xfffffff00893ea08 */               v86 = sub_FFFFFFF008944994(
/* line: 547 */                       a1: v84,
/* line: 548 */                       a2: v102,
/* line: 549 */                       a3: v85,
/* line: 550 */                       a4: a7,
/* line: 551 */                       a5: 0,
/* line: 552 */                       a6: 1,
/* line: 553 */                       a7: 4,
/* line: 554 */                       a8: &v132,
/* line: 555 */                       a9: (char *)&v133 + 4,
/* line: 556 */                       a10: &v133,
/* line: 557 */                       a11: v99,
/* line: 558 */                       a12: (char *)&v134 + 4,
/* line: 559 */                       a13: &v134,
/* line: 560 */                       a14: v17,
/* line: 561 */                       a15: &v141,
/* line: 562 */                       a16: (char *)&v131 + 1,
/* line: 563 */                       a17: v120,
/* line: 564 */                       a18: &v116,
/* line: 565 */                       a19: &v112,
/* line: 566 */                       a20: &v130,
/* line: 567 */                       a21: &v125);
/* line: 568, address: 0xfffffff00893ea0c */               v87 = v86;
/* line: 569 */               if ( dword_FFFFFFF00A407338 >= 20 )
/* line: 570 */                 sub_FFFFFFF008958174(
/* line: 571 */                   a1: "callout out to amfid for %s %s, return %d valid: %d isApple: %d\n",
/* line: 572 */                   "",
/* line: 573 */                   v102,
/* line: 574 */                   v86,
/* line: 575 */                   HIDWORD(v133),
/* line: 576 */                   HIDWORD(v134));
/* line: 577 */               if ( v87 == -308 )
/* line: 578 */               {
/* line: 579 */                 sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: verify_code_directory server is dead\n");
/* line: 580 */               }
/* line: 581 */               else
/* line: 582 */               {
/* line: 583 */                 if ( v87 == 268451845 )
/* line: 584 */                 {
/* line: 585 */                   sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: verify_code_directory interrupted\n", 4);
/* line: 586, address: 0xfffffff00893ebe0 */                   goto LABEL_180;
/* line: 587 */                 }
/* line: 588 */                 if ( v87 != 0 )
/* line: 589 */                 {
/* line: 590 */                   sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: verify_code_directory returned 0x%x\n");
/* line: 591 */                 }
/* line: 592 */                 else
/* line: 593 */                 {
/* line: 594, address: 0xfffffff00893ea70 */                   v128 = v125;
/* line: 595, address: 0xfffffff00893ea70 */                   v129 = v126;
/* line: 596 */                   if ( (unsigned int)sub_FFFFFFF00893ABFC(a1: &v128) != 0 )
/* line: 597 */                   {
/* line: 598 */                     if ( HIDWORD(v133) == 1 )
/* line: 599 */                     {
/* line: 600, address: 0xfffffff00893ea8c */                       v88 = (unsigned __int8)v131;
/* line: 601, address: 0xfffffff00893ea98 */                       if ( HIBYTE(v131) == 20 )
/* line: 602 */                       {
/* line: 603, address: 0xfffffff00893eaa0 */                         if ( (unsigned __int8)v131 >= 0x14u )
/* line: 604 */                         {
/* line: 605, address: 0xfffffff00893eaa8 */                           v89 = v83;
/* line: 606, address: 0xfffffff00893eaac */                           v88 = 20;
/* line: 607, address: 0xfffffff00893eab0 */                           goto LABEL_194;
/* line: 608 */                         }
/* line: 609 */                       }
/* line: 610, address: 0xfffffff00893ecdc */                       else if ( (unsigned __int8)v131 == HIBYTE(v131) )
/* line: 611 */                       {
/* line: 612, address: 0xfffffff00893ece4 */                         v89 = v83;
/* line: 613 */ LABEL_194:
/* line: 614, address: 0xfffffff00893ecec */                         if ( (unsigned int)sub_FFFFFFF008958BA4(a1: v89, a2: &v141, a3: v88) == 0 )
/* line: 615, address: 0xfffffff00893ecec */                           goto LABEL_199;
/* line: 616 */                       }
/* line: 617 */                       sub_FFFFFFF008958174(a1: "%s: Possible race detected. Rejecting.\n", v102);
/* line: 618 */                       sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: Unable to match identity\n", 4);
/* line: 619, address: 0xfffffff00893ed24 */                       *v108 &= ~1u;
/* line: 620, address: 0xfffffff00893ed2c */                       sub_FFFFFFF0089428DC(a1: v140);
/* line: 621 */                     }
/* line: 622 */                   }
/* line: 623 */                   else
/* line: 624 */                   {
/* line: 625 */                     sub_FFFFFFF008958174(a1: "StaticPlatformPolicy<%d>: unable to verify audit token came from amfid\n");
/* line: 626 */                   }
/* line: 627 */                 }
/* line: 628 */               }
/* line: 629, address: 0xfffffff00893ec48 */               v16 = v105;
/* line: 630, address: 0xfffffff00893ec4c */               v20 = *((_QWORD *)&a9 + 1);
/* line: 631, address: 0xfffffff00893ec4c */               v72 = (unsigned __int8)v31 & 0x90;
/* line: 632, address: 0xfffffff00893ec50 */               v25 = a9;
/* line: 633, address: 0xfffffff00893ec58 */               goto LABEL_117;
/* line: 634 */             }
/* line: 635, address: 0xfffffff00893eab4 */             v16 = v69;
/* line: 636, address: 0xfffffff00893eac8 */             sub_FFFFFFF008958174(a1: "QA Hierarchy used -> %s\n", v102);
/* line: 637, address: 0xfffffff00893ead8 */             *v108 |= 0x4000u;
/* line: 638, address: 0xfffffff00893eadc */             v82 = "qa-path-appstore";
/* line: 639 */           }
/* line: 640, address: 0xfffffff00893eae4 */           a4 = v103;
/* line: 641, address: 0xfffffff00893eaf0 */           sub_FFFFFFF00893BFD0(a1: v103, a2: 4, a3: v82);
/* line: 642, address: 0xfffffff00893eaf8 */           v107 = 1;
/* line: 643, address: 0xfffffff00893eafc */           LODWORD(v31) = 1;
/* line: 644, address: 0xfffffff00893eb04 */           goto LABEL_119;
/* line: 645 */         }
/* line: 646 */         sub_FFFFFFF008958174(a1: "AMFI: %s: unable to find code directory\n", "_checkForOOPJit");
/* line: 647 */       }
/* line: 648, address: 0xfffffff00893e5a8 */       v70 = false;
/* line: 649, address: 0xfffffff00893e5ac */       v69 = v105;
/* line: 650, address: 0xfffffff00893e5ac */       goto LABEL_111;
/* line: 651 */     }
/* line: 652, address: 0xfffffff00893e2d8 */     *(_QWORD *)&v125 = 0;
/* line: 653, address: 0xfffffff00893e2ec */     LOBYTE(v125) = (unsigned int)sub_FFFFFFF008958264(a1: 0) != 0;
/* line: 654, address: 0xfffffff00893e2f0 */     v54 = v52 + 8;
/* line: 655, address: 0xfffffff00893e2f4 */     *((_QWORD *)&v125 + 1) = v52 + 8;
/* line: 656, address: 0xfffffff00893e300 */     v55 = bswap32(*(_DWORD *)(v52 + 4)) - 8LL;
/* line: 657, address: 0xfffffff00893e304 */     v127 = 0;
/* line: 658, address: 0xfffffff00893e308 */     v126 = (unsigned __int64)v55;
/* line: 659, address: 0xfffffff00893e314 */     v56 = sub_FFFFFFF00894882C(a1: &v141, a2: &v125);
/* line: 660 */     if ( BYTE1(v56) != 0 )
/* line: 661 */     {
/* line: 662 */       if ( BYTE1(v56) != 63 )
/* line: 663 */       {
/* line: 664 */         sub_FFFFFFF008958174(a1: "AMFI: '%s' cmsBlobInit failed: component=%#x error=%#x unique=%#x\n");
/* line: 665, address: 0xfffffff00893e430 */         goto LABEL_74;
/* line: 666 */       }
/* line: 667, address: 0xfffffff00893e328 */       v97 = v52;
/* line: 668, address: 0xfffffff00893e338 */       v57 = sub_FFFFFFF0089413D0(a1: v102, a2: &v125);
/* line: 669 */       if ( v57 == 0 )
/* line: 670 */       {
/* line: 671 */         sub_FFFFFFF008958174(a1: "AMFI: '%s' couldn't fetch certs\n");
/* line: 672, address: 0xfffffff00893e48c */         goto LABEL_74;
/* line: 673 */       }
/* line: 674, address: 0xfffffff00893e340 */       v58 = (unsigned int *)v57;
/* line: 675, address: 0xfffffff00893e350 */       v59 = (unsigned int)sub_FFFFFFF008958264(a1: 0) != 0;
/* line: 676, address: 0xfffffff00893e35c */       v60 = v58[26];
/* line: 677, address: 0xfffffff00893e360 */       v61 = bswap32(*(_DWORD *)(v97 + 4));
/* line: 678, address: 0xfffffff00893e368 */       LOBYTE(v125) = v59;
/* line: 679, address: 0xfffffff00893e370 */       *((_QWORD *)&v125 + 1) = v54;
/* line: 680, address: 0xfffffff00893e370 */       *(_QWORD *)&v126 = v61 - 8;
/* line: 681, address: 0xfffffff00893e374 */       *((_QWORD *)&v126 + 1) = v58 + 10;
/* line: 682, address: 0xfffffff00893e374 */       v127 = v60;
/* line: 683, address: 0xfffffff00893e380 */       v62 = sub_FFFFFFF00894882C(a1: &v141, a2: &v125);
/* line: 684 */       if ( BYTE1(v62) != 0 )
/* line: 685 */       {
/* line: 686 */         sub_FFFFFFF008958174(
/* line: 687 */           a1: "AMFI: '%s' cmsBlobInit failed: component=%#x error=%#x unique=%#x\n",
/* line: 688 */           v102,
/* line: 689 */           (unsigned __int8)v62,
/* line: 690 */           BYTE1(v62),
/* line: 691 */           HIWORD(v62));
/* line: 692, address: 0xfffffff00893e3ac */         v63 = 0;
/* line: 693, address: 0xfffffff00893e3b0 */         v122[0] = 0;
/* line: 694 */ LABEL_104:
/* line: 695, address: 0xfffffff00893e524 */         (*(void (__fastcall **)(unsigned int *))(*(_QWORD *)v58 + 40LL))(a1: v58);
/* line: 696, address: 0xfffffff00893e544 */         if ( (v63 & 1) == 0 )
/* line: 697, address: 0xfffffff00893e544 */           goto LABEL_74;
/* line: 698, address: 0xfffffff00893e544 */         goto LABEL_105;
/* line: 699 */       }
/* line: 700 */     }
/* line: 701 */     else
/* line: 702 */     {
/* line: 703, address: 0xfffffff00893e3d0 */       v58 = nullptr;
/* line: 704 */     }
/* line: 705, address: 0xfffffff00893e3e0 */     v64 = sub_FFFFFFF0089489F4(a1: &v141, a2: v47, a3: v48);
/* line: 706 */     if ( BYTE1(v64) != 0 )
/* line: 707 */     {
/* line: 708 */       sub_FFFFFFF008958174(
/* line: 709 */         a1: "AMFI: '%s' cmsBlobVerify failed: component=%#x error=%#x unique=%u\n",
/* line: 710 */         v102,
/* line: 711 */         (unsigned __int8)v64,
/* line: 712 */         BYTE1(v64),
/* line: 713 */         HIWORD(v64));
/* line: 714, address: 0xfffffff00893e40c */       v122[0] = 0;
/* line: 715 */     }
/* line: 716 */     else
/* line: 717 */     {
/* line: 718, address: 0xfffffff00893e438 */       v65 = sub_FFFFFFF008958754(a1: v103);
/* line: 719, address: 0xfffffff00893e44c */       v66 = sub_FFFFFFF008948B50(a1: &v141, a2: v98, a3: v43, a4: v65);
/* line: 720, address: 0xfffffff00893e454 */       if ( BYTE1(v66) == 0 )
/* line: 721, address: 0xfffffff00893e454 */         goto LABEL_96;
/* line: 722 */       if ( BYTE1(v66) != 36 )
/* line: 723 */       {
/* line: 724 */         if ( BYTE1(v66) == 40 )
/* line: 725 */           sub_FFFFFFF008958174(a1: "AMFI: '%s' cmsBlobVerifyWithAgilityHash unsupported: unique=%#x\n");
/* line: 726 */         else
/* line: 727 */           sub_FFFFFFF008958174(a1: "AMFI: '%s' cmsBlobVerifyWithAgilityHash failed: component=%#x error=%#x unique=%u\n");
/* line: 728, address: 0xfffffff00893e47c */         goto LABEL_102;
/* line: 729 */       }
/* line: 730, address: 0xfffffff00893e49c */       if ( sub_FFFFFFF008958714(a1: v103) == v47 )
/* line: 731 */       {
/* line: 732 */ LABEL_96:
/* line: 733, address: 0xfffffff00893e4bc */         sub_FFFFFFF008948CC0(a1: &v141, a2: v122);
/* line: 734, address: 0xfffffff00893e4c0 */         v128 = 0u;
/* line: 735, address: 0xfffffff00893e4dc */         if ( (sub_FFFFFFF008948C94(a1: &v141, a2: &v128) & 0xFF00) == 0 && *((_QWORD *)&v128 + 1) != 0 )
/* line: 736 */         {
/* line: 737, address: 0xfffffff00893e4e4 */           v67 = sub_FFFFFFF0089585F4();
/* line: 738, address: 0xfffffff00893e4f0 */           sub_FFFFFFF0089585C4(a1: v67, a2: *((_QWORD *)&v128 + 1), a3: v128, a4: v139);
/* line: 739 */         }
/* line: 740, address: 0xfffffff00893e4f4 */         if ( v58 == nullptr )
/* line: 741, address: 0xfffffff00893e4f4 */           goto LABEL_105;
/* line: 742, address: 0xfffffff00893e4f8 */         v63 = 1;
/* line: 743, address: 0xfffffff00893e4fc */         goto LABEL_104;
/* line: 744 */       }
/* line: 745 */       sub_FFFFFFF008958174(a1: "AMFI: '%s' no hash agility and first code directory is not the best one\n");
/* line: 746 */     }
/* line: 747 */ LABEL_102:
/* line: 748, address: 0xfffffff00893e51c */     if ( v58 == nullptr )
/* line: 749, address: 0xfffffff00893e51c */       goto LABEL_74;
/* line: 750, address: 0xfffffff00893e520 */     v63 = 0;
/* line: 751, address: 0xfffffff00893e520 */     goto LABEL_104;
/* line: 752 */   }
/* line: 753, address: 0xfffffff00893ddec */   *v108 |= 0x4000000u;
/* line: 754, address: 0xfffffff00893de04 */   if ( (unsigned int)sub_FFFFFFF008934734(a1: 3, a2: v17, a3: (unsigned __int8)v111, a4: 0) != 0 )
/* line: 755, address: 0xfffffff00893de18 */     *v108 |= 0x2200u;
/* line: 756, address: 0xfffffff00893de2c */   sub_FFFFFFF00893BFD0(a1: a4, a2: 1, a3: "trust-cache");
/* line: 757, address: 0xfffffff00893de44 */   if ( v21 == 0 || (v26 = sub_FFFFFFF008944374(a1: v21)) == 0 )
/* line: 758, address: 0xfffffff00893de50 */     v26 = sub_FFFFFFF008958354(a1: 0);
/* line: 759, address: 0xfffffff00893de7c */   v27 = sub_FFFFFFF00893FF44(
/* line: 760 */           a1: v13,
/* line: 761 */           a2: a4,
/* line: 762 */           a3: (unsigned int)*v108,
/* line: 763 */           a4: v26,
/* line: 764 */           a5: "in-kernel",
/* line: 765 */           a6: a9,
/* line: 766 */           a7: *((_QWORD *)&a9 + 1));
/* line: 767, address: 0xfffffff00893de9c */   (*(void (__fastcall **)(__int64))(*(_QWORD *)v26 + 40LL))(a1: v26);
/* line: 768, address: 0xfffffff00893dea4 */   v24 = sub_FFFFFFF008942104(a1: v17);
/* line: 769, address: 0xfffffff00893dea8 */   if ( (v24 & 1) != 0 )
/* line: 770 */   {
/* line: 771, address: 0xfffffff00893deac */     v28 = "Bad things happened.";
/* line: 772, address: 0xfffffff00893deb4 */     v29 = v13;
/* line: 773, address: 0xfffffff00893deb8 */     v30 = a9;
/* line: 774 */ LABEL_29:
/* line: 775, address: 0xfffffff00893ded8 */     v22 = *((_QWORD *)&a9 + 1);
/* line: 776, address: 0xfffffff00893dee0 */     v24 = sub_FFFFFFF00893C038(a1: v29, a2: v30, a3: *((_QWORD *)&a9 + 1), a4: v28);
/* line: 777, address: 0xfffffff00893dee4 */     v17 = 0;
/* line: 778, address: 0xfffffff00893dee4 */     goto LABEL_30;
/* line: 779 */   }
/* line: 780, address: 0xfffffff00893e1cc */   v22 = *((_QWORD *)&a9 + 1);
/* line: 781, address: 0xfffffff00893e1d0 */   v17 = 0;
/* line: 782 */   while ( 1 )
/* line: 783 */   {
/* line: 784 */ LABEL_34:
/* line: 785, address: 0xfffffff00893df1c */     v31 = v110;
/* line: 786, address: 0xfffffff00893df20 */     if ( v110 != 0 )
/* line: 787 */     {
/* line: 788, address: 0xfffffff00893df50 */       if ( (unsigned int)sub_FFFFFFF008943214(a1: v110, a2: "com.apple.developer.cs.allow-jit") != 0
/* line: 789 */         && (sub_FFFFFFF008943214(a1: v31, a2: "com.apple.developer.web-browser-engine.webcontent") & 1) == 0 )
/* line: 790 */       {
/* line: 791, address: 0xfffffff00893dfe8 */         v36 = "%s disallowed without %s";
/* line: 792 */ LABEL_53:
/* line: 793, address: 0xfffffff00893e000 */         sub_FFFFFFF00893C038(a1: v13, a2: a9, a3: v22, a4: v36);
/* line: 794, address: 0xfffffff00893e010 */         v35 = 1;
/* line: 795, address: 0xfffffff00893e014 */         v32 = v108;
/* line: 796 */ LABEL_54:
/* line: 797, address: 0xfffffff00893e018 */         v24 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v31 + 40LL))(a1: v31);
/* line: 798, address: 0xfffffff00893e034 */         goto LABEL_55;
/* line: 799 */       }
/* line: 800, address: 0xfffffff00893df64 */       v24 = sub_FFFFFFF008943214(a1: v31, a2: "dynamic-codesigning");
/* line: 801, address: 0xfffffff00893df68 */       if ( (_DWORD)v24 != 0 )
/* line: 802 */       {
/* line: 803, address: 0xfffffff00893df70 */         v24 = sub_FFFFFFF0089587C4(a1: a4);
/* line: 804, address: 0xfffffff00893df78 */         if ( (_DWORD)v24 != 1 )
/* line: 805 */         {
/* line: 806, address: 0xfffffff00893dff8 */           v36 = "%s disallowed on non-platform process";
/* line: 807, address: 0xfffffff00893dff8 */           goto LABEL_53;
/* line: 808 */         }
/* line: 809 */       }
/* line: 810 */     }
/* line: 811, address: 0xfffffff00893df7c */     if ( v27 == 0 )
/* line: 812 */     {
/* line: 813, address: 0xfffffff00893dfa0 */       v35 = 1;
/* line: 814, address: 0xfffffff00893dfa4 */       v32 = v108;
/* line: 815, address: 0xfffffff00893dfa8 */       if ( v31 != 0 )
/* line: 816, address: 0xfffffff00893dfa8 */         goto LABEL_54;
/* line: 817, address: 0xfffffff00893dfa8 */       goto LABEL_55;
/* line: 818 */     }
/* line: 819, address: 0xfffffff00893df80 */     v32 = v108;
/* line: 820, address: 0xfffffff00893df84 */     v33 = *v108;
/* line: 821, address: 0xfffffff00893df88 */     if ( (*v108 & 0x4000000) != 0 )
/* line: 822 */     {
/* line: 823, address: 0xfffffff00893dfb0 */       v34 = true;
/* line: 824 */     }
/* line: 825 */     else
/* line: 826 */     {
/* line: 827, address: 0xfffffff00893df90 */       if ( (unsigned int)sub_FFFFFFF008936EAC(a1: v24) == 0 )
/* line: 828 */       {
/* line: 829, address: 0xfffffff00893e1bc */         v24 = sub_FFFFFFF00893C038(
/* line: 830 */                 a1: v13,
/* line: 831 */                 a2: a9,
/* line: 832 */                 a3: v22,
/* line: 833 */                 a4: "The system only allows platform binaries until developer mode status has been resolved, and the code is "
/* line: 834 */                 "not a platform binary\n");
/* line: 835, address: 0xfffffff00893e1c0 */         v35 = 1;
/* line: 836, address: 0xfffffff00893e1c4 */         if ( v31 != 0 )
/* line: 837, address: 0xfffffff00893e1c4 */           goto LABEL_54;
/* line: 838, address: 0xfffffff00893e1c4 */         goto LABEL_55;
/* line: 839 */       }
/* line: 840, address: 0xfffffff00893df94 */       v33 = *v108;
/* line: 841, address: 0xfffffff00893df98 */       v34 = (*v108 & 0x4000000) != 0;
/* line: 842 */     }
/* line: 843, address: 0xfffffff00893dfb8 */     *v108 = v33 | 0x20000000;
/* line: 844, address: 0xfffffff00893dfc0 */     if ( v31 != 0 && v34 )
/* line: 845, address: 0xfffffff00893dfc8 */       sub_FFFFFFF008942DB0(a1: v31);
/* line: 846, address: 0xfffffff00893dfd4 */     v24 = sub_FFFFFFF008932974(a1: v31, a2: a4);
/* line: 847, address: 0xfffffff00893dfd8 */     v35 = 0;
/* line: 848, address: 0xfffffff00893dfdc */     if ( v31 != 0 )
/* line: 849, address: 0xfffffff00893dfdc */       goto LABEL_54;
/* line: 850 */ LABEL_55:
/* line: 851, address: 0xfffffff00893e038 */     if ( v17 != 0 )
/* line: 852, address: 0xfffffff00893e048 */       v24 = sub_FFFFFFF008958114(a1: &unk_FFFFFFF007BCD0C8, a2: v17);
/* line: 853, address: 0xfffffff00893e05c */     if ( (v16 & ~*v32 & 0xFFFFF7FE) != 0 )
/* line: 854, address: 0xfffffff00893ed8c */       sub_FFFFFFF008957498(a1: v24);
/* line: 855, address: 0xfffffff00893e074 */     v37 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v13 + 8LL))(a1: v13);
/* line: 856, address: 0xfffffff00893e08c */     if ( qword_FFFFFFF00A3A8000 == v154 )
/* line: 857, address: 0xfffffff00893e0b0 */       return v35;
/* line: 858, address: 0xfffffff00893ed90 */     sub_FFFFFFF008958514(a1: v37);
/* line: 859 */ LABEL_199:
/* line: 860, address: 0xfffffff00893ed98 */     if ( HIDWORD(v134) != 0 )
/* line: 861, address: 0xfffffff00893ed98 */       break;
/* line: 862, address: 0xfffffff00893eda0 */     if ( (_DWORD)v134 != 0 )
/* line: 863, address: 0xfffffff00893edb0 */       *v108 |= 0x40000000u;
/* line: 864, address: 0xfffffff00893edb8 */     if ( (_DWORD)v133 != 0 )
/* line: 865, address: 0xfffffff00893edc8 */       *v108 &= ~0x800u;
/* line: 866, address: 0xfffffff00893edd0 */     if ( v132 != 0 )
/* line: 867, address: 0xfffffff00893ede4 */       *v108 |= 0x4200u;
/* line: 868 */     if ( (v31 & 0x10000001EE80LL) != 0
/* line: 869 */       && (*(_QWORD *)&v137 = v120[0],
/* line: 870 */           *((_QWORD *)&v137 + 1) = v116,
/* line: 871 */           *(_QWORD *)&v138 = v112,
/* line: 872 */           *((_QWORD *)&v138 + 1) = v130,
/* line: 873 */           (unsigned int)sub_FFFFFFF00893B95C(a1: &v135) != 0) )
/* line: 874 */     {
/* line: 875 */ LABEL_180:
/* line: 876, address: 0xfffffff00893ebe4 */       LODWORD(v31) = 0;
/* line: 877, address: 0xfffffff00893ebec */       v107 = 1;
/* line: 878 */     }
/* line: 879 */     else
/* line: 880 */     {
/* line: 881 */       if ( (unsigned int)sub_FFFFFFF0089587C4(a1: v103) == 0 )
/* line: 882 */       {
/* line: 883, address: 0xfffffff00893ee3c */         if ( (v31 & 0x20) != 0 )
/* line: 884, address: 0xfffffff00893ee3c */           v95 = 6;
/* line: 885 */         else
/* line: 886, address: 0xfffffff00893ee3c */           v95 = 3;
/* line: 887 */         if ( (v31 & 0xB0) == 0 )
/* line: 888 */         {
/* line: 889 */           if ( (v31 & 0x10000) != 0 )
/* line: 890 */           {
/* line: 891, address: 0xfffffff00893ee64 */             v95 = 5;
/* line: 892 */           }
/* line: 893 */           else if ( (v31 & 0x400) != 0 )
/* line: 894 */           {
/* line: 895 */             sub_FFFFFFF008958174(a1: "%s: Signature meets iphone VPN Prod policy\n", v102);
/* line: 896, address: 0xfffffff00893ee80 */             v95 = 10;
/* line: 897 */           }
/* line: 898, address: 0xfffffff00893ee54 */           else if ( (v31 & 0xC000) != 0 )
/* line: 899 */           {
/* line: 900, address: 0xfffffff00893ee58 */             v95 = 2;
/* line: 901 */           }
/* line: 902, address: 0xfffffff00893ee9c */           else if ( (v31 & 0x180000003300LL) != 0 )
/* line: 903 */           {
/* line: 904, address: 0xfffffff00893ee9c */             v95 = 4;
/* line: 905 */           }
/* line: 906 */           else
/* line: 907 */           {
/* line: 908, address: 0xfffffff00893ee9c */             v95 = 10;
/* line: 909 */           }
/* line: 910 */         }
/* line: 911, address: 0xfffffff00893eeac */         sub_FFFFFFF00893BFD0(a1: v103, a2: v95, a3: "amfid_validated");
/* line: 912 */       }
/* line: 913, address: 0xfffffff00893eeb4 */       v107 = 1;
/* line: 914, address: 0xfffffff00893eeb8 */       LODWORD(v31) = 1;
/* line: 915 */     }
/* line: 916, address: 0xfffffff00893ebf4 */     a4 = v103;
/* line: 917, address: 0xfffffff00893ebf8 */     v16 = v105;
/* line: 918, address: 0xfffffff00893ebfc */     v20 = v97;
/* line: 919, address: 0xfffffff00893ebfc */     v72 = v98;
/* line: 920, address: 0xfffffff00893ec00 */     v25 = a9;
/* line: 921, address: 0xfffffff00893ec04 */     v17 = v101;
/* line: 922 */ LABEL_119:
/* line: 923, address: 0xfffffff00893e62c */     v73 = v100 ^ 1;
/* line: 924, address: 0xfffffff00893e638 */     if ( v72 == 0 )
/* line: 925, address: 0xfffffff00893e638 */       v73 = 1;
/* line: 926, address: 0xfffffff00893e63c */     if ( (v73 & 1) == 0 )
/* line: 927 */     {
/* line: 928, address: 0xfffffff00893e648 */       if ( qword_FFFFFFF00A4DF938 != 0 )
/* line: 929, address: 0xfffffff00893e670 */         v74 = qword_FFFFFFF00A4DF938;
/* line: 930 */       else
/* line: 931, address: 0xfffffff00893e748 */         v74 = 0;
/* line: 932, address: 0xfffffff00893e74c */       sub_FFFFFFF0089333A4(a1: v74);
/* line: 933 */     }
/* line: 934 */     if ( (_DWORD)v31 == 0 )
/* line: 935 */     {
/* line: 936 */       sub_FFFFFFF008958174(a1: "AMFI: code signature validation failed.\n");
/* line: 937 */       if ( v107 == 0 )
/* line: 938 */       {
/* line: 939 */         sub_FFFFFFF008958174(a1: "AMFI: OOP-JIT validation was unsuccessful.\n");
/* line: 940, address: 0xfffffff00893e7f4 */         v81 = v25;
/* line: 941, address: 0xfffffff00893e7f8 */         v22 = v20;
/* line: 942, address: 0xfffffff00893e800 */         v24 = sub_FFFFFFF00893C038(
/* line: 943 */                 a1: v13,
/* line: 944 */                 a2: v81,
/* line: 945 */                 a3: v20,
/* line: 946 */                 a4: "fatal error detected during OOP-JIT signature validation");
/* line: 947, address: 0xfffffff00893e804 */         goto LABEL_30;
/* line: 948 */       }
/* line: 949 */ LABEL_75:
/* line: 950, address: 0xfffffff00893e28c */       v22 = v20;
/* line: 951, address: 0xfffffff00893e2a0 */       v24 = sub_FFFFFFF00893BFD0(a1: a4, a2: 10, a3: "untrusted");
/* line: 952 */ LABEL_30:
/* line: 953, address: 0xfffffff00893dee8 */       v27 = 0;
/* line: 954, address: 0xfffffff00893deec */       continue;
/* line: 955 */     }
/* line: 956, address: 0xfffffff00893e764 */     if ( v110 == 0 || (v79 = sub_FFFFFFF008944374(a1: v110)) == 0 )
/* line: 957, address: 0xfffffff00893e770 */       v79 = sub_FFFFFFF008958354(a1: 0);
/* line: 958, address: 0xfffffff00893e790 */     v80 = v25;
/* line: 959, address: 0xfffffff00893e794 */     v22 = v20;
/* line: 960, address: 0xfffffff00893e7a0 */     v27 = sub_FFFFFFF00893FF44(a1: v13, a2: a4, a3: (unsigned int)*v108, a4: v79, a5: "dynamic", a6: v80, a7: v20);
/* line: 961, address: 0xfffffff00893e7c0 */     v24 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)v79 + 40LL))(a1: v79);
/* line: 962 */   }
/* line: 963, address: 0xfffffff00893eef0 */   v96 = sub_FFFFFFF008958C34(
/* line: 964 */           a1: "\"amfid is broken. (%s) (%d) (%d) (%d)\" @%s:%d",
/* line: 965 */           v102,
/* line: 966 */           0,
/* line: 967 */           HIDWORD(v133),
/* line: 968 */           HIDWORD(v134),
/* line: 969 */           "AppleMobileFileIntegrity.cpp",
/* line: 970 */           706);
/* line: 971, address: 0xfffffff00893e0b0 */   return sub_FFFFFFF00893EEF4(a1: v96);
/* line: 972 */ }