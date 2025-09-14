#include <runtime/buffer.h>
#include <runtime/mem.h>
#include <runtime/string.h>
#include <lib/stb/sprintf.h>
#include <string.h>

#ifdef _WIN32
#include <runtime/mswin/winapi.h>
#endif

void BufInit(Buf* buf, void* initial, size_t size)
{
    buf->initial = initial;
    buf->allocated = NULL;
    buf->ptr = initial;
    buf->bytesLeft = size;
    buf->hasNul = 0;
    buf->failed = 0;
}

NOINLINE
void BufFree(Buf* buf)
{
    buf->bytesLeft = 0;
    buf->ptr = NULL;
    buf->initial = NULL;
    buf->hasNul = 0;
    buf->failed = 1;

    if (buf->allocated) {
        MemFree(buf->allocated);
        buf->allocated = NULL;
    }
}

NOINLINE
void BufClear(Buf* buf)
{
    if (buf->allocated) {
        buf->bytesLeft += (size_t)(buf->ptr - buf->allocated);
        buf->ptr = buf->allocated;
    } else {
        buf->bytesLeft += (size_t)(buf->ptr - buf->initial);
        buf->ptr = buf->initial;
    }
}

size_t BufGetCapacity(Buf* buf)
{
    return buf->bytesLeft;
}

size_t BufGetCapacityUtf16(Buf* buf)
{
    return buf->bytesLeft / sizeof(uint16);
}

size_t BufGetCapacityUtf32(Buf* buf)
{
    return buf->bytesLeft / sizeof(uint32);
}

NOINLINE
char* BufReserve(Buf* buf, size_t size)
{
    if (size > buf->bytesLeft) {
        size_t oldSize, newSize;
        char* oldbuf, *newbuf;

        oldbuf = buf->allocated;
        if (oldbuf) {
            oldSize = (size_t)(buf->ptr - oldbuf);
            newSize = oldSize * 2;
            if (newSize <= oldSize + size)
                newSize = oldSize + size + 32;

            newbuf = (char*)MemRealloc(oldbuf, newSize);
            if (!newbuf) {
                buf->failed = true;
                return NULL;
            }
        } else {
            oldbuf = buf->initial;
            oldSize = (size_t)(buf->ptr - oldbuf);
            newSize = oldSize * 2;
            if (newSize <= oldSize + size)
                newSize = oldSize + size + 32;

            newbuf = (char*)MemAlloc(newSize);
            if (!newbuf) {
                buf->failed = true;
                return NULL;
            }

            memcpy(newbuf, oldbuf, oldSize);
        }

        buf->allocated = newbuf;
        buf->bytesLeft += (newSize - oldSize);
        buf->ptr = newbuf + oldSize;
    }

    buf->hasNul = 0;
    return buf->ptr;
}

uint16* BufReserveUtf16(Buf* buf, size_t length)
{
    return (uint16*)BufReserve(buf, length * sizeof(uint16));
}

uint32* BufReserveUtf32(Buf* buf, size_t length)
{
    return (uint32*)BufReserve(buf, length * sizeof(uint32));
}

void BufCommit(Buf* buf, size_t size)
{
    buf->ptr += size;
    buf->bytesLeft -= size;
}

void BufCommitUtf16(Buf* buf, size_t length)
{
    BufCommit(buf, length * sizeof(uint16));
}

void BufCommitUtf32(Buf* buf, size_t length)
{
    BufCommit(buf, length * sizeof(uint32));
}

size_t BufGetLength(Buf* buf)
{
    if (buf->allocated)
        return (size_t)(buf->ptr - buf->allocated);
    else
        return (size_t)(buf->ptr - buf->initial);
}

size_t BufGetLengthUtf16(Buf* buf)
{
    return BufGetLength(buf) / sizeof(uint16);
}

size_t BufGetLengthUtf32(Buf* buf)
{
    return BufGetLength(buf) / sizeof(uint32);
}

void* BufGetPtr(Buf* buf)
{
    if (buf->failed)
        return NULL;

    return (buf->allocated ? buf->allocated : buf->initial);
}

NOINLINE
char* BufGetCStr(Buf* buf)
{
    if (!buf->hasNul) {
        char* dst = BufReserve(buf, 1);
        if (dst) {
            buf->hasNul = 1;
            *dst = 0;
        }
    }

    if (buf->failed)
        return NULL;

    return (buf->allocated ? buf->allocated : buf->initial);
}

NOINLINE
char* BufGetCStrN(Buf* buf, size_t* outLength)
{
    if (!buf->hasNul) {
        char* dst = BufReserve(buf, 1);
        if (dst) {
            buf->hasNul = 1;
            *dst = 0;
        }
    }

    if (buf->failed) {
        *outLength = 0;
        return NULL;
    }

    if (buf->allocated) {
        *outLength = (size_t)(buf->ptr - buf->allocated);
        return buf->allocated;
    } else {
        *outLength = (size_t)(buf->ptr - buf->initial);
        return buf->initial;
    }
}

NOINLINE
uint16* BufGetUtf16(Buf* buf)
{
    if (buf->hasNul < sizeof(uint16)) {
        uint16* dst = (uint16*)BufReserve(buf, sizeof(uint16));
        if (dst) {
            buf->hasNul = sizeof(uint16);
            *dst = 0;
        }
    }

    if (buf->failed)
        return NULL;

    return (uint16*)(buf->allocated ? buf->allocated : buf->initial);
}

NOINLINE
uint16* BufGetUtf16N(Buf* buf, size_t* outLength)
{
    if (buf->hasNul < sizeof(uint16)) {
        uint16* dst = (uint16*)BufReserve(buf, sizeof(uint16));
        if (dst) {
            buf->hasNul = sizeof(uint16);
            *dst = 0;
        }
    }

    if (buf->failed) {
        *outLength = 0;
        return NULL;
    }

    if (buf->allocated) {
        *outLength = (size_t)((uint16*)buf->ptr - (uint16*)buf->allocated);
        return (uint16*)buf->allocated;
    } else {
        *outLength = (size_t)((uint16*)buf->ptr - (uint16*)buf->initial);
        return (uint16*)buf->initial;
    }
}

NOINLINE
uint32* BufGetUtf32(Buf* buf)
{
    if (buf->hasNul < sizeof(uint32)) {
        uint32* dst = (uint32*)BufReserve(buf, sizeof(uint32));
        if (dst) {
            buf->hasNul = sizeof(uint32);
            *dst = 0;
        }
    }

    if (buf->failed)
        return NULL;

    return (uint32*)(buf->allocated ? buf->allocated : buf->initial);
}

NOINLINE
uint32* BufGetUtf32N(Buf* buf, size_t* outLength)
{
    if (buf->hasNul < sizeof(uint32)) {
        uint32* dst = (uint32*)BufReserve(buf, sizeof(uint32));
        if (dst) {
            buf->hasNul = sizeof(uint32);
            *dst = 0;
        }
    }

    if (buf->failed) {
        *outLength = 0;
        return NULL;
    }

    if (buf->allocated) {
        *outLength = (size_t)((uint32*)buf->ptr - (uint32*)buf->allocated);
        return (uint32*)buf->allocated;
    } else {
        *outLength = (size_t)((uint32*)buf->ptr - (uint32*)buf->initial);
        return (uint32*)buf->initial;
    }
}

NOINLINE
bool BufAppend(Buf* buf, const void* data, size_t size)
{
    char* dst = BufReserve(buf, size);
    if (!dst)
        return false;

    memcpy(dst, data, size);
    BufCommit(buf, size);

    return true;
}

bool BufAppendChar(Buf* buf, int ch)
{
    uint8 b = (uint8)(ch & 0xff);
    return BufAppend(buf, &b, 1);
}

NOINLINE
bool BufAppendUtf8Char(Buf* buf, uint32 codepoint)
{
    char ch[UTF8_CHAR_MAX];
    size_t n;

    n = Utf32CharToUtf8(ch, codepoint);
    if (n == 0)
        n = Utf32CharToUtf8(ch, UNICODE_REPLACEMENT_CHAR);

    return BufAppend(buf, ch, n);
}

NOINLINE
bool BufAppendUtf16Char(Buf* buf, uint32 codepoint)
{
    char ch[UTF16_CHAR_MAX];
    size_t n;

    n = Utf32CharToUtf16(ch, codepoint);
    if (n == 0)
        n = Utf32CharToUtf16(ch, UNICODE_REPLACEMENT_CHAR);

    return BufAppend(buf, ch, n * sizeof(uint16));
}

bool BufAppendUtf32Char(Buf* buf, uint32 codepoint)
{
    return BufAppend(buf, &codepoint, sizeof(uint32));
}

NOINLINE
bool BufAppendCStr(Buf* buf, const char* str)
{
    size_t len;

    if (!str)
        return true;

    len = strlen(str);
    return BufAppend(buf, str, len);
}

NOINLINE
bool BufAppendUtf16Str(Buf* buf, const void* str)
{
  #ifdef _WIN32
    return BufAppend(buf, str, (size_t)lstrlenW((const WCHAR*)str) * sizeof(WCHAR));
  #else
    const uint16* p = (const uint16*)str;
    while (*p)
        ++p;
    return BufAppend(buf, str, (size_t)((const char*)p - (const char*)str));
  #endif
}

NOINLINE
bool BufAppendUtf32Str(Buf* buf, const void* str)
{
    const uint32* p = (const uint32*)str;
    while (*p)
        ++p;
    return BufAppend(buf, str, (size_t)((const char*)p - (const char*)str));
}

NOINLINE
bool BufAppendFmt(Buf* buf, const char* fmt, ...)
{
    va_list args;
    bool r;

    va_start(args, fmt);
    r = BufAppendFmtV(buf, fmt, args);
    va_end(args);

    return r;
}

static char* callback(const char* src, void* user, int len)
{
    Buf* dst = (Buf*)user;
    DONT_WARN_UNUSED(src);
    BufCommit(dst, (size_t)len);
    return BufReserve(dst, STB_SPRINTF_MIN);
}

bool BufAppendFmtV(Buf* buf, const char* fmt, va_list args)
{
    char* dst = BufReserve(buf, STB_SPRINTF_MIN);
    stbsp_vsprintfcb(callback, buf, dst, fmt, args);
    return !buf->failed;
}

NOINLINE
bool BufUtf8ToUtf16(Buf* buf, const void* src)
{
    size_t n = Utf8ToUtf16Chars(src);
    uint16* dst = BufReserveUtf16(buf, n);
    if (!dst)
        return false;
    Utf8ToUtf16(dst, src);
    BufCommitUtf16(buf, n);
    return true;
}

NOINLINE
bool BufUtf8ToUtf16N(Buf* buf, const void* src, size_t srcBytes)
{
    size_t n = Utf8ToUtf16CharsN(src, srcBytes);
    uint16* dst = BufReserveUtf16(buf, n);
    if (!dst)
        return false;
    Utf8ToUtf16N(dst, src, srcBytes);
    BufCommitUtf16(buf, n);
    return true;
}

NOINLINE
bool BufUtf16ToUtf8(Buf* buf, const void* src)
{
    size_t n = Utf16ToUtf8Bytes(src);
    char* dst = BufReserve(buf, n);
    if (!dst)
        return false;
    Utf16ToUtf8(dst, src);
    BufCommit(buf, n);
    return true;
}

NOINLINE
bool BufUtf16ToUtf8N(Buf* buf, const void* src, size_t srcLength)
{
    size_t n = Utf16ToUtf8BytesN(src, srcLength);
    char* dst = BufReserve(buf, n);
    if (!dst)
        return false;
    Utf16ToUtf8N(dst, src, srcLength);
    BufCommit(buf, n);
    return true;
}
