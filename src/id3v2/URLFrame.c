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

static void free_url(ST_URLFrame *f) {
    free(f->url);
    free(f);
}

ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_create(uint32_t len,
                                              const uint8_t *str) {
    ST_URLFrame *rv;

    if((rv = (ST_URLFrame *)malloc(sizeof(ST_URLFrame)))) {
        rv->base.type = ST_FrameType_URL;
        rv->base.dtor = (void (*)(ST_Frame *))free_url;
        rv->size = len;

        if(!(rv->url = (uint8_t *)malloc(len))) {
            free(rv);
            return NULL;
        }

        memcpy(rv->url, str, len);
    }
    
    return rv;
}

ST_LOCAL ST_URLFrame *ST_ID3v2_URLFrame_create_buf(const uint8_t *buf,
                                                   uint32_t sz) {
    ST_URLFrame *rv;

    if((rv = (ST_URLFrame *)malloc(sizeof(ST_URLFrame)))) {
        rv->base.type = ST_FrameType_URL;
        rv->base.dtor = (void (*)(ST_Frame *))free_url;
        rv->size = sz;

        if(!(rv->url = (uint8_t *)malloc(sz))) {
            free(rv);
            return NULL;
        }

        memcpy(rv->url, buf, sz);
    }

    return rv;
}

ST_FUNC const uint8_t *ST_ID3v2_URLFrame_URL(const ST_URLFrame *f) {
    if(!f)
        return NULL;

    return f->url;
}

ST_FUNC uint32_t ST_ID3v2_URLFrame_length(const ST_URLFrame *f) {
    if(!f)
        return 0;

    return f->size;
}

ST_FUNC ST_Error ST_ID3v2_URLFrame_setURL(ST_URLFrame *f, uint32_t sz,
                                          uint8_t *d, int own_buf) {
    uint8_t *tmp = d;

    if(!f || !d || !sz)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)sz)))
            return ST_Error_errno;

        memcpy(tmp, d, sz);
    }

    free(f->url);
    f->url = tmp;
    f->size = sz;

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_createURL(CFURLRef url) {
    if(!url)
        return NULL;

    return ST_ID3v2_URLFrame_createStr(CFURLGetString(url));
}

ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_createStr(CFStringRef s) {
    ST_URLFrame *rv;
    uint8_t *buf;
    CFIndex slen;
    CFIndex l;
    void *tmp;

    if(!s)
        return NULL;

    /* Make space for it */
    slen = CFStringGetLength(s);
    l = CFStringGetMaximumSizeForEncoding(slen, kCFStringEncodingISOLatin1);

    if(!(buf = (uint8_t *)malloc(l)))
        return NULL;

    CFStringGetBytes(s, CFRangeMake(0, slen), kCFStringEncodingISOLatin1, 0,
                     false, buf, l, &slen);

    /* Attempt to make it smaller */
    if(slen != l) {
        if((tmp = realloc(buf, slen)))
            buf = (uint8_t *)tmp;
    }

    if((rv = (ST_URLFrame *)malloc(sizeof(ST_URLFrame)))) {
        rv->base.type = ST_FrameType_URL;
        rv->base.dtor = (void (*)(ST_Frame *))free_url;
        rv->size = slen;
        rv->url = buf;
    }
    else {
        free(buf);
    }

    return rv;
}

ST_FUNC CFURLRef ST_ID3v2_URLFrame_copyURL(const ST_URLFrame *f) {
    if(!f)
        return NULL;

    return CFURLCreateWithBytes(kCFAllocatorDefault, f->url, f->size,
                                kCFStringEncodingISOLatin1, NULL);
}

ST_FUNC CFStringRef ST_ID3v2_URLFrame_copyURLString(const ST_URLFrame *f) {
    if(!f)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, f->url, f->size,
                                   kCFStringEncodingISOLatin1, false);
}

ST_FUNC ST_Error ST_ID3v2_URLFrame_setCFURL(ST_URLFrame *f, CFURLRef url) {
    if(!f || !url)
        return ST_Error_InvalidArgument;

    return ST_ID3v2_URLFrame_setURLStr(f, CFURLGetString(url));
}

ST_FUNC ST_Error ST_ID3v2_URLFrame_setURLStr(ST_URLFrame *f, CFStringRef s) {
    CFIndex slen;
    CFIndex l;
    uint8_t *buf;
    void *tmp;

    if(!f || !s) 
        return ST_Error_InvalidArgument;

    /* Make space for it */
    slen = CFStringGetLength(s);
    l = CFStringGetMaximumSizeForEncoding(slen, kCFStringEncodingISOLatin1);

    if(!(buf = (uint8_t *)malloc(l)))
        return ST_Error_errno;

    CFStringGetBytes(s, CFRangeMake(0, slen), kCFStringEncodingISOLatin1, 0,
                     false, buf, l, &slen);

    /* Attempt to make it smaller */
    if(slen != l) {
        if((tmp = realloc(buf, slen)))
            buf = (uint8_t *)tmp;
    }

    free(f->url);
    f->size = slen;
    f->url = buf;

    return ST_Error_None;
}

ST_FUNC CFStringRef ST_ID3v2_URLFrame_createString(const ST_URLFrame *f) {
    static const CFStringRef fmt = CFSTR("{ url='%@' }");
    CFStringRef url, rv;

    if(!f)
        return NULL;

    url = ST_ID3v2_URLFrame_copyURLString(f);
    rv = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, fmt, url);
    CFRelease(url);

    return rv;
}
#endif
