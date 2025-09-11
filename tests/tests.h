#ifndef RUNTIME_TESTS_H
#define RUNTIME_TESTS_H

#include <stdio.h>

static int g_errors;

#define ASSERT_INT_EQUAL(expected, actual) \
    do { \
        int expected__ = (int)(expected); \
        int actual__ = (int)(actual); \
        if (expected__ != actual__) { \
            printf("\nFAILED! EXPECTED %d (0x%x), ACTUAL %d (0x%x).\n\tat %s:%d\n", \
                expected__, expected__, actual__, actual__, __FILE__, __LINE__); \
            ++g_errors; \
        } \
    } while(0)

#include <tests/cases/sprintf.h>

static int run_tests(int argc, char** argv)
{
    DONT_WARN_UNUSED(argc);
    DONT_WARN_UNUSED(argv);

    test_sprintf();

    return g_errors;
}

#endif
