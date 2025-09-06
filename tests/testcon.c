#include <runtime/main.h>
#include <runtime/error.h>

int AppMain(int argc, char** argv)
{
    DONT_WARN_UNUSED(argc);
    DONT_WARN_UNUSED(argv);
    RuntimeErrorMessage("Hello, world!");
    return 0;
}
