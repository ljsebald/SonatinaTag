/*
    SonatinaTag
    Copyright (C) 2010, 2011, 2012 Lawrence Sebald

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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "SonatinaTag/SonatinaTag.h"
#include "SonatinaTag/Tags/ID3v1.h"
#include "../base/Tag.h"

struct ST_ID3v1_struct {
    ST_Tag base;
    char *title;
    char *artist;
    char *album;
    char *year;
    char *comment;
    uint8_t genre;
    uint8_t track;
};

/* Raw (file) representation of the tag */
struct ID3v1_Tag {
    char magic[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    union {
        char comment[30];
        struct {
            char comment[28];
            uint8_t zero;
            uint8_t track;
        } v1_1;
    } comment_field;
    uint8_t genre;
} __attribute__((packed));

/* List of Genres -- English only */
static const char *id3_genres[ID3v1GenreMax + 1] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
    "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other",
    "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",
    "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
    "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz-Funk", "Fusion",
    "Trance", "Classical", "Instrumental", "Acid", "House", "Game",
    "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass", "Soul",
    "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
    "Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic",
    "Pop-Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult",
    "Gangsta", "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
    "Native American", "Cabaret", "New Wave", "Psychadelic", "Rave",
    "Showtunes", "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz",
    "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",

    /* The rest of these were Winamp extensions */
    "Folk", "Folk-Rock", "National Folk", "Swing", "Fast Fusion", "Bebob",
    "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
    "Gothic Rock", "Progressive Rock", "Psychadelic Rock", "Symphonic Rock",
    "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",
    "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata",
    "Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire",
    "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
    "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet", "Punk Rock",
    "Drum Solo", "Acapella", "Euro-House", "Dance Hall", "Goa",
    "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie", "BritPop",
    "Negerpunk", "Polsk Punk", "Beat", "Christian Gangsta", "Heavy Metal",
    "Black Metal", "Crossover", "Contemporary", "Christian Rock",
    "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop", "SynthPop"
};

ST_FUNC ST_ID3v1 *ST_ID3v1_create(void) {
    ST_ID3v1 *rv = (ST_ID3v1 *)malloc(sizeof(ST_ID3v1));

    if(rv) {
        memset(rv, 0, sizeof(ST_ID3v1));
        rv->base.type = ST_TagType_ID3v1;
    }

    return rv;
}

ST_FUNC void ST_ID3v1_free(ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return;

    free(tag->title);
    free(tag->artist);
    free(tag->album);
    free(tag->year);
    free(tag->comment);
    free(tag);
}

ST_FUNC ST_ID3v1 *ST_ID3v1_createFromFile(const char *fn) {
    ST_ID3v1 *rv = ST_ID3v1_create();
    struct ID3v1_Tag tag;
    FILE *fp;
    char tmp[31];

    if(!rv)
        return NULL;

    /* Attempt to open the specified file */
    if(!(fp = fopen(fn, "rb")))
       goto out_rel;

    /* Go to the position where the ID3v1 should be in the file. */
    if(fseek(fp, -128, SEEK_END))
        goto out_close;

    /* Read in the whole tag area for checking */
    if(fread(&tag, 1, 128, fp) != 128)
        goto out_close;

    fclose(fp);

    /* Look for the magic value */
    if(tag.magic[0] != 'T' || tag.magic[1] != 'A' || tag.magic[2] != 'G')
        goto out_rel;

    /* Copy out each part of the tag. This is a bit of a dance just because of
       the fact that the ID3v1 fields may not be NUL terminated. */
    tmp[30] = 0;
    memcpy(tmp, tag.title, 30);
    if(!(rv->title = strdup(tmp)))
       goto out_rel;

    memcpy(tmp, tag.artist, 30);
    if(!(rv->artist = strdup(tmp)))
        goto out_rel;

    memcpy(tmp, tag.album, 30);
    if(!(rv->album = strdup(tmp)))
        goto out_rel;

    memcpy(tmp, tag.comment_field.comment, 30);
    if(!(rv->comment = strdup(tmp)))
        goto out_rel;

    tmp[4] = 0;
    memcpy(tmp, tag.year, 4);
    if(!(rv->year = strdup(tmp)))
        goto out_rel;

    rv->genre = tag.genre;

    /* Deal with v1.1 tags. */
    if(tag.comment_field.v1_1.zero == 0)
        rv->track = tag.comment_field.v1_1.track;

    return rv;

out_close:
    fclose(fp);
out_rel:
    ST_ID3v1_free(rv);

    return NULL;
}

#define COPY_IF_NOT_NULL(to, from, cnt) \
    if(from) strncpy(to, from, cnt)

#define COPY_IF_NOT_NULL_OR_ZERO(to, from, cnt) { \
    if(from) strncpy(to, from, cnt); \
    else     memset(to, 0, cnt); \
}

ST_FUNC ST_Error ST_ID3v1_writeToFile(const ST_ID3v1 *t, const char *fn) {
    FILE *fp;
    struct ID3v1_Tag tag;
    ST_Error rv = ST_Error_None;

    /* Verify the tag and filename are not NULL */
    if(!t || !fn || t->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    /* Open up the file for reading and writing. */
    fp = fopen(fn, "r+b");
    if(!fp) {
        rv = ST_Error_errno;
        goto out;
    }

    /* Go to the position where the ID3v1 should be in the file. */
    if(fseek(fp, -128, SEEK_END)) {
        rv = ST_Error_errno;
        goto out_close;
    }

    memset(&tag, 0, sizeof(tag));

    /* Read in what should be the magic header, if its there */
    if(fread(tag.magic, 1, 3, fp) != 3) {
        rv = ST_Error_errno;
        goto out_close;
    }

    if(tag.magic[0] != 'T' || tag.magic[1] != 'A' || tag.magic[2] != 'G') {
        /* No magic value, so we assume the tag isn't actually present. */
        if(fseek(fp, 0, SEEK_END)) {
            rv = ST_Error_errno;
            goto out_close;
        }

        tag.magic[0] = 'T';
        tag.magic[1] = 'A';
        tag.magic[2] = 'G';
    }
    else {
        /* We have a tag, reposition to the end so we overwrite the old tag. */
        if(fseek(fp, -128, SEEK_END)) {
            rv = ST_Error_errno;
            goto out_close;
        }
    }

    /* Fill in the tag. */
    COPY_IF_NOT_NULL(tag.title, t->title, 30);
    COPY_IF_NOT_NULL(tag.artist, t->artist, 30);
    COPY_IF_NOT_NULL(tag.album, t->album, 30);
    COPY_IF_NOT_NULL(tag.comment_field.comment, t->comment, 30);
    COPY_IF_NOT_NULL(tag.year, t->year, 4);
    tag.genre = t->genre;

    /* Fill in the track if we have one */
    if(t->track) {
        tag.comment_field.v1_1.zero = 0;
        tag.comment_field.v1_1.track = t->track;
    }

    /* Write it out to the file... */
    if(fwrite(&tag, 1, 128, fp) != 128) {
        rv = ST_Error_errno;
        goto out_close;
    }

out_close:
    fclose(fp);
out:
    return rv;
}

ST_FUNC ST_Error ST_ID3v1_title(const ST_ID3v1 *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    COPY_IF_NOT_NULL_OR_ZERO((char *)buf, tag->title, len);
    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v1_artist(const ST_ID3v1 *tag, uint8_t *buf,
                                 size_t len) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    COPY_IF_NOT_NULL_OR_ZERO((char *)buf, tag->artist, len);
    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v1_album(const ST_ID3v1 *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    COPY_IF_NOT_NULL_OR_ZERO((char *)buf, tag->album, len);
    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v1_comment(const ST_ID3v1 *tag, uint8_t *buf,
                                  size_t len) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    COPY_IF_NOT_NULL_OR_ZERO((char *)buf, tag->comment, len);
    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v1_year(const ST_ID3v1 *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    COPY_IF_NOT_NULL_OR_ZERO((char *)buf, tag->year, len);
    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_ID3v1_copyTitle(const ST_ID3v1 *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_ID3v1) {
        if(err)
            *err = ST_Error_InvalidArgument;
            
        return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!tag->title)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, tag->title,
                                     kCFStringEncodingISOLatin1);
}

ST_FUNC CFStringRef ST_ID3v1_copyArtist(const ST_ID3v1 *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_ID3v1) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!tag->artist)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, tag->artist,
                                     kCFStringEncodingISOLatin1);
}

ST_FUNC CFStringRef ST_ID3v1_copyAlbum(const ST_ID3v1 *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_ID3v1) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!tag->album)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, tag->album,
                                     kCFStringEncodingISOLatin1);
}

ST_FUNC CFStringRef ST_ID3v1_copyComment(const ST_ID3v1 *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_ID3v1) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!tag->comment)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, tag->comment,
                                     kCFStringEncodingISOLatin1);
}

ST_FUNC CFStringRef ST_ID3v1_copyYear(const ST_ID3v1 *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_ID3v1) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if(err)
        *err = ST_Error_None;

    if(!tag->year)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, tag->year,
                                     kCFStringEncodingISOLatin1);
}
#endif

ST_FUNC size_t ST_ID3v1_titleLength(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return (size_t)-1;
    else if(!tag->title)
        return 0;

    return strlen(tag->title);
}

ST_FUNC size_t ST_ID3v1_artistLength(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return (size_t)-1;
    else if(!tag->artist)
        return 0;

    return strlen(tag->artist);
}

ST_FUNC size_t ST_ID3v1_albumLength(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return (size_t)-1;
    else if(!tag->album)
        return 0;

    return strlen(tag->album);
}

ST_FUNC size_t ST_ID3v1_commentLength(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return (size_t)-1;
    else if(!tag->comment)
        return 0;

    return strlen(tag->comment);
}

ST_FUNC size_t ST_ID3v1_yearLength(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return (size_t)-1;
    else if(!tag->year)
        return 0;

    return strlen(tag->year);
}

ST_FUNC ST_ID3v1_GenreCode ST_ID3v1_genre(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ID3v1GenreError;

    return (ST_ID3v1_GenreCode)tag->genre;
}

ST_FUNC int ST_ID3v1_track(const ST_ID3v1 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return -1;

    return (int)tag->track;
}

static ST_Error setString(ST_ID3v1 *tag, char **ptr, const uint8_t *v,
                          size_t len, ST_TextEncoding e) {
    char *prev = *ptr;

    /* Make sure they aren't doing anything screwy */
    if(!tag || (!v && len) || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;
    else if(e != ST_TextEncoding_ISO8859_1)
        return ST_Error_InvalidEncoding;
    else if(len > 30)
        return ST_Error_InvalidArgument;

    if(!v) {
        free(prev);
        *ptr = NULL;
        return ST_Error_None;
    }

    if((*ptr = (char *)malloc(len + 1))) {
        memcpy(*ptr, v, len);
        (*ptr)[len] = 0;
        free(prev);
        return ST_Error_None;
    }

    *ptr = prev;
    return ST_Error_errno;
}

ST_FUNC ST_Error ST_ID3v1_setTitle(ST_ID3v1 *tag, const uint8_t *v,
                                   size_t len, ST_TextEncoding e) {
    return setString(tag, &tag->title, v, len, e);
}

ST_FUNC ST_Error ST_ID3v1_setArtist(ST_ID3v1 *tag, const uint8_t *v,
                                    size_t len, ST_TextEncoding e) {
    return setString(tag, &tag->artist, v, len, e);
}

ST_FUNC ST_Error ST_ID3v1_setAlbum(ST_ID3v1 *tag, const uint8_t *v,
                                   size_t len, ST_TextEncoding e) {
    return setString(tag, &tag->album, v, len, e);
}

ST_FUNC ST_Error ST_ID3v1_setComment(ST_ID3v1 *tag, const uint8_t *v,
                                     size_t len, ST_TextEncoding e) {
    return setString(tag, &tag->comment, v, len, e);
}

/* Just in case someone tries to use this on a braindead system... */
static inline int is_8859digit(uint8_t c) {
    return (c >= 0x30 && c <= 0x39);
}

ST_FUNC ST_Error ST_ID3v1_setYear(ST_ID3v1 *tag, const uint8_t *v,
                                  size_t len, ST_TextEncoding e) {
    if(!tag || (!v && len) || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;
    else if(e != ST_TextEncoding_ISO8859_1)
        return ST_Error_InvalidEncoding;
    else if(v && len == 4 && (!is_8859digit(v[0]) || !is_8859digit(v[1]) ||
                              !is_8859digit(v[2]) || !is_8859digit(v[3])))
        return ST_Error_InvalidArgument;
    else if(v && len != 0)
        return ST_Error_InvalidArgument;

    return setString(tag, &tag->year, v, len, e);
}

ST_FUNC ST_Error ST_ID3v1_setGenre(ST_ID3v1 *tag, ST_ID3v1_GenreCode v) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    tag->genre = v;
    return ST_Error_None;
}

ST_FUNC ST_Error ST_ID3v1_setTrack(ST_ID3v1 *tag, int v) {
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;

    tag->track = (uint8_t)v;
    return ST_Error_None;
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_Error setCFString(ST_ID3v1 *tag, char **ptr, CFStringRef s) {
    char *prev = *ptr;
    const char *str;
    char tmp[31];

    /* Make sure they aren't doing anything screwy */
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;
    else if(CFStringGetLength(s) > 30)
        return ST_Error_InvalidArgument;

    if(!s) {
        free(prev);
        *ptr = NULL;
        return ST_Error_None;
    }

    /* Grab the ISO-8859-1 representation of the string */
    if(!(str = CFStringGetCStringPtr(s, kCFStringEncodingISOLatin1))) {
        if(!CFStringGetCString(s, tmp, 31, kCFStringEncodingISOLatin1))
            return ST_Error_InvalidEncoding;

        str = tmp;
    }

    /* One last sanity check... */
    if(strlen(str) > 30)
        return ST_Error_InvalidArgument;

    /* Copy it in */
    if((*ptr = strdup(str))) {
        free(prev);
        return ST_Error_None;
    }

    *ptr = prev;
    return ST_Error_errno;
}

ST_FUNC ST_Error ST_ID3v1_setTitleStr(ST_ID3v1 *tag, CFStringRef str) {
    return setCFString(tag, &tag->title, str);
}

ST_FUNC ST_Error ST_ID3v1_setArtistStr(ST_ID3v1 *tag, CFStringRef str) {
    return setCFString(tag, &tag->artist, str);
}

ST_FUNC ST_Error ST_ID3v1_setAlbumStr(ST_ID3v1 *tag, CFStringRef str) {
    return setCFString(tag, &tag->album, str);
}

ST_FUNC ST_Error ST_ID3v1_setCommentStr(ST_ID3v1 *tag, CFStringRef str) {
    return setCFString(tag, &tag->comment, str);
}

ST_FUNC ST_Error ST_ID3v1_setYearStr(ST_ID3v1 *tag, CFStringRef s) {
    char *prev = tag->year;
    const char *str;
    char tmp[5];

    /* Make sure they aren't doing anything screwy */
    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return ST_Error_InvalidArgument;
    else if(CFStringGetLength(s) != 4)
        return ST_Error_InvalidArgument;

    if(!s) {
        free(tag->year);
        tag->year = NULL;
        return ST_Error_None;
    }

    /* Grab the ISO-8859-1 representation of the string */
    if(!(str = CFStringGetCStringPtr(s, kCFStringEncodingISOLatin1))) {
        if(!CFStringGetCString(s, tmp, 5, kCFStringEncodingISOLatin1))
            return ST_Error_InvalidEncoding;

        str = tmp;
    }

    /* One last sanity check... */
    if(strlen(str) != 4 || (!is_8859digit(str[0]) || !is_8859digit(str[1]) ||
                            !is_8859digit(str[2]) || !is_8859digit(str[3])))
        return ST_Error_InvalidArgument;

    /* Copy it in */
    if((tag->year = strdup(str))) {
        free(prev);
        return ST_Error_None;
    }

    tag->year = prev;
    return ST_Error_errno;
}
#endif

ST_FUNC const char *ST_ID3v1_stringForGenre(ST_ID3v1_GenreCode genre) {
    if(genre < ID3v1GenreMin || genre > ID3v1GenreMax)
        return NULL;

    return id3_genres[(int)genre];
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_ID3v1_createStringForGenre(ST_ID3v1_GenreCode g) {
    if(g < ID3v1GenreMin || g > ID3v1GenreMax)
        return NULL;

    return CFStringCreateWithCString(kCFAllocatorDefault, id3_genres[(int)g],
                                     kCFStringEncodingISOLatin1);
}

ST_FUNC CFArrayRef ST_ID3v1_copyGenres(void) {
    CFMutableArrayRef rv = CFArrayCreateMutable(kCFAllocatorDefault,
                                                ID3v1GenreMax + 1,
                                                &kCFTypeArrayCallBacks);
    CFStringRef tmp;
    int i;

    if(rv) {
        for(i = 0; i <= ID3v1GenreMax; ++i) {
            tmp = CFStringCreateWithCString(kCFAllocatorDefault, id3_genres[i],
                                            kCFStringEncodingISOLatin1);
            if(tmp) {
                CFArrayAppendValue(rv, tmp);
                CFRelease(tmp);
            }
            else {
                CFRelease(rv);
                return NULL;
            }
        }
    }

    return (CFArrayRef)rv;
}

ST_FUNC CFDictionaryRef ST_ID3v1_copyDictionary(const ST_ID3v1 *tag) {
    static const CFStringRef tstr = CFSTR("title");
    static const CFStringRef arstr = CFSTR("artist");
    static const CFStringRef alstr = CFSTR("album");
    static const CFStringRef cstr = CFSTR("comment");
    static const CFStringRef ystr = CFSTR("year");
    static const CFStringRef gstr = CFSTR("genre");
    static const CFStringRef tkstr = CFSTR("track");
    static const CFStringRef tkfmt = CFSTR("%d");
    CFMutableDictionaryRef rv;
    CFStringRef tmp;

    if(!tag || tag->base.type != ST_TagType_ID3v1)
        return NULL;

    /* Create the dictionary and add all the strings to it. */
    if((rv = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks))) {
        if((tmp = ST_ID3v1_copyTitle(tag, NULL))) {
            CFDictionaryAddValue(rv, tstr, tmp);
            CFRelease(tmp);
        }

        if((tmp = ST_ID3v1_copyArtist(tag, NULL))) {
            CFDictionaryAddValue(rv, arstr, tmp);
            CFRelease(tmp);
        }

        if((tmp = ST_ID3v1_copyAlbum(tag, NULL))) {
            CFDictionaryAddValue(rv, alstr, tmp);
            CFRelease(tmp);
        }

        if((tmp = ST_ID3v1_copyComment(tag, NULL))) {
            CFDictionaryAddValue(rv, cstr, tmp);
            CFRelease(tmp);
        }

        if((tmp = ST_ID3v1_copyYear(tag, NULL))) {
            CFDictionaryAddValue(rv, ystr, tmp);
            CFRelease(tmp);
        }

        if((tmp = ST_ID3v1_createStringForGenre(tag->genre))) {
            CFDictionaryAddValue(rv, gstr, tmp);
            CFRelease(tmp);
        }

        if(tag->track && (tmp = CFStringCreateWithFormat(kCFAllocatorDefault,
                                                         NULL, tkfmt,
                                                         tag->track))) {
            CFDictionaryAddValue(rv, tkstr, tmp);
            CFRelease(tmp);
        }
    }

    return rv;
}
#endif
