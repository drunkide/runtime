#include <runtime/mswin/winapi.h>
#include <runtime/main.h>
#include <runtime/version.h>
#include <string.h>

#ifdef RUNTIME_PLATFORM_MSWIN_WIN64
#include <shellapi.h>
#endif

static int g_argc;
static char** g_argv;

/********************************************************************************************************************/

static void WinParseCommandLine(void)
{
    LPWSTR* wargv;
    int wargc;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
  #else
    EXTPROC(Shell32, CommandLineToArgvW);
    if (!pfnCommandLineToArgvW)
        wargv = NULL;
    else {
        EXTPROC(Kernel32, GetCommandLineW);
        wargv = pfnCommandLineToArgvW(pfnGetCommandLineW(), &wargc);
    }
  #endif

    if (wargv) {
        /* FIXME */
        LocalFree(wargv);
        return;
    }

    /* FIXME */
}

static void WinRun(RuntimeVersion version, PFN_AppMain appMain)
{
    int r;

    g_hProcessHeap = GetProcessHeap();

    if (version > RUNTIME_VERSION_CURRENT) {
        PlatformErrorMessage("Application requires newer version of the runtime library.");
        ExitProcess(1);
    }

    WinParseCommandLine();

    r = appMain(g_argc, g_argv);
    if (r != 0)
        ExitProcess((UINT)r);

    ExitProcess(0);
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
