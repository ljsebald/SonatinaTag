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

#ifndef SonatinaTag__Picture_h
#define SonatinaTag__Picture_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <stdint.h>
#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Error.h>

typedef enum ST_PictureType_e {
    ST_PictureType_Other                = 0,
    ST_PictureType_SmallIcon            = 1,
    ST_PictureType_OtherIcon            = 2,
    ST_PictureType_FrontCover           = 3,
    ST_PictureType_BackCover            = 4,
    ST_PictureType_Leaflet              = 5,
    ST_PictureType_Media                = 6,
    ST_PictureType_LeadArtist           = 7,
    ST_PictureType_Artist               = 8,
    ST_PictureType_Conductor            = 9,
    ST_PictureType_Band                 = 10,
    ST_PictureType_Composer             = 11,
    ST_PictureType_Lyricist             = 12,
    ST_PictureType_RecordingLocation    = 13,
    ST_PictureType_DuringRecording      = 14,
    ST_PictureType_DuringPerformance    = 15,
    ST_PictureType_ScreenCapture        = 16,
    ST_PictureType_ABrightColoredFish   = 17,
    ST_PictureType_Illustration         = 18,
    ST_PictureType_ArtistLogo           = 19,
    ST_PictureType_PublisherLogo        = 20,
    ST_PictureType_MAX                  = 20,
    ST_PictureType_Any                  = 21
} ST_PictureType;

/* Opaque Picture structure */
struct ST_Picture_struct;
typedef struct ST_Picture_struct ST_Picture;

/* Create a new, initially blank picture */
ST_FUNC ST_Picture *ST_Picture_create(void);

/* Free a picture */
ST_FUNC void ST_Picture_free(ST_Picture *p);

/* Accessors */
ST_FUNC uint32_t ST_Picture_width(const ST_Picture *p);
ST_FUNC uint32_t ST_Picture_height(const ST_Picture *p);
ST_FUNC uint32_t ST_Picture_bitDepth(const ST_Picture *p);
ST_FUNC uint32_t ST_Picture_indexUsed(const ST_Picture *p);
ST_FUNC uint32_t ST_Picture_dataLength(const ST_Picture *p);
ST_FUNC ST_PictureType ST_Picture_type(const ST_Picture *p);
ST_FUNC const uint8_t *ST_Picture_data(const ST_Picture *p);
ST_FUNC const char *ST_Picture_mimeType(const ST_Picture *p);
ST_FUNC const uint8_t *ST_Picture_description(const ST_Picture *p);
ST_FUNC uint32_t ST_Picture_descriptionLength(const ST_Picture *p);
ST_FUNC ST_TextEncoding ST_Picture_descriptionEncoding(const ST_Picture *p);


/* Mutators */
ST_FUNC ST_Error ST_Picture_setWidth(ST_Picture *p, uint32_t w);
ST_FUNC ST_Error ST_Picture_setHeight(ST_Picture *p, uint32_t h);
ST_FUNC ST_Error ST_Picture_setBitDepth(ST_Picture *p, uint32_t b);
ST_FUNC ST_Error ST_Picture_setIndexUsed(ST_Picture *p, uint32_t u);
ST_FUNC ST_Error ST_Picture_setType(ST_Picture *p, ST_PictureType t);
ST_FUNC ST_Error ST_Picture_setMimeType(ST_Picture *p, const char *mt);
ST_FUNC ST_Error ST_Picture_setDescription(ST_Picture *p, const uint8_t *d,
                                           uint32_t desc_len,
                                           ST_TextEncoding enc);
ST_FUNC ST_Error ST_Picture_setData(ST_Picture *p, uint8_t *d, uint32_t len,
                                    int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_Picture_copyDesc(const ST_Picture *p, ST_Error *err);
ST_FUNC ST_Error ST_Picture_setDescStr(ST_Picture *p, CFStringRef s);
#endif

ST_END_DECLS

#endif /* !SonatinaTag__Picture_h */
