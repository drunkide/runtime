#include <runtime/log.h>
#include <runtime/platform.h>
#include <runtime/buffer.h>
#include <runtime/string.h>

void LogDebug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogDebugV(fmt, args);
    va_end(args);
}

void LogDebugV(const char* fmt, va_list args)
{
    char tmp[512];
    Buf buf;

    BufInit(&buf, tmp, sizeof(tmp));
    BufAppendFmtV(&buf, fmt, args);
    PlatformLogDebug(&buf);
    BufFree(&buf);
}

void LogWarn(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogWarnV(fmt, args);
    va_end(args);
}

void LogWarnV(const char* fmt, va_list args)
{
    char tmp[512];
    Buf buf;

    BufInit(&buf, tmp, sizeof(tmp));
    BufAppendFmtV(&buf, fmt, args);
    PlatformLogWarn(&buf);
    BufFree(&buf);
}

void LogError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogErrorV(fmt, args);
    va_end(args);
}

void LogErrorV(const char* fmt, va_list args)
{
    char tmp[512];
    Buf buf;

    BufInit(&buf, tmp, sizeof(tmp));
    BufAppendFmtV(&buf, fmt, args);
    PlatformLogError(&buf);
    BufFree(&buf);
}
