#include <runtime/platform.h>
#include <runtime/buffer.h>
#include <emscripten/html5.h>

void PlatformLogDebug(Buf* buf)
{
    const char* message = BufGetCStr(buf);
    EM_ASM({
        console.log(UTF8ToString($0));
    }, message);
}

void PlatformLogWarn(Buf* buf)
{
    const char* message = BufGetCStr(buf);
    EM_ASM({
        console.log(UTF8ToString($0));
    }, message);
}

void PlatformLogError(Buf* buf)
{
    const char* message = BufGetCStr(buf);
    EM_ASM({
        console.log(UTF8ToString($0));
    }, message);
}
