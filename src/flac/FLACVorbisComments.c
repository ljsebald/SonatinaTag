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
#include "SonatinaTag/Tags/FLAC.h"
#include "../base/Tag.h"

struct ST_FLAC_struct {
    ST_Tag base;
    ST_Dict *vorbisComments;
    ST_Picture **pictures;
    int npictures;
};

struct ST_FLAC_vcomment_struct {
    size_t length;
    uint8_t *data;
};

/* FLAC metadata types that we're concerned with. */
#define METADATA_TYPE_VORBIS_COMMENT    4
#define METADATA_TYPE_PICTURE           6

#ifdef MIN
#undef MIN
#endif

#define MIN(x, y) ((x < y) ? x : y)

/* Forward declarations */
static int parse_comments(ST_FLAC *tag, uint8_t *buf, uint32_t length);
static int parse_picture(ST_FLAC *tag, uint8_t *bytes, uint32_t len);

static ST_FLAC_vcomment *make_comment(const uint8_t *buf, size_t length) {
    ST_FLAC_vcomment *rv;

    if((rv = (ST_FLAC_vcomment *)malloc(sizeof(ST_FLAC_vcomment)))) {
        if((rv->data = (uint8_t *)malloc(length))) {
            memcpy(rv->data, buf, length);
            rv->length = length;
            return rv;
        }

        free(rv);
    }

    return NULL;
}

static void free_comment(ST_FLAC_vcomment *c) {
    free(c->data);
    free(c);
}

ST_FUNC size_t ST_FLAC_vcomment_length(const ST_FLAC_vcomment *c) {
    if(!c)
        return (size_t)-1;

    return c->length;
}

ST_FUNC const uint8_t *ST_FLAC_vcomment_data(const ST_FLAC_vcomment *c) {
    if(!c)
        return NULL;

    return c->data;
}

ST_FUNC ST_FLAC *ST_FLAC_create(void) {
    ST_FLAC *rv = (ST_FLAC *)malloc(sizeof(ST_FLAC));
    void (*f)(void *);

    if(rv) {
        f = (void (*)(void *))free_comment;
        if(!(rv->vorbisComments = ST_Dict_createString(10, f))) {
            free(rv);
            return NULL;
        }

        rv->pictures = NULL;
        rv->npictures = 0;
        rv->base.type = ST_TagType_FLAC;
    }

    return rv;
}

ST_FUNC void ST_FLAC_free(ST_FLAC *tag) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return;

    /* Clean up the dictionaries. This will free all the values in them too. */
    ST_Dict_free(tag->vorbisComments);

    while(tag->npictures) {
        ST_Picture_free(tag->pictures[--tag->npictures]);
    }

    free(tag->pictures);
    free(tag);
}

ST_FUNC ST_FLAC *ST_FLAC_createFromFile(const char *fn) {
    FILE *fp;
    uint8_t buf[4];
    uint8_t *tag;
    int done = 0;
    uint8_t block_type;
    uint32_t block_len;
    int got_meta = 0;
    ST_FLAC *rv = ST_FLAC_create();

    if(!rv) {
        return NULL;
    }

    /* Open up the file for reading */
    fp = fopen(fn, "rb");
    if(!fp) {
        goto out_free;
    }

    /* Check for the fLaC that starts FLAC files. */
    if(fread(buf, 1, 4, fp) != 4) {
        goto out_close;
    }

    /* Check for the signature */
    if(memcmp("fLaC", buf, 4)) {
        goto out_close;
    }

    /* Loop through the metadata blocks until we find a VORBIS_COMMENT or a
       PICTURE metadata block. */
    while(!done) {
        if(fread(buf, 1, 4, fp) != 4) {
            goto out_close;
        }

        block_type = buf[0] & 0x7F;
        block_len = (buf[1] << 16) | (buf[2] << 8) | (buf[3]);

        /* See if this is the last one */
        done = buf[0] & 0x80;

        /* If this isn't a type we care about, skip it. */
        if(block_type != METADATA_TYPE_VORBIS_COMMENT &&
           block_type != METADATA_TYPE_PICTURE) {
            fseek(fp, (long)block_len, SEEK_CUR);
            continue;
        }

        /* Since we're looking at the metadata block we want, allocate the space
           to store it. */
        tag = (uint8_t *)malloc((size_t)block_len);
        if(!tag) {
            goto out_close;
        }

        if(fread(tag, 1, (size_t)block_len, fp) != (size_t)block_len) {
            goto out_close;
        }

        if(block_type == METADATA_TYPE_VORBIS_COMMENT) {
            if(parse_comments(rv, tag, block_len) < 0) {
                free(tag);
                goto out_close;
            }

            got_meta = 1;
        }
        else if(block_type == METADATA_TYPE_PICTURE) {
            if(parse_picture(rv, tag, block_len) < 0) {
                free(tag);
                goto out_close;
            }

            got_meta = 1;
        }

        /* Clean up before the next round */
        free(tag);
    }

    /* We're done with the file at this point, no matter what. */
    fclose(fp);

    /* If we don't have any metadata to work with, we're kinda screwed at this
       point... */
    if(!got_meta)
        goto out_free;

    return rv;

out_close:
    fclose(fp);
out_free:
    ST_FLAC_free(rv);
    return NULL;
}

ST_FUNC ST_Error ST_FLAC_commentForKey(const ST_FLAC *tag, const char *key,
                                       int index, uint8_t *buf, size_t len) {
    const void **value;
    int count;
    ST_FLAC_vcomment *val;

    if(!tag || tag->base.type != ST_TagType_FLAC || index < 0)
        return ST_Error_InvalidArgument;

    if((value = ST_Dict_find(tag->vorbisComments, key, &count))) {
        if(index < count) {
            val = (ST_FLAC_vcomment *)value[index];
            memset(buf, 0, len);
            memcpy(buf, val->data, MIN(len, val->length));
            return ST_Error_None;
        }
    }

    return ST_Error_NotFound;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_FLAC_copyCommentForKey(const ST_FLAC *tag,
                                              const char *key, int index,
                                              ST_Error *err) {
    const void **value;
    int count;
    ST_FLAC_vcomment *val;

    if(!tag || tag->base.type != ST_TagType_FLAC || index < 0) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if((value = ST_Dict_find(tag->vorbisComments, key, &count))) {
        if(index < count) {
            val = (ST_FLAC_vcomment *)value[index];
            if(err)
                *err = ST_Error_None;

            return CFStringCreateWithBytes(kCFAllocatorDefault, val->data,
                                           val->length, kCFStringEncodingUTF8,
                                           false);
        }
    }

    if(err)
        *err = ST_Error_NotFound;

    return NULL;
}
#endif

ST_FUNC int ST_FLAC_commentCountForKey(const ST_FLAC *tag, const char *key) {
    const void **value;
    int count;

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return -1;

    if((value = ST_Dict_find(tag->vorbisComments, key, &count))) {
        return count;
    }

    return 0;
}

ST_FUNC size_t ST_FLAC_commentLengthForKey(const ST_FLAC *tag, const char *key,
                                           int index) {
    const void **value;
    int count;
    ST_FLAC_vcomment *val;

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return (size_t)-1;

    if((value = ST_Dict_find(tag->vorbisComments, key, &count))) {
        if(index < count) {
            val = (ST_FLAC_vcomment *)value[index];
            return val->length;
        }
    }

    return 0;
}

ST_FUNC ST_Error ST_FLAC_title(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "title", 0, buf, len);
}

ST_FUNC ST_Error ST_FLAC_artist(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "artist", 0, buf, len);
}

ST_FUNC ST_Error ST_FLAC_album(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "album", 0, buf, len);
}

ST_FUNC ST_Error ST_FLAC_comment(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "comment", 0, buf, len);
}

ST_FUNC ST_Error ST_FLAC_date(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "date", 0, buf, len);
}

ST_FUNC ST_Error ST_FLAC_genre(const ST_FLAC *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_FLAC_commentForKey(tag, "genre", 0, buf, len);
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_FLAC_copyTitle(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "title", 0, err);
}

ST_FUNC CFStringRef ST_FLAC_copyArtist(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "artist", 0, err);
}

ST_FUNC CFStringRef ST_FLAC_copyAlbum(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "album", 0, err);
}

ST_FUNC CFStringRef ST_FLAC_copyComment(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "comment", 0, err);
}

ST_FUNC CFStringRef ST_FLAC_copyDate(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "date", 0, err);
}

ST_FUNC CFStringRef ST_FLAC_copyGenre(const ST_FLAC *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_FLAC) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_FLAC_copyCommentForKey(tag, "genre", 0, err);
}
#endif

ST_FUNC int ST_FLAC_track(const ST_FLAC *tag) {
    uint8_t tmp[32];

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return -1;

    if(ST_FLAC_commentForKey(tag, "tracknumber", 0, tmp, 32) != ST_Error_None)
        return -1;

    tmp[31] = 0;
    return atoi((const char *)tmp);
}

ST_FUNC int ST_FLAC_disc(const ST_FLAC *tag) {
    uint8_t tmp[32];

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return -1;

    if(ST_FLAC_commentForKey(tag, "discnumber", 0, tmp, 32) != ST_Error_None)
        return -1;

    tmp[31] = 0;
    return atoi((const char *)tmp);
}

ST_FUNC const ST_Picture *ST_FLAC_picture(const ST_FLAC *tag, ST_PictureType pt,
                                          int index) {
    int i;

    if(!tag || tag->base.type != ST_TagType_FLAC || index >= tag->npictures)
        return NULL;

    /* Handle the easy case first... */
    if(pt == ST_PictureType_Any)
        return tag->pictures[index];

    /* Look through all the pictures of the given type until we get to the index
       that was requested. */
    for(i = 0; i < tag->npictures; ++i) {
        if(ST_Picture_type(tag->pictures[i]) == pt) {
            if(!index)
                return tag->pictures[i];
            --index;
        }
    }

    /* Didn't find one, punt. */
    return NULL;
}

ST_FUNC size_t ST_FLAC_titleLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "title", 0);
}

ST_FUNC size_t ST_FLAC_artistLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "artist", 0);
}

ST_FUNC size_t ST_FLAC_albumLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "album", 0);
}

ST_FUNC size_t ST_FLAC_commentLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "comment", 0);
}

ST_FUNC size_t ST_FLAC_dateLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "date", 0);
}

ST_FUNC size_t ST_FLAC_genreLength(const ST_FLAC *tag) {
    return ST_FLAC_commentLengthForKey(tag, "genre", 0);
}

static ST_Error replace_tag(ST_FLAC *tag, const char *k, const uint8_t *v,
                            size_t len, ST_TextEncoding e) {
    ST_Error rv;
    ST_FLAC_vcomment *c;

    if(!tag || !v || tag->base.type != ST_TagType_FLAC ||
       e != ST_TextEncoding_UTF8)
        return ST_Error_InvalidArgument;

    if(!(c = make_comment(v, len)))
        return ST_Error_errno;

    rv = ST_Dict_replace(tag->vorbisComments, k, 0, c);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->vorbisComments, k, c);

    if(rv != ST_Error_None)
        free_comment(c);

    return rv;
}

ST_FUNC ST_Error ST_FLAC_setTitle(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replace_tag(tag, "title", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setArtist(ST_FLAC *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replace_tag(tag, "artist", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setAlbum(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replace_tag(tag, "album", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setComment(ST_FLAC *tag, const uint8_t *v, size_t len,
                                    ST_TextEncoding e) {
    return replace_tag(tag, "comment", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setDate(ST_FLAC *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, "date", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setGenre(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replace_tag(tag, "genre", v, len, e);
}

ST_FUNC ST_Error ST_FLAC_setTrack(ST_FLAC *tag, int v) {
    char tmp[32];

    snprintf(tmp, 32, "%d", v);
    return replace_tag(tag, "tracknumber", (uint8_t *)tmp, strlen(tmp),
                       ST_TextEncoding_UTF8);
}

ST_FUNC ST_Error ST_FLAC_setDisc(ST_FLAC *tag, int v) {
    char tmp[32];

    snprintf(tmp, 32, "%d", v);
    return replace_tag(tag, "discnumber", (uint8_t *)tmp, strlen(tmp),
                       ST_TextEncoding_UTF8);
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_FLAC_vcomment *make_comment_str(CFStringRef s) {
    ST_FLAC_vcomment *rv;
    uint8_t *tmp, *stmp;
    CFIndex len, slen, clen;

    /* Setup and convert the string to UTF8 */
    slen = CFStringGetLength(s);
    len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(s),
                                            kCFStringEncodingUTF8);
    if(!(tmp = (uint8_t *)malloc(len)))
        return NULL;

    if(CFStringGetBytes(s, CFRangeMake(0, slen), kCFStringEncodingUTF8, 0,
                        false, (UInt8 *)tmp, len, &clen) != len) {
        free(tmp);
        errno = EILSEQ;
        return NULL;
    }

    /* Trim the buffer down to the smallest it can be, since we allocated the
       maximum length it could've been up above. */
    if(!(stmp = (uint8_t *)realloc(tmp, clen))) {
        free(tmp);
        return NULL;
    }

    free(tmp);

    /* Make the comment structure and return it! */
    if((rv = (ST_FLAC_vcomment *)malloc(sizeof(ST_FLAC_vcomment)))) {
        rv->data = stmp;
        rv->length = clen;
        return rv;
    }

    free(stmp);
    return NULL;
}

static ST_Error replace_tag_str(ST_FLAC *tag, const char *k, CFStringRef s) {
    ST_Error rv;
    ST_FLAC_vcomment *c;

    if(!tag || !s || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    if(!(c = make_comment_str(s)))
        return ST_Error_errno;

    rv = ST_Dict_replace(tag->vorbisComments, k, 0, c);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->vorbisComments, k, c);

    if(rv != ST_Error_None)
        free_comment(c);

    return rv;
}

ST_FUNC ST_Error ST_FLAC_setTitleStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "title", str);
}

ST_FUNC ST_Error ST_FLAC_setArtistStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "artist", str);
}

ST_FUNC ST_Error ST_FLAC_setAlbumStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "album", str);
}

ST_FUNC ST_Error ST_FLAC_setCommentStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "comment", str);
}

ST_FUNC ST_Error ST_FLAC_setDateStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "date", str);
}

ST_FUNC ST_Error ST_FLAC_setGenreStr(ST_FLAC *tag, CFStringRef str) {
    return replace_tag_str(tag, "genre", str);
}
#endif

ST_FUNC ST_Error ST_FLAC_addPicture(ST_FLAC *tag, ST_Picture *p) {
    void *tmp;

    if(!tag || !p || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    tmp = realloc(tag->pictures, (tag->npictures + 1) * sizeof(ST_Picture *));
    if(!tmp)
        return ST_Error_errno;

    tag->pictures = (ST_Picture **)tmp;
    tag->pictures[tag->npictures++] = p;

    return ST_Error_None;
}

static ST_Error remove_pict(ST_FLAC *tag, int i) {
    void *tmp;

    /* Free the picture in question */
    ST_Picture_free(tag->pictures[i]);

    /* Move each element after the one we're removing down a slot. */
    for(; i < tag->npictures - 1; ++i) {
        tag->pictures[i] = tag->pictures[i + 1];
    }

    /* Reduce the space we're using. If this fails, we still have the old
       pointer, so its not really a big deal (although, it really shouldn't
       ever fail). */
    tmp = realloc(tag->pictures, --tag->npictures * sizeof(ST_Picture *));
    if(tmp)
        tag->pictures = (ST_Picture **)tmp;

    return ST_Error_None;
}

ST_FUNC ST_Error ST_FLAC_removePicture(ST_FLAC *tag, ST_PictureType pt,
                                       int index) {
    int i;

    if(!tag || tag->base.type != ST_TagType_FLAC || index >= tag->npictures)
        return ST_Error_InvalidArgument;

    /* Handle the easy case first. */
    if(pt == ST_PictureType_Any)
        return remove_pict(tag, index);

    /* Look for the index of the value we're looking for. */
    for(i = 0; i < tag->npictures; ++i) {
        if(ST_Picture_type(tag->pictures[i]) == pt) {
            if(!index)
                return remove_pict(tag, i);
            --index;
        }
    }

    return ST_Error_NotFound;
}

ST_FUNC const ST_Dict *ST_FLAC_commentDictionary(const ST_FLAC *tag) {
    if(!tag || tag->base.type != ST_TagType_FLAC)
        return NULL;

    return tag->vorbisComments;
}

#ifdef ST_HAVE_COREFOUNDATION
static void dfunc(const ST_Dict *d, void *data, const void *key,
                  const void *v) {
    CFMutableDictionaryRef cfd = (CFMutableDictionaryRef)data;
    CFMutableArrayRef tmp;
    CFStringRef key2, value;
    const ST_FLAC_vcomment *comment = (const ST_FLAC_vcomment *)v;

    /* Create CoreFoundation strings of the key and value */
    key2 = CFStringCreateWithCString(kCFAllocatorDefault, (const char *)key,
                                     kCFStringEncodingASCII);
    if(!key2)
        return;

    value = CFStringCreateWithBytes(kCFAllocatorDefault,
                                    (const UInt8 *)comment->data,
                                    comment->length, kCFStringEncodingUTF8,
                                    false);
    if(!value) {
        CFRelease(key2);
        return;
    }

    if(!(tmp = (CFMutableArrayRef)CFDictionaryGetValue(cfd, key2))) {
        /* Create a new array and stick it in the dictionary */
        if(!(tmp = CFArrayCreateMutable(kCFAllocatorDefault, 0,
                                        &kCFTypeArrayCallBacks))) {
            CFRelease(value);
            CFRelease(key2);
            return;
        }

        CFDictionaryAddValue(cfd, key2, tmp);
        CFRelease(tmp);
    }

    /* Append our value to the array */
    CFArrayAppendValue(tmp, value);
    CFRelease(value);
    CFRelease(key2);
}

ST_FUNC CFDictionaryRef ST_FLAC_copyDictionary(const ST_FLAC *tag) {
    CFMutableDictionaryRef rv;

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return NULL;

    if((rv = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks))) {
        ST_Dict_foreach(tag->vorbisComments, rv, &dfunc);
    }

    return rv;
}
#endif

ST_FUNC ST_Error ST_FLAC_addComment(ST_FLAC *tag, const char *key,
                                    const uint8_t *value, size_t len) {
    ST_FLAC_vcomment *tmp;
    ST_Error rv;

    if(!tag || !key || !value || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    if(!(tmp = make_comment(value, len)))
        return ST_Error_errno;

    if((rv = ST_Dict_add(tag->vorbisComments, key, tmp)) != ST_Error_None)
        free(tmp);

    return rv;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC ST_Error ST_FLAC_addCommentStr(ST_FLAC *tag, const char *key,
                                       CFStringRef value) {
    ST_FLAC_vcomment *tmp;
    ST_Error rv;

    if(!tag || !key || !value || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    if(!(tmp = make_comment_str(value)))
        return ST_Error_errno;

    if((rv = ST_Dict_add(tag->vorbisComments, key, tmp)) != ST_Error_None)
        free(tmp);

    return rv;
}
#endif

ST_FUNC ST_Error ST_FLAC_removeComment(ST_FLAC *tag, const char *key,
                                       int index) {
    if(!tag || !key || index < -1 || tag->base.type != ST_TagType_FLAC)
        return ST_Error_InvalidArgument;

    return ST_Dict_remove(tag->vorbisComments, key, index);
}

static int parse_comments(ST_FLAC *tag, uint8_t *buf, uint32_t length) {
    uint32_t start = 0, sz, count;
    char *tmp, *tmp2, *tmp3;
    ST_FLAC_vcomment *c;

    /* Make sure things are relatively sane */
    if(length < 4)
        return -1;

    /* The first part of the Vorbis Comment is the vendor of the encoder. */
    sz = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

    if(length < 4 + sz || !(tmp = (char *)malloc(sz + 1)))
        return -1;

    memcpy(tmp, buf + 4, sz);
    tmp[sz] = 0;

    if(!(c = make_comment((uint8_t *)tmp, strlen(tmp))))
        return -1;

    free(tmp);
    ST_Dict_add(tag->vorbisComments, "vendor", c);

    /* Set up the rest of the parsing */
    start = sz + 4;
    count = buf[start] | (buf[start + 1] << 8) | (buf[start + 2] << 16) |
        (buf[start + 3] << 24);
    start += 4;

    while(start < length && count--) {
        /* Read the size of the next comment */
        sz = buf[start] | (buf[start + 1] << 8) | (buf[start + 2] << 16) |
            (buf[start + 3] << 24);

        if(length < start + sz + 4 || !(tmp = (char *)malloc(sz + 1)))
            return -1;

        /* Read in the comment and parse it */
        memcpy(tmp, buf + start + 4, sz);
        tmp[sz] = 0;

        /* Find the first equals and overwrite it with a NUL byte so that we
           now have a separate key and value */
        tmp2 = strchr(tmp, '=');

        if(tmp2) {
            *tmp2++ = 0;

            if(!(c = make_comment((uint8_t *)tmp2, sz - strlen(tmp) - 1))) {
                free(tmp);
                return -1;
            }

            /* Convert the key to all lowercase. Since these are guaranteed
               by the spec to be ASCII, this is fine. */
            tmp3 = tmp;
            while(*tmp3) {
                *tmp3 = tolower(*tmp3);
                ++tmp3;
            }

            ST_Dict_add(tag->vorbisComments, tmp, c);
        }

        free(tmp);
        start += sz + 4;
    }

    return 0;
}

static int parse_picture(ST_FLAC *tag, uint8_t *bytes, uint32_t len) {
    uint32_t start, sz, desc_sz;
    uint32_t pictureType, width, height, bpp, iu;
    char *mime;
    uint8_t *desc;
    uint8_t *data;
    ST_Picture *p;
    void *tmp;

    if(len < 32)
        return -1;

    /* The picture type is the first thing, make sure its valid */
    pictureType = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) |
        bytes[3];

    if(pictureType > ST_PictureType_MAX)
        return -1;

    /* Next up is the mime type string (ASCII) */
    sz = (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];

    if(len < 8 + sz || !(mime = (char *)malloc(sz + 1)))
        return -1;

    memcpy(mime, bytes + 8, sz);
    mime[sz] = 0;

    /* Next up is the description (UTF-8) */
    start = sz + 8;

    if(len < start + 4) {
        free(mime);
        return -1;
    }

    desc_sz = (bytes[start] << 24) | (bytes[start + 1] << 16) |
        (bytes[start + 2] << 8) | bytes[start + 3];

    if(len < start + 4 + desc_sz || !(desc = (uint8_t *)malloc(desc_sz))) {
        free(mime);
        return -1;
    }

    memcpy(desc, bytes + start + 4, desc_sz);

    /* Next up is the width/height/color depth/index info */
    start += desc_sz + 4;

    if(len < start + 20) {
        free(desc);
        free(mime);
        return -1;
    }

    width = (bytes[start] << 24) | (bytes[start + 1] << 16) |
        (bytes[start + 2] << 8) | bytes[start + 3];
    height = (bytes[start + 4] << 24) | (bytes[start + 5] << 16) |
        (bytes[start + 6] << 8) | bytes[start + 7];
    bpp = (bytes[start + 8] << 24) | (bytes[start + 9] << 16) |
        (bytes[start + 10] << 8) | bytes[start + 11];
    iu = (bytes[start + 12] << 24) | (bytes[start + 13] << 16) |
        (bytes[start + 14] << 8) | bytes[start + 15];

    /* Finally is the picture data */
    sz = (bytes[start + 16] << 24) | (bytes[start + 17] << 16) |
        (bytes[start + 18] << 8) | bytes[start + 19];

    if(len < start + 20 + sz || !(data = (uint8_t *)malloc(sz))) {
        free(desc);
        free(mime);
        return -1;
    }

    memcpy(data, bytes + start + 20, sz);

    /* Store it into a new picture */
    if(!(p = ST_Picture_create())) {
        free(data);
        free(desc);
        free(mime);
        return -1;
    }

    ST_Picture_setWidth(p, width);
    ST_Picture_setHeight(p, height);
    ST_Picture_setBitDepth(p, bpp);
    ST_Picture_setIndexUsed(p, iu);
    ST_Picture_setType(p, (ST_PictureType)pictureType);

    if(ST_Picture_setMimeType(p, mime) ||
       ST_Picture_setDescription(p, desc, desc_sz, ST_TextEncoding_UTF8)) {
        ST_Picture_free(p);
        free(data);
        free(desc);
        free(mime);
        return -1;
    }

    free(desc);
    free(mime);

    if(ST_Picture_setData(p, data, sz, 1)) {
        ST_Picture_free(p);
        free(data);
        return -1;
    }

    /* Add the picture to the list */
    tmp = realloc(tag->pictures, (tag->npictures + 1) * sizeof(ST_Picture *));
    if(!tmp) {
        ST_Picture_free(p);
        return -1;
    }

    tag->pictures = (ST_Picture **)tmp;
    tag->pictures[tag->npictures++] = p;

    return 0;
}
