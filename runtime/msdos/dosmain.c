#include <runtime/main.h>
#include <runtime/version.h>
#include <stdio.h>

int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv)
{
    DONT_WARN_UNUSED(version); /* doesn't matter for static linkage */

    if (appMain(argc, argv) != 0) {
        fprintf(stderr, "Application initialization failed.\n");
        return 1;
    }

    return 0;
}
