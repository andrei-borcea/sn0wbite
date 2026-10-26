/* line: 0, address: 0xfffffff00a199b44 */ __int64 __fastcall sub_FFFFFFF00A199B44(
/* line: 1 */         __int64 a1,
/* line: 2 */         __int64 a2,
/* line: 3 */         __int64 a3,
/* line: 4 */         __int64 a4,
/* line: 5 */         __int64 a5,
/* line: 6 */         __int64 a6,
/* line: 7 */         unsigned int a7)
/* line: 8 */ {
/* line: 9 */   __int64 v12; // x27
/* line: 10 */   unsigned int v13; // w28
/* line: 11 */   __int64 v14; // x19
/* line: 12 */   __int64 v15; // x22
/* line: 13 */   __int64 v16; // x0
/* line: 14 */   int v17; // w0
/* line: 15 */   __int64 v18; // x0
/* line: 16 */   __int64 v19; // x20
/* line: 17 */   __int64 v20; // x10
/* line: 18 */   __int64 (__fastcall *v21)(__int64, __int64, __int64, __int64, __int64, __int64, __int64, _QWORD, const char **, __int64 *); // x22
/* line: 19 */   __int64 v22; // x0
/* line: 20 */   unsigned int v23; // w0
/* line: 21 */   __int64 v30; // x23
/* line: 22 */   const char *v31; // x1
/* line: 23 */   const char *v33; // x8
/* line: 24 */   __int64 v34; // x0
/* line: 25 */   __int64 v37; // [xsp+38h] [xbp-68h] BYREF
/* line: 26 */   __int64 v38; // [xsp+40h] [xbp-60h] BYREF
/* line: 27 */   const char *v39; // [xsp+48h] [xbp-58h] BYREF
/* line: 28 */ 
/* line: 29, address: 0xfffffff00a199b80 */   v38 = 0;
/* line: 30, address: 0xfffffff00a199b80 */   v39 = nullptr;
/* line: 31, address: 0xfffffff00a199b88 */   if ( a3 != 0 )
/* line: 32, address: 0xfffffff00a199b8c */     v12 = *(unsigned int *)(a3 + 56);
/* line: 33 */   else
/* line: 34, address: 0xfffffff00a199b94 */     v12 = 0xFFFFFFFFLL;
/* line: 35, address: 0xfffffff00a199ca8 */   v13 = 0;
/* line: 36, address: 0xfffffff00a199cac */   v14 = 0;
/* line: 37, address: 0xfffffff00a199cb4 */   if ( (unsigned int)sub_FFFFFFF00A18B728() != 0 )
/* line: 38 */   {
/* line: 39, address: 0xfffffff00a199ddc */     do
/* line: 40 */     {
/* line: 41, address: 0xfffffff00a199de0 */       v20 = *(_QWORD *)(8LL * v13);
/* line: 42, address: 0xfffffff00a199de4 */       if ( v20 != 0 )
/* line: 43 */       {
/* line: 44, address: 0xfffffff00a199dec */         v21 = *(__int64 (__fastcall **)(__int64, __int64, __int64, __int64, __int64, __int64, __int64, _QWORD, const char **, __int64 *))(*(_QWORD *)(v20 + 32) + 2432LL);
/* line: 45, address: 0xfffffff00a199df0 */         if ( v21 != nullptr )
/* line: 46 */         {
/* line: 47, address: 0xfffffff00a199df8 */           v22 = sub_FFFFFFF00A18F248(a1: a1 + 232);
/* line: 48, address: 0xfffffff00a199e30 */           v23 = v21(a1, a2: v22, a3: v12, a4: a2, a5: a4, a6: a5, a7: a6, a8: a7, a9: &v39, a10: &v38);
/* line: 49, address: 0xfffffff00a199e38 */           if ( v23 == 11 || (_DWORD)v14 == 11 )
/* line: 50 */           {
/* line: 51, address: 0xfffffff00a199dc8 */             v14 = 11;
/* line: 52 */           }
/* line: 53, address: 0xfffffff00a199e44 */           else if ( v23 == 22 || (_DWORD)v14 == 22 )
/* line: 54 */           {
/* line: 55, address: 0xfffffff00a199e4c */             v14 = 22;
/* line: 56 */           }
/* line: 57, address: 0xfffffff00a199e58 */           else if ( v23 == 3 || (_DWORD)v14 == 3 )
/* line: 58 */           {
/* line: 59, address: 0xfffffff00a199e60 */             v14 = 3;
/* line: 60 */           }
/* line: 61, address: 0xfffffff00a199e6c */           else if ( v23 == 2 || (_DWORD)v14 == 2 )
/* line: 62 */           {
/* line: 63, address: 0xfffffff00a199e74 */             v14 = 2;
/* line: 64 */           }
/* line: 65, address: 0xfffffff00a199e80 */           else if ( v23 == 13 || (_DWORD)v14 == 13 )
/* line: 66 */           {
/* line: 67, address: 0xfffffff00a199e88 */             v14 = 13;
/* line: 68 */           }
/* line: 69, address: 0xfffffff00a199e94 */           else if ( v23 == 1 || (_DWORD)v14 == 1 )
/* line: 70 */           {
/* line: 71, address: 0xfffffff00a199e9c */             v14 = 1;
/* line: 72 */           }
/* line: 73, address: 0xfffffff00a199ea8 */           else if ( v23 != 0 )
/* line: 74 */           {
/* line: 75, address: 0xfffffff00a199ea8 */             v14 = v23;
/* line: 76 */           }
/* line: 77 */           else
/* line: 78 */           {
/* line: 79, address: 0xfffffff00a199ea8 */             v14 = (unsigned int)v14;
/* line: 80 */           }
/* line: 81 */         }
/* line: 82 */       }
/* line: 83, address: 0xfffffff00a199dd4 */       ++v13;
/* line: 84 */     }
/* line: 85, address: 0xfffffff00a199ddc */     while ( v13 == 0 );
/* line: 86, address: 0xfffffff00a199cc8 */     sub_FFFFFFF00A18B804();
/* line: 87 */   }
/* line: 88, address: 0xfffffff00a199cd4 */   if ( v38 == 0 )
/* line: 89, address: 0xfffffff00a199cd4 */     goto LABEL_61;
/* line: 90, address: 0xfffffff00a199cec */   v15 = sub_FFFFFFF009A3B320(a1: 0, a2: 0, a3: 0);
/* line: 91, address: 0xfffffff00a199cf4 */   LODWORD(v37) = 1024;
/* line: 92, address: 0xfffffff00a199cf8 */   v16 = sub_FFFFFFF009A4374C();
/* line: 93, address: 0xfffffff00a199d1c */   v17 = sub_FFFFFFF009B74444(a1, a2: 0, a3: v15, a4: 1024, a5: &v37, a6: 0, a7: 1, a8: v16);
/* line: 94 */   if ( (_DWORD)v14 == 0 )
/* line: 95 */   {
/* line: 96, address: 0xfffffff00a199f28 */     v33 = "(get vnode path failed)";
/* line: 97, address: 0xfffffff00a199f34 */     if ( v17 == 0 )
/* line: 98, address: 0xfffffff00a199f34 */       v33 = (const char *)v15;
/* line: 99 */     sub_FFFFFFF00A1C0498(
/* line: 100 */       a1: "mac_vnode_check_signature: MAC hook returned no error, but status is claimed to be fatal? path: '%s', fatal_failur"
/* line: 101 */       "e_desc_len: %ld, fatal_failure_desc:\n"
/* line: 102 */       "%s\n"
/* line: 103 */       " @%s:%d",
/* line: 104 */       v33,
/* line: 105 */       v38,
/* line: 106 */       v39,
/* line: 107 */       "mac_vfs.c",
/* line: 108 */       1362);
/* line: 109, address: 0xfffffff00a199f5c */     goto LABEL_68;
/* line: 110 */   }
/* line: 111 */   if ( a3 != 0 )
/* line: 112 */   {
/* line: 113, address: 0xfffffff00a199d30 */     v18 = sub_FFFFFFF009F6DDF4(a1: 3, a2: 1);
/* line: 114 */     if ( v18 != 0 )
/* line: 115 */     {
/* line: 116, address: 0xfffffff00a199d38 */       *(_QWORD *)(a3 + 808) = v18;
/* line: 117, address: 0xfffffff00a199d40 */       *(_QWORD *)(v18 + 32) = 66;
/* line: 118 */       if ( v39 != nullptr )
/* line: 119 */       {
/* line: 120, address: 0xfffffff00a199d4c */         v37 = 0;
/* line: 121, address: 0xfffffff00a199d58 */         if ( (unsigned int)v38 < 0xFFFFFFC1 )
/* line: 122 */         {
/* line: 123, address: 0xfffffff00a199d5c */           v19 = v18;
/* line: 124, address: 0xfffffff00a199d8c */           if ( (unsigned int)sub_FFFFFFF009F6DE84() == 0
/* line: 125 */             && (unsigned int)sub_FFFFFFF009A52A4C(a1: v19 + 48, a2: 4098) == 0 )
/* line: 126 */           {
/* line: 127, address: 0xfffffff00a199da0 */             if ( (*(_WORD *)(v19 + 52) & 1) != 0 )
/* line: 128, address: 0xfffffff00a199eb4 */               sub_FFFFFFF009B309B4(a1: v39, a2: v37, a3: (unsigned int)v38);
/* line: 129 */             else
/* line: 130, address: 0xfffffff00a199dac */               sub_FFFFFFF00A1D4470(a1: v37, a2: v39, a3: (unsigned int)v38);
/* line: 131 */           }
/* line: 132, address: 0xfffffff00a199db0 */           goto LABEL_59;
/* line: 133 */         }
/* line: 134 */ LABEL_68:
/* line: 135 */         v34 = sub_FFFFFFF00A1C0498(
/* line: 136 */                 a1: "%s: Overflow in required buffer size estimate @%s:%d",
/* line: 137 */                 "kcdata_estimate_required_buffer_size",
/* line: 138 */                 "kern_cdata.c",
/* line: 139 */                 301);
/* line: 140, address: 0xfffffff00a199f85 */         return sub_FFFFFFF00A199F88(a1: v34);
/* line: 141 */       }
/* line: 142 */     }
/* line: 143 */   }
/* line: 144 */ LABEL_59:
/* line: 145, address: 0xfffffff00a199eb8 */   if ( v15 != 0 )
/* line: 146 */   {
/* line: 147, address: 0xfffffff00a199ec0 */     v30 = MEMORY[0x34];
/* line: 148, address: 0xfffffff00a199ecc */     sub_FFFFFFF00A1D4620(a1: v15, a2: MEMORY[0x34]);
/* line: 149, address: 0xfffffff00a199ee0 */     sub_FFFFFFF009A3A73C(a1: 0, a2: 0, a3: v15, a4: v30);
/* line: 150 */   }
/* line: 151 */ LABEL_61:
/* line: 152, address: 0xfffffff00a199ee8 */   if ( v38 != 0 )
/* line: 153 */   {
/* line: 154, address: 0xfffffff00a199eec */     v31 = v39;
/* line: 155, address: 0xfffffff00a199ef0 */     if ( v39 != nullptr )
/* line: 156 */     {
/* line: 157, address: 0xfffffff00a199ef4 */       v39 = nullptr;
/* line: 158, address: 0xfffffff00a199f00 */       sub_FFFFFFF0099D1B8C(a1: &qword_FFFFFFF007A0E468, a2: v31, a3: v38);
/* line: 159 */     }
/* line: 160 */   }
/* line: 161, address: 0xfffffff00a199f24 */   return v14;
/* line: 162 */ }