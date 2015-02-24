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

static void free_usertext(ST_UserTextFrame *f) {
    free(f->desc);
    free(f->string);
    free(f);
}

ST_FUNC ST_UserTextFrame *ST_ID3v2_UserTextFrame_create(ST_TextEncoding enc,
                                                        uint32_t sl,
                                                        const uint8_t *str,
                                                        uint32_t dl,
                                                        const uint8_t *desc) {
    ST_UserTextFrame *rv;

    if(!str || !desc)
        return NULL;

    if(enc > ST_TextEncoding_UTF8 || enc <= ST_TextEncoding_Invalid)
        return NULL;

    if((rv = (ST_UserTextFrame *)malloc(sizeof(ST_UserTextFrame)))) {
        rv->base.type = ST_FrameType_UserText;
        rv->base.dtor = (void (*)(ST_Frame *))free_usertext;
        rv->encoding = enc;
        rv->string_size = sl;

        if(!(rv->string = (uint8_t *)malloc(sl))) {
            free(rv);
            return NULL;
        }

        if(!(rv->desc = (uint8_t *)malloc(dl))) {
            free(rv->string);
            free(rv);
            return NULL;
        }

        memcpy(rv->string, str, sl);
        memcpy(rv->desc, desc, dl);
    }

    return rv;
}

static inline int blen_utf8(const uint8_t *buf, uint32_t sz) {
    const uint8_t *ch = buf;
    int rv = 0;

    while(*ch && rv < sz - 1) {
        ++rv;
        ++ch;
    }

    if(rv > sz - 1)
        return -1;

    return rv;
}

static inline int blen_utf16(const uint8_t *buf, uint32_t sz) {
    /* We can ignore endianness, since we're looking for 0. */
    uint16_t ch = buf[0] | (buf[1] << 8);
    int rv = 0;

    while(ch && rv < sz - 2) {
        rv += 2;
        ch = buf[rv] | (buf[rv + 1] << 8);
    }

    if(rv > sz - 2)
        return -1;

    return rv;
}

ST_LOCAL ST_UserTextFrame *ST_ID3v2_UserTextFrame_create_buf(const uint8_t *buf,
                                                             uint32_t sz) {
    ST_UserTextFrame *rv;
    int enc_len;

    if(sz < 1 || buf[0] > (uint8_t)ST_TextEncoding_UTF8)
        return NULL;

    if((rv = (ST_UserTextFrame *)malloc(sizeof(ST_UserTextFrame)))) {
        rv->base.type = ST_FrameType_UserText;
        rv->base.dtor = (void (*)(ST_Frame *))free_usertext;
        rv->encoding = (ST_TextEncoding)buf[0];

        enc_len = ((buf[0] == (uint8_t)ST_TextEncoding_UTF16) ||
                   (buf[0] == (uint8_t)ST_TextEncoding_UTF16BE)) ? 2 : 1;

        if(enc_len == 1)
            rv->desc_size = (uint32_t)blen_utf8(buf + 1, sz - 1);
        else
            rv->desc_size = (uint32_t)blen_utf16(buf + 1, sz - 1);

        rv->string_size = sz - rv->desc_size - 1 - enc_len;

        /* Sanity check... */
        if(rv->desc_size == (uint32_t)-1 || rv->string_size == (uint32_t)-1) {
            free(rv);
            return NULL;
        }

        if(!(rv->desc = (uint8_t *)malloc(rv->desc_size))) {
            free(rv);
            return NULL;
        }

        if(!(rv->string = (uint8_t *)malloc(rv->string_size))) {
            free(rv->desc);
            free(rv);
            return NULL;
        }

        memcpy(rv->desc, buf + 1, rv->desc_size);
        memcpy(rv->string, buf + 1 + rv->desc_size + enc_len, rv->string_size);
    }

    return rv;
}

ST_FUNC const uint8_t *ST_ID3v2_UserTextFrame_text(const ST_UserTextFrame *f) {
    if(!f)
        return NULL;

    return f->string;
}

ST_FUNC uint32_t ST_ID3v2_UserTextFrame_textLength(const ST_UserTextFrame *f) {
    if(!f)
        return 0;

    return f->string_size;
}

ST_FUNC const uint8_t *ST_ID3v2_UserTextFrame_desc(const ST_UserTextFrame *f) {
    if(!f)
        return NULL;

    return f->desc;
}

ST_FUNC uint32_t ST_ID3v2_UserTextFrame_descLength(const ST_UserTextFrame *f) {
    if(!f)
        return 0;

    return f->desc_size;
}

ST_FUNC ST_TextEncoding ST_ID3v2_UserTextFrame_encoding(const ST_UserTextFrame *f) {
    if(!f)
        return 0;

    return f->encoding;
}

ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setText(ST_UserTextFrame *f,
                                                ST_TextEncoding enc,
                                                uint32_t str_sz, uint8_t *str,
                                                int own_buf) {
    uint8_t *tmp = str;

    if(!f || !str || !str_sz || enc > ST_TextEncoding_UTF8 ||
       enc <= ST_TextEncoding_Invalid)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)str_sz)))
            return ST_Error_errno;

        memcpy(tmp, str, str_sz);
    }

    free(f->string);
    f->string = tmp;
    f->string_size = str_sz;
    f->encoding = enc;

    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setDesc(ST_UserTextFrame *f,
                                                uint32_t desc_sz, uint8_t *desc,
                                                int own_buf) {
    uint8_t *tmp = desc;

    if(!f || !desc || !desc_sz)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)desc_sz)))
            return ST_Error_errno;

        memcpy(tmp, desc, desc_sz);
    }

    free(f->desc);
    f->desc = tmp;
    f->desc_size = desc_sz;

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
static CFStringEncoding encs[4] = {
    kCFStringEncodingISOLatin1,
    kCFStringEncodingUTF16,
    kCFStringEncodingUTF16BE,
    kCFStringEncodingUTF8
};

ST_FUNC ST_UserTextFrame *ST_ID3v2_UserTextFrame_createStr(CFStringRef str,
                                                           CFStringRef desc,
                                                           ST_TextEncoding e) {
    ST_UserTextFrame *rv;
    uint8_t *buf, *dbuf;
    CFIndex slen, dlen;
    CFIndex l;
    void *tmp;
    Boolean ext = e == ST_TextEncoding_UTF16;

    if(e > ST_TextEncoding_UTF8 || e <= ST_TextEncoding_Invalid || !str ||
       !desc)
        return NULL;

    /* Make space for the string */
    slen = CFStringGetLength(str);
    l = CFStringGetMaximumSizeForEncoding(slen, encs[e]);

    if(!(buf = (uint8_t *)malloc(l)))
        return NULL;

    CFStringGetBytes(str, CFRangeMake(0, slen), encs[e], 0, ext, buf, l, &slen);

    /* Attempt to make it smaller */
    if(slen != l) {
        if((tmp = realloc(buf, slen)))
            buf = (uint8_t *)tmp;
    }

    /* Do the same for the description */
    dlen = CFStringGetLength(desc);
    l = CFStringGetMaximumSizeForEncoding(dlen, encs[e]);

    if(!(dbuf = (uint8_t *)malloc(l))) {
        free(buf);
        return NULL;
    }

    CFStringGetBytes(desc, CFRangeMake(0, dlen), encs[e], 0, ext, dbuf, l,
                     &dlen);

    if(dlen != l) {
        if((tmp = realloc(dbuf, dlen)))
            dbuf = (uint8_t *)tmp;
    }

    if((rv = (ST_UserTextFrame *)malloc(sizeof(ST_UserTextFrame)))) {
        rv->base.type = ST_FrameType_UserText;
        rv->base.dtor = (void (*)(ST_Frame *))free_usertext;
        rv->encoding = e;
        rv->string_size = slen;
        rv->string = buf;
        rv->desc_size = dlen;
        rv->desc = dbuf;
    }
    else {
        free(buf);
        free(dbuf);
    }

    return rv;
}

ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_copyText(const ST_UserTextFrame *f) {
    if(!f)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, f->string,
                                   f->string_size, encs[f->encoding],
                                   f->encoding == ST_TextEncoding_UTF16);
}

ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_copyDesc(const ST_UserTextFrame *f) {
    if(!f)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, f->desc,
                                   f->desc_size, encs[f->encoding],
                                   f->encoding == ST_TextEncoding_UTF16);
}

static ST_Error set_str(ST_UserTextFrame *f, CFStringRef s, uint8_t **ptr,
                        uint32_t *sz) {
    CFIndex slen;
    CFIndex l;
    uint8_t *buf;
    void *tmp;

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

    free(*ptr);
    *sz = slen;
    *ptr = buf;

    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setTextStr(ST_UserTextFrame *f,
                                                   CFStringRef s) {
    if(!f || !s) 
        return ST_Error_InvalidArgument;

    return set_str(f, s, &f->string, &f->string_size);
}

ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setDescStr(ST_UserTextFrame *f,
                                                   CFStringRef d) {
    if(!f || !d) 
        return ST_Error_InvalidArgument;

    return set_str(f, d, &f->desc, &f->desc_size);
}

ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_createString(const ST_UserTextFrame *f) {
    static const CFStringRef fmt = CFSTR("{ desc='%@' text='%@' }");
    CFStringRef desc, text, rv;

    if(!f)
        return NULL;

    desc = ST_ID3v2_UserTextFrame_copyDesc(f);
    text = ST_ID3v2_UserTextFrame_copyText(f);
    rv = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, fmt, desc, text);
    CFRelease(text);
    CFRelease(desc);

    return rv;
}
#endif
