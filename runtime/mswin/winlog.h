#ifndef RUNTIME_MSWIN_WINLOG_H
#define RUNTIME_MSWIN_WINLOG_H

#include <runtime/log.h>

EXTERN_C_BEGIN

void WinPreinitLogger(void);
void WinInitLogger(void);
void WinTerminateLogger(void);

bool WinMaybeShowLogWindow(void);
RUNTIME_API bool WinShowLogWindow(void* hWndRef);
void WinWaitLogWindowClosed(void);

EXTERN_C_END

#endif
