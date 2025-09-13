#include <runtime/string.h>

static void test_utf8(const char* file, int line, const char* input, size_t expectLen,
    uint16* expect16, size_t expect16Len, uint32* expect32, size_t expect32Len)
{
    uint32 u32[64];
    uint16 u16[64];

    ASSERT_SIZE_EQUAL_(file, line, expectLen, Utf8Length(input));
    ASSERT_SIZE_EQUAL_(file, line, expect16Len - 1, Utf8ToUtf16Chars(input));

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
        static uint16 expect16[] = UTF16; \
        static uint32 expect32[] = UTF32; \
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

    /* N versions */

    {
        const char* input =         "\xE2\x82\xAC" "\0" "\xE2" "\0" "\xE2\x82" "\0" "\xE2\x82\xAC";
        static uint16 expect16[] = { 0x20ac       _ 0 _ 0xfffd _ 0 _ 0xfffd    _ 0 _ 0x20ac      _ 0 };
        static uint32 expect32[] = { 0x20ac       _ 0 _ 0xfffd _ 0 _ 0xfffd    _ 0 _ 0x20ac      _ 0 };
        uint16 u16[24];
        uint32 u32[24];

        /* UTF-8 Length */

        ASSERT_SIZE_EQUAL(1, Utf8Length(input));
        ASSERT_SIZE_EQUAL(0, Utf8LengthN(input, 0));
        ASSERT_SIZE_EQUAL(1, Utf8LengthN(input, 1));
        ASSERT_SIZE_EQUAL(1, Utf8LengthN(input, 2));
        ASSERT_SIZE_EQUAL(1, Utf8LengthN(input, 3));
        ASSERT_SIZE_EQUAL(2, Utf8LengthN(input, 4));
        ASSERT_SIZE_EQUAL(3, Utf8LengthN(input, 5));
        ASSERT_SIZE_EQUAL(4, Utf8LengthN(input, 6));
        ASSERT_SIZE_EQUAL(5, Utf8LengthN(input, 7));
        ASSERT_SIZE_EQUAL(5, Utf8LengthN(input, 8));
        ASSERT_SIZE_EQUAL(6, Utf8LengthN(input, 9));
        ASSERT_SIZE_EQUAL(7, Utf8LengthN(input, 10));
        ASSERT_SIZE_EQUAL(7, Utf8LengthN(input, 11));
        ASSERT_SIZE_EQUAL(7, Utf8LengthN(input, 12));
        ASSERT_SIZE_EQUAL(8, Utf8LengthN(input, 13));

        /* UTF-16 Length */

        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16Chars(input));
        ASSERT_SIZE_EQUAL(0, Utf8ToUtf16CharsN(input, 0));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(input, 1));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(input, 2));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(input, 3));
        ASSERT_SIZE_EQUAL(2, Utf8ToUtf16CharsN(input, 4));
        ASSERT_SIZE_EQUAL(3, Utf8ToUtf16CharsN(input, 5));
        ASSERT_SIZE_EQUAL(4, Utf8ToUtf16CharsN(input, 6));
        ASSERT_SIZE_EQUAL(5, Utf8ToUtf16CharsN(input, 7));
        ASSERT_SIZE_EQUAL(5, Utf8ToUtf16CharsN(input, 8));
        ASSERT_SIZE_EQUAL(6, Utf8ToUtf16CharsN(input, 9));
        ASSERT_SIZE_EQUAL(7, Utf8ToUtf16CharsN(input, 10));
        ASSERT_SIZE_EQUAL(7, Utf8ToUtf16CharsN(input, 11));
        ASSERT_SIZE_EQUAL(7, Utf8ToUtf16CharsN(input, 12));
        ASSERT_SIZE_EQUAL(8, Utf8ToUtf16CharsN(input, 13));

        /* UTF-16 */

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16(u16, input);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 1);
        ASSERT_INT_EQUAL(u16[1], 0);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 0);
        ASSERT_INT_EQUAL(u16[0], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 1);
        ASSERT_INT_EQUAL(u16[0], 0xfffd);
        ASSERT_INT_EQUAL(u16[1], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 2);
        ASSERT_INT_EQUAL(u16[0], 0xfffd);
        ASSERT_INT_EQUAL(u16[1], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 3);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 1);
        ASSERT_INT_EQUAL(u16[1], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 4);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 2);
        ASSERT_INT_EQUAL(u16[2], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 5);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 3);
        ASSERT_INT_EQUAL(u16[3], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 6);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 4);
        ASSERT_INT_EQUAL(u16[4], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 7);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 4);
        ASSERT_INT_EQUAL(u16[4], 0xfffd);
        ASSERT_INT_EQUAL(u16[5], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 8);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 5);
        ASSERT_INT_EQUAL(u16[5], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 9);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 6);
        ASSERT_INT_EQUAL(u16[6], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 10);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 6);
        ASSERT_INT_EQUAL(u16[6], 0xfffd);
        ASSERT_INT_EQUAL(u16[7], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 11);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 6);
        ASSERT_INT_EQUAL(u16[6], 0xfffd);
        ASSERT_INT_EQUAL(u16[7], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 12);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 7);
        ASSERT_INT_EQUAL(u16[7], 0xffff);

        memset(u16, 0xff, sizeof(u16));
        Utf8ToUtf16N(u16, input, 13);
        ASSERT_UINT16_ARRAY_EQUAL(expect16, u16, 8);
        ASSERT_INT_EQUAL(u16[8], 0xffff);

        /* UTF-32 */

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32(u32, input);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 1);
        ASSERT_UINT32_EQUAL(u32[1], 0);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 0);
        ASSERT_UINT32_EQUAL(u32[0], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 1);
        ASSERT_UINT32_EQUAL(u32[0], 0xfffd);
        ASSERT_UINT32_EQUAL(u32[1], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 2);
        ASSERT_UINT32_EQUAL(u32[0], 0xfffd);
        ASSERT_UINT32_EQUAL(u32[1], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 3);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 1);
        ASSERT_UINT32_EQUAL(u32[1], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 4);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 2);
        ASSERT_UINT32_EQUAL(u32[2], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 5);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 3);
        ASSERT_UINT32_EQUAL(u32[3], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 6);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 4);
        ASSERT_UINT32_EQUAL(u32[4], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 7);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 4);
        ASSERT_UINT32_EQUAL(u32[4], 0xfffd);
        ASSERT_UINT32_EQUAL(u32[5], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 8);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 5);
        ASSERT_UINT32_EQUAL(u32[5], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 9);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 6);
        ASSERT_UINT32_EQUAL(u32[6], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 10);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 6);
        ASSERT_UINT32_EQUAL(u32[6], 0xfffd);
        ASSERT_UINT32_EQUAL(u32[7], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 11);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 6);
        ASSERT_UINT32_EQUAL(u32[6], 0xfffd);
        ASSERT_UINT32_EQUAL(u32[7], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 12);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 7);
        ASSERT_UINT32_EQUAL(u32[7], 0xffffffff);

        memset(u32, 0xff, sizeof(u32));
        Utf8ToUtf32N(u32, input, 13);
        ASSERT_UINT32_ARRAY_EQUAL(expect32, u32, 8);
        ASSERT_UINT32_EQUAL(u32[8], 0xffffffff);
    }

    /* Utf8LengthN */

    ASSERT_SIZE_EQUAL(0, Utf8LengthN("\xF0\x9F\x98\x80 x", 0));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 1));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 2));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 3));
    ASSERT_SIZE_EQUAL(1, Utf8LengthN("\xF0\x9F\x98\x80 x", 4));
    ASSERT_SIZE_EQUAL(2, Utf8LengthN("\xF0\x9F\x98\x80 x", 5));
    ASSERT_SIZE_EQUAL(3, Utf8LengthN("\xF0\x9F\x98\x80 x", 6));

    /* Utf8ToUtf16CharsN */

    {
        const char* inp = "\xF0\x9F\x98\x80\0"
                          "\xF0\0"
                          "\xF0\x9F\0"
                          "\xF0\x9F\x98\0"
                          "\xF0\x9F\x98\x80";

        ASSERT_SIZE_EQUAL(2, Utf8ToUtf16Chars(inp));
        ASSERT_SIZE_EQUAL(0, Utf8ToUtf16CharsN(inp, 0));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(inp, 1));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(inp, 2));
        ASSERT_SIZE_EQUAL(1, Utf8ToUtf16CharsN(inp, 3));
        ASSERT_SIZE_EQUAL(2, Utf8ToUtf16CharsN(inp, 4));
        ASSERT_SIZE_EQUAL(3, Utf8ToUtf16CharsN(inp, 5));
        ASSERT_SIZE_EQUAL(4, Utf8ToUtf16CharsN(inp, 6));
        ASSERT_SIZE_EQUAL(5, Utf8ToUtf16CharsN(inp, 7));
        ASSERT_SIZE_EQUAL(6, Utf8ToUtf16CharsN(inp, 8));
        ASSERT_SIZE_EQUAL(6, Utf8ToUtf16CharsN(inp, 9));
        ASSERT_SIZE_EQUAL(7, Utf8ToUtf16CharsN(inp, 10));
        ASSERT_SIZE_EQUAL(8, Utf8ToUtf16CharsN(inp, 11));
        ASSERT_SIZE_EQUAL(8, Utf8ToUtf16CharsN(inp, 12));
        ASSERT_SIZE_EQUAL(8, Utf8ToUtf16CharsN(inp, 13));
        ASSERT_SIZE_EQUAL(9, Utf8ToUtf16CharsN(inp, 14));
        ASSERT_SIZE_EQUAL(10, Utf8ToUtf16CharsN(inp, 15));
        ASSERT_SIZE_EQUAL(10, Utf8ToUtf16CharsN(inp, 16));
        ASSERT_SIZE_EQUAL(10, Utf8ToUtf16CharsN(inp, 17));
        ASSERT_SIZE_EQUAL(11, Utf8ToUtf16CharsN(inp, 18));
        ASSERT_SIZE_EQUAL(12, Utf8ToUtf16CharsN(inp, 19));
    }
}
