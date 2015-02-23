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

#ifndef ST_INTERNAL__id3v2__Frame_h
#define ST_INTERNAL__id3v2__Frame_h

#include "SonatinaTag/cdefs.h"

ST_BEGIN_DECLS

#include "SonatinaTag/Tags/ID3v2Frame.h"

struct ST_Frame_struct {
    uint8_t type;
    uint16_t flags;
    void (*dtor)(ST_Frame *);
};

struct ST_TextFrame_struct {
    ST_Frame base;
    ST_TextEncoding encoding;
    size_t size;
    uint8_t *string;
};

struct ST_UserTextFrame_struct {
    ST_Frame base;
    ST_TextEncoding encoding;
    uint32_t desc_size;
    uint8_t *desc;
    uint32_t string_size;
    uint8_t *string;
};

struct ST_URLFrame_struct {
    ST_Frame base;
    uint32_t size;
    uint8_t *url;
};

struct ST_UserURLFrame_struct {
    ST_Frame base;
    ST_TextEncoding encoding;
    uint32_t desc_size;
    uint8_t *desc;
    uint32_t url_size;
    uint8_t *url;
};

struct ST_CommentFrame_struct {
    ST_Frame base;
    ST_TextEncoding encoding;
    char language[4];
    uint32_t desc_size;
    uint8_t *desc;
    uint32_t string_size;
    uint8_t *string;
};

struct ST_PictureFrame_struct {
    ST_Frame base;
    ST_Picture *picture;
};

struct ST_GenericFrame_struct {
    ST_Frame base;
    uint32_t size;
    uint8_t *data;
};

/* Internal use only functions! */
ST_LOCAL ST_TextFrame *ST_ID3v2_TextFrame_create_buf(const uint8_t *buf,
                                                     uint32_t sz);
ST_LOCAL ST_UserTextFrame *ST_ID3v2_UserTextFrame_create_buf(const uint8_t *buf,
                                                             uint32_t sz);
ST_LOCAL ST_URLFrame *ST_ID3v2_URLFrame_create_buf(const uint8_t *buf,
                                                   uint32_t sz);
ST_LOCAL ST_UserURLFrame *ST_ID3v2_UserURLFrame_create_buf(const uint8_t *buf,
                                                           uint32_t sz);
ST_LOCAL ST_CommentFrame *ST_ID3v2_CommentFrame_create_buf(const uint8_t *buf,
                                                           uint32_t sz);
ST_LOCAL ST_PictureFrame *ST_ID3v2_PictureFrame_create_buf(const uint8_t *buf,
                                                           uint32_t sz);
ST_LOCAL ST_PictureFrame *ST_ID3v2_PictureFrame_create_buf2(const uint8_t *buf,
                                                            uint32_t sz);

ST_END_DECLS

#endif /* !ST_INTERNAL__id3v2__Frame_h */
