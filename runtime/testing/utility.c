#include <runtime/testing/utility.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef _WIN32
#include <runtime/mswin/winapi.h>
int sprintf(char* dst, const char* fmt, ...);
int vsprintf(char* dst, const char* fmt, va_list args);
#else
#include <stdio.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

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

  #ifdef __EMSCRIPTEN__
    const char* cstr, *cbg = "#000000";
    switch (color) {
        case COLOR_DEFAULT: default: cstr = "#00000"; break;
        case COLOR_DARK_GREEN: cstr = "#008000"; break;
        case COLOR_LIGHT_GREEN: cstr = "#00ff00"; break;
        case COLOR_DARK_RED: cstr = "#800000"; break;
        case COLOR_LIGHT_RED: cstr = "#ff0000"; break;
        case COLOR_WHITE: cstr = "#ffffff"; break;
        case COLOR_WHITE_ON_RED: cstr = "#ffffff"; cbg = "#ff0000"; break;
    }

    DONT_WARN_UNUSED(len);
    EM_ASM({
        console.log(UTF8ToString($0));
        document.body.innerHTML = document.body.innerHTML +
            "<div style=\"color:" + UTF8ToString($1) + ";" +
              "background-color:" + UTF8ToString($2) + "\">" +
              UTF8ToString($0).replaceAll('\n','<br>') +
            "</div>";
    }, buf, cstr, cbg);
  #elif defined(_WIN32)
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
  #else
    DONT_WARN_UNUSED(color);
    fwrite(buf, 1, len, stdout);
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

void ASSERT_TRUE_(const char* file, int line, const char* conditionStr, bool condition)
{
    ++g_tests;
    if (condition)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %s TO BE true, BUT GOT false.\n\tat %s:%d\n", conditionStr, file, line);
}

void ASSERT_FALSE_(const char* file, int line, const char* conditionStr, bool condition)
{
    ++g_tests;
    if (!condition)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %s TO BE false, BUT GOT true.\n\tat %s:%d\n", conditionStr, file, line);
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

void ASSERT_INT32_EQUAL_(const char* file, int line, int32 expected, int32 actual)
{
    ++g_tests;
    if (expected == actual)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %ld (0x%lx), ACTUAL %ld (0x%lx).\n\tat %s:%d\n",
        (long)expected, (long)expected, (long)actual, (long)actual, file, line);
}

void ASSERT_UINT32_EQUAL_(const char* file, int line, uint32 expected, uint32 actual)
{
    ++g_tests;
    if (expected == actual)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %lu (0x%lx), ACTUAL %lu (0x%lx).\n\tat %s:%d\n",
        (unsigned long)expected, (unsigned long)expected, (unsigned long)actual, (unsigned long)actual, file, line);
}

void ASSERT_UINT64_EQUAL_(const char* file, int line, uint64 expected, uint64 actual)
{
    char e[128], a[128];

    ++g_tests;
    if (expected.half.low == actual.half.low && expected.half.high == actual.half.high)
        return;

    if (expected.half.high == 0)
        sprintf(e, "%lx", (unsigned long)expected.half.low);
    else
        sprintf(e, "%lx%08lx", (unsigned long)expected.half.high, (unsigned long)expected.half.low);

    if (actual.half.high == 0)
        sprintf(a, "%lx", (unsigned long)actual.half.low);
    else
        sprintf(a, "%lx%08lx", (unsigned long)actual.half.high, (unsigned long)actual.half.low);

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED 0x%s, ACTUAL 0x%s.\n\tat %s:%d\n", e, a, file, line);
}

void ASSERT_INT64_EQUAL_(const char* file, int line, int64 expected, int64 actual)
{
    char e[128], a[128];

    ++g_tests;
    if (expected.half.low == actual.half.low && expected.half.high == actual.half.high)
        return;

    if (expected.half.high == 0)
        sprintf(e, "%lx", (unsigned long)expected.half.low);
    else
        sprintf(e, "%lx%08lx", (unsigned long)(uint32)expected.half.high, (unsigned long)expected.half.low);

    if (actual.half.high == 0)
        sprintf(a, "%lx", (unsigned long)actual.half.low);
    else
        sprintf(a, "%lx%08lx", (unsigned long)(uint32)actual.half.high, (unsigned long)actual.half.low);

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED 0x%s, ACTUAL 0x%s.\n\tat %s:%d\n", e, a, file, line);
}

void ASSERT_DOUBLE_EQUAL_(const char* file, int line, double expected, double actual)
{
    ++g_tests;
    if (fabs(expected - actual) <= 1e-9)
        return;

    ++g_errors;
    outf(COLOR_DARK_RED, "\nFAILED! EXPECTED %.17g, ACTUAL %.17g.\n\tat %s:%d\n",
        expected, actual, file, line);
}

int run_tests(int argc, char** argv, int appType, const Test* tests)
{
    int wasTests;
    int wasErrors;
    int ntests;

    DONT_WARN_UNUSED(argc);
    DONT_WARN_UNUSED(argv);

    g_appType = appType;
    g_tests = 0;
    g_errors = 0;

  #ifdef __EMSCRIPTEN__
    EM_ASM({
        document.body.innerHTML = "";
    });
  #endif

    for (; tests->name; ++tests) {
        wasTests = g_tests;
        wasErrors = g_errors;

        tests->fn();

        ntests = g_tests - wasTests;
        if (g_errors == wasErrors) {
            outf(COLOR_DARK_GREEN, "%s: %d TEST%s PASSED\n",
                tests->name, ntests, (ntests == 1 ? "" : "S"));
        } else {
            int nerrors = g_errors - wasErrors;
            outf(COLOR_LIGHT_RED, "\n%s: %d of %d test%s failed\n",
                tests->name, nerrors, ntests, (ntests == 1 ? "" : "s"));
        }
    }

    if (g_errors == 0)
        outf(COLOR_LIGHT_GREEN, "=== ALL TESTS PASSED ===");
    else
        outf(COLOR_WHITE_ON_RED, "=== SOME TESTS FAILED ===");
    outf(COLOR_DEFAULT, "\n");

  #ifdef __EMSCRIPTEN__
    return 0;
  #else
    return g_errors;
  #endif
}
