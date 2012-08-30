/*
    SonatinaTag
    Copyright (C) 2011, 2012 Lawrence Sebald

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License version 2.1 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SonatinaTag/Error.h"
#include "Frame.h"

static void free_text(ST_TextFrame *f) {
    free(f->string);
    free(f);
}

ST_FUNC ST_TextFrame *ST_ID3v2_TextFrame_create(ST_TextEncoding e, uint32_t len,
                                                const uint8_t *str) {
    ST_TextFrame *rv;

    if(e > ST_TextEncoding_UTF8 || e <= ST_TextEncoding_Invalid)
        return NULL;

    if((rv = (ST_TextFrame *)malloc(sizeof(ST_TextFrame)))) {
        rv->base.type = ST_FrameType_Text;
        rv->base.dtor = (void (*)(ST_Frame *))free_text;
        rv->encoding = e;
        rv->size = len;

        if(!(rv->string = (uint8_t *)malloc(len))) {
            free(rv);
            return NULL;
        }

        memcpy(rv->string, str, len);
    }

    return rv;
}

ST_LOCAL ST_TextFrame *ST_ID3v2_TextFrame_create_buf(const uint8_t *buf,
                                                     uint32_t sz) {
    ST_TextFrame *rv;

    if(sz < 1 || buf[0] > (uint8_t)ST_TextEncoding_UTF8)
        return NULL;

    if((rv = (ST_TextFrame *)malloc(sizeof(ST_TextFrame)))) {
        rv->base.type = ST_FrameType_Text;
        rv->base.dtor = (void (*)(ST_Frame *))free_text;
        rv->encoding = (ST_TextEncoding)buf[0];
        rv->size = sz - 1;

        if(!(rv->string = (uint8_t *)malloc(sz - 1))) {
            free(rv);
            return NULL;
        }

        memcpy(rv->string, buf + 1, sz - 1);
    }

    return rv;
}

ST_FUNC const uint8_t *ST_ID3v2_TextFrame_text(const ST_TextFrame *f) {
    if(!f)
        return NULL;

    return f->string;
}

ST_FUNC uint32_t ST_ID3v2_TextFrame_length(const ST_TextFrame *f) {
    if(!f)
        return 0;

    return f->size;
}

ST_FUNC ST_TextEncoding ST_ID3v2_TextFrame_encoding(const ST_TextFrame *f) {
    if(!f)
        return 0;

    return f->encoding;
}

ST_FUNC ST_Error ST_ID3v2_TextFrame_setText(ST_TextFrame *f, ST_TextEncoding e,
                                            uint32_t sz, uint8_t *d,
                                            int own_buf) {
    uint8_t *tmp = d;

    if(!f || !d || !sz || e > ST_TextEncoding_UTF8 ||
       e <= ST_TextEncoding_Invalid)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)sz)))
            return ST_Error_errno;

        memcpy(tmp, d, sz);
    }

    free(f->string);
    f->string = tmp;
    f->size = sz;
    f->encoding = e;

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
static CFStringEncoding encs[4] = {
    kCFStringEncodingISOLatin1,
    kCFStringEncodingUTF16,
    kCFStringEncodingUTF16BE,
    kCFStringEncodingUTF8
};

ST_FUNC ST_TextFrame *ST_ID3v2_TextFrame_createStr(CFStringRef str,
                                                   ST_TextEncoding enc) {
    ST_TextFrame *rv;
    uint8_t *buf;
    CFIndex slen;
    CFIndex l;
    void *tmp;

    if(enc > ST_TextEncoding_UTF8 || enc <= ST_TextEncoding_Invalid || !str)
        return NULL;

    /* Make space for it */
    slen = CFStringGetLength(str);
    l = CFStringGetMaximumSizeForEncoding(slen, encs[enc]);

    if(!(buf = (uint8_t *)malloc(l)))
        return NULL;

    CFStringGetBytes(str, CFRangeMake(0, slen), encs[enc], 0,
                     enc == ST_TextEncoding_UTF16, buf, l, &slen);

    /* Attempt to make it smaller */
    if(slen != l) {
        if((tmp = realloc(buf, slen)))
            buf = (uint8_t *)tmp;
    }

    if((rv = (ST_TextFrame *)malloc(sizeof(ST_TextFrame)))) {
        rv->base.type = ST_FrameType_Text;
        rv->base.dtor = (void (*)(ST_Frame *))free_text;
        rv->encoding = enc;
        rv->size = slen;
        rv->string = buf;
    }
    else {
        free(buf);
    }

    return rv;
}

ST_FUNC CFStringRef ST_ID3v2_TextFrame_copyText(const ST_TextFrame *f) {
    if(!f)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, f->string, f->size,
                                   encs[f->encoding],
                                   f->encoding == ST_TextEncoding_UTF16);
}

ST_FUNC ST_Error ST_ID3v2_TextFrame_setTextStr(ST_TextFrame *f, CFStringRef s) {
    CFIndex slen;
    CFIndex l;
    uint8_t *buf;
    void *tmp;

    if(!f || !s) 
        return ST_Error_InvalidArgument;

    /* Make space for it */
    slen = CFStringGetLength(s);
    l = CFStringGetMaximumSizeForEncoding(slen, encs[f->encoding]);

    if(!(buf = (uint8_t *)malloc(l)))
        return ST_Error_errno;

    CFStringGetBytes(s, CFRangeMake(0, slen), encs[f->encoding], 0,
                     f->encoding == ST_TextEncoding_UTF16, buf, l, &slen);

    /* Attempt to make it smaller */
    if(slen != l) {
        if((tmp = realloc(buf, slen)))
            buf = (uint8_t *)tmp;
    }

    free(f->string);
    f->size = slen;
    f->string = buf;

    return ST_Error_None;
}

ST_FUNC CFStringRef ST_ID3v2_TextFrame_createString(const ST_TextFrame *f) {
    static const CFStringRef fmt = CFSTR("{ text='%@' }");
    CFStringRef text, rv;

    if(!f)
        return NULL;

    text = ST_ID3v2_TextFrame_copyText(f);
    rv = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, fmt, text);
    CFRelease(text);

    return rv;
}
#endif
