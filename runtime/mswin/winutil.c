#include <runtime/mswin/winutil.h>
#include <runtime/mswin/winapi.h>
#include <runtime/log.h>
#include <runtime/buffer.h>

#if defined(OLD_BORLAND) || defined(OLD_WATCOM)
#define wcslen lstrlenW
#endif

/********************************************************************************************************************/

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64

bool BufMultiByteToWideChar(Buf* buf, const char* src)
{
    size_t srcLen;
    WCHAR* dst;
    int dstLen;

    if (!src)
        return true;

    srcLen = strlen(src);
    if (srcLen == 0)
        return true;

    dstLen = MultiByteToWideChar(CP_ACP, 0, src, (int)srcLen, NULL, 0);
    if (dstLen != 0) {
        dst = (WCHAR*)BufReserveUtf16(buf, (size_t)dstLen);
        if (dst) {
            dstLen = MultiByteToWideChar(CP_ACP, 0, src, (int)srcLen, dst, dstLen);
            if (dstLen != 0) {
                BufCommitUtf16(buf, (size_t)dstLen);
                return true;
            }
        }
    }

    LogDebugError("%s failed (code 0x%08X).", "MultiByteToWideChar", (uint32)GetLastError());

    dst = (WCHAR*)BufReserveUtf16(buf, srcLen);
    if (dst) {
        while (*src)
            *dst++ = (WCHAR)(uint8)*src++;
        BufCommitUtf16(buf, srcLen);
    }

    return false;
}

#endif

/********************************************************************************************************************/

static bool WinGetModuleFileNameW(Buf* buf, HMODULE hModule)
{
    DWORD bufSize, retSize;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    EXTPROC(Kernel32, GetModuleFileNameW);
    if (!pfnGetModuleFileNameW)
        return false;
  #endif

    bufSize = (DWORD)BufGetCapacityUtf16(buf);
    do {
        WCHAR* dst = (WCHAR*)BufReserveUtf16(buf, bufSize);
        if (!dst)
            return false;

        SetLastError(0);

      #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
        retSize = GetModuleFileNameW(hModule, dst, bufSize);
      #else
        retSize = pfnGetModuleFileNameW(hModule, dst, bufSize);
      #endif

        if (retSize != bufSize) {
            if (retSize == 0) {
                LogDebugError("%s failed (code 0x%08X).", "GetModuleFileNameW", (uint32)GetLastError());
                return false;
            }

            BufCommitUtf16(buf, retSize);
            buf->hasNul = 2; /* WinAPI has already put a NUL there */
            return true;
        }

        bufSize *= 2;
    } while (bufSize < 1048576); /* stay at least minimally reasonable... */

    return false;
}

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
static bool WinGetModuleFileNameA(Buf* buf, HMODULE hModule)
{
    DWORD bufSize, retSize;

    bufSize = BufGetCapacity(buf);
    do {
        char* dst = (char*)BufReserve(buf, bufSize);
        if (!dst)
            return false;

        SetLastError(0);

        retSize = GetModuleFileNameA(hModule, dst, bufSize);
        if (retSize != bufSize) {
            if (retSize == 0) {
                LogDebugError("%s failed (code 0x%08X).", "GetModuleFileNameA", (uint32)GetLastError());
                return false;
            }

            BufCommit(buf, retSize);
            buf->hasNul = 1; /* WinAPI has already put a NUL there */
            return true;
        }

        bufSize *= 2;
    } while (bufSize < 1048576); /* stay at least minimally reasonable... */

    return false;
}
#endif

bool BufGetModuleFileNameW(Buf* buf, void* hModule)
{
    if (WinGetModuleFileNameW(buf, hModule))
        return true;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    {
        char ansi[MAX_PATH];
        Buf ansibuf;
        BufInit(&ansibuf, ansi, sizeof(ansi));
        if (WinGetModuleFileNameA(&ansibuf, hModule)) {
            BufClear(buf);
            BufMultiByteToWideChar(buf, BufGetCStr(&ansibuf));
            return true;
        }
        BufFree(&ansibuf);
    }
  #endif

    return false;
}

/********************************************************************************************************************/

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64

static int WinParseCommandLine(LPWSTR p, WCHAR** argv)
{
    int argc = 0;

    do {
        if (*p != '"') {
            if (argv)
                argv[argc] = p;
            ++argc;
            while (*p && *p != ' ' && *p != '\t')
                ++p;
        } else {
            ++p;
            if (argv)
                argv[argc] = p;
            ++argc;
            while (*p && *p != '"')
                ++p;
        }

        if (!*p)
            break;
        if (argv)
            *p = 0;
        ++p;

        while (*p && (*p == ' ' || *p == '\t'))
            ++p;
    } while (*p);

    return argc;
}

#endif

wchar_t** WinCommandLineToArgv(Buf* exeBuf, const wchar_t* cmdline, int* argc)
{
    LPWSTR* argv;
    int n = 0;

    /* try to use CommandLineToArgvW */

    EXTPROC(Shell32, CommandLineToArgvW);
    if (!pfnCommandLineToArgvW)
        argv = NULL;
    else {
        argv = pfnCommandLineToArgvW(cmdline, &n);
        if (!argv)
            LogDebugError("%s failed (code 0x%08X).", "CommandLineToArgvW", (uint32)GetLastError());
    }

    /* if that failed, use our own implementation */

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!argv) {
        int count = WinParseCommandLine((LPWSTR)cmdline, NULL);

        LPWSTR cmd;
        size_t cmdSize = sizeof(WCHAR) * (wcslen(cmdline) + 1);
        size_t argSize = sizeof(WCHAR*) * (size_t)count;
        char* buf = (char*)LocalAlloc(LMEM_FIXED, argSize + cmdSize);
        if (!buf) {
            LogDebugError("%s failed (code 0x%08X).", "LocalAlloc", (uint32)GetLastError());
            return NULL;
        }

        argv = (LPWSTR*)buf;
        cmd = (LPWSTR)(buf + argSize);
        memcpy(cmd, cmdline, cmdSize);

        n = WinParseCommandLine(cmd, argv);
    }
  #endif

    /* make sure we have at least argv[0] */

    if (!argv || n == 0) {
        if (argv)
            LocalFree(argv);

        argv = (LPWSTR*)LocalAlloc(LMEM_FIXED, sizeof(LPWSTR));
        if (!argv) {
            LogDebugError("%s failed (code 0x%08X).", "LocalAlloc", (uint32)GetLastError());
            return NULL;
        }

        argv[0] = NULL;
        n = 1;
    }

    /* replace argv[0] with executable name */

    if (BufGetModuleFileNameW(exeBuf, NULL))
        argv[0] = BufGetUtf16(exeBuf);

    *argc = n;
    return argv;
}

/********************************************************************************************************************/

NOINLINE
void WinErrorMessage(const char* message)
{
    LogError(message);

    if (g_isGuiProgram) {
        char tmp[1024];
        uint16* p;
        Buf buf;

        BufInit(&buf, tmp, sizeof(tmp));
        BufUtf8ToUtf16(&buf, message);
        p = BufGetUtf16(&buf);
        if (p)
            MessageBoxW(NULL, p, 0, MB_ICONSTOP | MB_OK);
        else
            MessageBoxA(NULL, message, 0, MB_ICONSTOP | MB_OK);
        BufFree(&buf);
    }
}
