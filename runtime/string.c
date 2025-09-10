#include <runtime/string.h>
#include <lib/stb/sprintf.h>

/********************************************************************************************************************/

void StringFormat(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    stbsp_vsnprintf(buf, (int)size, fmt, args);
    va_end(args);
}

void StringFormatV(char* buf, size_t size, const char* fmt, va_list args)
{
    stbsp_vsnprintf(buf, (int)size, fmt, args);
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
