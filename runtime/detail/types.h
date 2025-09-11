#ifndef RUNTIME_DETAIL_TYPES_H
#define RUNTIME_DETAIL_TYPES_H

#include <stddef.h>
#include <limits.h>
#include <runtime/detail/endian.h>

/********************************************************************************************************************/
/* Boolean type */

#if (defined(__STDC__) && __STDC__ && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(_MSC_VER) && _MSC_VER >= 1800) \
    || (defined(__GNUC__) && __GNUC__ >= 3) \
    || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 900) \
    || (defined(__SUNPRO_C) && __SUNPRO_C >= 0x590) \
    || (defined(__WATCOMC__) && __WATCOMC__ >= 1280)
 #include <stdbool.h>
#elif !defined(__cplusplus)
 #define true 1
 #define false 0
 #define bool unsigned char
#endif

/********************************************************************************************************************/
/* 8, 16 and 32 bit integer types */

#if (defined(__STDC__) && __STDC__ && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(_MSC_VER) && _MSC_VER >= 1600) \
    || (defined(__GNUC__) && __GNUC__ >= 3) \
    || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 900) \
    || (defined(__SUNPRO_C) && __SUNPRO_C >= 0x570) \
    || (defined(__WATCOMC__) && __WATCOMC__ >= 1250)
 #include <stdint.h>
 typedef uint8_t uint8;
 typedef uint16_t uint16;
 typedef uint32_t uint32;
 typedef int8_t int8;
 typedef int16_t int16;
 typedef int32_t int32;
#else
 #if CHAR_BIT == 8
    typedef signed char int8;
    typedef unsigned char uint8;
 #else
    #error Unable to detect 8-bit data type
 #endif

 #if USHRT_MAX == 0xffff
    typedef signed short int16;
    typedef unsigned short uint16;
 #elif UINT_MAX == 0xffff
    typedef signed int int16;
    typedef unsigned int uint16;
 #else
    #error Unable to detect 16-bit data type
 #endif

 #if UINT_MAX == 0xfffffffful
    typedef signed int int32;
    typedef unsigned int uint32;
 #elif ULONG_MAX == 0xfffffffful
    typedef signed long int32;
    typedef unsigned long uint32;
 #else
    #error Unable to detect 32-bit data type
 #endif
#endif

/********************************************************************************************************************/
/* 64-bit integer types */

#if defined(CPU64)
  #define RUNTIME_FULL_INT64 int64_t
  #define RUNTIME_FULL_UINT64 uint64_t
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 291)
  #define RUNTIME_FULL_INT64 __extension__ signed long long
  #define RUNTIME_FULL_UINT64 __extension__ unsigned long long
#elif defined(_MSC_VER) && _MSC_VER >= 900
  #define RUNTIME_FULL_INT64 signed __int64
  #define RUNTIME_FULL_UINT64 unsigned __int64
#elif defined(__STDC__) && __STDC__ && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
  #define RUNTIME_FULL_INT64 int64_t
  #define RUNTIME_FULL_UINT64 uint64_t
#elif defined(LONG_LONG_MAX)
 #if LONG_LONG_MAX == 0x7fffffffffffffff
  #define RUNTIME_FULL_INT64 signed long long
  #define RUNTIME_FULL_UINT64 unsigned long long
 #endif
#endif

typedef union {
    struct {
      #ifdef LITTLE_ENDIAN
        #define INT64_INITIALIZER(high, low) {{(low),(high)}}
        uint32 low;
        int32 high;
      #else
        #define INT64_INITIALIZER(high, low) {{(high),(low)}}
        int32 high;
        uint32 low;
      #endif
    } half;

  #ifdef RUNTIME_FULL_INT64
    RUNTIME_FULL_INT64 full;
  #endif
} int64;

typedef union {
    struct {
      #ifdef LITTLE_ENDIAN
        #define UINT64_INITIALIZER(high, low) {{(low),(high)}}
        uint32 low;
        uint32 high;
      #else
        #define UINT64_INITIALIZER(high, low) {{(high),(low)}}
        uint32 high;
        uint32 low;
      #endif
    } half;

  #ifdef RUNTIME_FULL_UINT64
    RUNTIME_FULL_UINT64 full;
  #endif
} uint64;

#include <runtime/detail/int64.h>

/********************************************************************************************************************/
/* Other integer types */

typedef size_t uintptr;
typedef ptrdiff_t intptr;

#endif
