#include <runtime/string.h>

static void test_utf8(const char* file, int line, const char* input, size_t expectLen,
    uint16* expect16, size_t expect16Len, uint32* expect32, size_t expect32Len)
{
    uint32 u32[64];
    uint16 u16[64];

    ASSERT_SIZE_EQUAL_(file, line, expectLen, Utf8Length(input));

    memset(u16, 0xff, sizeof(u16));
    Utf8ToUtf16(u16, input);
    ASSERT_UINT16_ARRAY_EQUAL_(file, line, expect16, u16, expect16Len);

    memset(u16, 0xff, sizeof(u16));
    Utf8ToUtf16N(u16, input, strlen(input));
    expect16[expect16Len - 1] = 0xffff;
    ASSERT_UINT16_ARRAY_EQUAL_(file, line, expect16, u16, expect16Len);

    memset(u32, 0xff, sizeof(u32));
    Utf8ToUtf32(u32, input);
    ASSERT_UINT32_ARRAY_EQUAL_(file, line, expect32, u32, expect32Len);

    memset(u32, 0xff, sizeof(u32));
    Utf8ToUtf32N(u32, input, strlen(input));
    expect32[expect32Len - 1] = 0xffffffff;
    ASSERT_UINT32_ARRAY_EQUAL_(file, line, expect32, u32, expect32Len);
}

#define _ ,
#define TEST_UTF8(STR, LEN, UTF16, UTF32) \
    { \
        uint16 expect16[] = UTF16; \
        uint32 expect32[] = UTF32; \
        test_utf8(__FILE__, __LINE__, (STR), (LEN), \
            expect16, sizeof(expect16) / sizeof(expect16[0]), \
            expect32, sizeof(expect32) / sizeof(expect32[0]) \
            ); \
    } \

static void test_utf8str(void)
{
    TEST_UTF8("abc",
        3,
        { 'a' _ 'b' _ 'c' _ 0 },
        { 'a' _ 'b' _ 'c' _ 0 }
        );

    TEST_UTF8("\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82", /* привет */
        6,
        { 0x043f _ 0x0440 _ 0x0438 _ 0x0432 _ 0x0435 _ 0x0442 _ 0 },
        { 0x043f _ 0x0440 _ 0x0438 _ 0x0432 _ 0x0435 _ 0x0442 _ 0 }
        );

    TEST_UTF8("\xF0\x9F\x98\x80", /* 😀 */
        1,
        { 0xd83d _ 0xde00 _ 0 },
        { 0x1f600 _ 0 }
        );

    TEST_UTF8("\xC0",
        1,
        { 0xfffd _ 0 },
        { 0xfffd _ 0 }
        );

    TEST_UTF8("\xC0\xC1",
        2,
        { 0xfffd _ 0xfffd _ 0 },
        { 0xfffd _ 0xfffd _ 0 }
        );

    TEST_UTF8("\xC0" "A",
        2,
        { 0xfffd _ 'A' _ 0 },
        { 0xfffd _ 'A' _ 0 }
        );

    TEST_UTF8("A" "\xC0",
        2,
        { 'A' _ 0xfffd _ 0 },
        { 'A' _ 0xfffd _ 0 }
        );

    TEST_UTF8("\xE2",
        1,
        { 0xfffd _ 0 },
        { 0xfffd _ 0 }
        );

    TEST_UTF8("\xE2\x82",
        1,
        { 0xfffd _ 0 },
        { 0xfffd _ 0 }
        );

    TEST_UTF8("\xE2\x82" "Q",
        2,
        { 0xfffd _ 'Q' _ 0 },
        { 0xfffd _ 'Q' _ 0 }
        );

    TEST_UTF8("\xE2\xE2\x82\xAC",
        2,
        { 0xfffd _ 0x20ac _ 0 },
        { 0xfffd _ 0x20ac _ 0 }
        );

    TEST_UTF8("\xE2\x82\xE2\x82\xAC",
        2,
        { 0xfffd _ 0x20ac _ 0 },
        { 0xfffd _ 0x20ac _ 0 }
        );

    TEST_UTF8("\xE2\x82\xAC\xE2\x82\xAC",
        2,
        { 0x20ac _ 0x20ac _ 0 },
        { 0x20ac _ 0x20ac _ 0 }
        );

    TEST_UTF8("\xE2\xAC\xE2\x82\xAC",
        2,
        { 0xfffd _ 0x20ac _ 0 },
        { 0xfffd _ 0x20ac _ 0 }
        );

    /* Utf8LengthN */

    ASSERT_SIZE_EQUAL(0, Utf8LengthN("\xF0\x9F\x98\x80 x", 0));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 1));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 2));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 3));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 4));
    ASSERT_SIZE_EQUAL(2, Utf8LengthN("\xF0\x9F\x98\x80 x", 5));
    ASSERT_SIZE_EQUAL(3, Utf8LengthN("\xF0\x9F\x98\x80 x", 6));
}
