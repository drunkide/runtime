#include <runtime/main.h>
#include <runtime/version.h>
#include <stdio.h>

int RuntimeMain(RuntimeVersion version, PFN_AppMain appMain, int argc, char** argv)
{
    if (version > RUNTIME_VERSION_CURRENT) {
        fprintf(stderr, "Application requires newer version of the runtime library.\n");
        return 1;
    }

    if (appMain(argc, argv) != 0) {
        fprintf(stderr, "Application initialization failed.\n");
        return 1;
    }

    return 0;
}
