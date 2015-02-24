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

#include <string.h>

#include "SonatinaTag/SonatinaTag.h"
#include "Tag.h"

#include "SonatinaTag/Tags/ID3v1.h"
#include "SonatinaTag/Tags/ID3v2.h"
#include "SonatinaTag/Tags/FLAC.h"
#include "SonatinaTag/Tags/M4A.h"
#include "SonatinaTag/Tags/APE.h"

ST_FUNC ST_TagType ST_Tag_type(const ST_Tag *tag) {
    if(!tag)
        return ST_TagType_Invalid;

    return tag->type;
}

ST_FUNC ST_Tag *ST_Tag_createFromFile(const char *fn) {
    char *ext = strrchr(fn, '.');
    ST_Tag *rv;

    if(!ext)
        return NULL;

    /* Prefer ID3v2 over APEv2 and ID3v1 for MP3 files */
    if(!strcasecmp(ext, ".mp3")) {
        if((rv = (ST_Tag *)ST_ID3v2_createFromFile(fn))) {
            return rv;
        }

        if((rv = (ST_Tag *)ST_APE_createFromFile(fn))) {
            return rv;
        }

        return (ST_Tag *)ST_ID3v1_createFromFile(fn);
    }
    else if(!strcasecmp(ext, ".m4a") || !strcasecmp(ext, ".mp4") ||
            !strcasecmp(ext, ".m4p")) {
        return (ST_Tag *)ST_M4A_createFromFile(fn);
    }
    else if(!strcasecmp(ext, ".flac") || !strcasecmp(ext, ".fla")) {
        return (ST_Tag *)ST_FLAC_createFromFile(fn);
    }

    return NULL;
}

ST_FUNC void ST_Tag_free(ST_Tag *tag) {
    if(!tag)
        return;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_free((ST_ID3v1 *)tag);

        case ST_TagType_ID3v2:
            return ST_ID3v2_free((ST_ID3v2 *)tag);

        case ST_TagType_FLAC:
            return ST_FLAC_free((ST_FLAC *)tag);

        case ST_TagType_M4A:
            return ST_M4A_free((ST_M4A *)tag);

        case ST_TagType_APE:
            return ST_APE_free((ST_APE *)tag);

        default:
            return;
    }
}

ST_FUNC int ST_Tag_track(const ST_Tag *tag) {
    if(!tag)
        return -1;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_track((const ST_ID3v1 *)tag);

        case ST_TagType_ID3v2:
            return ST_ID3v2_track((const ST_ID3v2 *)tag);

        case ST_TagType_FLAC:
            return ST_FLAC_track((const ST_FLAC *)tag);

        case ST_TagType_M4A:
            return ST_M4A_track((const ST_M4A *)tag);

        case ST_TagType_APE:
            return ST_APE_track((const ST_APE *)tag);

        default:
            return -1;
    }
}

ST_FUNC int ST_Tag_disc(const ST_Tag *tag) {
    if(!tag)
        return -1;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            /* ID3v1 doesn't support a disc number attribute. */
            return 0;

        case ST_TagType_ID3v2:
            return ST_ID3v2_disc((const ST_ID3v2 *)tag);

        case ST_TagType_FLAC:
            return ST_FLAC_disc((const ST_FLAC *)tag);

        case ST_TagType_M4A:
            return ST_M4A_disc((const ST_M4A *)tag);

        case ST_TagType_APE:
            return ST_APE_disc((const ST_APE *)tag);

        default:
            return -1;
    }
}

ST_FUNC const ST_Picture *ST_Tag_picture(const ST_Tag *tag, ST_PictureType pt,
                                         int index) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            /* ID3v1 doesn't support pictures. */
            return NULL;

        case ST_TagType_ID3v2:
            return ST_ID3v2_picture((const ST_ID3v2 *)tag, pt, index);

        case ST_TagType_FLAC:
            return ST_FLAC_picture((const ST_FLAC *)tag, pt, index);

        case ST_TagType_M4A:
            /* The picture type is ignored for M4A... */
            return ST_M4A_picture((const ST_M4A *)tag, index);

        case ST_TagType_APE:
            /* We don't support pictures in APE tags yet... */
            return NULL;

        default:
            return NULL;
    }
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_Tag_copyTitle(const ST_Tag *tag, ST_Error *err) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyTitle((const ST_ID3v1 *)tag, err);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyTitle((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyTitle((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyTitle((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyTitle((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFStringRef ST_Tag_copyArtist(const ST_Tag *tag, ST_Error *err) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyArtist((const ST_ID3v1 *)tag, err);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyArtist((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyArtist((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyArtist((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyArtist((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFStringRef ST_Tag_copyAlbum(const ST_Tag *tag, ST_Error *err) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyAlbum((const ST_ID3v1 *)tag, err);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyAlbum((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyAlbum((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyAlbum((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyAlbum((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFStringRef ST_Tag_copyComment(const ST_Tag *tag, ST_Error *err) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyComment((const ST_ID3v1 *)tag, err);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyComment((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyComment((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyComment((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyComment((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFStringRef ST_Tag_copyDate(const ST_Tag *tag, ST_Error *err) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyYear((const ST_ID3v1 *)tag, err);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyDate((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyDate((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyDate((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyDate((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFStringRef ST_Tag_copyGenre(const ST_Tag *tag, ST_Error *err) {
    ST_ID3v1_GenreCode g;

    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            g = ST_ID3v1_genre((const ST_ID3v1 *)tag);
            return ST_ID3v1_createStringForGenre(g);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyGenre((const ST_ID3v2 *)tag, err);

        case ST_TagType_FLAC:
            return ST_FLAC_copyGenre((const ST_FLAC *)tag, err);

        case ST_TagType_M4A:
            return ST_M4A_copyGenre((const ST_M4A *)tag, err);

        case ST_TagType_APE:
            return ST_APE_copyGenre((const ST_APE *)tag, err);

        default:
            return NULL;
    }
}

ST_FUNC CFDictionaryRef ST_Tag_copyDictionary(const ST_Tag *tag) {
    if(!tag)
        return NULL;

    switch(tag->type) {
        case ST_TagType_ID3v1:
            return ST_ID3v1_copyDictionary((const ST_ID3v1 *)tag);

        case ST_TagType_ID3v2:
            return ST_ID3v2_copyDictionary((const ST_ID3v2 *)tag);

        case ST_TagType_FLAC:
            return ST_FLAC_copyDictionary((const ST_FLAC *)tag);

        case ST_TagType_M4A:
            return ST_M4A_copyDictionary((const ST_M4A *)tag);

        case ST_TagType_APE:
            return ST_APE_copyDictionary((const ST_APE *)tag);

        default:
            return NULL;
    }
}
#endif
