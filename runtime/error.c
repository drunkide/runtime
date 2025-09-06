#include <runtime/error.h>
#include <runtime/platform.h>

void RuntimeErrorMessage(const char* message)
{
    PlatformErrorMessage(message);
}
