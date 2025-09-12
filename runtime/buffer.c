#include <runtime/buffer.h>
#include <runtime/mem.h>
#include <runtime/string.h>
#include <lib/stb/sprintf.h>
#include <string.h>

void BufInit(Buf* buf, void* initial, size_t size)
{
    buf->initial = initial;
    buf->allocated = NULL;
    buf->ptr = initial;
    buf->bytesLeft = size;
    buf->failed = false;
}

void BufFree(Buf* buf)
{
    buf->bytesLeft = 0;
    buf->ptr = NULL;
    buf->initial = NULL;

    if (buf->allocated) {
        MemFree(buf->allocated);
        buf->allocated = NULL;
    }
}

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

    return buf->ptr;
}

void BufCommit(Buf* buf, size_t size)
{
    buf->ptr += size;
    buf->bytesLeft -= size;
}

bool BufAppend(Buf* buf, const void* data, size_t size)
{
    char* dst = BufReserve(buf, size);
    if (!dst)
        return false;

    memcpy(dst, data, size);
    BufCommit(buf, size);

    return true;
}

bool BufAppendCStr(Buf* buf, const char* str)
{
    size_t len;

    if (!str)
        return true;

    len = strlen(str);
    return BufAppend(buf, str, len);
}

bool BufAppendUtf8(Buf* buf, uint32 codepoint)
{
    char ch[UTF8_CHAR_MAX];
    size_t n;

    n = Utf32CharToUtf8(ch, codepoint);
    if (n == 0)
        n = Utf32CharToUtf8(ch, UNICODE_REPLACEMENT_CHAR);

    return BufAppend(buf, ch, n);
}

bool BufFormat(Buf* buf, const char* fmt, ...)
{
    va_list args;
    bool r;

    va_start(args, fmt);
    r = BufFormatV(buf, fmt, args);
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

bool BufFormatV(Buf* buf, const char* fmt, va_list args)
{
    char* dst = BufReserve(buf, STB_SPRINTF_MIN);
    stbsp_vsprintfcb(callback, buf, dst, fmt, args);
    return !buf->failed;
}
