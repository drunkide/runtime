#ifndef RUNTIME_MSWIN_WINDISP_H
#define RUNTIME_MSWIN_WINDISP_H

#include <runtime/common.h>

EXTERN_C_BEGIN

void WinGetDpiForWindow(void* hWnd, const char* debugName, int* outDpiX, int* outDpiY);

EXTERN_C_END

#endif
