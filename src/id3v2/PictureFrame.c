/*
    SonatinaTag
    Copyright (C) 2011, 2015 Lawrence Sebald

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

static void free_picture(ST_PictureFrame *f) {
    ST_Picture_free(f->picture);
    free(f);
}

ST_FUNC ST_PictureFrame *ST_ID3v2_PictureFrame_create(ST_Picture *p) {
    ST_PictureFrame *rv;

    if((rv = (ST_PictureFrame *)malloc(sizeof(ST_PictureFrame)))) {
        rv->base.type = ST_FrameType_Picture;
        rv->base.dtor = (void (*)(ST_Frame *))free_picture;
        rv->picture = p;
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

ST_LOCAL ST_PictureFrame *ST_ID3v2_PictureFrame_create_buf(const uint8_t *buf,
                                                           uint32_t sz) {
    ST_PictureFrame *rv = NULL;
    ST_Picture *p;
    uint8_t *tmp;
    int mime_len, desc_len, enc_len, img_len;
    ST_TextEncoding e;
    ST_PictureType t;

    /* Sanity checks... */
    if(sz < 4 || buf[0] > (uint8_t)ST_TextEncoding_UTF8)
        return NULL;

    e = (ST_TextEncoding)buf[0];
    mime_len = blen_utf8(buf + 1, sz - 1);

    if(mime_len < 0 || sz - mime_len < 3)
        return NULL;

    if(buf[mime_len + 2] > (uint8_t)ST_PictureType_MAX)
        return NULL;

    t = (ST_PictureType)buf[mime_len + 2];

    enc_len = ((buf[0] == (uint8_t)ST_TextEncoding_UTF16) ||
               (buf[0] == (uint8_t)ST_TextEncoding_UTF16BE)) ? 2 : 1;

    if(enc_len == 1)
        desc_len = blen_utf8(buf + 3 + mime_len, sz - mime_len - 3);
    else
        desc_len = blen_utf16(buf + 3 + mime_len, sz - mime_len - 3);

    if(desc_len < 0 || sz - mime_len - 3 - desc_len < enc_len)
        return NULL;

    /* Create the picture and read the data in. */
    if(!(p = ST_Picture_create()))
        return NULL;

    /* Deal with the MIME type */
    if(!(tmp = (uint8_t *)malloc(mime_len + 1)))
        goto err_pic;

    memcpy(tmp, buf + 1, mime_len);
    tmp[mime_len] = 0;

    if(ST_Picture_setMimeType(p, (const char *)tmp) != ST_Error_None)
        goto err_pic;

    free(tmp);

    /* Now, deal with the description */
    if(!(tmp = (uint8_t *)malloc(desc_len)))
        goto err_pic;

    memcpy(tmp, buf + 3 + mime_len, desc_len);

    if(ST_Picture_setDescription(p, tmp, desc_len, e) != ST_Error_None)
        goto err_pic;

    free(tmp);

    /* Finally, deal with the image itself */
    img_len = sz - mime_len - desc_len - enc_len - 3;

    if(!(tmp = (uint8_t *)malloc(img_len)))
        goto err_pic;

    memcpy(tmp, buf + 3 + mime_len + desc_len + enc_len, img_len);

    if(ST_Picture_setData(p, tmp, img_len, 1))
        goto err_pic;

    /* Last thing... Set the type */
    if(ST_Picture_setType(p, t) != ST_Error_None)
        goto err_pic;

    /* Finally, make the frame */
    if(!(rv = (ST_PictureFrame *)malloc(sizeof(ST_PictureFrame))))
        goto err_pic;

    rv->base.type = ST_FrameType_Picture;
    rv->base.dtor = (void (*)(ST_Frame *))free_picture;
    rv->picture = p;

    return rv;

err_pic:
    ST_Picture_free(p);
    return NULL;
}

/* Ugh. ID3v2.2 uses a different format for its PIC frames than do the later
   versions for their APIC frames. This parses the v2.2 version. The above
   function deals with the later versions. */
ST_LOCAL ST_PictureFrame *ST_ID3v2_PictureFrame_create_buf2(const uint8_t *buf,
                                                            uint32_t sz) {
    ST_PictureFrame *rv = NULL;
    ST_Picture *p;
    uint8_t *tmp;
    int desc_len, enc_len, img_len;
    ST_TextEncoding e;
    ST_PictureType t;

    /* Sanity checks... */
    if(sz < 4 || buf[0] > (uint8_t)ST_TextEncoding_UTF8)
        return NULL;

    e = (ST_TextEncoding)buf[0];

    if(buf[4] > (uint8_t)ST_PictureType_MAX)
        return NULL;

    t = (ST_PictureType)buf[4];

    enc_len = ((buf[0] == (uint8_t)ST_TextEncoding_UTF16) ||
               (buf[0] == (uint8_t)ST_TextEncoding_UTF16BE)) ? 2 : 1;

    if(enc_len == 1)
        desc_len = blen_utf8(buf + 5, sz - 5);
    else
        desc_len = blen_utf16(buf + 5, sz - 5);

    if(desc_len < 0 || sz - 5 < enc_len)
        return NULL;

    /* Create the picture and read the data in. */
    if(!(p = ST_Picture_create()))
        return NULL;

    /* Deal with the MIME type */
    if(!(tmp = (uint8_t *)malloc(4)))
        goto err_pic;

    tmp[0] = buf[1];
    tmp[1] = buf[2];
    tmp[2] = buf[3];
    tmp[3] = 0;

    if(ST_Picture_setMimeType(p, (const char *)tmp) != ST_Error_None)
        goto err_pic;

    free(tmp);

    /* Now, deal with the description */
    if(!(tmp = (uint8_t *)malloc(desc_len)))
        goto err_pic;

    memcpy(tmp, buf + 5, desc_len);

    if(ST_Picture_setDescription(p, tmp, desc_len, e) != ST_Error_None)
        goto err_pic;

    free(tmp);

    /* Finally, deal with the image itself */
    img_len = sz - desc_len - enc_len - 5;

    if(!(tmp = (uint8_t *)malloc(img_len)))
        goto err_pic;

    memcpy(tmp, buf + 5 + desc_len + enc_len, img_len);

    if(ST_Picture_setData(p, tmp, img_len, 1))
        goto err_pic;

    /* Last thing... Set the type */
    if(ST_Picture_setType(p, t) != ST_Error_None)
        goto err_pic;

    /* Finally, make the frame */
    if(!(rv = (ST_PictureFrame *)malloc(sizeof(ST_PictureFrame))))
        goto err_pic;

    rv->base.type = ST_FrameType_Picture;
    rv->base.dtor = (void (*)(ST_Frame *))free_picture;
    rv->picture = p;

    return rv;

err_pic:
    ST_Picture_free(p);
    return NULL;
}

ST_FUNC ST_Picture *ST_ID3v2_PictureFrame_picture(const ST_PictureFrame *f) {
    if(!f)
        return NULL;

    return f->picture;
}

ST_FUNC ST_Error ST_ID3v2_PictureFrame_setPicture(ST_PictureFrame *f,
                                                  ST_Picture *p) {
    if(!f || !p)
        return ST_Error_InvalidArgument;

    if(p == f->picture)
        return ST_Error_None;

    ST_Picture_free(f->picture);
    f->picture = p;

    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_ID3v2_PictureFrame_createString(const ST_PictureFrame *f)
{
    static const CFStringRef fmt = CFSTR("{ desc='%@' }");
    CFStringRef desc, rv;

    if(!f)
        return NULL;

    desc = ST_Picture_copyDesc(f->picture, NULL);
    rv = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, fmt, desc);
    CFRelease(desc);

    return rv;
}
#endif
