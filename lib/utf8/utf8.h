#ifndef RUNTIME_LIB_UTF8_UTF8_H
#define RUNTIME_LIB_UTF8_UTF8_H

#include <runtime/common.h>

/*****************************************************************************
 * Based on Flexible and Economical UTF-8 Decoder.
 * See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
 * Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 ****************************************************************************/

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

extern const uint8 g_utf8d[];

STRUCT(Utf8State)
{
    uint32 codep;
    unsigned state;
};

#define UTF8_INIT(state) \
    (state).state = UTF8_ACCEPT

#define UTF8_STEP(CTX, CH, EMIT) \
    { \
        const unsigned curState_ = (CTX).state; \
        const uint8 in_ = (uint8)(CH); \
        const uint8 type_ = g_utf8d[in_]; \
        unsigned nextState_; \
        if (curState_ == UTF8_ACCEPT) { \
            (CTX).codep = (uint32)(in_ & (0xff >> type_)); \
        } else { \
            (CTX).codep = ((CTX).codep << 6) | (uint32)(in_ & 0x3f); \
        } \
        nextState_ = g_utf8d[256 + curState_ + type_]; \
        if (nextState_ == UTF8_ACCEPT) { \
            (CTX).state = UTF8_ACCEPT; \
            EMIT; \
        } else if (nextState_ != UTF8_REJECT) { \
            (CTX).state = nextState_; \
        } else { \
            (CTX).codep = 0xFFFD; \
            EMIT; \
            if (curState_ != UTF8_ACCEPT) { \
                nextState_ = g_utf8d[256 + type_]; \
                if (nextState_ == UTF8_ACCEPT) { \
                    (CTX).codep = (uint32)in_; \
                  emit: \
                    (CTX).state = UTF8_ACCEPT; \
                    EMIT; \
                } else if (nextState_ != UTF8_REJECT) { \
                    (CTX).state = nextState_; \
                    (CTX).codep = (uint32)in_; \
                } else { \
                    (CTX).codep = 0xFFFD; \
                    goto emit; \
                } \
            } \
        } \
    }

#endif
