#include <runtime/platform.h>
#include <runtime/log.h>
#include <runtime/buffer.h>
#include <runtime/mswin/winutil.h>
#include <runtime/mswin/winapi.h>

static HANDLE g_hStdOut;
static BOOL g_isRealConsole;

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
static UINT g_consoleOutputCP;
#endif

/********************************************************************************************************************/

static CRITICAL_SECTION g_criticalSection;
static CONSOLE_SCREEN_BUFFER_INFO g_csbi;

typedef BOOL (WINAPI* PFNWRITERPROC)(HANDLE, const void*, DWORD, LPDWORD, LPVOID);

static bool WinBeginPrint(void)
{
    if (g_hStdOut == INVALID_HANDLE_VALUE)
        return false;

    EnterCriticalSection(&g_criticalSection);
    return true;
}

static void WinEndPrint(void)
{
    if (g_isRealConsole)
        SetConsoleTextAttribute(g_hStdOut, g_csbi.wAttributes);

    LeaveCriticalSection(&g_criticalSection);
}

static void WinWrite(const char* str, size_t len, PFNWRITERPROC writer)
{
    DWORD dwBytesWritten;
    while (len != 0) {
        if (!writer(g_hStdOut, str, (DWORD)len, &dwBytesWritten, NULL))
            break;
        len -= dwBytesWritten;
        str += dwBytesWritten;
    }
}

static void WinPrintLatin1(char* dst, const char* dstEnd, const WCHAR* str, size_t len, PFNWRITERPROC writer)
{
    DWORD dwBytesWritten;
    char* p = dst;
    for (;;) {
        if (len == 0 || p == dstEnd) {
            if (!writer(g_hStdOut, dst, (DWORD)(dstEnd - dst), &dwBytesWritten, NULL))
                break;
            if (len == 0)
                break;
            p = dst;
        }
        *p++ = (char)(*str > 0 && *str <= 0xff ? (uint8)*str : '?');
        ++str;
    }
}

static void WinPrint(int wAttr, const WCHAR* str, size_t len)
{
    DWORD dwBytesWritten;
    const char* p;
    char tmp[512];
    Buf buf;

    if (g_isRealConsole) {
        SetConsoleTextAttribute(g_hStdOut, (WORD)wAttr);

      #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
        if (g_isWinNT)
      #endif
        {
            while (len != 0) {
                if (!WriteConsoleW(g_hStdOut, str, (DWORD)len, &dwBytesWritten, NULL))
                    break;
                len -= dwBytesWritten;
                str += dwBytesWritten;
            }
        }
      #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
        else {
            BufInit(&buf, tmp, sizeof(tmp));
            if (BufWideCharToMultiByte(&buf, g_consoleOutputCP, str) && (p = BufGetCStr(&buf)) != NULL)
                WinWrite(p, BufGetLength(&buf), (PFNWRITERPROC)WriteConsoleA);
            else {
                /* do some stupid conversion as a fallback */
                WinPrintLatin1(tmp, tmp + sizeof(tmp), str, len, (PFNWRITERPROC)WriteConsoleA);
            }
            BufFree(&buf);
        }
      #endif
    } else {
        BufInit(&buf, tmp, sizeof(tmp));
        if (BufUtf16ToUtf8N(&buf, str, len) && (p = BufGetCStr(&buf)) != NULL)
            WinWrite(p, BufGetLength(&buf), (PFNWRITERPROC)WriteFile);
        else {
            /* do some stupid conversion as a fallback */
            WinPrintLatin1(tmp, tmp + sizeof(tmp), str, len, (PFNWRITERPROC)WriteFile);
        }
        BufFree(&buf);
    }
}

/********************************************************************************************************************/

static const char failmsg[] = "(logger string allocation failed)\r\n";

static void WinLogMessage(const char* prefix, Buf* buf, int color)
{
    Buf utf16;
    WCHAR tmp16[512];
    WCHAR* wstr;
    const char* msg;
    size_t msgLen, wlen;

    msg = BufGetCStr(buf);
    if (msg)
        msgLen = BufGetLength(buf);
    else {
        msg = failmsg;
        msgLen = sizeof(failmsg) - 3; /* without \r\n */
    }

    BufInit(&utf16, tmp16, sizeof(tmp16));

    BufUtf8ToUtf16(&utf16, prefix);
    BufUtf8ToUtf16N(&utf16, msg, msgLen);
    BufAppendUtf16Str(&utf16, L"\r\n");

    wstr = BufGetUtf16(&utf16);
    if (wstr)
        wlen = BufGetLengthUtf16(&utf16);
    else {
        msg = failmsg;
        wstr = tmp16;
        while (*msg)
            *wstr++ = (WCHAR)(uint8)*msg++;
        *wstr++ = 0;
        wlen = (size_t)(wstr - tmp16);
        wstr = tmp16;
    }

    OutputDebugStringW(wstr);

    if (!g_isGuiProgram || !g_isRealConsole) {
        if (WinBeginPrint()) {
            WinPrint(color, wstr, wlen);
            WinEndPrint();
        }
    }

    BufFree(&utf16);
}

void PlatformLogDebug(Buf* buf)
{
    WinLogMessage("[DBG] ", buf, FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void PlatformLogWarn(Buf* buf)
{
    WinLogMessage("[WRN] ", buf, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

void PlatformLogError(Buf* buf)
{
    WinLogMessage("[ERR] ", buf, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

/********************************************************************************************************************/

void WinInitLogger(void)
{
    g_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    g_isRealConsole = GetConsoleScreenBufferInfo(g_hStdOut, &g_csbi);

    InitializeCriticalSection(&g_criticalSection);

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    LogDebug("Logger initialized (method: %s)", (g_isRealConsole ? "WriteConsoleW" : "WriteFile"));
  #else
    if (!g_isRealConsole)
        LogDebug("Logger initialized (method: %s)", "WriteFile");
    else if (g_isWinNT)
        LogDebug("Logger initialized (method: %s)", "WriteConsoleW");
    else {
        g_consoleOutputCP = GetConsoleOutputCP();
        LogDebug("Logger initialized (method: %s, codepage: %u)", "WriteConsoleA", (unsigned)g_consoleOutputCP);
    }
  #endif
}

void WinTerminateLogger(void)
{
    DeleteCriticalSection(&g_criticalSection);
}
