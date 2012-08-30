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

static void free_userurl(ST_UserURLFrame *f) {
    free(f->desc);
    free(f->url);
    free(f);
}

ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_create(ST_TextEncoding enc,
                                                      uint32_t ul,
                                                      const uint8_t *url,
                                                      uint32_t dl,
                                                      const uint8_t *desc) {
    ST_UserURLFrame *rv;

    if(!url || !desc)
        return NULL;

    if(enc > ST_TextEncoding_UTF8 || enc <= ST_TextEncoding_Invalid)
        return NULL;

    if((rv = (ST_UserURLFrame *)malloc(sizeof(ST_UserURLFrame)))) {
        rv->base.type = ST_FrameType_UserURL;
        rv->base.dtor = (void (*)(ST_Frame *))free_userurl;
        rv->encoding = enc;
        rv->url_size = ul;

        if(!(rv->url = (uint8_t *)malloc(ul))) {
            free(rv);
            return NULL;
        }

        if(!(rv->desc = (uint8_t *)malloc(dl))) {
            free(rv->url);
            free(rv);
            return NULL;
        }

        memcpy(rv->url, url, ul);
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

ST_LOCAL ST_UserURLFrame *ST_ID3v2_UserURLFrame_create_buf(const uint8_t *buf,
                                                           uint32_t sz) {
    ST_UserURLFrame *rv;
    int enc_len;

    if(sz < 1 || buf[0] > (uint8_t)ST_TextEncoding_UTF8)
        return NULL;

    if((rv = (ST_UserURLFrame *)malloc(sizeof(ST_UserURLFrame)))) {
        rv->base.type = ST_FrameType_UserURL;
        rv->base.dtor = (void (*)(ST_Frame *))free_userurl;
        rv->encoding = (ST_TextEncoding)buf[0];

        enc_len = ((buf[0] == (uint8_t)ST_TextEncoding_UTF16) ||
                   (buf[0] == (uint8_t)ST_TextEncoding_UTF16BE)) ? 2 : 1;

        if(enc_len == 1)
            rv->desc_size = blen_utf8(buf + 1, sz - 1);
        else
            rv->desc_size = blen_utf16(buf + 1, sz - 1);

        rv->url_size = sz - rv->desc_size - 1 - enc_len;

        /* Sanity check... */
        if(rv->desc_size < 0 || rv->url_size < 0) {
            free(rv);
            return NULL;
        }

        if(!(rv->desc = (uint8_t *)malloc(rv->desc_size))) {
            free(rv);
            return NULL;
        }

        if(!(rv->url = (uint8_t *)malloc(rv->url_size))) {
            free(rv->desc);
            free(rv);
            return NULL;
        }

        memcpy(rv->desc, buf + 1, rv->desc_size);
        memcpy(rv->url, buf + 1 + rv->desc_size + enc_len, rv->url_size);
    }

    return rv;
}

ST_FUNC const uint8_t *ST_ID3v2_UserURLFrame_URL(const ST_UserURLFrame *f) {
    if(!f)
        return NULL;

    return f->url;
}

ST_FUNC uint32_t ST_ID3v2_UserURLFrame_URLLength(const ST_UserURLFrame *f) {
    if(!f)
        return 0;

    return f->url_size;
}

ST_FUNC const uint8_t *ST_ID3v2_UserURLFrame_desc(const ST_UserURLFrame *f) {
    if(!f)
        return NULL;

    return f->desc;
}

ST_FUNC uint32_t ST_ID3v2_UserURLFrame_descLength(const ST_UserURLFrame *f) {
    if(!f)
        return 0;

    return f->desc_size;
}

ST_FUNC ST_TextEncoding ST_ID3v2_UserURLFrame_descEncoding(const ST_UserURLFrame *f) {
    if(!f)
        return 0;

    return f->encoding;
}

ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setURL(ST_UserURLFrame *f, uint32_t ul,
                                              uint8_t *url, int own_buf) {
    uint8_t *tmp = url;

    if(!f || !url || !ul)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)ul)))
            return ST_Error_errno;

        memcpy(tmp, url, ul);
    }

    free(f->url);
    f->url = tmp;
    f->url_size = ul;

    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setDesc(ST_UserURLFrame *f,
                                               ST_TextEncoding enc,
                                               uint32_t desc_sz, uint8_t *desc,
                                               int own_buf) {
    uint8_t *tmp = desc;

    if(!f || !desc || !desc_sz || enc > ST_TextEncoding_UTF8 ||
       enc <= ST_TextEncoding_Invalid)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)desc_sz)))
            return ST_Error_errno;

        memcpy(tmp, desc, desc_sz);
    }

    free(f->desc);
    f->desc = tmp;
    f->desc_size = desc_sz;
    f->encoding = enc;

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
static CFStringEncoding encs[4] = {
    kCFStringEncodingISOLatin1,
    kCFStringEncodingUTF16,
    kCFStringEncodingUTF16BE,
    kCFStringEncodingUTF8
};

ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_createURL(CFURLRef url,
                                                         CFStringRef desc,
                                                         ST_TextEncoding e) {
    return ST_ID3v2_UserURLFrame_createStr(CFURLGetString(url), desc, e);
}

ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_createStr(CFStringRef url,
                                                         CFStringRef desc,
                                                         ST_TextEncoding e) {
    ST_UserURLFrame *rv;
    uint8_t *buf, *dbuf;
    CFIndex slen, dlen;
    CFIndex l;
    void *tmp;

    if(e > ST_TextEncoding_UTF8 || e <= ST_TextEncoding_Invalid || !url ||
       !desc)
        return NULL;

    /* Make space for the string */
    slen = CFStringGetLength(url);
    l = CFStringGetMaximumSizeForEncoding(slen, kCFStringEncodingISOLatin1);

    if(!(buf = (uint8_t *)malloc(l)))
        return NULL;

    CFStringGetBytes(url, CFRangeMake(0, slen), kCFStringEncodingISOLatin1, 0,
                     false, buf, l, &slen);

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

    CFStringGetBytes(desc, CFRangeMake(0, dlen), encs[e], 0,
                     e == ST_TextEncoding_UTF16, dbuf, l, &dlen);

    if(dlen != l) {
        if((tmp = realloc(dbuf, dlen)))
            dbuf = (uint8_t *)tmp;
    }

    if((rv = (ST_UserURLFrame *)malloc(sizeof(ST_UserURLFrame)))) {
        rv->base.type = ST_FrameType_UserURL;
        rv->base.dtor = (void (*)(ST_Frame *))free_userurl;
        rv->encoding = e;
        rv->url_size = slen;
        rv->url = buf;
        rv->desc_size = dlen;
        rv->desc = dbuf;
    }
    else {
        free(buf);
        free(dbuf);
    }

    return rv;
}

static ST_Error set_str(ST_UserURLFrame *f, CFStringRef s, uint8_t **ptr,
                        uint32_t *sz, CFStringEncoding e) {
    CFIndex slen;
    CFIndex l;
    uint8_t *buf;
    void *tmp;

    /* Make space for it */
    slen = CFStringGetLength(s);
    l = CFStringGetMaximumSizeForEncoding(slen, e);

    if(!(buf = (uint8_t *)malloc(l)))
        return ST_Error_errno;

    CFStringGetBytes(s, CFRangeMake(0, slen), e, 0, e == kCFStringEncodingUTF16,
                     buf, l, &slen);

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

ST_FUNC CFURLRef ST_ID3v2_UserURLFrame_copyURL(const ST_UserURLFrame *f) {
    if(!f)
        return NULL;

    return CFURLCreateWithBytes(kCFAllocatorDefault, f->url, f->url_size,
                                kCFStringEncodingISOLatin1, NULL);
}

ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_copyURLString(const ST_UserURLFrame *f) {
    if(!f)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, f->url, f->url_size,
                                   kCFStringEncodingISOLatin1, false);
}

ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_copyDesc(const ST_UserURLFrame *f) {
    if(!f)
        return NULL;
    
    return CFStringCreateWithBytes(kCFAllocatorDefault, f->desc, f->desc_size,
                                   encs[f->encoding],
                                   f->encoding == ST_TextEncoding_UTF16);
}

ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setCFURL(ST_UserURLFrame *f,
                                                CFURLRef url) {
    if(!f || !url)
        return ST_Error_InvalidArgument;

    return ST_ID3v2_UserURLFrame_setURLStr(f, CFURLGetString(url));
}

ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setURLStr(ST_UserURLFrame *f,
                                                 CFStringRef s) {
    return set_str(f, s, &f->url, &f->url_size, kCFStringEncodingISOLatin1);
}

ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setDescStr(ST_UserURLFrame *f,
                                                  CFStringRef s) {
    if(!f || !s) 
        return ST_Error_InvalidArgument;

    return set_str(f, s, &f->desc, &f->desc_size, encs[f->encoding]);
}

ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_createString(const ST_UserURLFrame *f)
{
    static const CFStringRef fmt = CFSTR("{ desc='%@' url='%@' }");
    CFStringRef desc, url, rv;

    if(!f)
        return NULL;

    desc = ST_ID3v2_UserURLFrame_copyDesc(f);
    url = ST_ID3v2_UserURLFrame_copyURLString(f);
    rv = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, fmt, desc, url);
    CFRelease(url);
    CFRelease(desc);

    return rv;
}

#endif
