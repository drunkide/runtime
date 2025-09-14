#ifndef RUNTIME_MSWIN_WINLOG_H
#define RUNTIME_MSWIN_WINLOG_H

#include <runtime/log.h>

EXTERN_C_BEGIN

void WinInitLogger(void);
void WinTerminateLogger(void);

bool WinShowLogWindow(void* hWndRef);
void WinWaitLogWindowClosed(void);

EXTERN_C_END

#endif
