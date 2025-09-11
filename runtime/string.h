#ifndef RUNTIME_STRING_H
#define RUNTIME_STRING_H

#include <runtime/common.h>
#include <stdarg.h>

EXTERN_C_BEGIN

RUNTIME_API int StringFormat(char* buf, size_t size, const char* fmt, ...);
RUNTIME_API int StringFormatV(char* buf, size_t size, const char* fmt, va_list args);

EXTERN_C_END

#endif
