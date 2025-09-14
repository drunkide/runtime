#include <runtime/mswin/windisp.h>
#include <runtime/mswin/winapi.h>
#include <runtime/log.h>

void WinGetDpiForWindow(void* hWnd, const char* debugName, int* outDpiX, int* outDpiY)
{
    const char* method = NULL;
    int dpiX = 0, dpiY = 0;
    HDC hDC;

  #ifdef RUNTIME_PLATFORM_MSWIN_WIN64
    #define pfnMonitorFromWindow MonitorFromWindow
    #define AND_pfnMonitorFromWindow
    EXTPROC(ShCore, GetDpiForMonitor);
  #else
    #define AND_pfnMonitorFromWindow && pfnMonitorFromWindow
    EXTPROC(User32, MonitorFromWindow);
    EXTPROCDECL(GetDpiForMonitor) = NULL;
    if (!g_isWin32s)
        EXTPROCLOAD(ShCore, GetDpiForMonitor);
  #endif

    if (pfnGetDpiForMonitor AND_pfnMonitorFromWindow) {
        UINT mdpiX = 0, mdpiY = 0;
        HANDLE hMonitor = pfnMonitorFromWindow(hWnd, 1 /*MONITOR_DEFAULTTOPRIMARY*/);

        if (!hMonitor) {
            DWORD dwError = GetLastError();
            LogDebugError("%s: MonitorFromWindow() failed (code 0x%08X).", debugName, (unsigned)dwError);
        } else {
            HRESULT hr = pfnGetDpiForMonitor(hMonitor, 0 /*MDT_EFFECTIVE_DPI*/, &mdpiX, &mdpiY);
            if (hr != 0 /*S_OK*/)
                LogDebugError("%s: GetDpiForMonitor() failed (code 0x%08X)", debugName, (unsigned)(size_t)hr);
            else {
                dpiX = (int)mdpiX;
                dpiY = (int)mdpiY;
                method = "via GetDpiForMonitor";
                if (dpiX == 0 || dpiY == 0)
                    LogDebug("%s: GetDpiForMonitor() returned dpiX=%d, dpiY=%d.", debugName, dpiX, dpiY);
            }
        }
    }

    if (dpiX == 0 || dpiY == 0) {
        method = "via GetDeviceCaps";

        hDC = GetDC(hWnd);
        dpiX = GetDeviceCaps(hDC, LOGPIXELSX);
        dpiY = GetDeviceCaps(hDC, LOGPIXELSY);
        ReleaseDC(hWnd, hDC);

        if (dpiX == 0 || dpiY == 0) {
            method = "hardcoded value";
            LogDebug("%s: GetDeviceCaps() returned dpiX=%d, dpiY=%d.", debugName, dpiX, dpiY);
            dpiX = 96;
            dpiY = 96;
        }
    }

    LogDebug("%s: dpiX=%d, dpiY=%d (%s).", debugName, dpiX, dpiY, method);

    if (outDpiX)
        *outDpiX = dpiX;
    if (outDpiY)
        *outDpiY = dpiY;
}
