#include <runtime/string.h>
#include <lib/stb/sprintf.h>

/********************************************************************************************************************/

int StringFormat(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    int r;

    va_start(args, fmt);
    r = stbsp_vsnprintf(buf, (int)size, fmt, args);
    va_end(args);

    return r;
}

int StringFormatV(char* buf, size_t size, const char* fmt, va_list args)
{
    return stbsp_vsnprintf(buf, (int)size, fmt, args);
}

/********************************************************************************************************************/

#ifdef __GNUC__
 #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#ifdef _MSC_VER
  #pragma warning(disable:4701)
#endif

#define STB_SPRINTF_IMPLEMENTATION
#include <lib/stb/sprintf.h>
