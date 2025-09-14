#ifndef RUNTIME_MSWIN_WINCONF_H
#define RUNTIME_MSWIN_WINCONF_H

#include <runtime/common.h>

EXTERN_C_BEGIN

int WinGetPrivateProfileInt(const char* section, const char* option, int defValue);
bool WinGetPrivateProfileString(Buf* buf, const char* section, const char* option, const char* defValue);

bool WinOpenConfigFile(void);
void WinCloseConfigFile(void);

EXTERN_C_END

#endif
