#include <runtime/mswin/winapi.h>
#include <runtime/log.h>

BOOL g_isGuiProgram;
HINSTANCE g_hInstance;

#if !defined(RUNTIME_PLATFORM_MSWIN_WIN64)
BOOL g_isWin32s;
BOOL g_isWinNT;
#endif

#if !defined(RUNTIME_PLATFORM_MSWIN_WIN16)
HANDLE g_hKernel32;
HANDLE g_hUser32;
HANDLE g_hShell32;
HANDLE g_hShCore;
HANDLE g_hProcessHeap;
HINSTANCE g_hRuntimeDll;
#endif

NOINLINE
FARPROC WinGetProcAddress_(HANDLE* hDll, const TCHAR* dll, const char* proc)
{
    HANDLE handle = *hDll;
    FARPROC ret;

    if (!handle) {
        handle = GetModuleHandle(dll);
        if (!handle) {
            handle = LoadLibrary(dll);
            if (!handle)
                return NULL;
        }
        *hDll = handle;
    }

    ret = GetProcAddress(handle, proc);
    if (ret)
        LogDebug("Successfully resolved entry point %s.", proc);
    else
        LogDebug("Entry point %s does not exist.", proc);

    return ret;
}
