#undef __NO_ISOCEXT
#include <math.h>
#include <runtime/string.h>
#include <float.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef union dbl { uint64 uu; double d; } dbl;
static const dbl f_fedcbap = { UINT64_INITIALIZER(0x461FEDCB, 0xA0000000) }; /* 0x1.fedcbap+98 */
static const dbl f_ffp1023 = { UINT64_INITIALIZER(0x000FF800, 0x00000000) }; /* 0x1.ffp-1023 */
static const dbl f_ab0p_m5 = { UINT64_INITIALIZER(0xBFAAB000, 0x00000000) }; /* -0x1.abp-5 */

#ifndef NAN
#define NAN (nan.d)
static const dbl nan = { UINT64_INITIALIZER(0x7FF80000, 0x00000000) };
#endif

#ifndef INFINITY
#define INFINITY (infinity.d)
static const dbl infinity = { UINT64_INITIALIZER(0x7FF00000, 0x00000000) };
#endif

#define SNPRINTF StringFormat

#define CHECK_END(str) \
    ASSERT(strcmp(buf, str) == 0 && (unsigned)ret == strlen(str), "'%s'", str, "'%s'", buf)
#define CHECK_END_ALT(str1, str2) \
    ASSERT( \
        (strcmp(buf, str1) == 0 && (unsigned)ret == strlen(str1)) || \
        (strcmp(buf, str2) == 0 && (unsigned)ret == strlen(str2)), \
            "'%s'", str1, "'%s'", buf)
#define CHECK_END_ALT3(str1, str2, str3) \
    ASSERT( \
        (strcmp(buf, str1) == 0 && (unsigned)ret == strlen(str1)) || \
        (strcmp(buf, str2) == 0 && (unsigned)ret == strlen(str2)) || \
        (strcmp(buf, str3) == 0 && (unsigned)ret == strlen(str3)), \
            "'%s'", str1, "'%s'", buf)

#define CHECK9(str, v1, v2, v3, v4, v5, v6, v7, v8, v9) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4, v5, v6, v7, v8, v9); CHECK_END(str); }
#define CHECK8(str, v1, v2, v3, v4, v5, v6, v7, v8    ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4, v5, v6, v7, v8    ); CHECK_END(str); }
#define CHECK7(str, v1, v2, v3, v4, v5, v6, v7        ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4, v5, v6, v7        ); CHECK_END(str); }
#define CHECK6(str, v1, v2, v3, v4, v5, v6            ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4, v5, v6            ); CHECK_END(str); }
#define CHECK5(str, v1, v2, v3, v4, v5                ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4, v5                ); CHECK_END(str); }
#define CHECK4(str, v1, v2, v3, v4                    ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3, v4                    ); CHECK_END(str); }
#define CHECK3(str, v1, v2, v3                        ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2, v3                        ); CHECK_END(str); }
#define CHECK2(str, v1, v2                            ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2                            ); CHECK_END(str); }
#define CHECK1(str, v1                                ) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1                                ); CHECK_END(str); }

#define CHECK2_ALT(str1, str2, v1, v2) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2); CHECK_END_ALT(str1, str2); }
#define CHECK2_ALT3(str1, str2, str3, v1, v2) \
    { int ret = SNPRINTF(buf, sizeof(buf), v1, v2); CHECK_END_ALT3(str1, str2, str3); }

static void test_strfmt(void)
{
   union { double d; uint64 u; } conv;
   uint64 u = UINT64_INITIALIZER(0x2, 0x4D6AC5C2);
   uint64 u2 = UINT64_INITIALIZER(0, 2), u4 = UINT64_INITIALIZER(0, 4);
   char buf[1024];
   int n = 0;
   const double pow_2_75 = 37778931862957161709568.0;
   const double pow_2_85 = 38685626227668133590597632.0;

   /* integers */
   CHECK4("a b     1", "%c %s     %d", 'a', "b", 1);
   CHECK2("abc     ", "%-8.3s", "abcdefgh");
   CHECK2("+5", "%+2d", 5);
   CHECK2("  6", "% 3i", 6);
   CHECK2("-7  ", "%-4d", -7);
   CHECK2("+0", "%+d", 0);
   CHECK3("     00003:     00004", "%10.5d:%10.5d", 3, 4);
   CHECK2("-100006789", "%d", -100006789);
   CHECK3("20 0020", "%u %04u", 20u, 20u);
   CHECK4("12 1e 3C", "%o %x %X", 10u, 30u, 60u);
   CHECK4(" 12 1e 3C ", "%3o %2x %-3X", 10u, 30u, 60u);
   CHECK4("012 0x1e 0X3C", "%#o %#x %#X", 10u, 30u, 60u);
   CHECK2("", "%.0x", 0);
   CHECK2("0", "%.0d", 0);  /* stb_sprintf gives "0" */
   CHECK3("33 555", "%hi %ld", (short)33, 555l);
   CHECK2("9888777666", "%llu", u);
   CHECK4("-1 2 -3", "%ji %zi %ti", (size_t)-1, (size_t)2, (ptrdiff_t)-3);

   /* floating-point numbers */
   CHECK2("-3.000000", "%f", -3.0);
   CHECK2("-8.8888888800", "%.10f", -8.88888888);
   CHECK2("880.0888888800", "%.10f", 880.08888888);
   CHECK2("4.1", "%.1f", 4.1);
   CHECK2(" 0", "% .0f", 0.1);
   CHECK2("0.00", "%.2f", 1e-4);
   CHECK2("-5.20", "%+4.2f", -5.2);
   CHECK2("0.0       ", "%-10.1f", 0.);

   conv.d = -0.;
   if (conv.u.half.low == 0 && conv.u.half.high == 0) { /* at least Watcom 10 and early MSVC lose sign bit here */
      conv.u.half.low = 0;
      conv.u.half.high = 0x80000000;
   }
   CHECK2("-0.000000", "%f", conv.d);

   CHECK2("0.000001", "%f", 9.09834e-07);
   CHECK2_ALT3("38685626227668133600000000.0",
   /*DOS:*/    "38685626227668134400000000.0",
   /*Watcom:*/ "38685626227668142100000000.0",
        "%.1f", pow_2_85);
   CHECK2_ALT("0.000000499999999999999978",   /*DOS:*/ "0.000000500000000000000000", "%.24f", 5e-7);
   CHECK2("0.000000000000000020000000", "%.24f", 2e-17);
   CHECK3("0.0000000100 100000000", "%.10f %.0f", 1e-8, 1e+8);
   CHECK2("100056789.0", "%.1f", 100056789.0);
   CHECK4(" 1.23 %", "%*.*f %%", 5, 2, 1.23);
   CHECK2("-3.000000e+00", "%e", -3.0);
   CHECK2("4.1E+00", "%.1E", 4.1);

   CHECK2("-5.20e+00", "%+4.2e", -5.2);
   CHECK3("+0.3 -3", "%+g %+g", 0.3, -3.0);
   CHECK2("4", "%.1G", 4.1);
   CHECK2("-5.2", "%+4.2g", -5.2);
   CHECK2("3e-300", "%g", 3e-300);
   CHECK2("1", "%.0g", 1.2);
   CHECK3(" 3.7 3.71", "% .3g %.3g", 3.704, 3.706);

   conv.d = 2e-315;
   if (conv.u.half.low == 0 && conv.u.half.high == 0) { /* at least Watcom 10 can't parse 2e-315 */
      conv.u.half.low = 0x1820d39b;
      conv.u.half.high = 0;
   }
   CHECK3("2e-315:1e+308", "%g:%g", conv.d, 1e+308);

   CHECK4("Inf Inf NaN", "%g %G %f", INFINITY, INFINITY, NAN);
   CHECK2("N", "%.1g", NAN);

   /* %n */
   CHECK3("aaa ", "%.3s %n", "aaaaaaaaaaaaa", &n);
   ASSERT_INT_EQUAL(4, n);

   /* hex floats */
   CHECK2("0x1.fedcbap+98", "%a", f_fedcbap.d);
   CHECK2("0x1.999999999999a0p-4", "%.14a", 0.1);
   CHECK2("0x1.0p-1022", "%.1a", f_ffp1023.d);
   CHECK2("0x1.009117p-1022", "%a", 2.23e-308);
   CHECK2("-0x1.AB0P-5", "%.3A", f_ab0p_m5.d);

   /* %p */
 #ifdef CPU64
   CHECK2("0000000000000000", "%p", (void*)NULL);
 #else
   CHECK2("00000000", "%p", (void*)NULL);
 #endif

   /* snprintf */
   ASSERT_INT_EQUAL(10, SNPRINTF(buf, 100, " %s     %d",  "b", 123));
   ASSERT_INT_EQUAL(0, strcmp(buf, " b     123"));
   ASSERT_INT_EQUAL(30, SNPRINTF(buf, 100, "%f", pow_2_75));
   ASSERT_TRUE(strncmp(buf, "37778931862957161709568.000000", 17) ||
      /*DOS:*/ strncmp(buf, "37778931862957164800000.000000", 17));
   n = SNPRINTF(buf, 10, "number %f", 123.456789);
   ASSERT_INT_EQUAL(0, strcmp(buf, "number 12"));
   ASSERT_INT_EQUAL(17, n);  /* written vs would-be written bytes */
   n = SNPRINTF(buf, 0, "7 chars");
   ASSERT_INT_EQUAL(7, n);
   /* stb_sprintf uses internal buffer of 512 chars - test longer string */
   ASSERT_INT_EQUAL(603, SNPRINTF(buf, 512, "%d  %600s", 3, "abc"));
   ASSERT_INT_EQUAL(511, (int)strlen(buf));
   SNPRINTF(buf, 550, "%d  %600s", 3, "abc");
   ASSERT_INT_EQUAL(549, (int)strlen(buf));
   ASSERT_INT_EQUAL(516, SNPRINTF(buf, 600, "%510s     %c", "a", 'b'));

   /* length check */
   ASSERT_INT_EQUAL(10, SNPRINTF(NULL, 0, " %s     %d",  "b", 123));

   /* ' modifier. Non-standard, but supported by glibc. */
   CHECK2("1,200,000", "%'d", 1200000);
   CHECK2("-100,006,789", "%'d", -100006789);
   CHECK2("9,888,777,666", "%'lld", u);
   CHECK2("200,000,000.000000", "%'18f", 2e8);
   CHECK2("100,056,789", "%'.0f", 100056789.0);
   CHECK2("100,056,789.0", "%'.1f", 100056789.0);
   CHECK2("000,001,200,000", "%'015d", 1200000);

   /* things not supported by glibc */
   CHECK2("(null)", "%s", (char*)NULL);
   CHECK2("123,4abc:", "%'x:", 0x1234ABC);
   CHECK2("100000000", "%b", 256);
   CHECK3("0b10 0B11", "%#b %#B", 2, 3);
   CHECK4("2 3 4", "%I64d %I32d %Id", u2, 3, u4);
   CHECK3("1k 2.54 M", "%$_d %$.2d", 1000, 2536000);
   CHECK3("2.42 Mi 2.4 M", "%$$.2d %$$$d", 2536000, 2536000);

   /* different separators
   stbsp_set_separators(' ', ',');
   CHECK2("12 345,678900", "%'f", 12345.6789);
   */
}
