#include <runtime/main.h>
#include <runtime/version.h>
#include <stdlib.h>
#include <emscripten/html5.h>

static bool WebFrame(double time, void* userData)
{
    /* FIXME */
    DONT_WARN_UNUSED(time);
    DONT_WARN_UNUSED(userData);

    return false;
}

int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv)
{
    DONT_WARN_UNUSED(version); /* doesn't matter for static linkage */

    if (appMain(argc, argv) != 0) {
        EM_ASM(document.body.innerHTML = "<h1>Application initialization failed.</h1>";);
        return 1;
    }

    emscripten_request_animation_frame_loop(WebFrame, NULL);
    return 0;
}
