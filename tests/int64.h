#include <runtime/common.h>

static const int64 iZero = INT64_INITIALIZER(0, 0);
static const int64 iOne = INT64_INITIALIZER(0, 1);
static const int64 iMinus1 = INT64_INITIALIZER(0xffffffff, 0xffffffff);
static const int64 imin = INT64_INITIALIZER(0x80000000, 0x00000000);
static const int64 imax = INT64_INITIALIZER(0x7fffffff, 0xffffffff);
static const uint64 umax = UINT64_INITIALIZER(0xffffffff, 0xffffffff);

static void test_int64(void)
{
    int64 i1, i2;
    uint64 u1, u2;

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
    ASSERT_UINT32_EQUAL(123, i2.half.low);
    ASSERT_UINT32_EQUAL(0, i2.half.high);

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
}
