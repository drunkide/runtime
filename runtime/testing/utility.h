#ifndef RUNTIME_TESTING_UTILITY_H
#define RUNTIME_TESTING_UTILITY_H

#include <runtime/common.h>

enum {
    APP_CONSOLE,
    APP_GUI
};

enum {
    COLOR_DEFAULT,
    COLOR_DARK_GREEN,
    COLOR_LIGHT_GREEN,
    COLOR_DARK_RED,
    COLOR_LIGHT_RED,
    COLOR_WHITE,
    COLOR_WHITE_ON_RED
};

STRUCT(Test) {
    const char* name;
    void (*fn)(void);
};

#define TEST(NAME) { { #NAME, test_##NAME } }

void ASSERT_(const char* file, int line, bool condition,
    const char* expectedFmt, const char* expected,
    const char* actualFmt, const char* actual);
#define ASSERT(condition, expectedFmt, expected, actualFmt, actual) \
    ASSERT_(__FILE__, __LINE__, (condition), \
        (expectedFmt), (expected), (actualFmt), (actual))

void ASSERT_INT_EQUAL_(const char* file, int line, int expected, int actual);
#define ASSERT_INT_EQUAL(expected, actual) ASSERT_INT_EQUAL_(__FILE__, __LINE__, (expected), (actual))

int run_tests(int argc, char** argv, int appType, const Test* tests);

#endif
