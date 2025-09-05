#ifndef RUNTIME_WIN32_H
#define RUNTIME_WIN32_H
#ifdef _WIN32

#ifdef _MSC_VER
 #if _MSC_VER >= 1200
  #pragma warning(push)
 #endif
 #if _MSC_VER >= 1912
  #pragma warning(disable:5039) /* potentially throwing function passed to 'extern "C"' function */
 #endif
 #pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union */
 #pragma warning(disable:4214) /* nonstandard extension used : bit field types other than int */
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER >= 1200
 #pragma warning(pop)
#endif

#endif
#endif
