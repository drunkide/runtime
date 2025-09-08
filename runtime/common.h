#ifndef RUNTIME_COMMON_H
#define RUNTIME_COMMON_H

#ifdef _MSC_VER
 #pragma warning(disable:4324) /* structure was padded due to alignment */
 #pragma warning(disable:4514) /* unreferenced inline function has been removed */
 #pragma warning(disable:4710) /* function not inlined */
 #pragma warning(disable:4711) /* function selected for automatic inline expansion */
 #pragma warning(disable:4738) /* storing 32-bit float result in memory, possible loss of performance */
 #if _MSC_VER >= 1300
  #pragma warning(disable:4820) /* padding added after data member */
 #endif
 #if _MSC_VER >= 1914
  #pragma warning(disable:5045) /* compiler will insert Spectre mitigation if /Qspectre switch specified */
 #endif
#endif

#include <runtime/detail/types.h>

typedef uint8 byte;

#define STRUCT(X) \
    struct X; \
    typedef struct X X; \
    struct X

#define UNION(X) \
    union X; \
    typedef union X X; \
    union X

#define EMPTY_STRUCT \
    unsigned char dummy__;

#if defined(_MSC_VER) && _MSC_VER < 1000
 #define DONT_WARN_UNUSED(X) \
   ((X) = (X))
#else
 #define DONT_WARN_UNUSED(X) \
   ((void)(X))
#endif

#if !defined(__cplusplus)
 #define NOTHROW
 #define EXTERN_C extern
 #define EXTERN_C_BEGIN
 #define EXTERN_C_END
#else
 #define NOTHROW throw()
 #define EXTERN_C extern "C"
 #define EXTERN_C_BEGIN extern "C" {
 #define EXTERN_C_END }
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1020
 #define NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
 #define NOINLINE __attribute__((noinline))
#else
 #define NOINLINE /* empty */
#endif

#if defined(_WIN32) \
    && (!defined(__BORLANDC__) || __BORLANDC__ >= 0x0550) \
    && (!defined(__WATCOMC__) || __WATCOMC__ >= 1280)
 #define DLLEXPORT __declspec(dllexport)
 #define DLLIMPORT __declspec(dllimport)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
 #define DLLEXPORT __attribute__((visibility("default")))
 #define DLLIMPORT __attribute__((visibility("default")))
#else
 #define DLLEXPORT /* empty */
 #define DLLIMPORT /* empty */
#endif

#ifdef RUNTIME_STATIC
 #define RUNTIME_API /* empty */
#elif defined(RUNTIME_EXPORTS)
 #define RUNTIME_API DLLEXPORT
#else
 #define RUNTIME_API DLLIMPORT
#endif

#endif
