#include <runtime/mswin/winapi.h>

BOOL g_isGuiProgram;
HINSTANCE g_hInstance;

#if !defined(RUNTIME_PLATFORM_MSWIN_WIN16)
HANDLE g_hKernel32;
HANDLE g_hShell32;
HANDLE g_hProcessHeap;
#endif

NOINLINE
FARPROC WinGetProcAddress_(HANDLE* hDll, const TCHAR* dll, const char* proc)
{
    HANDLE handle = *hDll;
    if (!handle) {
        handle = GetModuleHandle(dll);
        if (!handle) {
            handle = LoadLibrary(dll);
            if (!handle)
                return NULL;
        }
        *hDll = handle;
    }

    return GetProcAddress(handle, proc);
}
