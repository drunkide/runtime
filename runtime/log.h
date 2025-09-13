#ifndef RUNTIME_LOG_H
#define RUNTIME_LOG_H

#include <runtime/common.h>
#include <stdarg.h>

EXTERN_C_BEGIN

RUNTIME_API void LogDebug(const char* fmt, ...);
RUNTIME_API void LogDebugV(const char* fmt, va_list args);
RUNTIME_API void LogDebugError(const char* fmt, ...);
RUNTIME_API void LogDebugErrorV(const char* fmt, va_list args);
RUNTIME_API void LogWarn(const char* fmt, ...);
RUNTIME_API void LogWarnV(const char* fmt, va_list args);
RUNTIME_API void LogError(const char* fmt, ...);
RUNTIME_API void LogErrorV(const char* fmt, va_list args);

EXTERN_C_END

#endif
