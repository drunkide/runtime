#include <runtime/common.h>

/********************************************************************************************************************/

int64 RuntimeDoubleToI64(double x)
{
    const double mul = 4294967296.0; /* 0x100000000 */
    const double min_i64 = -9223372036854775808.0;
    const double max_i64 =  9223372036854775807.0;

    int64 r = {{0,0}};
    double rem;
    uint32 high;

    if (x <= min_i64) {
        r.half.low = 0;
        r.half.high = (int32)0x80000000;
        return r;
    }
    if (x >= max_i64) {
        r.half.low = 0xffffffff;
        r.half.high = 0x7fffffff;
        return r;
    }

    if (x >= 0) {
        high = (uint32)(x / mul);
        rem = x - (double)high * mul;
        r.half.low = (uint32)rem;
        r.half.high = (int32)high;
    } else {
        x = -x;
        high = (uint32)(x / mul);
        rem = x - (double)high * mul;
        r.half.low = ~(uint32)rem + 1;
        r.half.high = (int32)(~high + (r.half.low == 0));
    }

    return r;
}

double RuntimeI64ToDouble(int64 x)
{
  #ifdef CPU64
    return (double)x.full;
  #else
    const double mul = 4294967296.0; /* 0x100000000 */
    uint32 low, high;

    if (x.half.high >= 0)
        return (double)x.half.high * mul + (double)x.half.low;

    low = (uint32)x.half.low;
    high = (uint32)x.half.high;

    if (low == 0 && high == 0x80000000)
        return -9223372036854775808.0;

    low = ~low;
    high = ~high;
    if (++low == 0)
        ++high;

    return -((double)high * mul + (double)low);
  #endif
}

/********************************************************************************************************************/

uint32 RuntimeU64DivMod32(uint64* dst, uint32 divisor)
{
  #ifdef CPU64
    uint32 r = (uint32)(dst->full % divisor);
    dst->full /= divisor;
    return r;
  #else
    unsigned shift;
    uint32 r = 0;

    uint32 origHigh = dst->half.high;
    uint32 origLow = dst->half.low;

    uint32 newHigh = 0;
    uint32 newLow = 0;

    for (shift = 32; shift != 0; ) {
        --shift;
        r = (r << 1) | ((origHigh >> shift) & 1);
        if (r >= divisor) {
            r -= divisor;
            newHigh |= (uint32)1 << shift;
        }
    }

    for (shift = 32; shift != 0; ) {
        --shift;
        r = (r << 1) | ((origLow >> shift) & 1);
        if (r >= divisor) {
            r -= divisor;
            newLow |= (uint32)1 << shift;
        }
    }

    dst->half.low = newLow;
    dst->half.high = newHigh;

    return r;
  #endif
}

/********************************************************************************************************************/

uint64 RuntimeU64DivMod(uint64* dst, uint64 divisor)
{
  #ifdef CPU64
    uint64 r;
    r.full = dst->full % divisor.full;
    dst->full /= divisor.full;
    return r;
  #else
    uint64 dividend, result = {{0,0}}, remainder = {{0,0}};
    uint32 tmp_borrow;
    int i;

    dividend = *dst;
    if (U64_IS_LESS(dividend, divisor) || UI64_IS_ZERO(divisor)) {
        UI64_SET0(*dst);
        return dividend;
    }

    for (i = 31; i >= 0; i--) {
        U64_SHL32(remainder, 1);
        remainder.half.low |= (dividend.half.high >> i) & 1;

        if (!U64_IS_LESS(remainder, divisor)) {
            UI64_SUB(remainder, divisor, tmp_borrow);
            result.half.high |= (uint32)1 << i;
        }
    }

    for (i = 31; i >= 0; i--) {
        U64_SHL32(remainder, 1);
        remainder.half.low |= (dividend.half.low >> i) & 1;

        if (!U64_IS_LESS(remainder, divisor)) {
            UI64_SUB(remainder, divisor, tmp_borrow);
            result.half.low |= (uint32)1 << i;
        }
    }

    *dst = result;
    return remainder;
  #endif
}
