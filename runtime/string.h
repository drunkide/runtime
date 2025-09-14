#ifndef RUNTIME_STRING_H
#define RUNTIME_STRING_H

#include <runtime/common.h>
#include <stdarg.h>

enum {
    UTF8_CHAR_MAX = 4,
    UTF16_CHAR_MAX = 2,

    UTF16_LOW_SURROGATE_MIN = 0xdc00,
    UTF16_LOW_SURROGATE_MAX = 0xdfff,
    UTF16_HIGH_SURROGATE_MIN = 0xd800,
    UTF16_HIGH_SURROGATE_MAX = 0xdbff,

    UNICODE_MAX_CODEPOINT = 0x10ffff,
    UNICODE_REPLACEMENT_CHAR = 0xfffd
};

EXTERN_C_BEGIN

RUNTIME_API size_t Utf8Length(const void* str);
RUNTIME_API size_t Utf8LengthN(const void* str, size_t strBytes);

RUNTIME_API size_t Utf8ToUtf16Chars(const void* str);
RUNTIME_API size_t Utf8ToUtf16CharsN(const void* str, size_t strBytes);
RUNTIME_API void Utf8ToUtf16(void* dst, const void* src);
RUNTIME_API void Utf8ToUtf16N(void* dst, const void* src, size_t srcBytes);

RUNTIME_API void Utf8ToUtf32(void* dst, const void* src);
RUNTIME_API void Utf8ToUtf32N(void* dst, const void* src, size_t srcBytes);

RUNTIME_API size_t Utf16Length(const void* str);
RUNTIME_API size_t Utf16LengthN(const void* str, size_t strChars);

RUNTIME_API size_t Utf16ToUtf8Bytes(const void* str);
RUNTIME_API size_t Utf16ToUtf8BytesN(const void* str, size_t strChars);
RUNTIME_API size_t Utf16ToUtf8(void* dst, const void* src);
RUNTIME_API size_t Utf16ToUtf8N(void* dst, const void* src, size_t srcBytes);

RUNTIME_API size_t Utf32CharToUtf8(void* dst, uint32 codepoint);
RUNTIME_API size_t Utf32CharToUtf16(void* dst, uint32 codepoint);

RUNTIME_API char* StrDup(const void* str);
RUNTIME_API char* StrDupN(const void* str, size_t strBytes);
RUNTIME_API uint16* StrDupUtf16(const void* str);
RUNTIME_API uint16* StrDupUtf16N(const void* str, size_t strChars);

RUNTIME_API int StrFormat(char* buf, size_t size, const char* fmt, ...);
RUNTIME_API int StrFormatV(char* buf, size_t size, const char* fmt, va_list args);

EXTERN_C_END

#endif
