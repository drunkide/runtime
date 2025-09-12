#include <runtime/string.h>
#include <lib/stb/sprintf.h>
#include <lib/utf8/utf8.h>

/********************************************************************************************************************/

#define UTF8_FOREACH(SRC, ACTION) \
    const char* inp = (const char*)(SRC); \
    Utf8State state; \
    UTF8_INIT(state); \
    for (; *inp; ++inp) \
        UTF8_STEP(state, *inp, ACTION) \
    UTF8_END(state, ACTION)

#define UTF8_FOREACH_N(SRC, N, ACTION) \
    const char* inp = (const char*)(SRC); \
    const char* inp_end = inp + (N); \
    Utf8State state; \
    UTF8_INIT(state); \
    for (; inp != inp_end; ++inp) \
        UTF8_STEP(state, *inp, ACTION) \
    UTF8_END(state, ACTION)

size_t Utf8Length(const void* str)
{
    size_t length = 0;
    UTF8_FOREACH(str,
        ++length
        )
    return length;
}

size_t Utf8LengthN(const void* str, size_t bytes)
{
    size_t length = 0;
    UTF8_FOREACH_N(str, bytes,
        ++length
        )
    return length;
}

#define UTF16_WRITE(codep) \
    if ((codep) < 0x10000) \
        *outp++ = (uint16)(codep); \
    else { \
        *outp++ = (uint16)(UTF16_HIGH_SURROGATE_MIN - 0x40 + ((codep) >> 10)); \
        *outp++ = (uint16)(UTF16_LOW_SURROGATE_MIN + ((codep) & 0x3ff)); \
    }

void Utf8ToUtf16(void* dst, const void* src)
{
    uint16* outp = (uint16*)dst;
    UTF8_FOREACH(src,
            UTF16_WRITE(state.codep)
        )
    *outp = 0;
}

void Utf8ToUtf16N(void* dst, const void* src, size_t bytes)
{
    uint16* outp = (uint16*)dst;
    UTF8_FOREACH_N(src, bytes,
            UTF16_WRITE(state.codep)
        )
    DONT_WARN_UNUSED(outp);
}

void Utf8ToUtf32(void* dst, const void* src)
{
    uint32* outp = (uint32*)dst;
    UTF8_FOREACH(src,
            *outp++ = state.codep
        )
    *outp = 0;
}

void Utf8ToUtf32N(void* dst, const void* src, size_t bytes)
{
    uint32* outp = (uint32*)dst;
    UTF8_FOREACH_N(src, bytes,
            *outp++ = state.codep
        )
    DONT_WARN_UNUSED(outp);
}

/********************************************************************************************************************/

size_t Utf32CharToUtf8(void* dst, uint32 codepoint)
{
    uint8* p = (uint8*)dst;

    if (codepoint <= 0x7f) {
        *p = (uint8)codepoint;
        return 1;
    }

    if (codepoint <= 0x7ff) {
        p[0] = (uint8)(0xc0 | (codepoint >> 6));
        p[1] = (uint8)(0x80 | (codepoint & 0x3f));
        return 2;
    }

    if (codepoint <= 0xffff) {
        if (codepoint >= UTF16_HIGH_SURROGATE_MIN && codepoint <= UTF16_LOW_SURROGATE_MAX)
            return 0;
        p[0] = (uint8)(0xe0 |  (codepoint >> 12));
        p[1] = (uint8)(0x80 | ((codepoint >>  6) & 0x3f));
        p[2] = (uint8)(0x80 |  (codepoint & 0x3f));
        return 3;
    }

    if (codepoint <= UNICODE_MAX_CODEPOINT) {
        p[0] = (uint8)(0xf0 |  (codepoint >> 18));
        p[1] = (uint8)(0x80 | ((codepoint >> 12) & 0x3f));
        p[2] = (uint8)(0x80 | ((codepoint >>  6) & 0x3f));
        p[3] = (uint8)(0x80 |  (codepoint & 0x3f));
        return 4;
    }

    return 0;
}

/********************************************************************************************************************/

int StrFormat(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    int r;

    va_start(args, fmt);
    r = stbsp_vsnprintf(buf, (int)size, fmt, args);
    va_end(args);

    return r;
}

int StrFormatV(char* buf, size_t size, const char* fmt, va_list args)
{
    return stbsp_vsnprintf(buf, (int)size, fmt, args);
}

/********************************************************************************************************************/

#ifdef _MSC_VER
 #pragma warning(disable:4756) /* overflow in constant arithmetic */
#endif

#define STB_SPRINTF_IMPLEMENTATION
#include <lib/stb/sprintf.h>
