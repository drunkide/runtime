#ifndef RUNTIME_BUFFER_H
#define RUNTIME_BUFFER_H

#include <runtime/common.h>
#include <stdarg.h>

STRUCT(Buf) {
    char* initial;
    char* allocated;
    char* ptr;
    size_t bytesLeft;
    bool failed;
};

EXTERN_C_BEGIN

RUNTIME_API void BufInit(Buf* buf, void* initial, size_t size);
RUNTIME_API void BufFree(Buf* buf);

RUNTIME_API void BufClear(Buf* buf);

RUNTIME_API char* BufReserve(Buf* buf, size_t size);
RUNTIME_API void BufCommit(Buf* buf, size_t size);

RUNTIME_API bool BufAppend(Buf* buf, const void* data, size_t size);
RUNTIME_API bool BufAppendCStr(Buf* buf, const char* str);
RUNTIME_API bool BufAppendUtf8(Buf* buf, uint32 codepoint);
RUNTIME_API bool BufFormat(Buf* buf, const char* fmt, ...);
RUNTIME_API bool BufFormatV(Buf* buf, const char* fmt, va_list args);

EXTERN_C_END

#endif
