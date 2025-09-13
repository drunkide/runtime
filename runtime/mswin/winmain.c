#include <runtime/mswin/winapi.h>
#include <runtime/mswin/winlog.h>
#include <runtime/mswin/winutil.h>
#include <runtime/main.h>
#include <runtime/version.h>
#include <runtime/mem.h>
#include <runtime/buffer.h>
#include <runtime/string.h>
#include <runtime/log.h>
#include <string.h>

#ifdef RUNTIME_PLATFORM_MSWIN_WIN64
#include <shellapi.h>
#endif

static char* const g_dummy_argv[] = { (char*)"(null)", NULL };
static int g_argc;
static char** g_argv;

/********************************************************************************************************************/

NOINLINE
static void WinGetCommandLine(void)
{
    const char* getter;
    Buf exeBuf = UNINITIALIZED_BUFFER;
    WCHAR exe[MAX_PATH];
    LPWSTR lpCommandLine;
    LPWSTR* wargv;
    int wargc;

    /* get command line */

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    lpCommandLine = GetCommandLineW();
  #else
    Buf tmpBuf = UNINITIALIZED_BUFFER;
    char tmp[1024]; /* for GetCommandLineA */

    EXTPROC(Kernel32, GetCommandLineW);
    if (!pfnGetCommandLineW)
        lpCommandLine = NULL;
    else {
        lpCommandLine = pfnGetCommandLineW();
        getter = "GetCommandLineW";
    }

    if (!lpCommandLine) {
        const char* str;
        str = GetCommandLineA();
        BufInit(&tmpBuf, tmp, sizeof(tmp));
        BufMultiByteToWideChar(&tmpBuf, str);
        lpCommandLine = (WCHAR*)BufGetUtf16(&tmpBuf);
        getter = "GetCommandLineA";
    }
  #endif

    /* parse command line */

    if (lpCommandLine)
        LogDebug("Successfully retrieved command line using %s.", getter);
    else {
        LogError("Unable to retrieve command line.");
        lpCommandLine = L"";
    }

    BufInit(&exeBuf, exe, sizeof(exe));
    wargv = WinCommandLineToArgv(&exeBuf, lpCommandLine, &wargc);

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    BufFree(&tmpBuf); /* we don't need original command line anymore */
  #endif

    /* convert UTF-16 to UTF-8 */

    g_argv = NULL;
    if (wargv) {
        size_t arrSize = (size_t)(wargc + 1) * sizeof(char*);
        size_t argSize = 0;
        int i;

        for (i = 0; i < wargc; i++) {
            if (wargv[i])
                argSize += Utf16ToUtf8Bytes(wargv[0]) + 1;
            else
                argSize += strlen(g_dummy_argv[0]) + 1;
        }

        g_argv = (char**)MemAlloc(arrSize + argSize);
        if (!g_argv)
            LogError("Unable to allocate memory for command line arguments.");
        else {
            char* dst = (char*)g_argv + arrSize;
            for (i = 0; i < wargc; i++) {
                g_argv[i] = dst;
                if (wargv[i]) {
                    dst += Utf16ToUtf8(dst, wargv[0]);
                    *dst++ = 0;
                } else {
                    size_t length = strlen(g_dummy_argv[0]) + 1;
                    memcpy(dst, g_dummy_argv[0], length);
                    dst += length;
                }
            }
            g_argc = wargc;
            g_argv[wargc] = NULL;
        }

        LocalFree(wargv);
    }

    /* free memory */

    BufFree(&exeBuf);

    /* handle errors */

    if (!g_argv) {
        LogError("Unable to parse command line.");
        g_argc = 1;
        g_argv = (char**)g_dummy_argv;
    } else {
        int i;
        LogDebug("Command line parsing complete (argc = %d).", g_argc);
        for (i = 0; i < g_argc; i++) {
            LogDebug("    argv[%d] = \"%s\"\n", i, g_argv[i]);
        }
    }
}

NOINLINE
static void WinRun(RuntimeVersion version, PFN_AppMain appMain)
{
    int r = 1;

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    #if defined(_MSC_VER) && _MSC_VER > 1200
     #pragma warning(push)
     #pragma warning(disable:4996)
    #endif
    OSVERSIONINFO osvi = { sizeof(osvi), 0, 0, 0, 0 };
    GetVersionEx(&osvi);
    g_isWin32s = (osvi.dwPlatformId == VER_PLATFORM_WIN32s);
    g_isWinNT = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
    #if defined(_MSC_VER) && _MSC_VER > 1200
     #pragma warning(pop)
    #endif
  #endif

    g_hProcessHeap = GetProcessHeap();
    WinInitLogger();

  #ifndef RUNTIME_PLATFORM_MSWIN_WIN64
    LogDebug("Running on %s", (g_isWin32s ? "Win32s" : (g_isWinNT ? "Windows NT" : "Windows 9.x/ME")));
  #endif

    if (version > RUNTIME_VERSION_CURRENT) {
        char tmp[256];
        strcpy(tmp, "Application");
        strcat(tmp, " requires newer version of the runtime library.");
        WinErrorMessage(tmp);
        goto exit;
    }

    WinGetCommandLine();

    r = appMain(g_argc, g_argv);

  exit:
    if (g_argv && g_argv != g_dummy_argv)
        MemFree(g_argv);

    WinTerminateLogger();
    ExitProcess((UINT)r);
}

/********************************************************************************************************************/

#if defined(_MSC_VER) && _MSC_VER >= 1200
 #pragma warning(push)
 #pragma warning(disable:4702) /* unreachable code */
#endif

NOINLINE
int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv)
{
    g_isGuiProgram = false;
    g_hInstance = GetModuleHandle(NULL);
    g_argc = argc;
    g_argv = argv;

    WinRun(version, appMain);
    return 0;
}

NOINLINE
int RuntimeWinMain(RuntimeVersion version, PFN_AppMain appMain,
    void* hInstance, void* hPrevInstance, const char* cmdLine, int nShowCmd)
{
    g_isGuiProgram = true;
    g_hInstance = hInstance;

    DONT_WARN_UNUSED(hPrevInstance);
    DONT_WARN_UNUSED(cmdLine);
    DONT_WARN_UNUSED(nShowCmd);

    WinRun(version, appMain);
    return 0;
}

#if defined(_MSC_VER) && _MSC_VER >= 1200
 #pragma warning(pop)
#endif

/********************************************************************************************************************/

NOINLINE
int RuntimeDllMain(RuntimeVersion version, void* hinstDll, uint32 fdwReason, void* lpvReserved)
{
    DONT_WARN_UNUSED(lpvReserved);

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            WinDisableThreadLibraryCalls(hinstDll);
            if (version > RUNTIME_VERSION_CURRENT) {
                char tmp1[1024], tmp2[1024];
                Buf msgBuf, dllBuf;
                WCHAR* p, *slash;
                const char* msg;

                BufInit(&msgBuf, tmp1, sizeof(tmp1));
                BufAppendCStr(&msgBuf, "DLL");

                BufInit(&dllBuf, tmp2, sizeof(tmp2));
                if (BufGetModuleFileNameW(&dllBuf, hinstDll)) {
                    p = (WCHAR*)BufGetUtf16(&dllBuf);
                    if (p) {
                        for (slash = p; *p; ++p) {
                            if (*p == '\\' || *p == '/')
                                slash = p + 1;
                        }

                        BufAppendCStr(&msgBuf, " \"");
                        BufWideCharToMultiByte(&msgBuf, CP_ACP, slash);
                        BufAppendChar(&msgBuf, '"');
                    }
                }
                BufFree(&dllBuf);

                BufAppendCStr(&msgBuf, " requires newer version of the runtime library.");
                msg = BufGetCStr(&msgBuf);

                if (!msg) {
                    strcpy(tmp1, "DLL");
                    strcat(tmp1, " requires newer version of the runtime library.");
                    msg = tmp1;
                }

                g_isGuiProgram = 1; /* this is not yet known at this moment, so force MessageBox */
                WinInitLogger(); /* ensure logger is initialized, because it is called by WinErrorMessage */
                WinErrorMessage(msg);
                WinTerminateLogger();

                BufFree(&msgBuf);
                return FALSE;
            }
            break;
    }

    return TRUE;
}

#ifdef RUNTIME_EXPORTS /* if we are compiled as DLL */
DefaultDllMain()
#endif
