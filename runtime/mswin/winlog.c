#include <runtime/platform.h>
#include <runtime/buffer.h>
#include <runtime/mswin/winapi.h>

void PlatformLogDebug(Buf* buf)
{
    BufAppendChar(buf, '\n');
    OutputDebugStringA(BufGetCStr(buf)); /* FIXME */
}

void PlatformLogWarn(Buf* buf)
{
    BufAppendChar(buf, '\n');
    OutputDebugStringA(BufGetCStr(buf)); /* FIXME */
}

void PlatformLogError(Buf* buf)
{
    BufAppendChar(buf, '\n');
    OutputDebugStringA(BufGetCStr(buf)); /* FIXME */
}

void WinInitLogger(void)
{
}
