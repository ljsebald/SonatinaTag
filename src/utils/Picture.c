/*
    SonatinaTag
    Copyright (C) 2011 Lawrence Sebald

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
#include <stdint.h>

#include "SonatinaTag/Picture.h"

struct ST_Picture_struct {
    ST_PictureType picture_type;
    char *mime_type;
    uint8_t *description;
    uint32_t desc_length;
    ST_TextEncoding desc_enc;
    uint32_t width;
    uint32_t height;
    uint32_t bit_depth;
    uint32_t index_used;
    uint8_t *data;
    uint32_t data_len;
};

ST_FUNC ST_Picture *ST_Picture_create(void) {
    ST_Picture *rv = (ST_Picture *)malloc(sizeof(ST_Picture));

    if(rv) {
        memset(rv, 0, sizeof(ST_Picture));
    }

    return rv;
}

ST_FUNC void ST_Picture_free(ST_Picture *p) {
    if(!p)
        return;

    free(p->mime_type);
    free(p->description);
    free(p->data);
    free(p);
}

ST_FUNC uint32_t ST_Picture_width(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->width;
}

ST_FUNC uint32_t ST_Picture_height(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->height;
}

ST_FUNC uint32_t ST_Picture_bitDepth(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->bit_depth;
}

ST_FUNC uint32_t ST_Picture_indexUsed(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->index_used;
}

ST_FUNC uint32_t ST_Picture_dataLength(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->data_len;
}

ST_FUNC ST_PictureType ST_Picture_type(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->picture_type;
}

ST_FUNC const uint8_t *ST_Picture_data(const ST_Picture *p) {
    if(!p)
        return NULL;

    return p->data;
}

ST_FUNC const char *ST_Picture_mimeType(const ST_Picture *p) {
    if(!p)
        return NULL;

    return p->mime_type;
}

ST_FUNC const uint8_t *ST_Picture_description(const ST_Picture *p) {
    if(!p)
        return NULL;

    return p->description;
}

ST_FUNC uint32_t ST_Picture_descriptionLength(const ST_Picture *p) {
    if(!p)
        return (uint32_t)-1;

    return p->desc_length;
}

ST_FUNC ST_TextEncoding ST_Picture_descriptionEncoding(const ST_Picture *p) {
    if(!p)
        return ST_TextEncoding_Invalid;

    return p->desc_enc;
}

ST_FUNC ST_Error ST_Picture_setWidth(ST_Picture *p, uint32_t w) {
    if(!p)
        return ST_Error_InvalidArgument;

    p->width = w;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setHeight(ST_Picture *p, uint32_t h) {
    if(!p)
        return ST_Error_InvalidArgument;

    p->height = h;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setBitDepth(ST_Picture *p, uint32_t b) {
    if(!p)
        return ST_Error_InvalidArgument;

    p->bit_depth = b;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setIndexUsed(ST_Picture *p, uint32_t u) {
    if(!p)
        return ST_Error_InvalidArgument;

    p->index_used = u;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setType(ST_Picture *p, ST_PictureType t) {
    if(!p)
        return ST_Error_InvalidArgument;

    p->picture_type = t;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setMimeType(ST_Picture *p, const char *mt) {
    char *tmp = NULL;

    if(!p)
        return ST_Error_InvalidArgument;

    if(mt) {
        if(!(tmp = strdup(mt))) {
            return ST_Error_errno;
        }
    }

    free(p->mime_type);
    p->mime_type = tmp;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setDescription(ST_Picture *p, const uint8_t *d,
                                           uint32_t desc_len,
                                           ST_TextEncoding enc) {
    uint8_t *tmp = NULL;

    if(!p || enc <= ST_TextEncoding_Invalid || enc > ST_TextEncoding_UTF8)
        return ST_Error_InvalidArgument;

    if(d) {
        if(!(tmp = malloc(desc_len))) {
            return ST_Error_errno;
        }

        memcpy(tmp, d, desc_len);
    }

    free(p->description);
    p->description = tmp;
    p->desc_length = desc_len;
    p->desc_enc = enc;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Picture_setData(ST_Picture *p, uint8_t *d, uint32_t len,
                                    int own_buf) {
    uint8_t *tmp;

    if(!p || !d || !len)
        return ST_Error_InvalidArgument;

    if(own_buf) {
        free(p->data);
        p->data = d;
        p->data_len = len;
    }
    else {
        if(!(tmp = (uint8_t *)malloc(len))) {
            return ST_Error_errno;
        }

        memcpy(tmp, d, len);
        free(p->data);
        p->data = tmp;
        p->data_len = len;
    }

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_Picture_copyDesc(const ST_Picture *p, ST_Error *err) {
    CFStringEncoding enc;
    Boolean ext_rep = false;

    if(!p) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    switch(p->desc_enc) {
        case ST_TextEncoding_ISO8859_1:
            enc = kCFStringEncodingISOLatin1;
            break;

        case ST_TextEncoding_UTF16:
            enc = kCFStringEncodingUTF16;
            ext_rep = true;
            break;

        case ST_TextEncoding_UTF16BE:
            enc = kCFStringEncodingUTF16BE;
            break;

        case ST_TextEncoding_UTF8:
            enc = kCFStringEncodingUTF8;
            break;

        default:
            if(err)
                *err = ST_Error_InvalidEncoding;
            return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!p->description)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, p->description,
                                   p->desc_length, enc, ext_rep);
}

ST_FUNC ST_Error ST_Picture_setDescStr(ST_Picture *p, CFStringRef s) {
    CFIndex len;
    char *tmp, *tmp2;

    if(!p)
        return ST_Error_InvalidArgument;

    if(!s) {
        free(p->description);
        p->description = NULL;
        p->desc_length = 0;
        return ST_Error_None;
    }

    /* Get the length and allocate space */
    len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(s),
                                            kCFStringEncodingUTF8);
    if(!(tmp = (char *)malloc(len + 1)))
        return ST_Error_errno;

    /* Copy data in */
    if(!CFStringGetCString(s, tmp, len + 1, kCFStringEncodingUTF8)) {
        free(tmp);
        return ST_Error_InvalidEncoding;
    }

    if(!(tmp2 = strdup(tmp))) {
        free(tmp);
        return ST_Error_errno;
    }

    /* Set up the data in the picture */
    p->description = (uint8_t *)tmp2;
    p->desc_length = strlen(tmp2);
    p->desc_enc = ST_TextEncoding_UTF8;
    free(tmp);

    return ST_Error_None;
}
#endif
