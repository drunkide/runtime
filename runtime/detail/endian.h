#ifndef RUNTIME_DETAIL_ENDIAN_H
#define RUNTIME_DETAIL_ENDIAN_H

#if defined(__GLIBC__) || defined(__GNU_LIBRARY__) || defined(__ANDROID__) || defined(__OpenBSD__)
 #include <endian.h>
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
 #include <machine/endian.h>
#elif defined(BSD) || defined(_SYSTYPE_BSD)
 #include <sys/endian.h>
#endif

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#ifdef __BYTE_ORDER
 #if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
  #define BIG_ENDIAN
 #elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
  #define LITTLE_ENDIAN
 #endif
#endif

#if !defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
 #if (defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)) \
    || (defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)) \
    || defined(__AARCH64EB__) \
    || defined(__ARMEB__) \
    || defined(__THUMBEB__) \
    || defined(_MIPSEB) \
    || defined(__MIPSEB) \
    || defined(__MIPSEB__) \
    || defined(_POWER) \
    || defined(__powerpc__) \
    || defined(__sparc) \
    || defined(__sparc__) \
    || defined(__ppc__) \
    || defined(__hpux) \
    || defined(__hppa) \
    || defined(__s390__)
  #define BIG_ENDIAN
 #elif (defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) \
    || (defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)) \
    || defined(__AARCH64EL__) \
    || defined(__ARMEL__) \
    || defined(__THUMBEL__) \
    || defined(_MIPSEL) \
    || defined(__MIPSEL) \
    || defined(__MIPSEL__) \
    || defined(_M_ALPHA) \
    || defined(_M_IX86) \
    || defined(_M_IA64) \
    || defined(_M_AMD64) \
    || defined(_M_X64) \
    || defined(_X86_) \
    || defined(__X86__) \
    || defined(__riscv) \
    || defined(__e2k__) \
    || defined(__loongarch__) \
    || defined(__alpha__) \
    || defined(__bfin__) \
    || defined(__386__) \
    || defined(__i386__) \
    || defined(__ia64) \
    || defined(__ia64__) \
    || defined(__amd64) \
    || defined(__amd64__) \
    || defined(__x86_64) \
    || defined(__x86_64__)
  #define LITTLE_ENDIAN
 #else
  #error Unable to detect endianess
 #endif
#endif

#endif
