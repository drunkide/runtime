#ifndef RUNTIME_STRING_H
#define RUNTIME_STRING_H

#include <runtime/common.h>
#include <stdarg.h>

EXTERN_C_BEGIN

RUNTIME_API size_t Utf8Length(const void* str);
RUNTIME_API size_t Utf8LengthN(const void* str, size_t bytes);

RUNTIME_API void Utf8ToUtf16(void* dst, const void* src);
RUNTIME_API void Utf8ToUtf16N(void* dst, const void* src, size_t bytes);
RUNTIME_API void Utf8ToUtf32(void* dst, const void* src);
RUNTIME_API void Utf8ToUtf32N(void* dst, const void* src, size_t bytes);

RUNTIME_API int StringFormat(char* buf, size_t size, const char* fmt, ...);
RUNTIME_API int StringFormatV(char* buf, size_t size, const char* fmt, va_list args);

EXTERN_C_END

#endif
