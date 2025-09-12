#ifndef RUNTIME_DETAIL_INT64_H
#define RUNTIME_DETAIL_INT64_H

/* initialization */

#ifdef CPU64
 #define UI64_SET0(x) \
    ((void)((x).full = 0))
 #define UI64_SETU32(x, value) \
    ((void)((x).full = (value)))
#else
 #define UI64_SET0(x) \
    ((void)((x).half.low = 0, (x).half.high = 0))
 #define UI64_SETU32(x, value) \
    ((void)((x).half.low = (value), (x).half.high = 0))
#endif

/* type conversion */

#ifdef CPU64
 #define I64_SETU64(x, value) \
    ((void)((x).full = (RUNTIME_FULL_INT64)((value).full)))
 #define U64_SETI64(x, value) \
    ((void)((x).full = (RUNTIME_FULL_UINT64)((value).full)))
#else
 #define I64_SETU64(x, value) \
    ((void)((x).half.low = (value).half.low, (x).half.high = (int32)((value).half.high)))
 #define U64_SETI64(x, value) \
    ((void)((x).half.low = (value).half.low, (x).half.high = (uint32)((value).half.high)))
#endif

/* comparison */

#ifdef CPU64
 #define UI64_IS_ZERO(x) \
    ((x).full == 0)
 #define UI64_IS_NOT_ZERO(x) \
    ((x).full != 0)
 #define U64_IS_LESS32(a, b) \
    ((a).full < (b))
 #define U64_IS_LESS(a, b) \
    ((RUNTIME_FULL_UINT64)((a).full) < (RUNTIME_FULL_UINT64)((b).full))
 #define I64_IS_NEGATIVE(x) \
    ((x).full < 0)
#else
 #define UI64_IS_ZERO(x) \
    ((x).half.low == 0 && (x).half.high == 0)
 #define UI64_IS_NOT_ZERO(x) \
    ((x).half.low != 0 || (x).half.high != 0)
 #define U64_IS_LESS32(a, b) \
    ((a).half.low < (b) && (a).half.high == 0)
 #define U64_IS_LESS(a, b) \
    ((uint32)((a).half.high) < (uint32)((b).half.high) \
        || ((uint32)((a).half.high) == (uint32)((b).half.high) && (a).half.low < (b).half.low))
 #define I64_IS_NEGATIVE(x) \
    ((x).half.high < 0)
#endif

/* negation */

#ifdef CPU64
 #define I64_NEG(x) \
    ((void)((x).full = -(x).full))
#else
 #define I64_NEG(x) \
    ((void)( \
        ((x).half.low = ~(x).half.low + 1, \
         (x).half.high = (int32)(~(uint32)((x).half.high) + ((x).half.low == 0))) \
    ))
#endif

/* left shift */

#ifdef CPU64
 #define U64_SHL(x, shift) \
    ((void)((x).full <<= (shift)))
 #define U64_SHL32(x, shift) \
    ((void)((x).full <<= (shift)))
#else
 #define U64_SHL(x, shift) \
    ((void)(((shift) == 0) \
        ? 0 /* do nothing */ \
        : (((shift) < 32) \
            ? (((x).half.high = ((x).half.high << (shift)) | ((x).half.low >> (32 - (shift)))), \
               ((x).half.low <<= (shift))) \
            : (((x).half.high = (x).half.low << ((shift) - 32)), \
               ((x).half.low = 0)) \
          ) \
    ))
 #define U64_SHL32(x, shift) \
    ((void)( \
        ((x).half.high = ((x).half.high << (shift)) | ((x).half.low >> (32 - (shift)))), \
        ((x).half.low <<= (shift)) \
    ))
#endif

/* unsigned right shift */

#ifdef CPU64
 #define U64_SHR(x, shift) \
    ((void)((x).full = ((RUNTIME_FULL_UINT64)(x).full) >> (shift)))
 #define U64_SHR32(x, shift) \
    ((void)((x).full = ((RUNTIME_FULL_UINT64)(x).full) >> (shift)))
#else
 #define U64_SHR(x, shift) \
    ((void)(((shift) == 0) \
        ? 0 /* do nothing*/ \
        : (((shift) < 32) \
            ? (((x).half.low = ((x).half.low >> (shift)) | ((uint32)((x).half.high) << (32 - (shift)))), \
               ((x).half.high >>= (shift))) \
            : (((x).half.low = (uint32)((x).half.high) >> ((shift) - 32)), \
               ((x).half.high = 0)) \
          ) \
    ))
 #define U64_SHR32(x, shift) \
    ((void)(((shift) == 0) \
        ? 0 /* do nothing */ \
        : (((x).half.low = ((x).half.low >> (shift)) | ((uint32)((x).half.high) << (32 - (shift)))), \
           ((x).half.high >>= (shift))) \
    ))
#endif

/* addition */

#ifdef RUNTIME_FULL_INT64
 #define UI64_ADD(a, b, tmp_carry) \
    ((void)(DONT_WARN_UNUSED(tmp_carry), (a).full += (b).full))
#else
 #define UI64_ADD(a, b, tmp_carry) \
    ( \
        (tmp_carry) = (a).half.low + (b).half.low, \
        (a).half.high += (b).half.high, \
        (a).half.high += ((tmp_carry) < (a).half.low), /* carry */ \
        (a).half.low = (tmp_carry) \
    )
#endif

/* subtraction */

#ifdef RUNTIME_FULL_INT64
 #define UI64_SUB(a, b, tmp_borrow) \
    ((void)(DONT_WARN_UNUSED(tmp_borrow), (a).full -= (b).full))
#else
 #define UI64_SUB(a, b, tmp_borrow) \
    ( \
        (tmp_borrow) = (a).half.low < (b).half.low, \
        (a).half.low -= (b).half.low, \
        (a).half.high -= (b).half.high + (tmp_borrow) \
    )
#endif

/* API functions */

RUNTIME_API int64 RuntimeDoubleToI64(double x);
RUNTIME_API double RuntimeI64ToDouble(int64 x);
RUNTIME_API uint32 RuntimeU64DivMod32(uint64* dst, uint32 divisor);
RUNTIME_API uint64 RuntimeU64DivMod(uint64* dst, uint64 divisor);

#endif
