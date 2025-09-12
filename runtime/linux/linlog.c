#include <runtime/platform.h>
#include <runtime/buffer.h>
#include <stdio.h>

void PlatformLogDebug(Buf* buf)
{
    BufAppendChar(buf, '\n');
    fwrite(BufGetPtr(buf), 1, BufGetLength(buf), stdout);
}

void PlatformLogWarn(Buf* buf)
{
    BufAppendChar(buf, '\n');
    fwrite(BufGetPtr(buf), 1, BufGetLength(buf), stderr);
}

void PlatformLogError(Buf* buf)
{
    BufAppendChar(buf, '\n');
    fwrite(BufGetPtr(buf), 1, BufGetLength(buf), stderr);
}
