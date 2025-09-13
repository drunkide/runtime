#ifndef RUNTIME_MSWIN_WINUTIL_H
#define RUNTIME_MSWIN_WINUTIL_H

#include <runtime/buffer.h>

EXTERN_C_BEGIN

#ifndef RUNTIME_PLATFORM_MSWIN_WIN64
RUNTIME_API bool BufMultiByteToWideChar(Buf* buf, const char* src);
#endif

RUNTIME_API bool BufGetModuleFileNameW(Buf* buf, void* hModule);

RUNTIME_API wchar_t** WinCommandLineToArgv(Buf* exeBuf, const wchar_t* cmdline, int* argc);

EXTERN_C_END

#endif
