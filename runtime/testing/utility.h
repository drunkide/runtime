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

void ASSERT_TRUE_(const char* file, int line, const char* conditionStr, bool condition);
#define ASSERT_TRUE(condition) ASSERT_TRUE_(__FILE__, __LINE__, #condition, (condition))

void ASSERT_FALSE_(const char* file, int line, const char* conditionStr, bool condition);
#define ASSERT_FALSE(condition) ASSERT_FALSE_(__FILE__, __LINE__, #condition, (condition))

void ASSERT_INT_EQUAL_(const char* file, int line, int expected, int actual);
#define ASSERT_INT_EQUAL(expected, actual) ASSERT_INT_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_INT32_EQUAL_(const char* file, int line, int32 expected, int32 actual);
#define ASSERT_INT32_EQUAL(expected, actual) ASSERT_INT32_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_UINT32_EQUAL_(const char* file, int line, uint32 expected, uint32 actual);
#define ASSERT_UINT32_EQUAL(expected, actual) ASSERT_UINT32_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_UINT64_EQUAL_(const char* file, int line, uint64 expected, uint64 actual);
#define ASSERT_UINT64_EQUAL(expected, actual) ASSERT_UINT64_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_INT64_EQUAL_(const char* file, int line, int64 expected, int64 actual);
#define ASSERT_INT64_EQUAL(expected, actual) ASSERT_INT64_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_SIZE_EQUAL_(const char* file, int line, size_t expected, size_t actual);
#define ASSERT_SIZE_EQUAL(expected, actual) ASSERT_SIZE_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_DOUBLE_EQUAL_(const char* file, int line, double expected, double actual);
#define ASSERT_DOUBLE_EQUAL(expected, actual) ASSERT_DOUBLE_EQUAL_(__FILE__, __LINE__, (expected), (actual))

void ASSERT_UINT8_ARRAY_EQUAL_(const char* file, int line, const uint8* expected, const uint8* actual, size_t n);
#define ASSERT_UINT8_ARRAY_EQUAL(expected, actual, n) ASSERT_UINT8_ARRAY_EQUAL_(__FILE__, __LINE__, (expected), (actual), (n))

void ASSERT_UINT16_ARRAY_EQUAL_(const char* file, int line, const uint16* expected, const uint16* actual, size_t n);
#define ASSERT_UINT16_ARRAY_EQUAL(expected, actual, n) ASSERT_UINT16_ARRAY_EQUAL_(__FILE__, __LINE__, (expected), (actual), (n))

void ASSERT_UINT32_ARRAY_EQUAL_(const char* file, int line, const uint32* expected, const uint32* actual, size_t n);
#define ASSERT_UINT32_ARRAY_EQUAL(expected, actual, n) ASSERT_UINT32_ARRAY_EQUAL_(__FILE__, __LINE__, (expected), (actual), (n))

int run_tests(int argc, char** argv, int appType, const Test* tests);

#endif
