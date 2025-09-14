#ifndef RUNTIME_BUFFER_H
#define RUNTIME_BUFFER_H

#include <runtime/common.h>
#include <stdarg.h>

struct Buf {
    char* initial;
    char* allocated;
    char* ptr;
    size_t bytesLeft;
    uint8 hasNul;
    uint8 failed;
};

#define UNINITIALIZED_BUFFER \
    { NULL, NULL, NULL, 0, 0, 0 }

EXTERN_C_BEGIN

RUNTIME_API void BufInit(Buf* buf, void* initial, size_t size);
RUNTIME_API void BufFree(Buf* buf);

RUNTIME_API void BufClear(Buf* buf);

RUNTIME_API size_t BufGetCapacity(Buf* buf);
RUNTIME_API size_t BufGetCapacityUtf16(Buf* buf);
RUNTIME_API size_t BufGetCapacityUtf32(Buf* buf);

RUNTIME_API char* BufReserve(Buf* buf, size_t size);
RUNTIME_API uint16* BufReserveUtf16(Buf* buf, size_t length);
RUNTIME_API uint32* BufReserveUtf32(Buf* buf, size_t length);
RUNTIME_API void BufCommit(Buf* buf, size_t size);
RUNTIME_API void BufCommitUtf16(Buf* buf, size_t length);
RUNTIME_API void BufCommitUtf32(Buf* buf, size_t length);

RUNTIME_API size_t BufGetLength(Buf* buf);
RUNTIME_API size_t BufGetLengthUtf16(Buf* buf);
RUNTIME_API size_t BufGetLengthUtf32(Buf* buf);
RUNTIME_API void* BufGetPtr(Buf* buf);
RUNTIME_API char* BufGetCStr(Buf* buf);
RUNTIME_API char* BufGetCStrN(Buf* buf, size_t* outLength);
RUNTIME_API uint16* BufGetUtf16(Buf* buf);
RUNTIME_API uint16* BufGetUtf16N(Buf* buf, size_t* outLength);
RUNTIME_API uint32* BufGetUtf32(Buf* buf);
RUNTIME_API uint32* BufGetUtf32N(Buf* buf, size_t* outLength);

RUNTIME_API bool BufAppend(Buf* buf, const void* data, size_t size);
RUNTIME_API bool BufAppendChar(Buf* buf, int ch);
RUNTIME_API bool BufAppendUtf8Char(Buf* buf, uint32 codepoint);
RUNTIME_API bool BufAppendUtf16Char(Buf* buf, uint32 codepoint);
RUNTIME_API bool BufAppendUtf32Char(Buf* buf, uint32 codepoint);
RUNTIME_API bool BufAppendCStr(Buf* buf, const char* str);
RUNTIME_API bool BufAppendUtf16Str(Buf* buf, const void* str);
RUNTIME_API bool BufAppendUtf32Str(Buf* buf, const void* str);
RUNTIME_API bool BufAppendFmt(Buf* buf, const char* fmt, ...);
RUNTIME_API bool BufAppendFmtV(Buf* buf, const char* fmt, va_list args);

RUNTIME_API bool BufUtf8ToUtf16(Buf* buf, const void* src);
RUNTIME_API bool BufUtf8ToUtf16N(Buf* buf, const void* src, size_t srcBytes);
RUNTIME_API bool BufUtf16ToUtf8(Buf* buf, const void* src);
RUNTIME_API bool BufUtf16ToUtf8N(Buf* buf, const void* src, size_t srcLength);

EXTERN_C_END

#endif
