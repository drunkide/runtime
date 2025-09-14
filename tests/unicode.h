#include <runtime/string.h>

#ifdef _WIN32
#include <runtime/mswin/winutil.h>
#endif
#include <runtime/log.h>

static void test_unicode(void)
{
    /* Utf32CharToUtf8 */

    {
        uint8 buf[12];

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(1, Utf32CharToUtf8(buf, 0));
        { uint8 expect8[] = { 0x00,0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(1, Utf32CharToUtf8(buf, 'Q'));
        { uint8 expect8[] = { 'Q',0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(2, Utf32CharToUtf8(buf, 0x416));
        { uint8 expect8[] = { 0xd0,0x96,0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(3, Utf32CharToUtf8(buf, 0x20ac));
        { uint8 expect8[] = { 0xe2,0x82,0xac,0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(4, Utf32CharToUtf8(buf, 0x1f600));
        { uint8 expect8[] = { 0xf0,0x9f,0x98,0x80,0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(4, Utf32CharToUtf8(buf, UNICODE_MAX_CODEPOINT));
        { uint8 expect8[] = { 0xf4,0x8f,0xbf,0xbf,0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(0, Utf32CharToUtf8(buf, UNICODE_MAX_CODEPOINT + 1));
        { uint8 expect8[] = {0xff }; ASSERT_UINT8_ARRAY_EQUAL(expect8, buf, sizeof(expect8)); }
    }

    /* Utf32CharToUtf16 */

    {
        uint16 buf[12];

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(1, Utf32CharToUtf16(buf, 0));
        { uint16 expect16[] = { 0x0000,0xffff }; ASSERT_UINT16_ARRAY_EQUAL(expect16, buf, sizeof(expect16)/2); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(1, Utf32CharToUtf16(buf, 0x20ac));
        { uint16 expect16[] = { 0x20ac,0xffff }; ASSERT_UINT16_ARRAY_EQUAL(expect16, buf, sizeof(expect16)/2); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(2, Utf32CharToUtf16(buf, 0x1f600));
        { uint16 expect16[] = { 0xd83d,0xde00,0xffff }; ASSERT_UINT16_ARRAY_EQUAL(expect16, buf, sizeof(expect16)/2); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(2, Utf32CharToUtf16(buf, UNICODE_MAX_CODEPOINT));
        { uint16 expect16[] = { 0xdbff,0xdfff,0xffff }; ASSERT_UINT16_ARRAY_EQUAL(expect16, buf, sizeof(expect16)/2); }

        memset(buf, 0xff, sizeof(buf));
        ASSERT_SIZE_EQUAL(0, Utf32CharToUtf16(buf, UNICODE_MAX_CODEPOINT + 1));
        { uint16 expect16[] = { 0xffff }; ASSERT_UINT16_ARRAY_EQUAL(expect16, buf, sizeof(expect16)/2); }
    }

    /* Utf16Length */

    ASSERT_SIZE_EQUAL(0, Utf16Length("\0"));
    ASSERT_SIZE_EQUAL(1, Utf16Length("\xa3\x0\0"));
    ASSERT_SIZE_EQUAL(1, Utf16Length("\xff\xdf\0"));
    ASSERT_SIZE_EQUAL(2, Utf16Length("\xff\xdf\xff\xdf\0"));
    ASSERT_SIZE_EQUAL(1, Utf16Length("\xff\xdb\xff\xdf\0"));
    ASSERT_SIZE_EQUAL(2, Utf16Length("\xff\xdf\xff\xdb\0"));
    ASSERT_SIZE_EQUAL(2, Utf16Length("\xff\xdf\xff\xdb\xff\xdf\0"));

    ASSERT_SIZE_EQUAL(0, Utf16LengthN("\xff\xdb\xff\xdf\0", 0));
    ASSERT_SIZE_EQUAL(1, Utf16LengthN("\xff\xdb\xff\xdf\0", 1));
    ASSERT_SIZE_EQUAL(1, Utf16LengthN("\xff\xdb\xff\xdf\0", 2));
    ASSERT_SIZE_EQUAL(2, Utf16LengthN("\xff\xdb\xff\xdf\0", 3));

    ASSERT_SIZE_EQUAL(0, Utf16LengthN("\xff\xdf\xff\xdb\xff\xdf\0", 0));
    ASSERT_SIZE_EQUAL(1, Utf16LengthN("\xff\xdf\xff\xdb\xff\xdf\0", 1));
    ASSERT_SIZE_EQUAL(2, Utf16LengthN("\xff\xdf\xff\xdb\xff\xdf\0", 2));
    ASSERT_SIZE_EQUAL(2, Utf16LengthN("\xff\xdf\xff\xdb\xff\xdf\0", 3));
    ASSERT_SIZE_EQUAL(3, Utf16LengthN("\xff\xdf\xff\xdb\xff\xdf\0", 4));
}
