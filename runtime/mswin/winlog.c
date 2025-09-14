#include <runtime/platform.h>
#include <runtime/log.h>
#include <runtime/buffer.h>
#include <runtime/mem.h>
#include <runtime/string.h>
#include <runtime/mswin/windisp.h>
#include <runtime/mswin/winutil.h>
#include <runtime/mswin/winapi.h>

#define ENABLE_LOG_WINDOW 1

static const char failmsg[] = "(logger string allocation failed)\r\n";

/********************************************************************************************************************/
/* Console */

static HANDLE g_hStdOut;
static BOOL g_isRealConsole;
static BOOL g_initialized;

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
static UINT g_consoleOutputCP;
#endif

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
#if ENABLE_LOG_WINDOW
/********************************************************************************************************************/

static ATOM g_loggerWndClassAtom;
static HWND g_hLogWindow;
static HWND g_hLogEdit;
static HFONT g_hLogFont;

/*****************************/
/* Logger view text printing */

static DWORD g_loggerSelStart;
static DWORD g_loggerSelEnd;
static DWORD g_loggerTextLength;

static void WinSaveLoggerPosition(void)
{
    SendMessage(g_hLogEdit, EM_GETSEL, (WPARAM)&g_loggerSelStart, (LPARAM)&g_loggerSelEnd);
    g_loggerTextLength = (DWORD)GetWindowTextLength(g_hLogEdit);
}

static void WinRestoreLoggerPosition(void)
{
    SendMessage(g_hLogEdit, EM_SETSEL, (WPARAM)g_loggerSelStart, (LPARAM)g_loggerSelEnd);
    SendMessage(g_hLogEdit, EM_SCROLLCARET, 0, 0);
}

static void WinScrollLoggerToBottom(void)
{
    g_loggerTextLength = (DWORD)GetWindowTextLength(g_hLogEdit);
    SendMessage(g_hLogEdit, EM_SETSEL, (WPARAM)g_loggerTextLength, (LPARAM)g_loggerTextLength);
    SendMessage(g_hLogEdit, EM_SCROLL, SB_BOTTOM, 0);
    SendMessage(g_hLogEdit, EM_SCROLLCARET, 0, 0);
}

static bool WinLoggerWasAtBottom(void)
{
    return (g_loggerSelStart == g_loggerTextLength
         && g_loggerSelEnd == g_loggerTextLength);
}

static void WinLoggerRestoreOrScroll(void)
{
    if (WinLoggerWasAtBottom())
        WinScrollLoggerToBottom();
    else
        WinRestoreLoggerPosition();
}

static void WinAppendLoggerMessage(const uint16* utf16)
{
    WinSaveLoggerPosition();

    SendMessage(g_hLogEdit, EM_SETSEL, (WPARAM)g_loggerTextLength, (LPARAM)g_loggerTextLength);

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_isWinNT) {
        const char* p;
        char tmp[512];
        Buf buf;
        BufInit(&buf, tmp, sizeof(tmp));
        if (BufWideCharToMultiByte(&buf, CP_ACP, utf16) && (p = BufGetCStr(&buf)) != NULL)
            SendMessageA(g_hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)p);
        else
            SendMessageA(g_hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)failmsg);
        BufFree(&buf);
    } else
  #endif
        SendMessageW(g_hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)utf16);

    WinLoggerRestoreOrScroll();
}

/*****************************/
/* Queue of pending messages */

STRUCT(PendingMessage) {
    PendingMessage* next;
    uint16 buffer[1];
};

static PendingMessage* g_firstPending;
static PendingMessage* g_lastPending;

static void WinEnqueueLoggerMessage(const uint16* str, size_t len)
{
    size_t size = (len + 1) * sizeof(WCHAR);
    PendingMessage* msg = (PendingMessage*)MemAlloc(offsetof(PendingMessage, buffer) + size);
    if (!msg)
        return;

    msg->next = NULL;
    memcpy(msg->buffer, str, size);

    if (!g_firstPending)
        g_firstPending = msg;
    else
        g_lastPending->next = msg;
    g_lastPending = msg;
}

static void WinFlushPendingMessages(void)
{
    PendingMessage* p, *next;
    for (p = g_firstPending; p; p = next) {
        next = p->next;
        if (g_hLogWindow)
            WinAppendLoggerMessage(p->buffer);
        MemFree(p);
    }

    g_firstPending = NULL;
    g_lastPending = NULL;
}

/***************************/
/* Logger window procedure */

static bool WinLoggerCreateEdit(HWND hParentWnd)
{
    g_hLogEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
        WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        0, 0, 0, 0, hParentWnd, (HMENU)1, g_hInstance, NULL);
    return g_hLogEdit != NULL;
}

static void WinLoggerUpdateFont(int dpiY)
{
    LOGFONT lf;
    HFONT hOldFont = g_hLogFont;
    HFONT hNewFont;

    hNewFont = GetStockObject(SYSTEM_FIXED_FONT);
    if (!GetObject(hNewFont, sizeof(lf), &lf))
        LogDebugError("WinLoggerUpdateFont: GetObject() failed for SYSTEM_FIXED_FONT.");
    else {
        lf.lfWidth = 0;
        lf.lfHeight = -MulDiv(7, dpiY, 72);
        lstrcpy(lf.lfFaceName, TEXT("Courier"));
        hNewFont = CreateFontIndirect(&lf);
        if (!hNewFont)
            LogDebugError("WinLoggerUpdateFont: CreateFontIndirect() failed.");
        else {
            g_hLogFont = hNewFont;
            SendMessage(g_hLogEdit, WM_SETFONT, (WPARAM)hNewFont, TRUE);
            if (hOldFont)
                DeleteObject(hOldFont);
        }
    }
}

static void WinLoggerResize(int width, int height)
{
    WinSaveLoggerPosition();
    MoveWindow(g_hLogEdit, 0, 0, width, height, TRUE);
    WinLoggerRestoreOrScroll();
}

static LRESULT CALLBACK WinLoggerWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int dpiY = 0;

    switch (msg) {
        case WM_CREATE:
            if (!WinLoggerCreateEdit(hWnd))
                return FALSE;
            WinGetDpiForWindow(hWnd, "WinCreateLogWindow", NULL, &dpiY);
            WinLoggerUpdateFont(dpiY);
            break;

        case WM_DESTROY:
        case WM_NCDESTROY:
            g_hLogWindow = NULL;
            break;

        case WM_CLOSE:
            DestroyWindow(g_hLogWindow);
            return TRUE;

        case WM_SIZE:
            WinLoggerResize(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_DPICHANGED:
            WinLoggerUpdateFont((int)HIWORD(wParam));
            break;
    }

  /*FIXME defwnd:*/
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/******************/
/* Initialization */

NOINLINE
static bool WinCreateLogWindow(HWND hWndRef)
{
    int x, y, width, height;
    const TCHAR* p = NULL;
    TCHAR className[64];
    char tmp[512];
    Buf title;
    WNDCLASS wc;
    RECT rc;

    if (g_hLogWindow)
        return true;

    wsprintf(className, TEXT("df_%08lx"), (unsigned long)GetTickCount());

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WinLoggerWndProc;
    wc.hInstance = g_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className;

    g_loggerWndClassAtom = RegisterClass(&wc);
    if (!g_loggerWndClassAtom) {
        StrFormat(tmp, sizeof(tmp),
            "Unable to register log window class (code 0x%08lX).", (unsigned long)GetLastError());
        WinErrorMessage(tmp);
        return false;
    }

    height = 215;
    if (hWndRef) {
        GetWindowRect(hWndRef, &rc);
        x = rc.left + 20;
        y = rc.bottom - 145;
        width = (rc.right - rc.left) - 40;
    } else {
        x = 10;
        y = 10;
        width = 600;
    }

    BufInit(&title, tmp, sizeof(tmp));
  #if defined(UNICODE) || defined(_UNICODE)
    if (WinGetModuleFileNameW(&title, NULL) && (p = BufGetUtf16(&title)) != NULL) {
        const WCHAR* slash = p;
        while (*p) {
            if (*p == '\\' || *p == '/')
                slash = p + 1;
            ++p;
        }
        p = slash;
    }
  #else
    if (WinGetModuleFileNameA(&title, NULL) && (p = BufGetCStr(&title)) != NULL) {
        const char* slash = p;
        while (*p) {
            if (*p == '\\' || *p == '/')
                slash = p + 1;
            ++p;
        }
        p = slash;
    }
  #endif
    if (!p)
        p = TEXT("Debug Log");

    g_hLogWindow = CreateWindowEx(0,
        MAKEINTATOM(g_loggerWndClassAtom), p,
        WS_POPUPWINDOW | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        x, y, width, height, NULL, NULL, g_hInstance, NULL);

    if (!g_hLogWindow) {
        StrFormat(tmp, sizeof(tmp),
            "Unable to create log window (code 0x%08lX).", (unsigned long)GetLastError());
        BufFree(&title);
        WinErrorMessage(tmp);
        return false;
    }

    BufFree(&title);
    WinFlushPendingMessages();

    ShowWindow(g_hLogWindow, SW_SHOWNORMAL);
    UpdateWindow(g_hLogWindow);

    WinScrollLoggerToBottom();

    return true;
}

/********************************************************************************************************************/
#endif /* ENABLE_LOG_WINDOW */
/********************************************************************************************************************/

static void WinLogMessage(const char* prefix, Buf* buf, int color)
{
    Buf utf16;
    WCHAR tmp16[512];
    WCHAR* wstr;
    const char* msg;
    size_t msgLen, wlen;

    msg = BufGetCStrN(buf, &msgLen);
    if (!msg) {
        msg = failmsg;
        msgLen = sizeof(failmsg) - 3; /* without \r\n */
    }

    BufInit(&utf16, tmp16, sizeof(tmp16));

    BufUtf8ToUtf16(&utf16, prefix);
    BufUtf8ToUtf16N(&utf16, msg, msgLen);
    BufAppendUtf16Str(&utf16, L"\r\n");

    wstr = BufGetUtf16N(&utf16, &wlen);
    if (!wstr) {
        msg = failmsg;
        wstr = tmp16;
        while (*msg)
            *wstr++ = (WCHAR)(uint8)*msg++;
        *wstr++ = 0;
        wlen = (size_t)(wstr - tmp16);
        wstr = tmp16;
    }

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    if (!g_isWin32s)
  #endif
        OutputDebugStringW(wstr);

    if (!g_isGuiProgram || !g_isRealConsole) {
        if (WinBeginPrint()) {
            WinPrint(color, wstr, wlen);
            WinEndPrint();
        }
    }

  #if ENABLE_LOG_WINDOW
    if (g_isGuiProgram) {
        if (g_hLogWindow)
            WinAppendLoggerMessage(wstr);
        else
            WinEnqueueLoggerMessage(wstr, wlen);
    }
  #endif

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
    if (g_initialized)
        return;

    g_initialized = true;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    WinDetectSystemVersion();
  #endif

    g_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    g_isRealConsole = GetConsoleScreenBufferInfo(g_hStdOut, &g_csbi);

    InitializeCriticalSection(&g_criticalSection);

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    LogDebug("Logger initialized (method: %s)", (g_isRealConsole ? "WriteConsoleW" : "WriteFile"));
  #else
    if (!g_isRealConsole)
        LogDebug("Logger initialized (method: %s)", (g_hStdOut != INVALID_HANDLE_VALUE ? "WriteFile" : "Log Window"));
    else if (g_isWinNT)
        LogDebug("Logger initialized (method: %s)", "WriteConsoleW");
    else {
        g_consoleOutputCP = GetConsoleOutputCP();
        LogDebug("Logger initialized (method: %s, codepage: %u)", "WriteConsoleA", (unsigned)g_consoleOutputCP);
    }
    LogDebug("Running on %s", (g_isWin32s ? "Win32s" : (g_isWinNT ? "Windows NT" : "Windows 9.x/ME")));
  #endif
}

void WinTerminateLogger(void)
{
  #if ENABLE_LOG_WINDOW
    if (g_hLogWindow) {
        DestroyWindow(g_hLogWindow);
        g_hLogWindow = NULL;
    }

    if (g_loggerWndClassAtom) {
      #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
        /* good thing to do on Win32s and maybe Win9x, not really needed on WinNT and causes crash on NT 3.1 */
        if (!g_isWinNT)
            UnregisterClass(MAKEINTATOM(g_loggerWndClassAtom), g_hInstance);
      #endif
        g_loggerWndClassAtom = 0;
    }

    WinFlushPendingMessages();
  #endif

    DeleteCriticalSection(&g_criticalSection);
}

bool WinShowLogWindow(void* hWndRef)
{
    DONT_WARN_UNUSED(hWndRef);

  #if ENABLE_LOG_WINDOW
    if (g_isGuiProgram) {
        if (!WinCreateLogWindow((HWND)hWndRef))
            return false;
    }
  #endif

    return true;
}

void WinWaitLogWindowClosed(void)
{
  #if ENABLE_LOG_WINDOW
    MSG msg;

    if (!g_hLogWindow)
        return;

    WinAppendLoggerMessage(L"\r\n\r\n=== PROGRAM EXITED ===");

    while (g_hLogWindow && GetMessage(&msg, g_hLogWindow, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
  #endif
}
