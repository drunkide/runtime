#include <runtime/testing/utility.h>

#ifdef _WIN32
#include <runtime/mswin/winapi.h>
#define sprintf wsprintfA
#define vsprintf wvsprintfA
#else
#include <stdio.h>
#endif

#include <string.h>
#include <stdarg.h>

static int g_appType;
static int g_tests;
static int g_errors;

static void outf(int color, const char* fmt, ...)
{
    static char buf[16384];
    va_list args;
    size_t len;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    len = strlen(buf);

  #ifndef _WIN32
    DONT_WARN_UNUSED(color);
    fwrite(buf, 1, len, stdout);
  #else
    if (g_appType == APP_CONSOLE) {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dwBytesWritten;
        WORD wAttr;

        GetConsoleScreenBufferInfo(hStdOut, &csbi);

        switch (color) {
            case COLOR_DEFAULT: default: wAttr = csbi.wAttributes; break;
            case COLOR_DARK_GREEN: wAttr = FOREGROUND_GREEN; break;
            case COLOR_LIGHT_GREEN: wAttr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case COLOR_DARK_RED: wAttr = FOREGROUND_RED; break;
            case COLOR_LIGHT_RED: wAttr = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
            case COLOR_WHITE: wAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
            case COLOR_WHITE_ON_RED: wAttr = BACKGROUND_RED | BACKGROUND_INTENSITY |
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        }

        SetConsoleTextAttribute(hStdOut, wAttr);
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)len, &dwBytesWritten, NULL);
        SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
    }
    OutputDebugStringA(buf);
  #endif
}

void ASSERT_(const char* file, int line, bool condition,
    const char* expectedFmt, const char* expected,
    const char* actualFmt, const char* actual)
{
    char fmt[1024];

    ++g_tests;
    if (condition)
        return;

    ++g_errors;
    sprintf(fmt, "\nFAILED! EXPECTED %s, ACTUAL %s.\n\tat %%s:%%d\n", expectedFmt, actualFmt);
    outf(COLOR_DARK_RED, fmt, expected, actual, file, line);
}

void ASSERT_INT_EQUAL_(const char* file, int line, int expected, int actual)
{
    ++g_tests;
    if (expected == actual)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %d (0x%x), ACTUAL %d (0x%x).\n\tat %s:%d\n",
        expected, expected, actual, actual, file, line);
}

int run_tests(int argc, char** argv, int appType, const Test* tests)
{
    int wasTests;
    int wasErrors;

    DONT_WARN_UNUSED(argc);
    DONT_WARN_UNUSED(argv);

    g_appType = appType;
    g_tests = 0;
    g_errors = 0;

    for (; tests->name; ++tests) {
        wasTests = g_tests;
        wasErrors = g_errors;

        tests->fn();

        if (g_errors == wasErrors)
            outf(COLOR_DARK_GREEN, "%s: PASSED\n", tests->name);
        else {
            int ntests = g_tests - wasTests;
            int nerrors = g_errors - wasErrors;
            outf(COLOR_LIGHT_RED, "%s: %d of %d test%s failed\n",
                tests->name, nerrors, ntests, (ntests == 1 ? "" : "s"));
        }
    }

    if (g_errors == 0)
        outf(COLOR_LIGHT_GREEN, "=== ALL TESTS PASSED ===");
    else
        outf(COLOR_WHITE_ON_RED, "=== SOME TESTS FAILED ===");
    outf(COLOR_DEFAULT, "\n");

    return g_errors;
}
