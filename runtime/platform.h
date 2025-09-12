#ifndef RUNTIME_PLATFORM_H
#define RUNTIME_PLATFORM_H

#include <runtime/common.h>
#include <runtime/version.h>

#if defined(__EMSCRIPTEN__)
 #define RUNTIME_PLATFORM_HTML5
#elif defined(__linux__)
 #define RUNTIME_PLATFORM_LINUX
#elif defined(__MSDOS__) || defined(__DJGPP__)
 #define RUNTIME_PLATFORM_MSDOS
#elif defined(_WIN32)
 #define RUNTIME_PLATFORM_MSWIN
 #define RUNTIME_PLATFORM_MSWIN_WIN32
 #if defined(CPU64)
  #define RUNTIME_PLATFORM_MSWIN_WIN64
 #endif
#else
 #error Unsupported platform.
#endif

EXTERN_C_BEGIN

void PlatformErrorMessage(const char* message);

void PlatformLogDebug(Buf* buf);
void PlatformLogWarn(Buf* buf);
void PlatformLogError(Buf* buf);

EXTERN_C_END

#endif
