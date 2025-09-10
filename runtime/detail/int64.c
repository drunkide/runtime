#include <runtime/common.h>

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

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
    uint64 result = {{0,0}};
    uint64 denominator, numerator;
    unsigned i, n, shift = 0;
    uint32 x;

    if (U64_IS_LESS(*dst, divisor) || UI64_IS_ZERO(divisor)) {
        numerator = *dst;
        UI64_SET0(*dst);
        return numerator;
    }

    if (divisor.half.high != 0)
        x = divisor.half.high;
    else {
        x = divisor.half.low;
        shift = 32;
    }
    while ((x & 0x80000000u) == 0) {
        x <<= 1;
        shift++;
    }

    denominator = divisor;
    numerator = *dst;

    UI64_SHL(denominator, shift);
    UI64_SHL(numerator, shift);

    n = shift;
    if (n > 32)
        n = 32;
    for (i = 0; i <= n; i++) {
        if (!U64_IS_LESS(numerator, denominator)) {
            uint32 borrow = (numerator.half.low < denominator.half.low);
            numerator.half.low -= denominator.half.low;
            numerator.half.high -= denominator.half.high + borrow;
            result.half.high |= (uint32)1 << i;
        }
        U64_SHR32(denominator, 1);
    }

    for (; i <= shift; i++) {
        if (!U64_IS_LESS(numerator, denominator)) {
            uint32 borrow = (numerator.half.low < denominator.half.low);
            numerator.half.low -= denominator.half.low;
            numerator.half.high -= denominator.half.high + borrow;
            result.half.low |= (uint32)1 << (i - 32);
        }
        U64_SHR32(denominator, 1);
    }

    *dst = result;
    U64_SHR(numerator, shift);

    return numerator;
  #endif
}
