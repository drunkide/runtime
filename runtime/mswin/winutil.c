#include <runtime/mswin/winutil.h>
#include <runtime/mswin/winapi.h>
#include <runtime/log.h>
#include <runtime/buffer.h>

/********************************************************************************************************************/

NOINLINE
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
        return true;
    }

    return false;
}

NOINLINE
bool BufWideCharToMultiByte(Buf* buf, unsigned codepage, const wchar_t* src)
{
    size_t srcLen;
    char* dst;
    int dstLen;

    if (!src)
        return true;

    srcLen = (size_t)lstrlenW(src);
    if (srcLen == 0)
        return true;

    dstLen = WideCharToMultiByte(codepage, 0, src, (int)srcLen, NULL, 0, NULL, NULL);
    if (dstLen != 0) {
        dst = (char*)BufReserve(buf, (size_t)dstLen);
        if (dst) {
            dstLen = WideCharToMultiByte(codepage, 0, src, (int)srcLen, dst, dstLen, NULL, NULL);
            if (dstLen != 0) {
                BufCommit(buf, (size_t)dstLen);
                return true;
            }
        }
    }

    LogDebugError("%s failed (code 0x%08X).", "WideCharToMultiByte", (uint32)GetLastError());

    dst = BufReserve(buf, srcLen);
    if (dst) {
        while (*src) {
            *dst++ = (char)(*src > 0 && *src <= 0xff ? (uint8)*src : '?');
            ++src;
        }
        BufCommit(buf, srcLen);
        return true;
    }

    return false;
}

/********************************************************************************************************************/

NOINLINE
bool WinGetModuleFileNameW(Buf* buf, void* hModule)
{
    DWORD bufSize, retSize;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_isWinNT)
        return false;
  #endif

    bufSize = (DWORD)BufGetCapacityUtf16(buf);
    do {
        WCHAR* dst = (WCHAR*)BufReserveUtf16(buf, bufSize);
        if (!dst)
            return false;

        SetLastError(0);

        retSize = GetModuleFileNameW(hModule, dst, bufSize);
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

NOINLINE
bool WinGetModuleFileNameA(Buf* buf, void* hModule)
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

NOINLINE
bool BufGetModuleFileNameW(Buf* buf, void* hModule)
{
  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (g_isWinNT)
  #endif
    {
        if (WinGetModuleFileNameW(buf, hModule))
            return true;
    }

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    {
        const char* ansistr;
        char ansi[MAX_PATH];
        Buf ansibuf;

        BufInit(&ansibuf, ansi, sizeof(ansi));
        if (WinGetModuleFileNameA(&ansibuf, hModule) && (ansistr = BufGetCStr(&ansibuf)) != NULL) {
            BufClear(buf);
            if (BufMultiByteToWideChar(buf, ansistr)) {
                BufFree(&ansibuf);
                return true;
            }
        }
        BufFree(&ansibuf);
    }
  #endif

    return false;
}

/********************************************************************************************************************/

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64

NOINLINE
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

NOINLINE
wchar_t** WinCommandLineToArgv(Buf* exeBuf, const wchar_t* cmdline, int* argc)
{
    LPWSTR* argv;
    int n = 0;

    /* try to use CommandLineToArgvW */

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_isWinNT)
        argv = NULL;
    else
  #endif
    {
        EXTPROC(Shell32, CommandLineToArgvW);
        if (!pfnCommandLineToArgvW)
            argv = NULL;
        else {
            argv = pfnCommandLineToArgvW(cmdline, &n);
            if (!argv)
                LogDebugError("%s failed (code 0x%08X).", "CommandLineToArgvW", (uint32)GetLastError());
            else
                LogDebug("Using CommandLineToArgvW.");
        }
    }

    /* if that failed, use our own implementation */

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!argv) {
        int count = WinParseCommandLine((LPWSTR)cmdline, NULL);

        LPWSTR cmd;
        size_t cmdSize = sizeof(WCHAR) * (size_t)(lstrlenW(cmdline) + 1);
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
        LogDebug("Using internal command line parsing algorithm.");
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
        if (p) {
         #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
            if (g_isWin32s) {
                /* Win32s is the only one that doesn't have working MessageBoxW */
                const char* pp;
                char tmp2[1024];
                Buf buf2;
                BufInit(&buf2, tmp2, sizeof(tmp2));
                if (BufWideCharToMultiByte(&buf2, CP_ACP, p) && (pp = BufGetCStr(&buf2)) != NULL) {
                    MessageBoxA(NULL, pp, NULL, MB_ICONSTOP | MB_OK);
                    BufFree(&buf2);
                    goto done;
                }
                BufFree(&buf2);
            } else
         #endif
            {
                MessageBoxW(NULL, p, NULL, MB_ICONSTOP | MB_OK);
                goto done;
            }
        }

        /* conversion failed, try to output at least something */
        MessageBoxA(NULL, message, NULL, MB_ICONSTOP | MB_OK);

      done:
        BufFree(&buf);
    }
}

/********************************************************************************************************************/

NOINLINE
void WinDisableThreadLibraryCalls(void* hinstDll)
{
  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!DisableThreadLibraryCalls((HINSTANCE)hinstDll))
        LogDebug("%s failed (code 0x%08lx).", "DisableThreadLibraryCalls", (unsigned long)GetLastError());
  #else
    static bool initialized = false;
    static EXTPROCDECL(DisableThreadLibraryCalls);
    if (!initialized) {
        initialized = true;
        EXTPROCLOAD(Kernel32, DisableThreadLibraryCalls);
    }
    if (pfnDisableThreadLibraryCalls) {
        if (!pfnDisableThreadLibraryCalls((HINSTANCE)hinstDll))
            LogDebug("%s failed (code 0x%08lx).", "DisableThreadLibraryCalls", (unsigned long)GetLastError());
    }
  #endif
}

/********************************************************************************************************************/

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
NOINLINE
void WinDetectSystemVersion(void)
{
  #if defined(_MSC_VER) && _MSC_VER > 1200
   #pragma warning(push)
   #pragma warning(disable:4996) /* GetVersion is deprecated */
  #endif

    DWORD dwVersion = GetVersion();
    int majorVersion = LOBYTE(LOWORD(dwVersion));
    g_isWinNT = (dwVersion & 0x80000000) == 0;
    g_isWin32s = majorVersion < 4 && !g_isWinNT;

  #if defined(_MSC_VER) && _MSC_VER > 1200
   #pragma warning(pop)
  #endif

    if (g_isWin32s)
        g_isGuiProgram = true; /* console doesn't work properly on Win32s */
}
#endif
