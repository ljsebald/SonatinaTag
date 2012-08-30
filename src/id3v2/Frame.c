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

#include "Frame.h"

ST_FUNC void ST_ID3v2_Frame_free(ST_Frame *f) {
    f->dtor(f);
}

ST_FUNC ST_ID3v2_FrameType ST_ID3v2_Frame_type(const ST_Frame *f) {
    if(!f)
        return ST_FrameType_Invalid;

    return f->type;
}

ST_FUNC uint16_t ST_ID3v2_Frame_flags(const ST_Frame *f) {
    if(!f)
        return (uint16_t)-1;

    return f->flags;
}

ST_FUNC ST_Error ST_ID3v2_Frame_setFlags(ST_Frame *f, uint16_t flags) {
    if(!f)
        return ST_Error_InvalidArgument;

    f->flags = flags;
    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_ID3v2_Frame_createString(const ST_Frame *f) {
    if(!f)
        return NULL;

    switch(f->type) {
        case ST_FrameType_Text:
            return ST_ID3v2_TextFrame_createString((const ST_TextFrame *)f);

        case ST_FrameType_UserText:
            return ST_ID3v2_UserTextFrame_createString((const ST_UserTextFrame *)f);

        case ST_FrameType_URL:
            return ST_ID3v2_URLFrame_createString((const ST_URLFrame *)f);

        case ST_FrameType_UserURL:
            return ST_ID3v2_UserURLFrame_createString((const ST_UserURLFrame *)f);

        case ST_FrameType_Comment:
            return ST_ID3v2_CommentFrame_createString((const ST_CommentFrame *)f);

        case ST_FrameType_Picture:
            return ST_ID3v2_PictureFrame_createString((const ST_PictureFrame *)f);
    }

    /* Don't know how to make a description for this one... */
    return NULL;
}
#endif
