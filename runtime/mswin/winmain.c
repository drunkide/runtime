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
    else
        lpCommandLine = pfnGetCommandLineW();

    if (!lpCommandLine) {
        const char* str = GetCommandLineA();
        BufInit(&tmpBuf, tmp, sizeof(tmp));
        BufMultiByteToWideChar(&tmpBuf, str);
        lpCommandLine = (WCHAR*)BufGetUtf16(&tmpBuf);
    }
  #endif

    /* parse command line */

    if (!lpCommandLine) {
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
    }
}

NOINLINE
static void WinRun(RuntimeVersion version, PFN_AppMain appMain)
{
    int r = 1;

    g_hProcessHeap = GetProcessHeap();

    if (version > RUNTIME_VERSION_CURRENT) {
        PlatformErrorMessage("Application requires newer version of the runtime library.");
        goto exit;
    }

    WinInitLogger();
    WinGetCommandLine();

    r = appMain(g_argc, g_argv);

  exit:
    if (g_argv && g_argv != g_dummy_argv)
        MemFree(g_argv);

    ExitProcess((UINT)r);
}

/********************************************************************************************************************/

#if defined(_MSC_VER) && _MSC_VER >= 1200
 #pragma warning(push)
 #pragma warning(disable:4702) /* unreachable code */
#endif

int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv)
{
    g_isGuiProgram = false;
    g_hInstance = GetModuleHandle(NULL);
    g_argc = argc;
    g_argv = argv;

    WinRun(version, appMain);
    return 0;
}

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

#ifdef RUNTIME_EXPORTS /* if we are compiled as DLL */
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    DONT_WARN_UNUSED(lpvReserved);

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
          #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
            DisableThreadLibraryCalls(hinstDll);
          #else
            EXTPROC(Kernel32, DisableThreadLibraryCalls);
            if (pfnDisableThreadLibraryCalls)
                pfnDisableThreadLibraryCalls(hinstDll);
          #endif
            break;
        }
    }

    return TRUE;
}
#endif
