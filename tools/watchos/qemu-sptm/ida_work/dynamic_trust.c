/* line: 0, address: 0xfffffff00893ff44 */ __int64 __fastcall sub_FFFFFFF00893FF44(__int64 a1, __int64 a2, int a3, __int64 a4, __int64 a5, __int64 a6, __int64 a7)
/* line: 1 */ {
/* line: 2 */   __int64 v13; // x0
/* line: 3 */   const char *v14; // x24
/* line: 4 */   __int64 v15; // x8
/* line: 5 */   __int64 result; // x0
/* line: 6 */ 
/* line: 7, address: 0xfffffff00893ffb0 */   v13 = (*(__int64 (__fastcall **)(__int64, const char *))(*(_QWORD *)a4 + 312LL))(
/* line: 8 */           a1: a4,
/* line: 9 */           a2: "com.apple.private.oop-jit.loader");
/* line: 10 */   if ( (a3 & 0x4000000) == 0 && v13 != 0 )
/* line: 11 */   {
/* line: 12 */     v14 = "%s: %s disallowed due to OOP-JIT loader entitlement";
/* line: 13 */ LABEL_7:
/* line: 14, address: 0xfffffff008940010 */     sub_FFFFFFF008958764(a1: a2);
/* line: 15, address: 0xfffffff00894002c */     sub_FFFFFFF00893C038(a1, a2: a6, a3: a7, a4: v14);
/* line: 16, address: 0xfffffff008940030 */     return 0;
/* line: 17 */   }
/* line: 18, address: 0xfffffff00893fff8 */   v15 = (*(__int64 (__fastcall **)(__int64, const char *))(*(_QWORD *)a4 + 312LL))(
/* line: 19 */           a1: a4,
/* line: 20 */           a2: "com.apple.private.oop-jit.runner");
/* line: 21, address: 0xfffffff00893fffc */   result = 1;
/* line: 22 */   if ( (a3 & 0x4000000) == 0 && v15 != 0 )
/* line: 23 */   {
/* line: 24 */     v14 = "%s: %s disallowed due to OOP-JIT runner entitlement";
/* line: 25, address: 0xfffffff008940008 */     goto LABEL_7;
/* line: 26 */   }
/* line: 27, address: 0xfffffff00894004c */   return result;
/* line: 28 */ }