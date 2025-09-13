#ifndef RUNTIME_MSWIN_WINUTIL_H
#define RUNTIME_MSWIN_WINUTIL_H

#include <runtime/buffer.h>

EXTERN_C_BEGIN

RUNTIME_API bool BufMultiByteToWideChar(Buf* buf, const char* src);
RUNTIME_API bool BufWideCharToMultiByte(Buf* buf, const wchar_t* src);

bool BufGetModuleFileNameW(Buf* buf, void* hModule);

RUNTIME_API wchar_t** WinCommandLineToArgv(Buf* exeBuf, const wchar_t* cmdline, int* argc);

RUNTIME_API void WinErrorMessage(const char* message);

RUNTIME_API void WinDisableThreadLibraryCalls(void* hinstDll);

EXTERN_C_END

#endif
