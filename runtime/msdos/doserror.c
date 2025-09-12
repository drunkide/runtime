#include <runtime/platform.h>
#include <stdio.h>

void PlatformErrorMessage(const char* message)
{
    fprintf(stderr, "ERROR: %s\n", message);
}
