/* line: 0, address: 0xfffffff009f79ca8 */ __int64 __fastcall sub_FFFFFFF009F79CA8(
/* line: 1 */         __int64 a1,
/* line: 2 */         __int64 a2,
/* line: 3 */         __int64 a3,
/* line: 4 */         unsigned int a4,
/* line: 5 */         __int64 a5,
/* line: 6 */         __int64 *a6,
/* line: 7 */         __int64 *a7,
/* line: 8 */         __int64 a8)
/* line: 9 */ {
/* line: 10 */   unsigned __int64 StatusReg; // x20
/* line: 11 */   __int64 v16; // x28
/* line: 12 */   unsigned __int64 v17; // x0
/* line: 13 */   __int64 v18; // x22
/* line: 14 */   __int64 *v19; // x26
/* line: 15 */   void *v20; // x21
/* line: 16 */   unsigned __int64 v21; // x8
/* line: 17 */   const void *v22; // x0
/* line: 18 */   int v23; // w8
/* line: 19 */   __int64 v24; // x0
/* line: 20 */   __int64 v25; // x8
/* line: 21 */   __int64 v26; // x9
/* line: 22 */   unsigned int v27; // w10
/* line: 23 */   __int64 v28; // x23
/* line: 24 */   __int64 v29; // x12
/* line: 25 */   _DWORD *v30; // x13
/* line: 26 */   int v31; // w14
/* line: 27 */   __int64 v32; // x15
/* line: 28 */   unsigned __int64 v33; // x17
/* line: 29 */   __int64 v34; // x0
/* line: 30 */   __int64 v35; // x24
/* line: 31 */   __int64 v36; // x0
/* line: 32 */   __int64 v37; // x3
/* line: 33 */   unsigned __int64 v38; // x0
/* line: 34 */   __int64 v39; // x25
/* line: 35 */   unsigned __int64 v40; // x0
/* line: 36 */   unsigned __int64 v41; // x0
/* line: 37 */   unsigned __int64 v42; // x8
/* line: 38 */   const void *v43; // x0
/* line: 39 */   int v44; // w8
/* line: 40 */   unsigned int *v46; // x27
/* line: 41 */   __int64 *v47; // x19
/* line: 42 */   void *v48; // x21
/* line: 43 */   unsigned __int64 v49; // x8
/* line: 44 */   const void *v50; // x0
/* line: 45 */   __int64 *v51; // x19
/* line: 46 */   void *v52; // x21
/* line: 47 */   unsigned __int64 v53; // x8
/* line: 48 */   const void *v54; // x0
/* line: 49 */   int v55; // w8
/* line: 50 */   char v56; // w8
/* line: 51 */   unsigned int v57; // w8
/* line: 52 */   __int64 v58; // x1
/* line: 53 */   __int64 v59; // x0
/* line: 54 */   __int64 v60; // x16
/* line: 55 */   __int64 v61; // x9
/* line: 56 */   __int64 *v62; // x20
/* line: 57 */   __int64 v63; // x0
/* line: 58 */   __int64 *v64; // x8
/* line: 59 */   __int64 v65; // x25
/* line: 60 */   __int64 v66; // x28
/* line: 61 */   __int64 v67; // x8
/* line: 62 */   __int64 v68; // x21
/* line: 63 */   __int64 *v69; // x21
/* line: 64 */   __int64 v70; // t1
/* line: 65 */   __int64 v71; // x9
/* line: 66 */   __int64 v72; // x16
/* line: 67 */   __int64 v73; // x28
/* line: 68 */   __int64 v74; // x1
/* line: 69 */   __int64 v75; // x0
/* line: 70 */   __int64 v76; // x0
/* line: 71 */   __int64 v77; // x0
/* line: 72 */   __int64 v78; // x8
/* line: 73 */   __int64 v79; // x8
/* line: 74 */   __int64 v80; // x0
/* line: 75 */   __int64 v81; // x28
/* line: 76 */   __int64 v82; // x16
/* line: 77 */   __int64 v83; // x8
/* line: 78 */   unsigned __int64 v84; // x8
/* line: 79 */   unsigned __int64 i; // x28
/* line: 80 */   __int64 v86; // x10
/* line: 81 */   unsigned __int64 v87; // x9
/* line: 82 */   __int64 v88; // x8
/* line: 83 */   bool v89; // zf
/* line: 84 */   unsigned __int64 v90; // x21
/* line: 85 */   unsigned __int64 v91; // x24
/* line: 86 */   _QWORD *v92; // x0
/* line: 87 */   __int64 v93; // x8
/* line: 88 */   unsigned __int64 v94; // x0
/* line: 89 */   __int64 v95; // x21
/* line: 90 */   unsigned __int64 v96; // x0
/* line: 91 */   void *v97; // x24
/* line: 92 */   int v98; // w8
/* line: 93 */   unsigned __int64 v99; // x0
/* line: 94 */   const void *v100; // x26
/* line: 95 */   unsigned __int64 v101; // x0
/* line: 96 */   unsigned __int64 v102; // x0
/* line: 97 */   void *v103; // x25
/* line: 98 */   int v104; // w8
/* line: 99 */   unsigned __int64 v105; // x0
/* line: 100 */   __int64 *v106; // x20
/* line: 101 */   const void *v107; // x21
/* line: 102 */   __int64 *v108; // x19
/* line: 103 */   void *v109; // x21
/* line: 104 */   int v110; // w8
/* line: 105 */   const void *v111; // x0
/* line: 106 */   unsigned __int64 v112; // x0
/* line: 107 */   __int64 v113; // x21
/* line: 108 */   unsigned __int64 v114; // x0
/* line: 109 */   unsigned __int64 v115; // x0
/* line: 110 */   unsigned __int64 v116; // x0
/* line: 111 */   __int64 v117; // x19
/* line: 112 */   __int64 v118; // x0
/* line: 113 */   int v119; // w8
/* line: 114 */   unsigned __int64 v120; // x0
/* line: 115 */   unsigned __int64 v121; // x0
/* line: 116 */   __int64 v122; // x21
/* line: 117 */   __int64 v123; // x19
/* line: 118 */   __int64 v124; // x24
/* line: 119 */   __int64 v125; // x23
/* line: 120 */   __int64 *v126; // x19
/* line: 121 */   unsigned __int64 v127; // x0
/* line: 122 */   __int64 v128; // x26
/* line: 123 */   unsigned __int64 v129; // x21
/* line: 124 */   char v130; // w0
/* line: 125 */   __int64 v131; // x8
/* line: 126 */   int v132; // w23
/* line: 127 */   const char *v133; // x19
/* line: 128 */   __int64 v134; // x21
/* line: 129 */   int v135; // w8
/* line: 130 */   const void *v136; // x0
/* line: 131 */   __int64 v137; // x9
/* line: 132 */   unsigned __int64 v138; // x10
/* line: 133 */   __int64 v139; // x24
/* line: 134 */   __int64 v140; // x23
/* line: 135 */   __int64 *v141; // x19
/* line: 136 */   void *v142; // x21
/* line: 137 */   int v143; // w8
/* line: 138 */   const void *v144; // x0
/* line: 139 */   void *v145; // x21
/* line: 140 */   int v146; // w8
/* line: 141 */   const void *v147; // x0
/* line: 142 */   void *v148; // x25
/* line: 143 */   int v149; // w8
/* line: 144 */   unsigned __int64 v150; // x0
/* line: 145 */   const void *v151; // x21
/* line: 146 */   void *v152; // x21
/* line: 147 */   int v153; // w8
/* line: 148 */   const void *v154; // x0
/* line: 149 */   void *v155; // x21
/* line: 150 */   int v156; // w8
/* line: 151 */   const void *v157; // x0
/* line: 152 */   void *v158; // x25
/* line: 153 */   int v159; // w8
/* line: 154 */   unsigned __int64 v160; // x0
/* line: 155 */   __int64 v161; // x24
/* line: 156 */   const void *v162; // x26
/* line: 157 */   int v163; // w19
/* line: 158 */   __int64 v164; // x26
/* line: 159 */   int v165; // w8
/* line: 160 */   const void *v166; // x0
/* line: 161 */   unsigned __int64 v167; // x0
/* line: 162 */   void *v168; // x25
/* line: 163 */   int v169; // w8
/* line: 164 */   unsigned __int64 v170; // x0
/* line: 165 */   const void *v171; // x26
/* line: 166 */   int v172; // w8
/* line: 167 */   unsigned __int64 v173; // x0
/* line: 168 */   const void *v174; // x24
/* line: 169 */   void *v175; // x21
/* line: 170 */   int v176; // w8
/* line: 171 */   const void *v177; // x0
/* line: 172 */   __int64 v178; // x19
/* line: 173 */   __int64 v179; // x19
/* line: 174 */   int v180; // w8
/* line: 175 */   __int64 v181; // x24
/* line: 176 */   __int64 v182; // x8
/* line: 177 */   int v183; // w23
/* line: 178 */   const char *v184; // x28
/* line: 179 */   __int64 v185; // x26
/* line: 180 */   int v186; // w8
/* line: 181 */   const void *v187; // x0
/* line: 182 */   int v188; // w8
/* line: 183 */   int v189; // w8
/* line: 184 */   int v190; // w19
/* line: 185 */   __int64 v191; // x21
/* line: 186 */   int v192; // w8
/* line: 187 */   const void *v193; // x0
/* line: 188 */   unsigned __int64 v194; // x8
/* line: 189 */   __int64 *v195; // x20
/* line: 190 */   int v196; // w24
/* line: 191 */   __int64 v197; // x21
/* line: 192 */   __int64 v198; // x19
/* line: 193 */   unsigned __int64 v199; // x26
/* line: 194 */   unsigned __int64 v200; // x0
/* line: 195 */   const void *v201; // x28
/* line: 196 */   const char *v202; // x27
/* line: 197 */   __int64 v203; // x0
/* line: 198 */   const void *v204; // x0
/* line: 199 */   int v205; // w19
/* line: 200 */   __int64 v206; // x21
/* line: 201 */   int v207; // w8
/* line: 202 */   const void *v208; // x0
/* line: 203 */   int v209; // w20
/* line: 204 */   __int64 v210; // x21
/* line: 205 */   __int64 v211; // x19
/* line: 206 */   unsigned __int64 v212; // x26
/* line: 207 */   unsigned __int64 v213; // x0
/* line: 208 */   const void *v214; // x28
/* line: 209 */   const char *v215; // x27
/* line: 210 */   __int64 v216; // x0
/* line: 211 */   const void *v217; // x0
/* line: 212 */   int v218; // w8
/* line: 213 */   int v219; // w8
/* line: 214 */   int v220; // w8
/* line: 215 */   unsigned __int64 v221; // x0
/* line: 216 */   __int64 *v222; // [xsp+60h] [xbp-450h]
/* line: 217 */   __int64 v223; // [xsp+68h] [xbp-448h]
/* line: 218 */   __int64 *v224; // [xsp+70h] [xbp-440h]
/* line: 219 */   __int64 v225; // [xsp+78h] [xbp-438h]
/* line: 220 */   const void *v226; // [xsp+80h] [xbp-430h]
/* line: 221 */   unsigned __int64 v227; // [xsp+90h] [xbp-420h]
/* line: 222 */   void *v229; // [xsp+98h] [xbp-418h]
/* line: 223 */   void *v230; // [xsp+98h] [xbp-418h]
/* line: 224 */   __int64 *v231; // [xsp+A0h] [xbp-410h]
/* line: 225 */   __int64 v232; // [xsp+A0h] [xbp-410h]
/* line: 226 */   int v233; // [xsp+ACh] [xbp-404h] BYREF
/* line: 227 */   _QWORD v234[8]; // [xsp+B0h] [xbp-400h] BYREF
/* line: 228 */   __int128 v235; // [xsp+F0h] [xbp-3C0h]
/* line: 229 */   __int128 v236; // [xsp+100h] [xbp-3B0h]
/* line: 230 */   __int128 v237; // [xsp+110h] [xbp-3A0h]
/* line: 231 */   __int128 v238; // [xsp+120h] [xbp-390h]
/* line: 232 */   __int128 v239; // [xsp+130h] [xbp-380h]
/* line: 233 */   __int128 v240; // [xsp+140h] [xbp-370h]
/* line: 234 */   __int128 v241; // [xsp+150h] [xbp-360h]
/* line: 235 */   __int128 v242; // [xsp+160h] [xbp-350h]
/* line: 236 */   __int128 v243; // [xsp+170h] [xbp-340h]
/* line: 237 */   __int128 v244; // [xsp+180h] [xbp-330h]
/* line: 238 */   __int128 v245; // [xsp+190h] [xbp-320h]
/* line: 239 */   __int128 v246; // [xsp+1A0h] [xbp-310h]
/* line: 240 */   __int128 v247; // [xsp+1B0h] [xbp-300h]
/* line: 241 */   __int128 v248; // [xsp+1C0h] [xbp-2F0h]
/* line: 242 */   __int128 v249; // [xsp+1D0h] [xbp-2E0h]
/* line: 243 */   __int128 v250; // [xsp+1E0h] [xbp-2D0h]
/* line: 244 */   __int128 v251; // [xsp+1F0h] [xbp-2C0h]
/* line: 245 */   __int128 v252; // [xsp+200h] [xbp-2B0h]
/* line: 246 */   __int128 v253; // [xsp+210h] [xbp-2A0h]
/* line: 247 */   __int128 v254; // [xsp+220h] [xbp-290h]
/* line: 248 */   __int128 v255; // [xsp+230h] [xbp-280h]
/* line: 249 */   __int128 v256; // [xsp+240h] [xbp-270h]
/* line: 250 */   __int128 v257; // [xsp+250h] [xbp-260h]
/* line: 251 */   __int128 v258; // [xsp+260h] [xbp-250h]
/* line: 252 */   __int64 v259; // [xsp+270h] [xbp-240h]
/* line: 253 */   _QWORD v260[2]; // [xsp+280h] [xbp-230h] BYREF
/* line: 254 */   __int128 v261; // [xsp+290h] [xbp-220h]
/* line: 255 */   __int128 v262; // [xsp+2A0h] [xbp-210h]
/* line: 256 */   __int128 v263; // [xsp+2B0h] [xbp-200h]
/* line: 257 */   __int128 v264; // [xsp+2C0h] [xbp-1F0h]
/* line: 258 */   __int128 v265; // [xsp+2D0h] [xbp-1E0h]
/* line: 259 */   __int128 v266; // [xsp+2E0h] [xbp-1D0h]
/* line: 260 */   __int128 v267; // [xsp+2F0h] [xbp-1C0h]
/* line: 261 */   __int128 v268; // [xsp+300h] [xbp-1B0h]
/* line: 262 */   __int128 v269; // [xsp+310h] [xbp-1A0h]
/* line: 263 */   __int128 v270; // [xsp+320h] [xbp-190h]
/* line: 264 */   __int128 v271; // [xsp+330h] [xbp-180h]
/* line: 265 */   __int128 v272; // [xsp+340h] [xbp-170h]
/* line: 266 */   __int128 v273; // [xsp+350h] [xbp-160h]
/* line: 267 */   __int128 v274; // [xsp+360h] [xbp-150h]
/* line: 268 */   __int128 v275; // [xsp+370h] [xbp-140h]
/* line: 269 */   __int128 v276; // [xsp+380h] [xbp-130h]
/* line: 270 */   __int128 v277; // [xsp+390h] [xbp-120h]
/* line: 271 */   __int128 v278; // [xsp+3A0h] [xbp-110h]
/* line: 272 */   __int128 v279; // [xsp+3B0h] [xbp-100h]
/* line: 273 */   __int128 v280; // [xsp+3C0h] [xbp-F0h]
/* line: 274 */   __int128 v281; // [xsp+3D0h] [xbp-E0h]
/* line: 275 */   __int128 v282; // [xsp+3E0h] [xbp-D0h]
/* line: 276 */   __int128 v283; // [xsp+3F0h] [xbp-C0h]
/* line: 277 */   __int128 v284; // [xsp+400h] [xbp-B0h]
/* line: 278 */   __int128 v285; // [xsp+410h] [xbp-A0h]
/* line: 279 */   __int128 v286; // [xsp+420h] [xbp-90h]
/* line: 280 */   __int128 v287; // [xsp+430h] [xbp-80h]
/* line: 281 */   __int64 v288; // [xsp+440h] [xbp-70h]
/* line: 282 */   __int64 v289; // [xsp+448h] [xbp-68h]
/* line: 283 */ 
/* line: 284, address: 0xfffffff009f79cfc */   v289 = qword_FFFFFFF00A3A8000;
/* line: 285, address: 0xfffffff009f79d04 */   v233 = 7;
/* line: 286, address: 0xfffffff009f79d08 */   StatusReg = _ReadStatusReg(TPIDR_EL1);
/* line: 287, address: 0xfffffff009f79d14 */   v16 = sub_FFFFFFF009A43FFC(a1: StatusReg);
/* line: 288 */   if ( dword_FFFFFFF00A38FFEC >= 3 )
/* line: 289 */   {
/* line: 290 */     if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 291 */     {
/* line: 292, address: 0xfffffff009f79d40 */       v17 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 293, address: 0xfffffff009f79dc4 */       if ( (StatusReg & 0x80000000000000LL) != 0 && v17 >= 0xFFFFFFDC00000000LL && v17 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 294, address: 0xfffffff009f79dc8 */         sub_FFFFFFF00998128C(a1: v17);
/* line: 295 */     }
/* line: 296 */     else
/* line: 297 */     {
/* line: 298, address: 0xfffffff009f79dd0 */       v231 = a6;
/* line: 299, address: 0xfffffff009f79dd4 */       v18 = a2;
/* line: 300, address: 0xfffffff009f79dd8 */       v19 = a7;
/* line: 301, address: 0xfffffff009f79de0 */       v20 = off_FFFFFFF00A38FFE0;
/* line: 302, address: 0xfffffff009f79de4 */       v21 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 303, address: 0xfffffff009f79df0 */       if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 304, address: 0xfffffff009f79df0 */         v22 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 305 */       else
/* line: 306, address: 0xfffffff009f79df0 */         v22 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 307, address: 0xfffffff009f7a364 */       if ( (StatusReg & 0x80000000000000LL) != 0 && v21 >= 0xFFFFFFDC00000000LL && v21 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 308, address: 0xfffffff009f7a36c */         v22 = (const void *)sub_FFFFFFF00998128C(a1: StatusReg | 0xFFFFFF8000000000LL);
/* line: 309, address: 0xfffffff009f79e5c */       if ( a1 != 0 )
/* line: 310, address: 0xfffffff009f79e60 */         v23 = *(_DWORD *)(a1 + 96);
/* line: 311 */       else
/* line: 312, address: 0xfffffff009f79e68 */         v23 = 0;
/* line: 313 */       sub_FFFFFFF00A0531B8(
/* line: 314 */         a1: &dword_FFFFFFF007008000,
/* line: 315 */         a2: v20,
/* line: 316 */         a3: 0,
/* line: 317 */         a4: "vm: shared_region: %p [%d(%s)] -> map_and_slide_setup\n",
/* line: 318 */         v22,
/* line: 319 */         v23,
/* line: 320 */         (const char *)(a1 + 1372));
/* line: 321, address: 0xfffffff009f79e94 */       a7 = v19;
/* line: 322, address: 0xfffffff009f79e98 */       a2 = v18;
/* line: 323, address: 0xfffffff009f79e9c */       a6 = v231;
/* line: 324 */     }
/* line: 325 */   }
/* line: 326, address: 0xfffffff009f79ea0 */   v259 = 0;
/* line: 327, address: 0xfffffff009f79ea8 */   v257 = 0u;
/* line: 328, address: 0xfffffff009f79ea8 */   v258 = 0u;
/* line: 329, address: 0xfffffff009f79eac */   v255 = 0u;
/* line: 330, address: 0xfffffff009f79eac */   v256 = 0u;
/* line: 331, address: 0xfffffff009f79eb0 */   v253 = 0u;
/* line: 332, address: 0xfffffff009f79eb0 */   v254 = 0u;
/* line: 333, address: 0xfffffff009f79eb4 */   v251 = 0u;
/* line: 334, address: 0xfffffff009f79eb4 */   v252 = 0u;
/* line: 335, address: 0xfffffff009f79eb8 */   v249 = 0u;
/* line: 336, address: 0xfffffff009f79eb8 */   v250 = 0u;
/* line: 337, address: 0xfffffff009f79ebc */   v247 = 0u;
/* line: 338, address: 0xfffffff009f79ebc */   v248 = 0u;
/* line: 339, address: 0xfffffff009f79ec0 */   v245 = 0u;
/* line: 340, address: 0xfffffff009f79ec0 */   v246 = 0u;
/* line: 341, address: 0xfffffff009f79ec4 */   v243 = 0u;
/* line: 342, address: 0xfffffff009f79ec4 */   v244 = 0u;
/* line: 343, address: 0xfffffff009f79ec8 */   v241 = 0u;
/* line: 344, address: 0xfffffff009f79ec8 */   v242 = 0u;
/* line: 345, address: 0xfffffff009f79ecc */   v239 = 0u;
/* line: 346, address: 0xfffffff009f79ecc */   v240 = 0u;
/* line: 347, address: 0xfffffff009f79ed0 */   v237 = 0u;
/* line: 348, address: 0xfffffff009f79ed0 */   v238 = 0u;
/* line: 349, address: 0xfffffff009f79ed4 */   v235 = 0u;
/* line: 350, address: 0xfffffff009f79ed4 */   v236 = 0u;
/* line: 351, address: 0xfffffff009f79eec */   memset(v234, 0, sizeof(v234));
/* line: 352, address: 0xfffffff009f79efc */   v24 = sub_FFFFFFF0099D1514(a1: &unk_FFFFFFF007A28979, a2: 56LL * (unsigned int)a2, a3: 4, a4: 0);
/* line: 353, address: 0xfffffff009f79f00 */   *a6 = v24;
/* line: 354, address: 0xfffffff009f79f04 */   v232 = a1;
/* line: 355, address: 0xfffffff009f79f08 */   if ( v24 == 0 )
/* line: 356 */   {
/* line: 357, address: 0xfffffff009f7a05c */     v37 = 0;
/* line: 358, address: 0xfffffff009f7a060 */     v39 = 12;
/* line: 359, address: 0xfffffff009f7a064 */     goto LABEL_35;
/* line: 360 */   }
/* line: 361, address: 0xfffffff009f79f0c */   v25 = 0;
/* line: 362, address: 0xfffffff009f79f10 */   v26 = 0;
/* line: 363, address: 0xfffffff009f79f14 */   v27 = 0;
/* line: 364, address: 0xfffffff009f79f20 */   v28 = 56LL * (unsigned int)a2;
/* line: 365, address: 0xfffffff009f79f8c */   do
/* line: 366 */   {
/* line: 367, address: 0xfffffff009f79f34 */     v29 = *a6 + 56 * v26;
/* line: 368, address: 0xfffffff009f79f3c */     v30 = (_DWORD *)(a3 + 12 * v26);
/* line: 369, address: 0xfffffff009f79f44 */     *(_DWORD *)v29 = *v30;
/* line: 370, address: 0xfffffff009f79f48 */     v31 = v30[1];
/* line: 371, address: 0xfffffff009f79f4c */     *(_DWORD *)(v29 + 4) = v31;
/* line: 372, address: 0xfffffff009f79f50 */     v32 = 48LL * v27;
/* line: 373, address: 0xfffffff009f79f54 */     v33 = a5 + v32;
/* line: 374, address: 0xfffffff009f79f60 */     if ( ((a5 ^ (a5 + v32)) & 0xFFC0000000000000LL) != 0 )
/* line: 375, address: 0xfffffff009f79f64 */       v33 = v33 & 0xFFFFFFFFFFFFLL | 0xC8A2000000000000LL;
/* line: 376, address: 0xfffffff009f79f68 */     *(_QWORD *)(v29 + 8) = v33;
/* line: 377, address: 0xfffffff009f79f6c */     v27 += v31;
/* line: 378, address: 0xfffffff009f79f74 */     if ( v27 > a4 )
/* line: 379, address: 0xfffffff009f79f74 */       goto LABEL_33;
/* line: 380, address: 0xfffffff009f79f7c */     *(_DWORD *)(v29 + 16) = v30[2];
/* line: 381, address: 0xfffffff009f79f80 */     ++v26;
/* line: 382, address: 0xfffffff009f79f84 */     v25 += 56;
/* line: 383 */   }
/* line: 384, address: 0xfffffff009f79f8c */   while ( v28 != v25 );
/* line: 385, address: 0xfffffff009f79f94 */   v34 = sub_FFFFFFF009AD9608(a1: v16);
/* line: 386, address: 0xfffffff009f79f98 */   *a7 = v34;
/* line: 387 */   if ( v34 == 0 )
/* line: 388 */   {
/* line: 389 */     if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 390 */     {
/* line: 391, address: 0xfffffff009f7a080 */       a1 = v232;
/* line: 392 */       if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 393 */       {
/* line: 394, address: 0xfffffff009f7a098 */         v40 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 395, address: 0xfffffff009f7a104 */         v37 = 0;
/* line: 396, address: 0xfffffff009f7a108 */         v39 = 22;
/* line: 397, address: 0xfffffff009f7a128 */         if ( (StatusReg & 0x80000000000000LL) != 0 && v40 >= 0xFFFFFFDC00000000LL && v40 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 398 */         {
/* line: 399, address: 0xfffffff009f7a12c */           sub_FFFFFFF00998128C(a1: v40);
/* line: 400, address: 0xfffffff009f7a130 */           v37 = 0;
/* line: 401, address: 0xfffffff009f7a134 */           v39 = 22;
/* line: 402 */         }
/* line: 403 */       }
/* line: 404 */       else
/* line: 405 */       {
/* line: 406, address: 0xfffffff009f7a454 */         v51 = a7;
/* line: 407, address: 0xfffffff009f7a45c */         v52 = off_FFFFFFF00A38FFE0;
/* line: 408, address: 0xfffffff009f7a460 */         v53 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 409, address: 0xfffffff009f7a46c */         if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 410, address: 0xfffffff009f7a46c */           v54 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 411 */         else
/* line: 412, address: 0xfffffff009f7a46c */           v54 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 413, address: 0xfffffff009f7aa68 */         if ( (StatusReg & 0x80000000000000LL) != 0 && v53 >= 0xFFFFFFDC00000000LL && v53 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 414, address: 0xfffffff009f7aa70 */           v54 = (const void *)sub_FFFFFFF00998128C(a1: StatusReg | 0xFFFFFF8000000000LL);
/* line: 415, address: 0xfffffff009f7a4d8 */         if ( v232 != 0 )
/* line: 416, address: 0xfffffff009f7a4dc */           v55 = *(_DWORD *)(v232 + 96);
/* line: 417 */         else
/* line: 418, address: 0xfffffff009f7a9b0 */           v55 = 0;
/* line: 419 */         sub_FFFFFFF00A0531B8(
/* line: 420 */           a1: &dword_FFFFFFF007008000,
/* line: 421 */           a2: v52,
/* line: 422 */           a3: 0,
/* line: 423 */           a4: "vm: shared_region: %p [%d(%s)] map(): no shared region\n",
/* line: 424 */           v54,
/* line: 425 */           v55,
/* line: 426 */           (const char *)(v232 + 1372));
/* line: 427, address: 0xfffffff009f7a9dc */         v37 = 0;
/* line: 428, address: 0xfffffff009f7a9e0 */         v39 = 22;
/* line: 429, address: 0xfffffff009f7a9e4 */         a7 = v51;
/* line: 430 */       }
/* line: 431, address: 0xfffffff009f7a138 */       goto LABEL_35;
/* line: 432 */     }
/* line: 433 */ LABEL_33:
/* line: 434, address: 0xfffffff009f7a13c */     v37 = 0;
/* line: 435, address: 0xfffffff009f7a140 */     v39 = 22;
/* line: 436 */ LABEL_34:
/* line: 437, address: 0xfffffff009f7a144 */     a1 = v232;
/* line: 438, address: 0xfffffff009f7a144 */     goto LABEL_35;
/* line: 439 */   }
/* line: 440, address: 0xfffffff009f79fa0 */   v35 = v34;
/* line: 441, address: 0xfffffff009f79fa4 */   v36 = sub_FFFFFFF009AE359C();
/* line: 442 */   if ( v36 != 0 )
/* line: 443 */   {
/* line: 444 */     if ( a8 != v36 )
/* line: 445 */     {
/* line: 446 */ LABEL_23:
/* line: 447, address: 0xfffffff009f79fb8 */       v37 = v35;
/* line: 448 */       if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 449 */       {
/* line: 450, address: 0xfffffff009f79fd4 */         a1 = v232;
/* line: 451 */         if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 452 */         {
/* line: 453, address: 0xfffffff009f79fe8 */           v38 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 454, address: 0xfffffff009f7a9ec */           v39 = 1;
/* line: 455, address: 0xfffffff009f7aa0c */           if ( (StatusReg & 0x80000000000000LL) != 0 && v38 >= 0xFFFFFFDC00000000LL && v38 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 456 */           {
/* line: 457, address: 0xfffffff009f7aa10 */             sub_FFFFFFF00998128C(a1: v38);
/* line: 458, address: 0xfffffff009f7aa14 */             v37 = v35;
/* line: 459, address: 0xfffffff009f7aa18 */             v39 = 1;
/* line: 460 */           }
/* line: 461 */         }
/* line: 462 */         else
/* line: 463 */         {
/* line: 464, address: 0xfffffff009f7a3ac */           v47 = a7;
/* line: 465, address: 0xfffffff009f7a3b4 */           v48 = off_FFFFFFF00A38FFE0;
/* line: 466, address: 0xfffffff009f7a3b8 */           v49 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 467, address: 0xfffffff009f7a3c4 */           if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 468, address: 0xfffffff009f7a3c4 */             v50 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 469 */           else
/* line: 470, address: 0xfffffff009f7a3c4 */             v50 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 471, address: 0xfffffff009f7aa3c */           if ( (StatusReg & 0x80000000000000LL) != 0 && v49 >= 0xFFFFFFDC00000000LL && v49 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 472, address: 0xfffffff009f7aa44 */             v50 = (const void *)sub_FFFFFFF00998128C(a1: StatusReg | 0xFFFFFF8000000000LL);
/* line: 473 */           sub_FFFFFFF00A0531B8(
/* line: 474 */             a1: &dword_FFFFFFF007008000,
/* line: 475 */             a2: v48,
/* line: 476 */             a3: 0,
/* line: 477 */             a4: "vm: shared_region: map(%p) root_dir mismatch\n",
/* line: 478 */             v50);
/* line: 479, address: 0xfffffff009f7a444 */           v39 = 1;
/* line: 480, address: 0xfffffff009f7a448 */           a7 = v47;
/* line: 481, address: 0xfffffff009f7a44c */           v37 = v35;
/* line: 482 */         }
/* line: 483, address: 0xfffffff009f7aa1c */         goto LABEL_35;
/* line: 484 */       }
/* line: 485, address: 0xfffffff009f7a3a4 */       v39 = 1;
/* line: 486, address: 0xfffffff009f7a3a8 */       goto LABEL_34;
/* line: 487 */     }
/* line: 488 */   }
/* line: 489, address: 0xfffffff009f7a324 */   else if ( a8 != qword_FFFFFFF00A407F90 )
/* line: 490 */   {
/* line: 491, address: 0xfffffff009f7a324 */     goto LABEL_23;
/* line: 492 */   }
/* line: 493, address: 0xfffffff009f7a328 */   v46 = (unsigned int *)*a6;
/* line: 494, address: 0xfffffff009f7a330 */   v224 = (__int64 *)(StatusReg + 1032);
/* line: 495, address: 0xfffffff009f7a330 */   v225 = v35;
/* line: 496, address: 0xfffffff009f7a340 */   v227 = StatusReg;
/* line: 497 */   while ( 1 )
/* line: 498 */   {
/* line: 499, address: 0xfffffff009f7a51c */     v57 = v46[1];
/* line: 500, address: 0xfffffff009f7a520 */     if ( v57 == 0 )
/* line: 501, address: 0xfffffff009f7a520 */       goto LABEL_74;
/* line: 502, address: 0xfffffff009f7a524 */     v58 = *v46;
/* line: 503 */     if ( (_DWORD)v58 == -1 )
/* line: 504 */     {
/* line: 505 */       if ( v57 == 1 )
/* line: 506 */       {
/* line: 507, address: 0xfffffff009f7a5b4 */         v64 = *((__int64 **)v46 + 1);
/* line: 508, address: 0xfffffff009f7a5b8 */         v66 = *v64;
/* line: 509, address: 0xfffffff009f7a5b8 */         v65 = v64[1];
/* line: 510, address: 0xfffffff009f7a5c0 */         v67 = *v224;
/* line: 511, address: 0xfffffff009f7a5c4 */         if ( *v224 == 0 )
/* line: 512 */         {
/* line: 513, address: 0xfffffff009f7a4e8 */           if ( (v66 & 0x3FFF) != 0 )
/* line: 514, address: 0xfffffff009f7a4e8 */             goto LABEL_277;
/* line: 515, address: 0xfffffff009f7a4e8 */           goto LABEL_72;
/* line: 516 */         }
/* line: 517 */         if ( ((unsigned int)v66 & ~(-1 << *(_BYTE *)(v67 + 44))) == 0 )
/* line: 518 */         {
/* line: 519, address: 0xfffffff009f7a8d0 */           if ( v67 != 0 )
/* line: 520 */           {
/* line: 521, address: 0xfffffff009f7a8e8 */             v56 = *(_BYTE *)(v67 + 44);
/* line: 522, address: 0xfffffff009f7a8ec */             goto LABEL_73;
/* line: 523 */           }
/* line: 524 */ LABEL_72:
/* line: 525, address: 0xfffffff009f7a4ec */           v56 = 14;
/* line: 526 */ LABEL_73:
/* line: 527, address: 0xfffffff009f7a500 */           if ( ((unsigned int)v65 & ~(-1 << v56)) == 0 )
/* line: 528, address: 0xfffffff009f7a500 */             goto LABEL_74;
/* line: 529, address: 0xfffffff009f7b1dc */           if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 530, address: 0xfffffff009f7b1dc */             goto LABEL_287;
/* line: 531, address: 0xfffffff009f7b1e8 */           v139 = v232;
/* line: 532, address: 0xfffffff009f7b1ec */           v140 = v225;
/* line: 533, address: 0xfffffff009f7b1fc */           if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 534, address: 0xfffffff009f7b1fc */             goto LABEL_285;
/* line: 535, address: 0xfffffff009f7b4e8 */           v141 = a7;
/* line: 536, address: 0xfffffff009f7b4f0 */           v175 = off_FFFFFFF00A38FFE0;
/* line: 537, address: 0xfffffff009f7b4fc */           v176 = sub_FFFFFFF009F771E4(a1: StatusReg);
/* line: 538, address: 0xfffffff009f7b500 */           v177 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 539, address: 0xfffffff009f7b504 */           if ( v176 != 0 )
/* line: 540 */           {
/* line: 541, address: 0xfffffff009f7b510 */             if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 542, address: 0xfffffff009f7b510 */               v177 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 543, address: 0xfffffff009f7b514 */             v177 = (const void *)sub_FFFFFFF009B3BF1C(a1: v177);
/* line: 544 */           }
/* line: 545, address: 0xfffffff009f7b768 */           else if ( (StatusReg & 0x80000000000000LL) != 0
/* line: 546 */                  && (unsigned __int64)v177 >= 0xFFFFFFDC00000000LL
/* line: 547 */                  && (unsigned __int64)v177 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 548 */           {
/* line: 549, address: 0xfffffff009f7b76c */             v177 = (const void *)sub_FFFFFFF00998128C(a1: v177);
/* line: 550 */           }
/* line: 551, address: 0xfffffff009f7be94 */           else if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 552 */           {
/* line: 553, address: 0xfffffff009f7be94 */             v177 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 554 */           }
/* line: 555, address: 0xfffffff009f7bea4 */           if ( v232 != 0 )
/* line: 556, address: 0xfffffff009f7bea8 */             v220 = *(_DWORD *)(v232 + 96);
/* line: 557 */           else
/* line: 558, address: 0xfffffff009f7beb0 */             v220 = 0;
/* line: 559 */           sub_FFFFFFF00A0531B8(
/* line: 560 */             a1: &dword_FFFFFFF007008000,
/* line: 561 */             a2: v175,
/* line: 562 */             a3: 0,
/* line: 563 */             a4: "vm: shared_region: %p [%d(%s)] map data size 0x%llx not aligned\n",
/* line: 564 */             v177,
/* line: 565 */             v220,
/* line: 566 */             (const char *)(v232 + 1372),
/* line: 567 */             v65);
/* line: 568 */ LABEL_560:
/* line: 569, address: 0xfffffff009f7bedc */           v39 = 22;
/* line: 570, address: 0xfffffff009f7bee0 */           a7 = v141;
/* line: 571 */ LABEL_561:
/* line: 572, address: 0xfffffff009f7bee4 */           v37 = v140;
/* line: 573, address: 0xfffffff009f7bee8 */           a1 = v139;
/* line: 574, address: 0xfffffff009f7beec */           goto LABEL_35;
/* line: 575 */         }
/* line: 576 */ LABEL_277:
/* line: 577, address: 0xfffffff009f7b178 */         if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 578, address: 0xfffffff009f7b178 */           goto LABEL_287;
/* line: 579, address: 0xfffffff009f7b184 */         v139 = v232;
/* line: 580, address: 0xfffffff009f7b188 */         v140 = v225;
/* line: 581 */         if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 582 */         {
/* line: 583, address: 0xfffffff009f7b19c */           v141 = a7;
/* line: 584, address: 0xfffffff009f7b1a4 */           v145 = off_FFFFFFF00A38FFE0;
/* line: 585, address: 0xfffffff009f7b1b0 */           v146 = sub_FFFFFFF009F771E4(a1: StatusReg);
/* line: 586, address: 0xfffffff009f7b1b4 */           v147 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 587, address: 0xfffffff009f7b1b8 */           if ( v146 != 0 )
/* line: 588 */           {
/* line: 589, address: 0xfffffff009f7b1c4 */             if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 590, address: 0xfffffff009f7b1c4 */               v147 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 591, address: 0xfffffff009f7b1c8 */             v147 = (const void *)sub_FFFFFFF009B3BF1C(a1: v147);
/* line: 592 */           }
/* line: 593, address: 0xfffffff009f7b740 */           else if ( (StatusReg & 0x80000000000000LL) != 0
/* line: 594 */                  && (unsigned __int64)v147 >= 0xFFFFFFDC00000000LL
/* line: 595 */                  && (unsigned __int64)v147 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 596 */           {
/* line: 597, address: 0xfffffff009f7b744 */             v147 = (const void *)sub_FFFFFFF00998128C(a1: v147);
/* line: 598 */           }
/* line: 599, address: 0xfffffff009f7be40 */           else if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 600 */           {
/* line: 601, address: 0xfffffff009f7be40 */             v147 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 602 */           }
/* line: 603, address: 0xfffffff009f7be50 */           if ( v232 != 0 )
/* line: 604, address: 0xfffffff009f7be54 */             v219 = *(_DWORD *)(v232 + 96);
/* line: 605 */           else
/* line: 606, address: 0xfffffff009f7be5c */             v219 = 0;
/* line: 607 */           sub_FFFFFFF00A0531B8(
/* line: 608 */             a1: &dword_FFFFFFF007008000,
/* line: 609 */             a2: v145,
/* line: 610 */             a3: 0,
/* line: 611 */             a4: "vm: shared_region: %p [%d(%s)] map data destination 0x%llx not aligned\n",
/* line: 612 */             v147,
/* line: 613 */             v219,
/* line: 614 */             (const char *)(v232 + 1372),
/* line: 615 */             v66);
/* line: 616, address: 0xfffffff009f7be88 */           goto LABEL_560;
/* line: 617 */         }
/* line: 618 */       }
/* line: 619 */       else
/* line: 620 */       {
/* line: 621, address: 0xfffffff009f7b114 */         if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 622, address: 0xfffffff009f7b114 */           goto LABEL_287;
/* line: 623, address: 0xfffffff009f7b120 */         v139 = v232;
/* line: 624, address: 0xfffffff009f7b124 */         v140 = v225;
/* line: 625 */         if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 626 */         {
/* line: 627, address: 0xfffffff009f7b138 */           v141 = a7;
/* line: 628, address: 0xfffffff009f7b140 */           v142 = off_FFFFFFF00A38FFE0;
/* line: 629, address: 0xfffffff009f7b14c */           v143 = sub_FFFFFFF009F771E4(a1: StatusReg);
/* line: 630, address: 0xfffffff009f7b150 */           v144 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 631, address: 0xfffffff009f7b154 */           if ( v143 != 0 )
/* line: 632 */           {
/* line: 633, address: 0xfffffff009f7b160 */             if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 634, address: 0xfffffff009f7b160 */               v144 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 635, address: 0xfffffff009f7b164 */             v144 = (const void *)sub_FFFFFFF009B3BF1C(a1: v144);
/* line: 636 */           }
/* line: 637, address: 0xfffffff009f7b718 */           else if ( (StatusReg & 0x80000000000000LL) != 0
/* line: 638 */                  && (unsigned __int64)v144 >= 0xFFFFFFDC00000000LL
/* line: 639 */                  && (unsigned __int64)v144 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 640 */           {
/* line: 641, address: 0xfffffff009f7b71c */             v144 = (const void *)sub_FFFFFFF00998128C(a1: v144);
/* line: 642 */           }
/* line: 643, address: 0xfffffff009f7bdec */           else if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 644 */           {
/* line: 645, address: 0xfffffff009f7bdec */             v144 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 646 */           }
/* line: 647, address: 0xfffffff009f7bdfc */           if ( v232 != 0 )
/* line: 648, address: 0xfffffff009f7be00 */             v218 = *(_DWORD *)(v232 + 96);
/* line: 649 */           else
/* line: 650, address: 0xfffffff009f7be08 */             v218 = 0;
/* line: 651 */           sub_FFFFFFF00A0531B8(
/* line: 652 */             a1: &dword_FFFFFFF007008000,
/* line: 653 */             a2: v142,
/* line: 654 */             a3: 0,
/* line: 655 */             a4: "vm: shared_region: %p [%d(%s)] map data >1 mapping\n",
/* line: 656 */             v144,
/* line: 657 */             v218,
/* line: 658 */             (const char *)(v232 + 1372));
/* line: 659, address: 0xfffffff009f7be34 */           goto LABEL_560;
/* line: 660 */         }
/* line: 661 */       }
/* line: 662 */ LABEL_285:
/* line: 663, address: 0xfffffff009f7b208 */       if ( (sub_FFFFFFF009F771E4(a1: StatusReg) & 1) != 0 )
/* line: 664 */       {
/* line: 665, address: 0xfffffff009f7b20c */         v39 = 22;
/* line: 666 */       }
/* line: 667 */       else
/* line: 668 */       {
/* line: 669, address: 0xfffffff009f7b404 */         v39 = 22;
/* line: 670, address: 0xfffffff009f7b408 */         if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 671 */         {
/* line: 672, address: 0xfffffff009f7b40c */           v167 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 673, address: 0xfffffff009f7b428 */           if ( (StatusReg | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v167 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 674 */           {
/* line: 675, address: 0xfffffff009f7b42c */             sub_FFFFFFF00998128C(a1: v167);
/* line: 676, address: 0xfffffff009f7b430 */             v39 = 22;
/* line: 677 */           }
/* line: 678 */         }
/* line: 679 */       }
/* line: 680, address: 0xfffffff009f7b210 */       goto LABEL_561;
/* line: 681 */     }
/* line: 682, address: 0xfffffff009f7a540 */     v59 = sub_FFFFFFF009E8C5C4(a1: v232, a2: v58, a3: 1, a4: 22, a5: v46 + 6);
/* line: 683 */     if ( (_DWORD)v59 != 0 )
/* line: 684 */     {
/* line: 685, address: 0xfffffff009f7aba4 */       v39 = v59;
/* line: 686, address: 0xfffffff009f7abb4 */       if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 687, address: 0xfffffff009f7abb4 */         goto LABEL_288;
/* line: 688, address: 0xfffffff009f7abc0 */       a1 = v232;
/* line: 689 */       if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 690 */       {
/* line: 691, address: 0xfffffff009f7abd4 */         v108 = a7;
/* line: 692, address: 0xfffffff009f7abdc */         v109 = off_FFFFFFF00A38FFE0;
/* line: 693, address: 0xfffffff009f7abe8 */         v110 = sub_FFFFFFF009F771E4(a1: StatusReg);
/* line: 694, address: 0xfffffff009f7abec */         v111 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 695, address: 0xfffffff009f7abf0 */         if ( v110 != 0 )
/* line: 696 */         {
/* line: 697, address: 0xfffffff009f7abfc */           if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 698, address: 0xfffffff009f7abfc */             v111 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 699, address: 0xfffffff009f7ac00 */           v111 = (const void *)sub_FFFFFFF009B3BF1C(a1: v111);
/* line: 700 */         }
/* line: 701, address: 0xfffffff009f7b538 */         else if ( (StatusReg & 0x80000000000000LL) != 0
/* line: 702 */                && (unsigned __int64)v111 >= 0xFFFFFFDC00000000LL
/* line: 703 */                && (unsigned __int64)v111 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 704 */         {
/* line: 705, address: 0xfffffff009f7b53c */           v111 = (const void *)sub_FFFFFFF00998128C(a1: v111);
/* line: 706 */         }
/* line: 707, address: 0xfffffff009f7b7ac */         else if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 708 */         {
/* line: 709, address: 0xfffffff009f7b7ac */           v111 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 710 */         }
/* line: 711, address: 0xfffffff009f7b7bc */         if ( v232 != 0 )
/* line: 712, address: 0xfffffff009f7b7c0 */           v180 = *(_DWORD *)(v232 + 96);
/* line: 713 */         else
/* line: 714, address: 0xfffffff009f7b7c8 */           v180 = 0;
/* line: 715 */         sub_FFFFFFF00A0531B8(
/* line: 716 */           a1: &dword_FFFFFFF007008000,
/* line: 717 */           a2: v109,
/* line: 718 */           a3: 0,
/* line: 719 */           a4: "vm: shared_region: %p [%d(%s)] map: fd=%d lookup failed (error=%d)\n",
/* line: 720 */           v111,
/* line: 721 */           v180,
/* line: 722 */           (const char *)(v232 + 1372),
/* line: 723 */           *v46,
/* line: 724 */           v39);
/* line: 725, address: 0xfffffff009f7b800 */         goto LABEL_443;
/* line: 726 */       }
/* line: 727, address: 0xfffffff009f7abd0 */       goto LABEL_207;
/* line: 728 */     }
/* line: 729, address: 0xfffffff009f7a54c */     v60 = *(_QWORD *)(*((_QWORD *)v46 + 3) + 16LL);
/* line: 730 */     if ( (*(_BYTE *)(v60 + 16) & 1) == 0 )
/* line: 731 */     {
/* line: 732 */       if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 733 */       {
/* line: 734, address: 0xfffffff009f7acc8 */         a1 = v232;
/* line: 735, address: 0xfffffff009f7acd8 */         if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 736 */         {
/* line: 737, address: 0xfffffff009f7ace4 */           v39 = 1;
/* line: 738, address: 0xfffffff009f7ace8 */           if ( (sub_FFFFFFF009F771E4(a1: StatusReg) & 1) != 0 )
/* line: 739, address: 0xfffffff009f7ace8 */             goto LABEL_235;
/* line: 740, address: 0xfffffff009f7acec */           v37 = v225;
/* line: 741, address: 0xfffffff009f7acf0 */           if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 742, address: 0xfffffff009f7acf0 */             goto LABEL_35;
/* line: 743, address: 0xfffffff009f7acf4 */           v115 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 744, address: 0xfffffff009f7ad10 */           if ( (StatusReg | 0xFFFFFF8000000000LL) < 0xFFFFFFDC00000000LL || v115 > 0xFFFFFFFBFFFFFFFFLL )
/* line: 745, address: 0xfffffff009f7ad10 */             goto LABEL_35;
/* line: 746 */ LABEL_234:
/* line: 747, address: 0xfffffff009f7aef4 */           v123 = v37;
/* line: 748, address: 0xfffffff009f7aef8 */           sub_FFFFFFF00998128C(a1: v115);
/* line: 749, address: 0xfffffff009f7aefc */           v37 = v123;
/* line: 750, address: 0xfffffff009f7af00 */           v39 = 1;
/* line: 751, address: 0xfffffff009f7af04 */           goto LABEL_35;
/* line: 752 */         }
/* line: 753, address: 0xfffffff009f7b29c */         v108 = a7;
/* line: 754, address: 0xfffffff009f7b2a4 */         v152 = off_FFFFFFF00A38FFE0;
/* line: 755, address: 0xfffffff009f7b2b0 */         v153 = sub_FFFFFFF009F771E4(a1: StatusReg);
/* line: 756, address: 0xfffffff009f7b2b4 */         v154 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 757, address: 0xfffffff009f7b2b8 */         if ( v153 != 0 )
/* line: 758 */         {
/* line: 759, address: 0xfffffff009f7b2c4 */           if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 760, address: 0xfffffff009f7b2c4 */             v154 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 761, address: 0xfffffff009f7b2c8 */           v154 = (const void *)sub_FFFFFFF009B3BF1C(a1: v154);
/* line: 762 */         }
/* line: 763, address: 0xfffffff009f7b58c */         else if ( (StatusReg & 0x80000000000000LL) != 0
/* line: 764 */                && (unsigned __int64)v154 >= 0xFFFFFFDC00000000LL
/* line: 765 */                && (unsigned __int64)v154 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 766 */         {
/* line: 767, address: 0xfffffff009f7b590 */           v154 = (const void *)sub_FFFFFFF00998128C(a1: v154);
/* line: 768 */         }
/* line: 769, address: 0xfffffff009f7b8f4 */         else if ( (StatusReg & 0x80000000000000LL) == 0 )
/* line: 770 */         {
/* line: 771, address: 0xfffffff009f7b8f4 */           v154 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 772 */         }
/* line: 773, address: 0xfffffff009f7b904 */         if ( v232 != 0 )
/* line: 774, address: 0xfffffff009f7b908 */           v188 = *(_DWORD *)(v232 + 96);
/* line: 775 */         else
/* line: 776, address: 0xfffffff009f7b910 */           v188 = 0;
/* line: 777 */         sub_FFFFFFF00A0531B8(
/* line: 778 */           a1: &dword_FFFFFFF007008000,
/* line: 779 */           a2: v152,
/* line: 780 */           a3: 0,
/* line: 781 */           a4: "vm: shared_region: %p [%d(%s)] map: fd=%d not readable\n",
/* line: 782 */           v154,
/* line: 783 */           v188,
/* line: 784 */           (const char *)(v232 + 1372),
/* line: 785 */           *v46);
/* line: 786 */ LABEL_436:
/* line: 787, address: 0xfffffff009f7b940 */         v39 = 1;
/* line: 788 */ LABEL_443:
/* line: 789, address: 0xfffffff009f7b9a4 */         a7 = v108;
/* line: 790, address: 0xfffffff009f7b9a8 */         v37 = v225;
/* line: 791, address: 0xfffffff009f7b9ac */         goto LABEL_35;
/* line: 792 */       }
/* line: 793 */ LABEL_270:
/* line: 794, address: 0xfffffff009f7b100 */       v39 = 1;
/* line: 795, address: 0xfffffff009f7b104 */       goto LABEL_288;
/* line: 796 */     }
/* line: 797, address: 0xfffffff009f7a564 */     v61 = *(_QWORD *)(v60 + 56);
/* line: 798, address: 0xfffffff009f7a578 */     v62 = a7;
/* line: 799 */     if ( v61 == 0 )
/* line: 800 */     {
/* line: 801, address: 0xfffffff009f7a5e8 */       v63 = sub_FFFFFFF009B9286C(a1: 0);
/* line: 802, address: 0xfffffff009f7a5ec */       if ( (_DWORD)v63 == 0 )
/* line: 803, address: 0xfffffff009f7a5ec */         goto LABEL_86;
/* line: 804 */ LABEL_205:
/* line: 805, address: 0xfffffff009f7ad18 */       v39 = v63;
/* line: 806, address: 0xfffffff009f7ad28 */       if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 807, address: 0xfffffff009f7ad28 */         goto LABEL_564;
/* line: 808, address: 0xfffffff009f7ad34 */       a1 = v232;
/* line: 809, address: 0xfffffff009f7ad48 */       StatusReg = v227;
/* line: 810 */       if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 811 */       {
/* line: 812, address: 0xfffffff009f7b2d0 */         v108 = a7;
/* line: 813, address: 0xfffffff009f7b2d8 */         v155 = off_FFFFFFF00A38FFE0;
/* line: 814, address: 0xfffffff009f7b2e4 */         v156 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 815, address: 0xfffffff009f7b2e8 */         v157 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 816, address: 0xfffffff009f7b2ec */         if ( v156 != 0 )
/* line: 817 */         {
/* line: 818, address: 0xfffffff009f7b2f8 */           if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 819, address: 0xfffffff009f7b2f8 */             v157 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 820, address: 0xfffffff009f7b2fc */           v157 = (const void *)sub_FFFFFFF009B3BF1C(a1: v157);
/* line: 821 */         }
/* line: 822, address: 0xfffffff009f7b5b4 */         else if ( (v227 & 0x80000000000000LL) != 0
/* line: 823 */                && (unsigned __int64)v157 >= 0xFFFFFFDC00000000LL
/* line: 824 */                && (unsigned __int64)v157 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 825 */         {
/* line: 826, address: 0xfffffff009f7b5b8 */           v157 = (const void *)sub_FFFFFFF00998128C(a1: v157);
/* line: 827 */         }
/* line: 828, address: 0xfffffff009f7b950 */         else if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 829 */         {
/* line: 830, address: 0xfffffff009f7b950 */           v157 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 831 */         }
/* line: 832, address: 0xfffffff009f7b960 */         if ( v232 != 0 )
/* line: 833, address: 0xfffffff009f7b964 */           v189 = *(_DWORD *)(v232 + 96);
/* line: 834 */         else
/* line: 835, address: 0xfffffff009f7b96c */           v189 = 0;
/* line: 836 */         sub_FFFFFFF00A0531B8(
/* line: 837 */           a1: &dword_FFFFFFF007008000,
/* line: 838 */           a2: v155,
/* line: 839 */           a3: 0,
/* line: 840 */           a4: "vm: shared_region: %p [%d(%s)] map: fd=%d getwithref failed (error=%d)\n",
/* line: 841 */           v157,
/* line: 842 */           v189,
/* line: 843 */           (const char *)(v232 + 1372),
/* line: 844 */           *v46,
/* line: 845 */           v39);
/* line: 846, address: 0xfffffff009f7b9a0 */         goto LABEL_443;
/* line: 847 */       }
/* line: 848 */ LABEL_207:
/* line: 849, address: 0xfffffff009f7ad58 */       if ( (sub_FFFFFFF009F771E4(a1: StatusReg) & 1) == 0 )
/* line: 850 */       {
/* line: 851, address: 0xfffffff009f7ad5c */         v37 = v225;
/* line: 852, address: 0xfffffff009f7ad60 */         if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 853 */         {
/* line: 854, address: 0xfffffff009f7ad64 */           v116 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 855, address: 0xfffffff009f7ad80 */           if ( (StatusReg | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v116 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 856 */           {
/* line: 857, address: 0xfffffff009f7ad88 */             sub_FFFFFFF00998128C(a1: v116);
/* line: 858, address: 0xfffffff009f7ad8c */             v37 = v225;
/* line: 859 */           }
/* line: 860 */         }
/* line: 861, address: 0xfffffff009f7ad90 */         goto LABEL_35;
/* line: 862 */       }
/* line: 863, address: 0xfffffff009f7ad58 */       goto LABEL_235;
/* line: 864 */     }
/* line: 865, address: 0xfffffff009f7a5a0 */     v63 = sub_FFFFFFF009B9286C(a1: v61);
/* line: 866, address: 0xfffffff009f7a5a4 */     if ( (_DWORD)v63 != 0 )
/* line: 867, address: 0xfffffff009f7a5a4 */       goto LABEL_205;
/* line: 868 */ LABEL_86:
/* line: 869, address: 0xfffffff009f7a5f0 */     v68 = *((_QWORD *)v46 + 3);
/* line: 870, address: 0xfffffff009f7a5f4 */     v70 = *(_QWORD *)(v68 + 16);
/* line: 871, address: 0xfffffff009f7a5f4 */     v69 = (__int64 *)(v68 + 16);
/* line: 872, address: 0xfffffff009f7a604 */     v71 = *(_QWORD *)(v70 + 56);
/* line: 873, address: 0xfffffff009f7a61c */     if ( v71 != 0 )
/* line: 874, address: 0xfffffff009f7a630 */       v72 = v71;
/* line: 875 */     else
/* line: 876, address: 0xfffffff009f7a640 */       v72 = 0;
/* line: 877, address: 0xfffffff009f7a644 */     *((_QWORD *)v46 + 4) = v72;
/* line: 878 */     if ( *(_BYTE *)(v72 + 113) != 1 )
/* line: 879 */     {
/* line: 880, address: 0xfffffff009f7ada0 */       if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 881, address: 0xfffffff009f7ada0 */         goto LABEL_563;
/* line: 882, address: 0xfffffff009f7adac */       a1 = v232;
/* line: 883, address: 0xfffffff009f7adb0 */       a7 = v62;
/* line: 884, address: 0xfffffff009f7adc0 */       StatusReg = v227;
/* line: 885, address: 0xfffffff009f7adc4 */       if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 886 */       {
/* line: 887, address: 0xfffffff009f7adc8 */         v117 = v72;
/* line: 888, address: 0xfffffff009f7add4 */         if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) != 0 )
/* line: 889 */         {
/* line: 890, address: 0xfffffff009f7add8 */           v118 = v117;
/* line: 891 */         }
/* line: 892 */         else
/* line: 893 */         {
/* line: 894, address: 0xfffffff009f7b5c0 */           v118 = v117;
/* line: 895, address: 0xfffffff009f7b5e4 */           if ( (v227 & 0x80000000000000LL) != 0
/* line: 896 */             && (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL
/* line: 897 */             && (v227 | 0xFFFFFF8000000000LL) <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 898 */           {
/* line: 899, address: 0xfffffff009f7b5ec */             sub_FFFFFFF00998128C(a1: v227 | 0xFFFFFF8000000000LL);
/* line: 900, address: 0xfffffff009f7b5f0 */             v118 = *((_QWORD *)v46 + 4);
/* line: 901 */           }
/* line: 902 */         }
/* line: 903, address: 0xfffffff009f7b5f4 */         v178 = v118;
/* line: 904, address: 0xfffffff009f7b5fc */         if ( (sub_FFFFFFF009F771E4(a1: v118) & 1) == 0 )
/* line: 905 */         {
/* line: 906, address: 0xfffffff009f7b638 */           v39 = 22;
/* line: 907, address: 0xfffffff009f7b63c */           v37 = v225;
/* line: 908, address: 0xfffffff009f7b640 */           if ( (v178 & 0x80000000000000LL) == 0 )
/* line: 909, address: 0xfffffff009f7b640 */             goto LABEL_35;
/* line: 910, address: 0xfffffff009f7b644 */           v102 = v178 | 0xFFFFFF8000000000LL;
/* line: 911, address: 0xfffffff009f7b644 */           goto LABEL_377;
/* line: 912 */         }
/* line: 913 */ LABEL_370:
/* line: 914, address: 0xfffffff009f7b600 */         v39 = 22;
/* line: 915, address: 0xfffffff009f7b604 */         v37 = v225;
/* line: 916, address: 0xfffffff009f7b608 */         goto LABEL_35;
/* line: 917 */       }
/* line: 918, address: 0xfffffff009f7b308 */       v158 = off_FFFFFFF00A38FFE0;
/* line: 919, address: 0xfffffff009f7b314 */       v159 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 920, address: 0xfffffff009f7b318 */       v160 = v227 | 0xFFFFFF8000000000LL;
/* line: 921, address: 0xfffffff009f7b31c */       v161 = a2;
/* line: 922, address: 0xfffffff009f7b320 */       if ( v159 != 0 )
/* line: 923 */       {
/* line: 924, address: 0xfffffff009f7b32c */         if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 925, address: 0xfffffff009f7b32c */           v160 = v227 & 0x7FFFFFFFFFFLL;
/* line: 926, address: 0xfffffff009f7b334 */         v162 = (const void *)sub_FFFFFFF009B3BF1C(a1: v160);
/* line: 927 */       }
/* line: 928, address: 0xfffffff009f7b628 */       else if ( (v227 & 0x80000000000000LL) != 0 && v160 >= 0xFFFFFFDC00000000LL && v160 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 929 */       {
/* line: 930, address: 0xfffffff009f7b630 */         v162 = (const void *)sub_FFFFFFF00998128C(a1: v160);
/* line: 931 */       }
/* line: 932, address: 0xfffffff009f7b9b8 */       else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 933 */       {
/* line: 934, address: 0xfffffff009f7b9b8 */         v162 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 935 */       }
/* line: 936 */       else
/* line: 937 */       {
/* line: 938, address: 0xfffffff009f7b9b8 */         v162 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 939 */       }
/* line: 940, address: 0xfffffff009f7b9c4 */       v106 = a7;
/* line: 941, address: 0xfffffff009f7b9cc */       if ( v232 != 0 )
/* line: 942, address: 0xfffffff009f7b9d0 */         v190 = *(_DWORD *)(v232 + 96);
/* line: 943 */       else
/* line: 944, address: 0xfffffff009f7b9d8 */         v190 = 0;
/* line: 945, address: 0xfffffff009f7b9e0 */       v191 = *((_QWORD *)v46 + 4);
/* line: 946, address: 0xfffffff009f7b9ec */       v192 = sub_FFFFFFF009F771E4(a1: v191);
/* line: 947, address: 0xfffffff009f7b9f0 */       v193 = (const void *)(v191 | 0xFFFFFF8000000000LL);
/* line: 948, address: 0xfffffff009f7b9f4 */       if ( v192 != 0 )
/* line: 949 */       {
/* line: 950, address: 0xfffffff009f7ba00 */         if ( (v191 & 0x80000000000000LL) == 0 )
/* line: 951, address: 0xfffffff009f7ba00 */           v193 = (const void *)(v191 & 0x7FFFFFFFFFFLL);
/* line: 952, address: 0xfffffff009f7ba04 */         v193 = (const void *)sub_FFFFFFF009B3BF1C(a1: v193);
/* line: 953 */       }
/* line: 954, address: 0xfffffff009f7ba28 */       else if ( (v191 & 0x80000000000000LL) != 0
/* line: 955 */              && (unsigned __int64)v193 >= 0xFFFFFFDC00000000LL
/* line: 956 */              && (unsigned __int64)v193 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 957 */       {
/* line: 958, address: 0xfffffff009f7ba2c */         v193 = (const void *)sub_FFFFFFF00998128C(a1: v193);
/* line: 959, address: 0xfffffff009f7ba30 */         v191 = *((_QWORD *)v46 + 4);
/* line: 960 */       }
/* line: 961, address: 0xfffffff009f7ba40 */       else if ( (v191 & 0x80000000000000LL) == 0 )
/* line: 962 */       {
/* line: 963, address: 0xfffffff009f7ba40 */         v193 = (const void *)(v191 & 0x7FFFFFFFFFFLL);
/* line: 964 */       }
/* line: 965 */       sub_FFFFFFF00A0531B8(
/* line: 966 */         a1: &dword_FFFFFFF007008000,
/* line: 967 */         a2: v158,
/* line: 968 */         a3: 0,
/* line: 969 */         a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): not a file (type=%d)\n",
/* line: 970 */         v162,
/* line: 971 */         v190,
/* line: 972 */         (const char *)(v232 + 1372),
/* line: 973 */         v193,
/* line: 974 */         *(const char **)(v191 + 184),
/* line: 975 */         *(unsigned __int8 *)(v191 + 113));
/* line: 976, address: 0xfffffff009f7ba70 */       goto LABEL_461;
/* line: 977 */     }
/* line: 978, address: 0xfffffff009f7a658 */     v73 = sub_FFFFFFF009A4374C();
/* line: 979, address: 0xfffffff009f7a664 */     if ( *v69 != 0 )
/* line: 980, address: 0xfffffff009f7a674 */       v74 = *v69;
/* line: 981 */     else
/* line: 982, address: 0xfffffff009f7a67c */       v74 = 0;
/* line: 983, address: 0xfffffff009f7a680 */     a7 = v62;
/* line: 984, address: 0xfffffff009f7a694 */     v75 = sub_FFFFFFF00A1AB9F4(a1: *(_QWORD *)(v73 + 8), a2: v74, a3: 7, a4: 18, a5: 0, a6: &v233);
/* line: 985, address: 0xfffffff009f7a698 */     StatusReg = v227;
/* line: 986, address: 0xfffffff009f7a69c */     if ( (_DWORD)v75 != 0 )
/* line: 987 */     {
/* line: 988, address: 0xfffffff009f7ade0 */       v39 = v75;
/* line: 989, address: 0xfffffff009f7ade4 */       goto LABEL_288;
/* line: 990 */     }
/* line: 991, address: 0xfffffff009f7a6a0 */     LODWORD(v234[2]) = 0;
/* line: 992, address: 0xfffffff009f7a6a8 */     *(_OWORD *)v234 = xmmword_FFFFFFF00700B200;
/* line: 993, address: 0xfffffff009f7a6b8 */     v76 = sub_FFFFFFF009BC0E0C(a1: *((_QWORD *)v46 + 4), a2: v234, a3: v73);
/* line: 994 */     if ( (_DWORD)v76 != 0 )
/* line: 995 */     {
/* line: 996, address: 0xfffffff009f7ade8 */       v39 = v76;
/* line: 997, address: 0xfffffff009f7adf8 */       a1 = v232;
/* line: 998 */       if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 999 */       {
/* line: 1000 */         if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 1001 */         {
/* line: 1002, address: 0xfffffff009f7ae20 */           v229 = off_FFFFFFF00A38FFE0;
/* line: 1003, address: 0xfffffff009f7ae2c */           v119 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1004, address: 0xfffffff009f7ae30 */           v120 = v227 | 0xFFFFFF8000000000LL;
/* line: 1005, address: 0xfffffff009f7ae34 */           if ( v119 != 0 )
/* line: 1006 */           {
/* line: 1007, address: 0xfffffff009f7ae40 */             if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1008, address: 0xfffffff009f7ae40 */               v120 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1009, address: 0xfffffff009f7ae48 */             v226 = (const void *)sub_FFFFFFF009B3BF1C(a1: v120);
/* line: 1010 */           }
/* line: 1011, address: 0xfffffff009f7b694 */           else if ( (v227 & 0x80000000000000LL) != 0 && v120 >= 0xFFFFFFDC00000000LL && v120 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1012 */           {
/* line: 1013, address: 0xfffffff009f7b69c */             v226 = (const void *)sub_FFFFFFF00998128C(a1: v120);
/* line: 1014 */           }
/* line: 1015 */           else
/* line: 1016 */           {
/* line: 1017, address: 0xfffffff009f7ba90 */             v194 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1018, address: 0xfffffff009f7ba98 */             if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1019, address: 0xfffffff009f7ba98 */               v194 = v227 | 0xFFFFFF8000000000LL;
/* line: 1020, address: 0xfffffff009f7ba9c */             v226 = (const void *)v194;
/* line: 1021 */           }
/* line: 1022, address: 0xfffffff009f7baac */           v223 = a2;
/* line: 1023, address: 0xfffffff009f7bab0 */           v195 = a7;
/* line: 1024, address: 0xfffffff009f7bab4 */           if ( v232 != 0 )
/* line: 1025, address: 0xfffffff009f7bab8 */             v196 = *(_DWORD *)(v232 + 96);
/* line: 1026 */           else
/* line: 1027, address: 0xfffffff009f7bac0 */             v196 = 0;
/* line: 1028, address: 0xfffffff009f7bac8 */           v197 = *((_QWORD *)v46 + 4);
/* line: 1029, address: 0xfffffff009f7bad4 */           v198 = v197 & 0x80000000000000LL;
/* line: 1030, address: 0xfffffff009f7bad8 */           v199 = v197 | 0xFFFFFF8000000000LL;
/* line: 1031, address: 0xfffffff009f7badc */           if ( (unsigned int)sub_FFFFFFF009F771E4(a1: v197) != 0 )
/* line: 1032 */           {
/* line: 1033, address: 0xfffffff009f7bae8 */             if ( v198 != 0 )
/* line: 1034, address: 0xfffffff009f7bae8 */               v200 = v197 | 0xFFFFFF8000000000LL;
/* line: 1035 */             else
/* line: 1036, address: 0xfffffff009f7bae8 */               v200 = v197 & 0x7FFFFFFFFFFLL;
/* line: 1037, address: 0xfffffff009f7baf0 */             v201 = (const void *)sub_FFFFFFF009B3BF1C(a1: v200);
/* line: 1038 */           }
/* line: 1039, address: 0xfffffff009f7bb14 */           else if ( v198 != 0 && v199 >= 0xFFFFFFDC00000000LL && v199 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1040 */           {
/* line: 1041, address: 0xfffffff009f7bb20 */             v201 = (const void *)sub_FFFFFFF00998128C(a1: v197 | 0xFFFFFF8000000000LL);
/* line: 1042, address: 0xfffffff009f7bb24 */             v197 = *((_QWORD *)v46 + 4);
/* line: 1043, address: 0xfffffff009f7bb28 */             v198 = v197 & 0x80000000000000LL;
/* line: 1044, address: 0xfffffff009f7bb2c */             v199 = v197 | 0xFFFFFF8000000000LL;
/* line: 1045 */           }
/* line: 1046, address: 0xfffffff009f7bb3c */           else if ( v198 != 0 )
/* line: 1047 */           {
/* line: 1048, address: 0xfffffff009f7bb3c */             v201 = (const void *)(v197 | 0xFFFFFF8000000000LL);
/* line: 1049 */           }
/* line: 1050 */           else
/* line: 1051 */           {
/* line: 1052, address: 0xfffffff009f7bb3c */             v201 = (const void *)(v197 & 0x7FFFFFFFFFFLL);
/* line: 1053 */           }
/* line: 1054, address: 0xfffffff009f7bb40 */           v202 = *(const char **)(v197 + 184);
/* line: 1055, address: 0xfffffff009f7bb4c */           if ( (unsigned int)sub_FFFFFFF009F771E4(a1: v197) != 0 )
/* line: 1056 */           {
/* line: 1057, address: 0xfffffff009f7bb58 */             if ( v198 != 0 )
/* line: 1058, address: 0xfffffff009f7bb58 */               v203 = v199;
/* line: 1059 */             else
/* line: 1060, address: 0xfffffff009f7bb58 */               v203 = v197 & 0x7FFFFFFFFFFLL;
/* line: 1061, address: 0xfffffff009f7bb5c */             v204 = (const void *)sub_FFFFFFF009B3BF1C(a1: v203);
/* line: 1062 */           }
/* line: 1063, address: 0xfffffff009f7bb80 */           else if ( v198 != 0 && v199 >= 0xFFFFFFDC00000000LL && v199 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1064 */           {
/* line: 1065, address: 0xfffffff009f7bb88 */             v204 = (const void *)sub_FFFFFFF00998128C(a1: v199);
/* line: 1066 */           }
/* line: 1067, address: 0xfffffff009f7bb98 */           else if ( v198 != 0 )
/* line: 1068 */           {
/* line: 1069, address: 0xfffffff009f7bb98 */             v204 = (const void *)v199;
/* line: 1070 */           }
/* line: 1071 */           else
/* line: 1072 */           {
/* line: 1073, address: 0xfffffff009f7bb98 */             v204 = (const void *)(v197 & 0x7FFFFFFFFFFLL);
/* line: 1074 */           }
/* line: 1075, address: 0xfffffff009f7bb9c */           a7 = v195;
/* line: 1076 */           sub_FFFFFFF00A0531B8(
/* line: 1077 */             a1: &dword_FFFFFFF007008000,
/* line: 1078 */             a2: v229,
/* line: 1079 */             a3: 0,
/* line: 1080 */             a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): vnode_getattr(%p) failed (error=%d)\n",
/* line: 1081 */             v226,
/* line: 1082 */             v196,
/* line: 1083 */             (const char *)(v232 + 1372),
/* line: 1084 */             v201,
/* line: 1085 */             v202,
/* line: 1086 */             v204,
/* line: 1087 */             v39);
/* line: 1088, address: 0xfffffff009f7bbd0 */           a1 = v232;
/* line: 1089, address: 0xfffffff009f7bbd4 */           goto LABEL_269;
/* line: 1090 */         }
/* line: 1091, address: 0xfffffff009f7ae14 */         goto LABEL_238;
/* line: 1092 */       }
/* line: 1093 */ LABEL_235:
/* line: 1094, address: 0xfffffff009f7af08 */       v37 = v225;
/* line: 1095, address: 0xfffffff009f7af0c */       goto LABEL_35;
/* line: 1096 */     }
/* line: 1097, address: 0xfffffff009f7a6c4 */     if ( DWORD1(v235) != 0 )
/* line: 1098, address: 0xfffffff009f7a6c4 */       break;
/* line: 1099, address: 0xfffffff009f7a6c8 */     v77 = *((_QWORD *)v46 + 4);
/* line: 1100, address: 0xfffffff009f7a6d4 */     if ( *(_QWORD *)(v77 + 216) != 0 )
/* line: 1101, address: 0xfffffff009f7a6e0 */       v78 = *(_QWORD *)(*((_QWORD *)v46 + 4) + 216LL);
/* line: 1102 */     else
/* line: 1103, address: 0xfffffff009f7a6e8 */       v78 = 0;
/* line: 1104 */     if ( v78 != *(_QWORD *)(a8 + 216) )
/* line: 1105 */     {
/* line: 1106, address: 0xfffffff009f7a70c */       v260[0] = 0;
/* line: 1107 */       if ( (unsigned int)sub_FFFFFFF009B92A00(a1: "/private/preboot/Cryptexes", a2: 0, a3: v260, a4: v73, a5: 0) != 0
/* line: 1108 */         || (*(_QWORD *)(*((_QWORD *)v46 + 4) + 216LL) == 0
/* line: 1109 */           ? (v79 = 0)
/* line: 1110 */           : (v79 = *(_QWORD *)(*((_QWORD *)v46 + 4) + 216LL)),
/* line: 1111 */             v79 != *(_QWORD *)(v260[0] + 216LL)) )
/* line: 1112 */       {
/* line: 1113, address: 0xfffffff009f7ac08 */         v62 = a7;
/* line: 1114, address: 0xfffffff009f7ac18 */         a1 = v232;
/* line: 1115 */         if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 1116 */         {
/* line: 1117 */           if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 1118 */           {
/* line: 1119, address: 0xfffffff009f7ac48 */             if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) == 0 && (v227 & 0x80000000000000LL) != 0 )
/* line: 1120 */             {
/* line: 1121, address: 0xfffffff009f7ac50 */               v112 = v227 | 0xFFFFFF8000000000LL;
/* line: 1122, address: 0xfffffff009f7ac6c */               if ( (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v112 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1123, address: 0xfffffff009f7ac70 */                 sub_FFFFFFF00998128C(a1: v112);
/* line: 1124 */             }
/* line: 1125, address: 0xfffffff009f7ac74 */             v113 = *((_QWORD *)v46 + 4);
/* line: 1126, address: 0xfffffff009f7ac84 */             if ( (sub_FFFFFFF009F771E4(a1: v113) & 1) == 0 && (v113 & 0x80000000000000LL) != 0 )
/* line: 1127 */             {
/* line: 1128, address: 0xfffffff009f7ac88 */               v114 = v113 | 0xFFFFFF8000000000LL;
/* line: 1129, address: 0xfffffff009f7aca4 */               if ( (v113 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v114 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1130, address: 0xfffffff009f7aca8 */                 sub_FFFFFFF00998128C(a1: v114);
/* line: 1131 */             }
/* line: 1132 */           }
/* line: 1133 */           else
/* line: 1134 */           {
/* line: 1135, address: 0xfffffff009f7b268 */             v148 = off_FFFFFFF00A38FFE0;
/* line: 1136, address: 0xfffffff009f7b278 */             v149 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1137, address: 0xfffffff009f7b27c */             v150 = v227 | 0xFFFFFF8000000000LL;
/* line: 1138, address: 0xfffffff009f7b280 */             if ( v149 != 0 )
/* line: 1139 */             {
/* line: 1140, address: 0xfffffff009f7b28c */               if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1141, address: 0xfffffff009f7b28c */                 v150 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1142, address: 0xfffffff009f7b294 */               v151 = (const void *)sub_FFFFFFF009B3BF1C(a1: v150);
/* line: 1143 */             }
/* line: 1144, address: 0xfffffff009f7b560 */             else if ( (v227 & 0x80000000000000LL) != 0 && v150 >= 0xFFFFFFDC00000000LL && v150 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1145 */             {
/* line: 1146, address: 0xfffffff009f7b568 */               v151 = (const void *)sub_FFFFFFF00998128C(a1: v150);
/* line: 1147 */             }
/* line: 1148, address: 0xfffffff009f7b810 */             else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1149 */             {
/* line: 1150, address: 0xfffffff009f7b810 */               v151 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 1151 */             }
/* line: 1152 */             else
/* line: 1153 */             {
/* line: 1154, address: 0xfffffff009f7b810 */               v151 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 1155 */             }
/* line: 1156, address: 0xfffffff009f7b81c */             v181 = a2;
/* line: 1157, address: 0xfffffff009f7b824 */             if ( v232 != 0 )
/* line: 1158 */             {
/* line: 1159, address: 0xfffffff009f7b828 */               v182 = v232;
/* line: 1160, address: 0xfffffff009f7b82c */               v183 = *(_DWORD *)(v232 + 96);
/* line: 1161 */             }
/* line: 1162 */             else
/* line: 1163 */             {
/* line: 1164, address: 0xfffffff009f7b834 */               v182 = 0;
/* line: 1165, address: 0xfffffff009f7b838 */               v183 = 0;
/* line: 1166 */             }
/* line: 1167, address: 0xfffffff009f7b83c */             v184 = (const char *)(v182 + 1372);
/* line: 1168, address: 0xfffffff009f7b840 */             v185 = *((_QWORD *)v46 + 4);
/* line: 1169, address: 0xfffffff009f7b84c */             v186 = sub_FFFFFFF009F771E4(a1: v185);
/* line: 1170, address: 0xfffffff009f7b850 */             v187 = (const void *)(v185 | 0xFFFFFF8000000000LL);
/* line: 1171, address: 0xfffffff009f7b854 */             if ( v186 != 0 )
/* line: 1172 */             {
/* line: 1173, address: 0xfffffff009f7b860 */               if ( (v185 & 0x80000000000000LL) == 0 )
/* line: 1174, address: 0xfffffff009f7b860 */                 v187 = (const void *)(v185 & 0x7FFFFFFFFFFLL);
/* line: 1175, address: 0xfffffff009f7b864 */               v187 = (const void *)sub_FFFFFFF009B3BF1C(a1: v187);
/* line: 1176 */             }
/* line: 1177, address: 0xfffffff009f7b888 */             else if ( (v185 & 0x80000000000000LL) != 0
/* line: 1178 */                    && (unsigned __int64)v187 >= 0xFFFFFFDC00000000LL
/* line: 1179 */                    && (unsigned __int64)v187 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1180 */             {
/* line: 1181, address: 0xfffffff009f7b88c */               v187 = (const void *)sub_FFFFFFF00998128C(a1: v187);
/* line: 1182, address: 0xfffffff009f7b890 */               v185 = *((_QWORD *)v46 + 4);
/* line: 1183 */             }
/* line: 1184, address: 0xfffffff009f7b8a0 */             else if ( (v185 & 0x80000000000000LL) == 0 )
/* line: 1185 */             {
/* line: 1186, address: 0xfffffff009f7b8a0 */               v187 = (const void *)(v185 & 0x7FFFFFFFFFFLL);
/* line: 1187 */             }
/* line: 1188 */             sub_FFFFFFF00A0531B8(
/* line: 1189 */               a1: &dword_FFFFFFF007008000,
/* line: 1190 */               a2: v148,
/* line: 1191 */               a3: 0,
/* line: 1192 */               a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): not on process' root volume nor preboot volume\n",
/* line: 1193 */               v151,
/* line: 1194 */               v183,
/* line: 1195 */               v184,
/* line: 1196 */               v187,
/* line: 1197 */               *(const char **)(v185 + 184));
/* line: 1198, address: 0xfffffff009f7b8d0 */             a1 = v232;
/* line: 1199, address: 0xfffffff009f7b8d4 */             a2 = v181;
/* line: 1200 */           }
/* line: 1201 */         }
/* line: 1202, address: 0xfffffff009f7b8dc */         if ( v260[0] != 0 )
/* line: 1203, address: 0xfffffff009f7b8e0 */           sub_FFFFFFF009B8F0E0();
/* line: 1204, address: 0xfffffff009f7b8e4 */         v39 = 1;
/* line: 1205, address: 0xfffffff009f7b8e8 */         goto LABEL_565;
/* line: 1206 */       }
/* line: 1207, address: 0xfffffff009f7a770 */       sub_FFFFFFF009B8F0E0();
/* line: 1208, address: 0xfffffff009f7a774 */       v77 = *((_QWORD *)v46 + 4);
/* line: 1209 */     }
/* line: 1210, address: 0xfffffff009f7a778 */     v288 = 0;
/* line: 1211, address: 0xfffffff009f7a780 */     v286 = 0u;
/* line: 1212, address: 0xfffffff009f7a780 */     v287 = 0u;
/* line: 1213, address: 0xfffffff009f7a784 */     v284 = 0u;
/* line: 1214, address: 0xfffffff009f7a784 */     v285 = 0u;
/* line: 1215, address: 0xfffffff009f7a788 */     v282 = 0u;
/* line: 1216, address: 0xfffffff009f7a788 */     v283 = 0u;
/* line: 1217, address: 0xfffffff009f7a78c */     v280 = 0u;
/* line: 1218, address: 0xfffffff009f7a78c */     v281 = 0u;
/* line: 1219, address: 0xfffffff009f7a790 */     v278 = 0u;
/* line: 1220, address: 0xfffffff009f7a790 */     v279 = 0u;
/* line: 1221, address: 0xfffffff009f7a794 */     v276 = 0u;
/* line: 1222, address: 0xfffffff009f7a794 */     v277 = 0u;
/* line: 1223, address: 0xfffffff009f7a798 */     v274 = 0u;
/* line: 1224, address: 0xfffffff009f7a798 */     v275 = 0u;
/* line: 1225, address: 0xfffffff009f7a79c */     v272 = 0u;
/* line: 1226, address: 0xfffffff009f7a79c */     v273 = 0u;
/* line: 1227, address: 0xfffffff009f7a7a0 */     v270 = 0u;
/* line: 1228, address: 0xfffffff009f7a7a0 */     v271 = 0u;
/* line: 1229, address: 0xfffffff009f7a7a4 */     v268 = 0u;
/* line: 1230, address: 0xfffffff009f7a7a4 */     v269 = 0u;
/* line: 1231, address: 0xfffffff009f7a7a8 */     v266 = 0u;
/* line: 1232, address: 0xfffffff009f7a7a8 */     v267 = 0u;
/* line: 1233, address: 0xfffffff009f7a7ac */     v264 = 0u;
/* line: 1234, address: 0xfffffff009f7a7ac */     v265 = 0u;
/* line: 1235, address: 0xfffffff009f7a7b0 */     v262 = 0u;
/* line: 1236, address: 0xfffffff009f7a7b0 */     v263 = 0u;
/* line: 1237, address: 0xfffffff009f7a7b4 */     v260[0] = 0;
/* line: 1238, address: 0xfffffff009f7a7b4 */     v261 = 0u;
/* line: 1239, address: 0xfffffff009f7a7bc */     v260[1] = 16;
/* line: 1240, address: 0xfffffff009f7a7c8 */     v80 = sub_FFFFFFF009BC0E0C(a1: v77, a2: v260, a3: v73);
/* line: 1241 */     if ( (_DWORD)v80 != 0 )
/* line: 1242 */     {
/* line: 1243, address: 0xfffffff009f7af10 */       v39 = v80;
/* line: 1244 */       if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 1245 */       {
/* line: 1246, address: 0xfffffff009f7af2c */         a1 = v232;
/* line: 1247 */         if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 1248 */         {
/* line: 1249, address: 0xfffffff009f7b4b8 */           v230 = off_FFFFFFF00A38FFE0;
/* line: 1250, address: 0xfffffff009f7b4c4 */           v172 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1251, address: 0xfffffff009f7b4c8 */           v173 = v227 | 0xFFFFFF8000000000LL;
/* line: 1252, address: 0xfffffff009f7b4cc */           if ( v172 != 0 )
/* line: 1253 */           {
/* line: 1254, address: 0xfffffff009f7b4d8 */             if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1255, address: 0xfffffff009f7b4d8 */               v173 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1256, address: 0xfffffff009f7b4e0 */             v174 = (const void *)sub_FFFFFFF009B3BF1C(a1: v173);
/* line: 1257 */           }
/* line: 1258, address: 0xfffffff009f7b6ec */           else if ( (v227 & 0x80000000000000LL) != 0 && v173 >= 0xFFFFFFDC00000000LL && v173 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1259 */           {
/* line: 1260, address: 0xfffffff009f7b6f4 */             v174 = (const void *)sub_FFFFFFF00998128C(a1: v173);
/* line: 1261 */           }
/* line: 1262, address: 0xfffffff009f7bcac */           else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1263 */           {
/* line: 1264, address: 0xfffffff009f7bcac */             v174 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 1265 */           }
/* line: 1266 */           else
/* line: 1267 */           {
/* line: 1268, address: 0xfffffff009f7bcac */             v174 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 1269 */           }
/* line: 1270, address: 0xfffffff009f7bcbc */           v222 = a7;
/* line: 1271, address: 0xfffffff009f7bcbc */           v223 = a2;
/* line: 1272, address: 0xfffffff009f7bcc0 */           if ( v232 != 0 )
/* line: 1273, address: 0xfffffff009f7bcc4 */             v209 = *(_DWORD *)(v232 + 96);
/* line: 1274 */           else
/* line: 1275, address: 0xfffffff009f7bccc */             v209 = 0;
/* line: 1276, address: 0xfffffff009f7bcd4 */           v210 = *((_QWORD *)v46 + 4);
/* line: 1277, address: 0xfffffff009f7bce0 */           v211 = v210 & 0x80000000000000LL;
/* line: 1278, address: 0xfffffff009f7bce4 */           v212 = v210 | 0xFFFFFF8000000000LL;
/* line: 1279, address: 0xfffffff009f7bce8 */           if ( (unsigned int)sub_FFFFFFF009F771E4(a1: v210) != 0 )
/* line: 1280 */           {
/* line: 1281, address: 0xfffffff009f7bcf4 */             if ( v211 != 0 )
/* line: 1282, address: 0xfffffff009f7bcf4 */               v213 = v210 | 0xFFFFFF8000000000LL;
/* line: 1283 */             else
/* line: 1284, address: 0xfffffff009f7bcf4 */               v213 = v210 & 0x7FFFFFFFFFFLL;
/* line: 1285, address: 0xfffffff009f7bcfc */             v214 = (const void *)sub_FFFFFFF009B3BF1C(a1: v213);
/* line: 1286 */           }
/* line: 1287, address: 0xfffffff009f7bd20 */           else if ( v211 != 0 && v212 >= 0xFFFFFFDC00000000LL && v212 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1288 */           {
/* line: 1289, address: 0xfffffff009f7bd2c */             v214 = (const void *)sub_FFFFFFF00998128C(a1: v210 | 0xFFFFFF8000000000LL);
/* line: 1290, address: 0xfffffff009f7bd30 */             v210 = *((_QWORD *)v46 + 4);
/* line: 1291, address: 0xfffffff009f7bd34 */             v211 = v210 & 0x80000000000000LL;
/* line: 1292, address: 0xfffffff009f7bd38 */             v212 = v210 | 0xFFFFFF8000000000LL;
/* line: 1293 */           }
/* line: 1294, address: 0xfffffff009f7bd48 */           else if ( v211 != 0 )
/* line: 1295 */           {
/* line: 1296, address: 0xfffffff009f7bd48 */             v214 = (const void *)(v210 | 0xFFFFFF8000000000LL);
/* line: 1297 */           }
/* line: 1298 */           else
/* line: 1299 */           {
/* line: 1300, address: 0xfffffff009f7bd48 */             v214 = (const void *)(v210 & 0x7FFFFFFFFFFLL);
/* line: 1301 */           }
/* line: 1302, address: 0xfffffff009f7bd4c */           v215 = *(const char **)(v210 + 184);
/* line: 1303, address: 0xfffffff009f7bd58 */           if ( (unsigned int)sub_FFFFFFF009F771E4(a1: v210) != 0 )
/* line: 1304 */           {
/* line: 1305, address: 0xfffffff009f7bd64 */             if ( v211 != 0 )
/* line: 1306, address: 0xfffffff009f7bd64 */               v216 = v212;
/* line: 1307 */             else
/* line: 1308, address: 0xfffffff009f7bd64 */               v216 = v210 & 0x7FFFFFFFFFFLL;
/* line: 1309, address: 0xfffffff009f7bd68 */             v217 = (const void *)sub_FFFFFFF009B3BF1C(a1: v216);
/* line: 1310 */           }
/* line: 1311, address: 0xfffffff009f7bd8c */           else if ( v211 != 0 && v212 >= 0xFFFFFFDC00000000LL && v212 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1312 */           {
/* line: 1313, address: 0xfffffff009f7bd94 */             v217 = (const void *)sub_FFFFFFF00998128C(a1: v212);
/* line: 1314 */           }
/* line: 1315, address: 0xfffffff009f7bda4 */           else if ( v211 != 0 )
/* line: 1316 */           {
/* line: 1317, address: 0xfffffff009f7bda4 */             v217 = (const void *)v212;
/* line: 1318 */           }
/* line: 1319 */           else
/* line: 1320 */           {
/* line: 1321, address: 0xfffffff009f7bda4 */             v217 = (const void *)(v210 & 0x7FFFFFFFFFFLL);
/* line: 1322 */           }
/* line: 1323 */           sub_FFFFFFF00A0531B8(
/* line: 1324 */             a1: &dword_FFFFFFF007008000,
/* line: 1325 */             a2: v230,
/* line: 1326 */             a3: 0,
/* line: 1327 */             a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): vnode_size(%p) failed (error=%d)\n",
/* line: 1328 */             v174,
/* line: 1329 */             v209,
/* line: 1330 */             (const char *)(v232 + 1372),
/* line: 1331 */             v214,
/* line: 1332 */             v215,
/* line: 1333 */             v217,
/* line: 1334 */             v39);
/* line: 1335, address: 0xfffffff009f7bdd8 */           a1 = v232;
/* line: 1336, address: 0xfffffff009f7bddc */           a7 = v222;
/* line: 1337, address: 0xfffffff009f7bde0 */           goto LABEL_269;
/* line: 1338 */         }
/* line: 1339 */ LABEL_238:
/* line: 1340, address: 0xfffffff009f7af40 */         v124 = a1;
/* line: 1341, address: 0xfffffff009f7af44 */         v125 = a2;
/* line: 1342, address: 0xfffffff009f7af48 */         v126 = a7;
/* line: 1343, address: 0xfffffff009f7af58 */         if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) == 0 && (v227 & 0x80000000000000LL) != 0 )
/* line: 1344 */         {
/* line: 1345, address: 0xfffffff009f7af5c */           v127 = v227 | 0xFFFFFF8000000000LL;
/* line: 1346, address: 0xfffffff009f7af78 */           if ( (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v127 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1347, address: 0xfffffff009f7af7c */             sub_FFFFFFF00998128C(a1: v127);
/* line: 1348 */         }
/* line: 1349, address: 0xfffffff009f7af80 */         v128 = *((_QWORD *)v46 + 4);
/* line: 1350, address: 0xfffffff009f7af8c */         v129 = v128 | 0xFFFFFF8000000000LL;
/* line: 1351, address: 0xfffffff009f7afb0 */         if ( (sub_FFFFFFF009F771E4(a1: v128) & 1) == 0
/* line: 1352 */           && (v128 & 0x80000000000000LL) != 0
/* line: 1353 */           && v129 >= 0xFFFFFFDC00000000LL
/* line: 1354 */           && v129 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1355 */         {
/* line: 1356, address: 0xfffffff009f7afb8 */           sub_FFFFFFF00998128C(a1: v128 | 0xFFFFFF8000000000LL);
/* line: 1357, address: 0xfffffff009f7afbc */           v128 = *((_QWORD *)v46 + 4);
/* line: 1358, address: 0xfffffff009f7afc0 */           v129 = v128 | 0xFFFFFF8000000000LL;
/* line: 1359 */         }
/* line: 1360, address: 0xfffffff009f7afc8 */         v130 = sub_FFFFFFF009F771E4(a1: v128);
/* line: 1361, address: 0xfffffff009f7afcc */         v37 = v225;
/* line: 1362, address: 0xfffffff009f7b480 */         if ( (v130 & 1) != 0 || (v128 & 0x80000000000000LL) == 0 || v129 < 0xFFFFFFDC00000000LL )
/* line: 1363 */         {
/* line: 1364, address: 0xfffffff009f7afd4 */           a7 = v126;
/* line: 1365, address: 0xfffffff009f7afd8 */           a2 = v125;
/* line: 1366, address: 0xfffffff009f7afdc */           a1 = v124;
/* line: 1367 */         }
/* line: 1368 */         else
/* line: 1369 */         {
/* line: 1370, address: 0xfffffff009f7b48c */           a2 = v125;
/* line: 1371, address: 0xfffffff009f7b490 */           if ( v129 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1372 */           {
/* line: 1373, address: 0xfffffff009f7b49c */             sub_FFFFFFF00998128C(a1: v129);
/* line: 1374, address: 0xfffffff009f7b4a0 */             v37 = v225;
/* line: 1375 */           }
/* line: 1376, address: 0xfffffff009f7b4a4 */           a7 = v126;
/* line: 1377, address: 0xfffffff009f7b4a8 */           a1 = v124;
/* line: 1378 */         }
/* line: 1379, address: 0xfffffff009f7b4ac */         goto LABEL_35;
/* line: 1380 */       }
/* line: 1381 */ LABEL_288:
/* line: 1382, address: 0xfffffff009f7b218 */       a1 = v232;
/* line: 1383, address: 0xfffffff009f7b21c */       v37 = v225;
/* line: 1384, address: 0xfffffff009f7b220 */       goto LABEL_35;
/* line: 1385 */     }
/* line: 1386, address: 0xfffffff009f7a7d4 */     *((_QWORD *)v46 + 5) = v263;
/* line: 1387, address: 0xfffffff009f7a7d8 */     v81 = *((_QWORD *)v46 + 4);
/* line: 1388 */     if ( v81 == 0 || *(_BYTE *)(v81 + 113) != 1 || (v82 = *(_QWORD *)(v81 + 120)) == 0 )
/* line: 1389 */     {
/* line: 1390, address: 0xfffffff009f7aab4 */       *((_QWORD *)v46 + 6) = 0;
/* line: 1391 */ LABEL_162:
/* line: 1392 */       if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 1393 */       {
/* line: 1394, address: 0xfffffff009f7aad0 */         a1 = v232;
/* line: 1395, address: 0xfffffff009f7aae0 */         if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 1396 */         {
/* line: 1397, address: 0xfffffff009f7aaf0 */           if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) == 0 && (v227 & 0x80000000000000LL) != 0 )
/* line: 1398 */           {
/* line: 1399, address: 0xfffffff009f7aaf4 */             v101 = v227 | 0xFFFFFF8000000000LL;
/* line: 1400, address: 0xfffffff009f7ab10 */             if ( (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v101 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1401 */             {
/* line: 1402, address: 0xfffffff009f7ab14 */               sub_FFFFFFF00998128C(a1: v101);
/* line: 1403, address: 0xfffffff009f7ab18 */               v81 = *((_QWORD *)v46 + 4);
/* line: 1404 */             }
/* line: 1405 */           }
/* line: 1406, address: 0xfffffff009f7ab24 */           if ( (sub_FFFFFFF009F771E4(a1: v81) & 1) == 0 )
/* line: 1407 */           {
/* line: 1408, address: 0xfffffff009f7ab28 */             v39 = 22;
/* line: 1409, address: 0xfffffff009f7ab2c */             v37 = v225;
/* line: 1410, address: 0xfffffff009f7ab30 */             if ( (v81 & 0x80000000000000LL) == 0 )
/* line: 1411, address: 0xfffffff009f7ab30 */               goto LABEL_35;
/* line: 1412, address: 0xfffffff009f7ab34 */             v102 = v81 | 0xFFFFFF8000000000LL;
/* line: 1413 */ LABEL_377:
/* line: 1414, address: 0xfffffff009f7b660 */             if ( v102 >= 0xFFFFFFDC00000000LL && v102 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1415 */             {
/* line: 1416, address: 0xfffffff009f7b664 */               v179 = v37;
/* line: 1417, address: 0xfffffff009f7b668 */               sub_FFFFFFF00998128C(a1: v102);
/* line: 1418, address: 0xfffffff009f7b66c */               v37 = v179;
/* line: 1419, address: 0xfffffff009f7b670 */               v39 = 22;
/* line: 1420 */             }
/* line: 1421, address: 0xfffffff009f7b674 */             goto LABEL_35;
/* line: 1422 */           }
/* line: 1423, address: 0xfffffff009f7ab24 */           goto LABEL_370;
/* line: 1424 */         }
/* line: 1425, address: 0xfffffff009f7ab6c */         v103 = off_FFFFFFF00A38FFE0;
/* line: 1426, address: 0xfffffff009f7ab78 */         v104 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1427, address: 0xfffffff009f7ab7c */         v105 = v227 | 0xFFFFFF8000000000LL;
/* line: 1428, address: 0xfffffff009f7ab84 */         v106 = a7;
/* line: 1429, address: 0xfffffff009f7ab88 */         if ( v104 != 0 )
/* line: 1430 */         {
/* line: 1431, address: 0xfffffff009f7ab94 */           if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1432, address: 0xfffffff009f7ab94 */             v105 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1433, address: 0xfffffff009f7ab9c */           v107 = (const void *)sub_FFFFFFF009B3BF1C(a1: v105);
/* line: 1434 */         }
/* line: 1435, address: 0xfffffff009f7b240 */         else if ( (v227 & 0x80000000000000LL) != 0 && v105 >= 0xFFFFFFDC00000000LL && v105 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1436 */         {
/* line: 1437, address: 0xfffffff009f7b248 */           v107 = (const void *)sub_FFFFFFF00998128C(a1: v105);
/* line: 1438 */         }
/* line: 1439, address: 0xfffffff009f7b348 */         else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1440 */         {
/* line: 1441, address: 0xfffffff009f7b348 */           v107 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 1442 */         }
/* line: 1443 */         else
/* line: 1444 */         {
/* line: 1445, address: 0xfffffff009f7b348 */           v107 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 1446 */         }
/* line: 1447, address: 0xfffffff009f7b354 */         v161 = a2;
/* line: 1448, address: 0xfffffff009f7b35c */         if ( v232 != 0 )
/* line: 1449, address: 0xfffffff009f7b360 */           v163 = *(_DWORD *)(v232 + 96);
/* line: 1450 */         else
/* line: 1451, address: 0xfffffff009f7b368 */           v163 = 0;
/* line: 1452, address: 0xfffffff009f7b370 */         v164 = *((_QWORD *)v46 + 4);
/* line: 1453, address: 0xfffffff009f7b37c */         v165 = sub_FFFFFFF009F771E4(a1: v164);
/* line: 1454, address: 0xfffffff009f7b380 */         v166 = (const void *)(v164 | 0xFFFFFF8000000000LL);
/* line: 1455, address: 0xfffffff009f7b384 */         if ( v165 != 0 )
/* line: 1456 */         {
/* line: 1457, address: 0xfffffff009f7b390 */           if ( (v164 & 0x80000000000000LL) == 0 )
/* line: 1458, address: 0xfffffff009f7b390 */             v166 = (const void *)(v164 & 0x7FFFFFFFFFFLL);
/* line: 1459, address: 0xfffffff009f7b394 */           v166 = (const void *)sub_FFFFFFF009B3BF1C(a1: v166);
/* line: 1460 */         }
/* line: 1461, address: 0xfffffff009f7b3b8 */         else if ( (v164 & 0x80000000000000LL) != 0
/* line: 1462 */                && (unsigned __int64)v166 >= 0xFFFFFFDC00000000LL
/* line: 1463 */                && (unsigned __int64)v166 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1464 */         {
/* line: 1465, address: 0xfffffff009f7b3bc */           v166 = (const void *)sub_FFFFFFF00998128C(a1: v166);
/* line: 1466, address: 0xfffffff009f7b3c0 */           v164 = *((_QWORD *)v46 + 4);
/* line: 1467 */         }
/* line: 1468, address: 0xfffffff009f7b3d0 */         else if ( (v164 & 0x80000000000000LL) == 0 )
/* line: 1469 */         {
/* line: 1470, address: 0xfffffff009f7b3d0 */           v166 = (const void *)(v164 & 0x7FFFFFFFFFFLL);
/* line: 1471 */         }
/* line: 1472 */         sub_FFFFFFF00A0531B8(
/* line: 1473 */           a1: &dword_FFFFFFF007008000,
/* line: 1474 */           a2: v103,
/* line: 1475 */           a3: 0,
/* line: 1476 */           a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): no memory object\n",
/* line: 1477 */           v107,
/* line: 1478 */           v163,
/* line: 1479 */           (const char *)(v232 + 1372),
/* line: 1480 */           v166,
/* line: 1481 */           *(const char **)(v164 + 184));
/* line: 1482 */ LABEL_461:
/* line: 1483, address: 0xfffffff009f7ba74 */         v39 = 22;
/* line: 1484 */ LABEL_462:
/* line: 1485, address: 0xfffffff009f7ba78 */         a1 = v232;
/* line: 1486, address: 0xfffffff009f7ba7c */         a7 = v106;
/* line: 1487, address: 0xfffffff009f7ba80 */         StatusReg = v227;
/* line: 1488, address: 0xfffffff009f7ba84 */         a2 = v161;
/* line: 1489, address: 0xfffffff009f7ba88 */         v37 = v225;
/* line: 1490, address: 0xfffffff009f7ba8c */         goto LABEL_35;
/* line: 1491 */       }
/* line: 1492 */ LABEL_287:
/* line: 1493, address: 0xfffffff009f7b214 */       v39 = 22;
/* line: 1494, address: 0xfffffff009f7b214 */       goto LABEL_288;
/* line: 1495 */     }
/* line: 1496, address: 0xfffffff009f7a804 */     v83 = *(_QWORD *)(v82 + 8);
/* line: 1497, address: 0xfffffff009f7a808 */     *((_QWORD *)v46 + 6) = v83;
/* line: 1498, address: 0xfffffff009f7a80c */     if ( v83 == 0 )
/* line: 1499, address: 0xfffffff009f7a80c */       goto LABEL_162;
/* line: 1500, address: 0xfffffff009f7a810 */     v84 = v46[1];
/* line: 1501 */     if ( (_DWORD)v84 != 0 )
/* line: 1502 */     {
/* line: 1503 */       for ( i = 0; i < v84; ++i )
/* line: 1504 */       {
/* line: 1505, address: 0xfffffff009f7a838 */         v86 = *((_QWORD *)v46 + 1);
/* line: 1506, address: 0xfffffff009f7a840 */         v87 = v86 + 48 * i;
/* line: 1507, address: 0xfffffff009f7a84c */         if ( ((v86 ^ v87) & 0xFFC0000000000000LL) != 0 )
/* line: 1508, address: 0xfffffff009f7a850 */           v87 = v87 & 0xFFFFFFFFFFFFLL | 0xC8A2000000000000LL;
/* line: 1509 */         if ( (*(_BYTE *)(v87 + 44) & 0x10) == 0 )
/* line: 1510 */         {
/* line: 1511, address: 0xfffffff009f7a85c */           v62 = a7;
/* line: 1512, address: 0xfffffff009f7a864 */           v88 = *(_QWORD *)(v87 + 8);
/* line: 1513, address: 0xfffffff009f7a86c */           v89 = *((_QWORD *)v46 + 4) == 0 || v88 == 0;
/* line: 1514 */           if ( v89
/* line: 1515 */             || (v90 = *(_QWORD *)(v87 + 16), v91 = v88 + v90, __CFADD__(v88, v90))
/* line: 1516 */             || (v92 = (_QWORD *)sub_FFFFFFF009F27114()) == nullptr
/* line: 1517 */             || (v93 = v92[5], v92[6] + v93 > v90)
/* line: 1518 */             || v92[7] + v93 < v91 )
/* line: 1519 */           {
/* line: 1520 */             if ( dword_FFFFFFF00A38FFEC >= 1 )
/* line: 1521 */             {
/* line: 1522, address: 0xfffffff009f7a908 */               a1 = v232;
/* line: 1523, address: 0xfffffff009f7a918 */               if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 1524 */               {
/* line: 1525, address: 0xfffffff009f7a92c */                 if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) == 0 && (v227 & 0x80000000000000LL) != 0 )
/* line: 1526 */                 {
/* line: 1527, address: 0xfffffff009f7a934 */                   v94 = v227 | 0xFFFFFF8000000000LL;
/* line: 1528, address: 0xfffffff009f7a950 */                   if ( (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v94 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1529, address: 0xfffffff009f7a954 */                     sub_FFFFFFF00998128C(a1: v94);
/* line: 1530 */                 }
/* line: 1531, address: 0xfffffff009f7a958 */                 v95 = *((_QWORD *)v46 + 4);
/* line: 1532, address: 0xfffffff009f7a964 */                 v39 = 22;
/* line: 1533, address: 0xfffffff009f7a968 */                 if ( (sub_FFFFFFF009F771E4(a1: v95) & 1) == 0 )
/* line: 1534 */                 {
/* line: 1535, address: 0xfffffff009f7a96c */                   v37 = v225;
/* line: 1536, address: 0xfffffff009f7a984 */                   if ( (v95 & 0x80000000000000LL) != 0
/* line: 1537 */                     && (v96 = v95 | 0xFFFFFF8000000000LL, (v95 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL) )
/* line: 1538 */                   {
/* line: 1539, address: 0xfffffff009f7a990 */                     a7 = v62;
/* line: 1540, address: 0xfffffff009f7a994 */                     if ( v96 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1541 */                     {
/* line: 1542, address: 0xfffffff009f7a99c */                       sub_FFFFFFF00998128C(a1: v96);
/* line: 1543, address: 0xfffffff009f7a9a0 */                       v37 = v225;
/* line: 1544, address: 0xfffffff009f7a9a4 */                       v39 = 22;
/* line: 1545 */                     }
/* line: 1546, address: 0xfffffff009f7a9a8 */                     StatusReg = v227;
/* line: 1547 */                   }
/* line: 1548 */                   else
/* line: 1549 */                   {
/* line: 1550, address: 0xfffffff009f7b258 */                     a7 = v62;
/* line: 1551, address: 0xfffffff009f7b25c */                     StatusReg = v227;
/* line: 1552 */                   }
/* line: 1553, address: 0xfffffff009f7a9ac */                   goto LABEL_35;
/* line: 1554 */                 }
/* line: 1555 */ LABEL_565:
/* line: 1556, address: 0xfffffff009f7c030 */                 a7 = v62;
/* line: 1557, address: 0xfffffff009f7c034 */                 StatusReg = v227;
/* line: 1558, address: 0xfffffff009f7c038 */                 v37 = v225;
/* line: 1559, address: 0xfffffff009f7c03c */                 goto LABEL_35;
/* line: 1560 */               }
/* line: 1561, address: 0xfffffff009f7aa7c */               v97 = off_FFFFFFF00A38FFE0;
/* line: 1562, address: 0xfffffff009f7aa8c */               v98 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1563, address: 0xfffffff009f7aa90 */               v99 = v227 | 0xFFFFFF8000000000LL;
/* line: 1564, address: 0xfffffff009f7aa94 */               v223 = a2;
/* line: 1565, address: 0xfffffff009f7aa98 */               if ( v98 != 0 )
/* line: 1566 */               {
/* line: 1567, address: 0xfffffff009f7aaa4 */                 if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1568, address: 0xfffffff009f7aaa4 */                   v99 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1569, address: 0xfffffff009f7aaac */                 v100 = (const void *)sub_FFFFFFF009B3BF1C(a1: v99);
/* line: 1570 */               }
/* line: 1571, address: 0xfffffff009f7ab58 */               else if ( (v227 & 0x80000000000000LL) != 0 && v99 >= 0xFFFFFFDC00000000LL && v99 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1572 */               {
/* line: 1573, address: 0xfffffff009f7ab60 */                 v100 = (const void *)sub_FFFFFFF00998128C(a1: v99);
/* line: 1574 */               }
/* line: 1575, address: 0xfffffff009f7aff0 */               else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1576 */               {
/* line: 1577, address: 0xfffffff009f7aff0 */                 v100 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 1578 */               }
/* line: 1579 */               else
/* line: 1580 */               {
/* line: 1581, address: 0xfffffff009f7aff0 */                 v100 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 1582 */               }
/* line: 1583, address: 0xfffffff009f7b000 */               if ( v232 != 0 )
/* line: 1584 */               {
/* line: 1585, address: 0xfffffff009f7b004 */                 v131 = v232;
/* line: 1586, address: 0xfffffff009f7b008 */                 v132 = *(_DWORD *)(v232 + 96);
/* line: 1587 */               }
/* line: 1588 */               else
/* line: 1589 */               {
/* line: 1590, address: 0xfffffff009f7b010 */                 v131 = 0;
/* line: 1591, address: 0xfffffff009f7b014 */                 v132 = 0;
/* line: 1592 */               }
/* line: 1593, address: 0xfffffff009f7b018 */               v133 = (const char *)(v131 + 1372);
/* line: 1594, address: 0xfffffff009f7b01c */               v134 = *((_QWORD *)v46 + 4);
/* line: 1595, address: 0xfffffff009f7b028 */               v135 = sub_FFFFFFF009F771E4(a1: v134);
/* line: 1596, address: 0xfffffff009f7b02c */               v136 = (const void *)(v134 | 0xFFFFFF8000000000LL);
/* line: 1597, address: 0xfffffff009f7b030 */               if ( v135 != 0 )
/* line: 1598 */               {
/* line: 1599, address: 0xfffffff009f7b03c */                 if ( (v134 & 0x80000000000000LL) == 0 )
/* line: 1600, address: 0xfffffff009f7b03c */                   v136 = (const void *)(v134 & 0x7FFFFFFFFFFLL);
/* line: 1601, address: 0xfffffff009f7b040 */                 v136 = (const void *)sub_FFFFFFF009B3BF1C(a1: v136);
/* line: 1602 */               }
/* line: 1603, address: 0xfffffff009f7b064 */               else if ( (v134 & 0x80000000000000LL) != 0
/* line: 1604 */                      && (unsigned __int64)v136 >= 0xFFFFFFDC00000000LL
/* line: 1605 */                      && (unsigned __int64)v136 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1606 */               {
/* line: 1607, address: 0xfffffff009f7b068 */                 v136 = (const void *)sub_FFFFFFF00998128C(a1: v136);
/* line: 1608, address: 0xfffffff009f7b06c */                 v134 = *((_QWORD *)v46 + 4);
/* line: 1609 */               }
/* line: 1610, address: 0xfffffff009f7b07c */               else if ( (v134 & 0x80000000000000LL) == 0 )
/* line: 1611 */               {
/* line: 1612, address: 0xfffffff009f7b07c */                 v136 = (const void *)(v134 & 0x7FFFFFFFFFFLL);
/* line: 1613 */               }
/* line: 1614, address: 0xfffffff009f7b088 */               v137 = *((_QWORD *)v46 + 1);
/* line: 1615, address: 0xfffffff009f7b08c */               v138 = v137 + 48 * i;
/* line: 1616, address: 0xfffffff009f7b098 */               if ( ((v137 ^ v138) & 0xFFC0000000000000LL) != 0 )
/* line: 1617, address: 0xfffffff009f7b09c */                 v138 = v138 & 0xFFFFFFFFFFFFLL | 0xC8A2000000000000LL;
/* line: 1618 */               sub_FFFFFFF00A0531B8(
/* line: 1619 */                 a1: &dword_FFFFFFF007008000,
/* line: 1620 */                 a2: v97,
/* line: 1621 */                 a3: 0,
/* line: 1622 */                 a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): mapping #%d/%d [0x%llx:0x%llx:0x%llx:0x%x:0x%x] is not code-signed\n",
/* line: 1623 */                 v100,
/* line: 1624 */                 v132,
/* line: 1625 */                 v133,
/* line: 1626 */                 v136,
/* line: 1627 */                 *(const char **)(v134 + 184),
/* line: 1628 */                 i,
/* line: 1629 */                 v46[1],
/* line: 1630 */                 *(_QWORD *)v138,
/* line: 1631 */                 *(_QWORD *)(v138 + 8),
/* line: 1632 */                 *(_QWORD *)(v138 + 16),
/* line: 1633 */                 *(_DWORD *)(v138 + 40),
/* line: 1634 */                 *(_DWORD *)(v138 + 44));
/* line: 1635, address: 0xfffffff009f7b0e4 */               v39 = 22;
/* line: 1636, address: 0xfffffff009f7b0e8 */               a1 = v232;
/* line: 1637, address: 0xfffffff009f7b0ec */               a7 = v62;
/* line: 1638 */ LABEL_269:
/* line: 1639, address: 0xfffffff009f7b0f0 */               StatusReg = v227;
/* line: 1640, address: 0xfffffff009f7b0f4 */               a2 = v223;
/* line: 1641, address: 0xfffffff009f7b0f8 */               v37 = v225;
/* line: 1642, address: 0xfffffff009f7b0fc */               goto LABEL_35;
/* line: 1643 */             }
/* line: 1644 */ LABEL_563:
/* line: 1645, address: 0xfffffff009f7c028 */             v39 = 22;
/* line: 1646 */ LABEL_564:
/* line: 1647, address: 0xfffffff009f7c02c */             a1 = v232;
/* line: 1648, address: 0xfffffff009f7c02c */             goto LABEL_565;
/* line: 1649 */           }
/* line: 1650, address: 0xfffffff009f7a820 */           v84 = v46[1];
/* line: 1651, address: 0xfffffff009f7a824 */           a7 = v62;
/* line: 1652, address: 0xfffffff009f7a828 */           StatusReg = v227;
/* line: 1653 */         }
/* line: 1654 */       }
/* line: 1655 */     }
/* line: 1656 */ LABEL_74:
/* line: 1657, address: 0xfffffff009f7a504 */     v39 = 0;
/* line: 1658, address: 0xfffffff009f7a508 */     v46 += 14;
/* line: 1659, address: 0xfffffff009f7a518 */     if ( (unsigned __int64)v46 >= *a6 + v28 )
/* line: 1660, address: 0xfffffff009f7a518 */       goto LABEL_36;
/* line: 1661 */   }
/* line: 1662, address: 0xfffffff009f7ae5c */   if ( dword_FFFFFFF00A38FFEC < 1 )
/* line: 1663, address: 0xfffffff009f7ae5c */     goto LABEL_270;
/* line: 1664, address: 0xfffffff009f7ae68 */   a1 = v232;
/* line: 1665 */   if ( (byte_FFFFFFF00A395DE4 & 1) == 0 )
/* line: 1666 */   {
/* line: 1667, address: 0xfffffff009f7b43c */     v168 = off_FFFFFFF00A38FFE0;
/* line: 1668, address: 0xfffffff009f7b448 */     v169 = sub_FFFFFFF009F771E4(a1: v227);
/* line: 1669, address: 0xfffffff009f7b44c */     v170 = v227 | 0xFFFFFF8000000000LL;
/* line: 1670, address: 0xfffffff009f7b450 */     v161 = a2;
/* line: 1671, address: 0xfffffff009f7b454 */     if ( v169 != 0 )
/* line: 1672 */     {
/* line: 1673, address: 0xfffffff009f7b460 */       if ( (v227 & 0x80000000000000LL) == 0 )
/* line: 1674, address: 0xfffffff009f7b460 */         v170 = v227 & 0x7FFFFFFFFFFLL;
/* line: 1675, address: 0xfffffff009f7b468 */       v171 = (const void *)sub_FFFFFFF009B3BF1C(a1: v170);
/* line: 1676 */     }
/* line: 1677, address: 0xfffffff009f7b6c0 */     else if ( (v227 & 0x80000000000000LL) != 0 && v170 >= 0xFFFFFFDC00000000LL && v170 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1678 */     {
/* line: 1679, address: 0xfffffff009f7b6c8 */       v171 = (const void *)sub_FFFFFFF00998128C(a1: v170);
/* line: 1680 */     }
/* line: 1681, address: 0xfffffff009f7bbe0 */     else if ( (v227 & 0x80000000000000LL) != 0 )
/* line: 1682 */     {
/* line: 1683, address: 0xfffffff009f7bbe0 */       v171 = (const void *)(v227 | 0xFFFFFF8000000000LL);
/* line: 1684 */     }
/* line: 1685 */     else
/* line: 1686 */     {
/* line: 1687, address: 0xfffffff009f7bbe0 */       v171 = (const void *)(v227 & 0x7FFFFFFFFFFLL);
/* line: 1688 */     }
/* line: 1689, address: 0xfffffff009f7bbec */     v106 = a7;
/* line: 1690, address: 0xfffffff009f7bbf4 */     if ( v232 != 0 )
/* line: 1691, address: 0xfffffff009f7bbf8 */       v205 = *(_DWORD *)(v232 + 96);
/* line: 1692 */     else
/* line: 1693, address: 0xfffffff009f7bc00 */       v205 = 0;
/* line: 1694, address: 0xfffffff009f7bc08 */     v206 = *((_QWORD *)v46 + 4);
/* line: 1695, address: 0xfffffff009f7bc14 */     v207 = sub_FFFFFFF009F771E4(a1: v206);
/* line: 1696, address: 0xfffffff009f7bc18 */     v208 = (const void *)(v206 | 0xFFFFFF8000000000LL);
/* line: 1697, address: 0xfffffff009f7bc1c */     if ( v207 != 0 )
/* line: 1698 */     {
/* line: 1699, address: 0xfffffff009f7bc28 */       if ( (v206 & 0x80000000000000LL) == 0 )
/* line: 1700, address: 0xfffffff009f7bc28 */         v208 = (const void *)(v206 & 0x7FFFFFFFFFFLL);
/* line: 1701, address: 0xfffffff009f7bc2c */       v208 = (const void *)sub_FFFFFFF009B3BF1C(a1: v208);
/* line: 1702 */     }
/* line: 1703, address: 0xfffffff009f7bc50 */     else if ( (v206 & 0x80000000000000LL) != 0
/* line: 1704 */            && (unsigned __int64)v208 >= 0xFFFFFFDC00000000LL
/* line: 1705 */            && (unsigned __int64)v208 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1706 */     {
/* line: 1707, address: 0xfffffff009f7bc54 */       v208 = (const void *)sub_FFFFFFF00998128C(a1: v208);
/* line: 1708, address: 0xfffffff009f7bc58 */       v206 = *((_QWORD *)v46 + 4);
/* line: 1709 */     }
/* line: 1710, address: 0xfffffff009f7bc68 */     else if ( (v206 & 0x80000000000000LL) == 0 )
/* line: 1711 */     {
/* line: 1712, address: 0xfffffff009f7bc68 */       v208 = (const void *)(v206 & 0x7FFFFFFFFFFLL);
/* line: 1713 */     }
/* line: 1714 */     sub_FFFFFFF00A0531B8(
/* line: 1715 */       a1: &dword_FFFFFFF007008000,
/* line: 1716 */       a2: v168,
/* line: 1717 */       a3: 0,
/* line: 1718 */       a4: "vm: shared_region: %p [%d(%s)] map(%p:'%s'): owned by uid=%d instead of 0\n",
/* line: 1719 */       v171,
/* line: 1720 */       v205,
/* line: 1721 */       (const char *)(v232 + 1372),
/* line: 1722 */       v208,
/* line: 1723 */       *(const char **)(v206 + 184),
/* line: 1724 */       DWORD1(v235));
/* line: 1725, address: 0xfffffff009f7bc9c */     v39 = 1;
/* line: 1726, address: 0xfffffff009f7bca0 */     goto LABEL_462;
/* line: 1727 */   }
/* line: 1728, address: 0xfffffff009f7ae7c */   v108 = a7;
/* line: 1729, address: 0xfffffff009f7ae8c */   if ( (sub_FFFFFFF009F771E4(a1: v227) & 1) == 0 && (v227 & 0x80000000000000LL) != 0 )
/* line: 1730 */   {
/* line: 1731, address: 0xfffffff009f7ae90 */     v121 = v227 | 0xFFFFFF8000000000LL;
/* line: 1732, address: 0xfffffff009f7aeac */     if ( (v227 | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v121 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1733, address: 0xfffffff009f7aeb0 */       sub_FFFFFFF00998128C(a1: v121);
/* line: 1734 */   }
/* line: 1735, address: 0xfffffff009f7aeb4 */   v122 = *((_QWORD *)v46 + 4);
/* line: 1736, address: 0xfffffff009f7aec0 */   if ( (sub_FFFFFFF009F771E4(a1: v122) & 1) != 0 )
/* line: 1737, address: 0xfffffff009f7aec0 */     goto LABEL_436;
/* line: 1738, address: 0xfffffff009f7aec4 */   v39 = 1;
/* line: 1739, address: 0xfffffff009f7aec8 */   v37 = v225;
/* line: 1740, address: 0xfffffff009f7aee0 */   if ( (v122 & 0x80000000000000LL) == 0
/* line: 1741 */     || (v115 = v122 | 0xFFFFFF8000000000LL, (v122 | 0xFFFFFF8000000000LL) < 0xFFFFFFDC00000000LL) )
/* line: 1742 */   {
/* line: 1743, address: 0xfffffff009f7bef0 */     a7 = v108;
/* line: 1744, address: 0xfffffff009f7bef4 */     goto LABEL_35;
/* line: 1745 */   }
/* line: 1746, address: 0xfffffff009f7aeec */   a7 = v108;
/* line: 1747, address: 0xfffffff009f7aef0 */   if ( v115 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1748, address: 0xfffffff009f7aef0 */     goto LABEL_234;
/* line: 1749 */ LABEL_35:
/* line: 1750 */   while ( 1 )
/* line: 1751 */   {
/* line: 1752, address: 0xfffffff009f7a154 */     sub_FFFFFFF009F7C448(a1, a2, a3: *a6, a4: v37);
/* line: 1753, address: 0xfffffff009f7a158 */     *a6 = 0;
/* line: 1754, address: 0xfffffff009f7a15c */     *a7 = 0;
/* line: 1755 */ LABEL_36:
/* line: 1756 */     if ( dword_FFFFFFF00A38FFEC >= 3 )
/* line: 1757 */     {
/* line: 1758 */       if ( (byte_FFFFFFF00A395DE4 & 1) != 0 )
/* line: 1759 */       {
/* line: 1760, address: 0xfffffff009f7a188 */         v41 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 1761, address: 0xfffffff009f7a20c */         if ( (StatusReg & 0x80000000000000LL) != 0 && v41 >= 0xFFFFFFDC00000000LL && v41 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1762, address: 0xfffffff009f7a210 */           sub_FFFFFFF00998128C(a1: v41);
/* line: 1763 */       }
/* line: 1764 */       else
/* line: 1765 */       {
/* line: 1766, address: 0xfffffff009f7a21c */         a7 = (__int64 *)off_FFFFFFF00A38FFE0;
/* line: 1767, address: 0xfffffff009f7a220 */         v42 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 1768, address: 0xfffffff009f7a22c */         if ( (StatusReg & 0x80000000000000LL) != 0 )
/* line: 1769, address: 0xfffffff009f7a22c */           v43 = (const void *)(StatusReg | 0xFFFFFF8000000000LL);
/* line: 1770 */         else
/* line: 1771, address: 0xfffffff009f7a22c */           v43 = (const void *)(StatusReg & 0x7FFFFFFFFFFLL);
/* line: 1772, address: 0xfffffff009f7a394 */         if ( (StatusReg & 0x80000000000000LL) != 0 && v42 >= 0xFFFFFFDC00000000LL && v42 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1773, address: 0xfffffff009f7a39c */           v43 = (const void *)sub_FFFFFFF00998128C(a1: StatusReg | 0xFFFFFF8000000000LL);
/* line: 1774, address: 0xfffffff009f7a29c */         if ( v232 != 0 )
/* line: 1775, address: 0xfffffff009f7a2a0 */           v44 = *(_DWORD *)(v232 + 96);
/* line: 1776 */         else
/* line: 1777, address: 0xfffffff009f7a2a8 */           v44 = 0;
/* line: 1778 */         sub_FFFFFFF00A0531B8(
/* line: 1779 */           a1: &dword_FFFFFFF007008000,
/* line: 1780 */           a2: a7,
/* line: 1781 */           a3: 0,
/* line: 1782 */           a4: "vm: shared_region: %p [%d(%s)] map_and_slide_setup <- %d\n",
/* line: 1783 */           v43,
/* line: 1784 */           v44,
/* line: 1785 */           (const char *)(v232 + 1372),
/* line: 1786 */           v39);
/* line: 1787 */       }
/* line: 1788 */     }
/* line: 1789, address: 0xfffffff009f7a2ec */     if ( qword_FFFFFFF00A3A8000 == v289 )
/* line: 1790, address: 0xfffffff009f7a2ec */       break;
/* line: 1791, address: 0xfffffff009f7c1b8 */     sub_FFFFFFF00A1BF914();
/* line: 1792, address: 0xfffffff009f7c1cc */     if ( (sub_FFFFFFF009F771E4(a1: StatusReg) & 1) == 0 && (StatusReg & 0x80000000000000LL) != 0 )
/* line: 1793 */     {
/* line: 1794, address: 0xfffffff009f7c1d0 */       v221 = StatusReg | 0xFFFFFF8000000000LL;
/* line: 1795, address: 0xfffffff009f7c1ec */       if ( (StatusReg | 0xFFFFFF8000000000LL) >= 0xFFFFFFDC00000000LL && v221 <= 0xFFFFFFFBFFFFFFFFLL )
/* line: 1796, address: 0xfffffff009f7c1f0 */         sub_FFFFFFF00998128C(a1: v221);
/* line: 1797 */     }
/* line: 1798, address: 0xfffffff009f7c3cc */     a1 = v232;
/* line: 1799, address: 0xfffffff009f7c3d4 */     v37 = v225;
/* line: 1800 */   }
/* line: 1801, address: 0xfffffff009f7a310 */   return v39;
/* line: 1802 */ }