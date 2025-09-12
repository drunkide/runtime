#include <runtime/common.h>

static const int64 iZero = INT64_INITIALIZER(0, 0);
static const int64 iOne = INT64_INITIALIZER(0, 1);
static const int64 iMinus1 = INT64_INITIALIZER((int32)0xffffffff, 0xffffffff);
static const int64 imin = INT64_INITIALIZER((int32)0x80000000, 0x00000000);
static const int64 imax = INT64_INITIALIZER(0x7fffffff, 0xffffffff);
static const uint64 umax = UINT64_INITIALIZER(0xffffffff, 0xffffffff);

static void test_int64(void)
{
    uint32 tmp_carry;
    int64 i1, i2;
    uint64 u1, u2, u3;

    /* initialization and comparison with 0 */

    UI64_SET0(u1);
    ASSERT_TRUE(UI64_IS_ZERO(u1));
    ASSERT_FALSE(UI64_IS_NOT_ZERO(u1));

    UI64_SET0(i1);
    ASSERT_TRUE(UI64_IS_ZERO(i1));
    ASSERT_FALSE(UI64_IS_NOT_ZERO(i1));

    UI64_SETU32(u2, 0);
    ASSERT_UINT64_EQUAL(u1, u2);
    ASSERT_TRUE(UI64_IS_ZERO(u2));
    ASSERT_FALSE(UI64_IS_NOT_ZERO(u2));

    UI64_SETU32(i2, 0);
    ASSERT_INT64_EQUAL(i1, i2);
    ASSERT_TRUE(UI64_IS_ZERO(i2));
    ASSERT_FALSE(UI64_IS_NOT_ZERO(i2));

    UI64_SETU32(u2, 123);
    ASSERT_FALSE(UI64_IS_ZERO(u2));
    ASSERT_TRUE(UI64_IS_NOT_ZERO(u2));
    ASSERT_UINT32_EQUAL(123, u2.half.low);
    ASSERT_UINT32_EQUAL(0, u2.half.high);

    UI64_SETU32(i2, 123);
    ASSERT_FALSE(UI64_IS_ZERO(i2));
    ASSERT_TRUE(UI64_IS_NOT_ZERO(i2));
    ASSERT_INT32_EQUAL(0, i2.half.high);
    ASSERT_UINT32_EQUAL(123, i2.half.low);

    /* comparison */

    UI64_SETU32(u1, 5);
    UI64_SETU32(u2, 5);
    ASSERT_FALSE(U64_IS_LESS(u1, u2));
    ASSERT_FALSE(U64_IS_LESS(u2, u1));
    ASSERT_FALSE(U64_IS_LESS32(u1, 4));
    ASSERT_FALSE(U64_IS_LESS32(u1, 5));
    ASSERT_TRUE(U64_IS_LESS32(u2, 6));

    UI64_SETU32(u1, 5);
    UI64_SETU32(u2, 6);
    ASSERT_TRUE(U64_IS_LESS(u1, u2));
    ASSERT_FALSE(U64_IS_LESS(u2, u1));
    ASSERT_FALSE(U64_IS_LESS32(u2, 5));
    ASSERT_TRUE(U64_IS_LESS32(u2, 7));

    ASSERT_TRUE(U64_IS_LESS(u1, umax));
    ASSERT_FALSE(U64_IS_LESS(umax, umax));

    /* negation */

    i1 = iZero;
    ASSERT_FALSE(I64_IS_NEGATIVE(i1));
    I64_NEG(i1);
    ASSERT_FALSE(I64_IS_NEGATIVE(i1));
    ASSERT_INT32_EQUAL(0, i1.half.high);
    ASSERT_UINT32_EQUAL(0, i1.half.low);

    i1 = iOne;
    ASSERT_FALSE(I64_IS_NEGATIVE(i1));
    I64_NEG(i1);
    ASSERT_TRUE(I64_IS_NEGATIVE(i1));
    ASSERT_INT32_EQUAL(-1, i1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, i1.half.low);

    i1 = iMinus1;
    ASSERT_TRUE(I64_IS_NEGATIVE(i1));
    I64_NEG(i1);
    ASSERT_FALSE(I64_IS_NEGATIVE(i1));
    ASSERT_INT32_EQUAL(0, i1.half.high);
    ASSERT_UINT32_EQUAL(1, i1.half.low);

    i1 = imin;
    ASSERT_TRUE(I64_IS_NEGATIVE(i1));
    I64_NEG(i1);
    ASSERT_TRUE(I64_IS_NEGATIVE(i1));
    ASSERT_INT32_EQUAL((int32)0x80000000, i1.half.high);
    ASSERT_UINT32_EQUAL(0, i1.half.low);

    i1 = imax;
    ASSERT_FALSE(I64_IS_NEGATIVE(i1));
    I64_NEG(i1);
    ASSERT_TRUE(I64_IS_NEGATIVE(i1));
    ASSERT_INT32_EQUAL((int32)0x80000000, i1.half.high);
    ASSERT_UINT32_EQUAL(1, i1.half.low);

    /* left shift */

    UI64_SETU32(u1, 1);
    U64_SHL(u1, 0);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(1, u1.half.low);

    U64_SHL(u1, 1);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(2, u1.half.low);

    U64_SHL(u1, 31);
    ASSERT_UINT32_EQUAL(1, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    UI64_SETU32(u1, 0x80000000);
    U64_SHL(u1, 0);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.low);

    U64_SHL(u1, 1);
    ASSERT_UINT32_EQUAL(1, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    UI64_SETU32(u1, 0x80000000);
    U64_SHL(u1, 32);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    UI64_SETU32(u1, 1);
    U64_SHL(u1, 63);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);
    U64_SHL(u1, 1);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    UI64_SETU32(u1, 0xffffffff);
    U64_SHL32(u1, 4);
    ASSERT_UINT32_EQUAL(0xf, u1.half.high);
    ASSERT_UINT32_EQUAL(0xfffffff0, u1.half.low);

    u1 = umax;
    U64_SHL32(u1, 1);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.high);
    ASSERT_UINT32_EQUAL(0xfffffffe, u1.half.low);

    u1 = umax;
    U64_SHL(u1, 32);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    u1 = umax;
    U64_SHL(u1, 63);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    /* right shift unsigned */

    U64_SETI64(u1, imin);
    U64_SHR(u1, 1);
    ASSERT_UINT32_EQUAL(0x40000000, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    U64_SHR(u1, 31);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.low);

    U64_SHR(u1, 31);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(1, u1.half.low);

    U64_SETI64(u1, imin);
    U64_SHR32(u1, 1);
    ASSERT_UINT32_EQUAL(0x40000000, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    U64_SHR32(u1, 31);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.low);

    U64_SHR32(u1, 31);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(1, u1.half.low);

    u1 = umax;
    U64_SHR(u1, 63);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(1, u1.half.low);

    UI64_SET0(u1);
    U64_SHR(u1, 63);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    u1 = umax;
    U64_SHR(u1, 32);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    u1 = umax;
    U64_SHR(u1, 1);
    ASSERT_UINT32_EQUAL(0x7fffffff, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    U64_SHR(u1, 1);
    ASSERT_UINT32_EQUAL(0x3fffffff, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    /* addition */

    UI64_SETU32(u1, 4);
    UI64_SETU32(u2, 11);
    UI64_ADD(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(15, u1.half.low);

    UI64_SETU32(u1, 0xffffffff);
    UI64_SET0(u2);
    UI64_ADD(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    UI64_SETU32(u2, 1);
    UI64_ADD(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(1, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    u1 = umax;
    UI64_SET0(u2);
    UI64_ADD(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    u1 = umax;
    UI64_SETU32(u2, 1);
    UI64_ADD(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);

    i1 = imax;
    UI64_SET0(i2);
    UI64_ADD(i1, i2, tmp_carry);
    ASSERT_INT32_EQUAL(0x7fffffff, i1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, i1.half.low);

    i1 = imax;
    UI64_SETU32(i2, 0);
    UI64_ADD(i1, i2, tmp_carry);
    ASSERT_INT32_EQUAL(0x7fffffff, i1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, i1.half.low);

    UI64_ADD(i1, iMinus1, tmp_carry);
    ASSERT_INT32_EQUAL(0x7fffffff, i1.half.high);
    ASSERT_UINT32_EQUAL(0xfffffffe, i1.half.low);

    /* subtraction */

    UI64_SETU32(u1, 11);
    UI64_SETU32(u2, 4);
    UI64_SUB(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(7, u1.half.low);

    u1.half.high = 1; u1.half.low = 0;
    UI64_SETU32(u2, 1);
    UI64_SUB(u1, u2, tmp_carry);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, u1.half.low);

    /* int64 to double conversion */

    ASSERT_DOUBLE_EQUAL(0.0, RuntimeI64ToDouble(iZero));
    ASSERT_DOUBLE_EQUAL(1.0, RuntimeI64ToDouble(iOne));
    ASSERT_DOUBLE_EQUAL(-1.0, RuntimeI64ToDouble(iMinus1));
    ASSERT_DOUBLE_EQUAL(-9223372036854775808.0, RuntimeI64ToDouble(imin));
    ASSERT_DOUBLE_EQUAL(9223372036854775807.0, RuntimeI64ToDouble(imax));

    UI64_SETU32(i1, 42);
    ASSERT_DOUBLE_EQUAL(42.0, RuntimeI64ToDouble(i1));
    UI64_SETU32(i1, 42);
    I64_NEG(i1);
    ASSERT_DOUBLE_EQUAL(-42.0, RuntimeI64ToDouble(i1));

    /* double to int64 conversion */

    i1 = RuntimeDoubleToI64(0.0);
    ASSERT_INT64_EQUAL(i1, iZero);

    i1 = RuntimeDoubleToI64(1.0);
    ASSERT_INT64_EQUAL(i1, iOne);

    i1 = RuntimeDoubleToI64(-1.0);
    ASSERT_INT64_EQUAL(i1, iMinus1);

    i1 = RuntimeDoubleToI64(42.0);
    ASSERT_INT32_EQUAL(0, i1.half.high);
    ASSERT_UINT32_EQUAL(42, i1.half.low);

    i1 = RuntimeDoubleToI64(-42.0);
    ASSERT_INT32_EQUAL(-1, i1.half.high);
    ASSERT_UINT32_EQUAL((uint32)-42, i1.half.low);

    i1 = RuntimeDoubleToI64(-9223372036854775808.0);
    ASSERT_INT32_EQUAL((int32)0x80000000, i1.half.high);
    ASSERT_UINT32_EQUAL(0, i1.half.low);

    i1 = RuntimeDoubleToI64(9223372036854775807.0);
    ASSERT_INT32_EQUAL(0x7fffffff, i1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, i1.half.low);

    i1 = RuntimeDoubleToI64(-1e20);
    ASSERT_INT32_EQUAL((int32)0x80000000, i1.half.high);
    ASSERT_UINT32_EQUAL(0, i1.half.low);

    i1 = RuntimeDoubleToI64(1e20);
    ASSERT_INT32_EQUAL(0x7fffffff, i1.half.high);
    ASSERT_UINT32_EQUAL(0xffffffff, i1.half.low);

    /* DivMod32 */

    UI64_SETU32(u1, 10);
    ASSERT_UINT32_EQUAL(1, RuntimeU64DivMod32(&u1, 3));
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(3, u1.half.low);

    u1.half.low = 0; u1.half.high = 1;
    ASSERT_UINT32_EQUAL(0, RuntimeU64DivMod32(&u1, 2));
    ASSERT_UINT32_EQUAL(0, u1.half.high);
    ASSERT_UINT32_EQUAL(0x80000000, u1.half.low);

    /* DivMod64 */

    u1.half.low = 123; u1.half.high = 456;
    u2.half.low = 123; u2.half.high = 456;
    u3 = RuntimeU64DivMod(&u1, u2);
    ASSERT_UINT32_EQUAL(0, u3.half.low);
    ASSERT_UINT32_EQUAL(0, u3.half.high);
    ASSERT_UINT32_EQUAL(1, u1.half.low);
    ASSERT_UINT32_EQUAL(0, u1.half.high);

    u1.half.low = 1; u1.half.high = 0;
    u2.half.low = 0; u2.half.high = 1;
    u3 = RuntimeU64DivMod(&u1, u2);
    ASSERT_UINT32_EQUAL(1, u3.half.low);
    ASSERT_UINT32_EQUAL(0, u3.half.high);
    ASSERT_UINT32_EQUAL(0, u1.half.low);
    ASSERT_UINT32_EQUAL(0, u1.half.high);

    u1.half.high = 0x0429d074; u1.half.low = 0xbcd97400; /* 300000050000000000 */
    u2.half.high = 0x00000017; u2.half.low = 0x4876e800; /* 100000000000 */
    u3 = RuntimeU64DivMod(&u1, u2);
    ASSERT_UINT32_EQUAL(3000000, u1.half.low);
    ASSERT_UINT32_EQUAL(0, u1.half.high);
}
