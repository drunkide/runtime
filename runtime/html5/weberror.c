#include <runtime/platform.h>
#include <emscripten/html5.h>

void PlatformErrorMessage(const char* message)
{
    EM_ASM({
        alert(UTF8ToString($0));
    }, message);
}
