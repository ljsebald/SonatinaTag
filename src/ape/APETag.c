/*
    SonatinaTag
    Copyright (C) 2012 Lawrence Sebald

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
#include "SonatinaTag/Tags/APE.h"
#include "../base/Tag.h"

struct ST_APE_struct {
    ST_Tag base;
    ST_Dict *tags;
    uint32_t ver;
    uint32_t flags;
};

struct ST_APE_item_struct {
    size_t length;
    uint32_t flags;
    uint8_t *data;
};

#ifdef MIN
#undef MIN
#endif

#define MIN(x, y) ((x < y) ? x : y)

static ST_APE_item *make_item(const uint8_t *buf, size_t length,
                              uint32_t flags) {
    ST_APE_item *rv;

    if((rv = (ST_APE_item *)malloc(sizeof(ST_APE_item)))) {
        if((rv->data = (uint8_t *)malloc(length))) {
            memcpy(rv->data, buf, length);
            rv->length = length;
            rv->flags = flags;
            return rv;
        }

        free(rv);
    }

    return NULL;
}

static ST_APE_item *make_item_nc(uint8_t *buf, size_t length, uint32_t flags) {
    ST_APE_item *rv;

    if((rv = (ST_APE_item *)malloc(sizeof(ST_APE_item)))) {
        rv->data = buf;
        rv->length = length;
        rv->flags = flags;
        return rv;
    }

    return NULL;
}

static void free_item(ST_APE_item *c) {
    free(c->data);
    free(c);
}

ST_FUNC size_t ST_APE_item_length(const ST_APE_item *c) {
    if(!c)
        return (size_t)-1;

    return c->length;
}

ST_FUNC uint32_t ST_APE_item_flags(const ST_APE_item *c) {
    if(!c)
        return (uint32_t)-1;

    return c->flags;
}

ST_FUNC const uint8_t *ST_APE_item_data(const ST_APE_item *c) {
    if(!c)
        return NULL;

    return c->data;
}

ST_FUNC ST_APE *ST_APE_create(void) {
    ST_APE *rv = (ST_APE *)malloc(sizeof(ST_APE));
    void (*f)(void *);

    if(rv) {
        f = (void (*)(void *))free_item;
        if(!(rv->tags = ST_Dict_createString(10, f))) {
            free(rv);
            return NULL;
        }

        rv->base.type = ST_TagType_APE;
    }

    return rv;
}

ST_FUNC void ST_APE_free(ST_APE *tag) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return;

    /* Clean up the dictionaries. This will free all the values in them too. */
    ST_Dict_free(tag->tags);

    free(tag);
}

ST_FUNC ST_APE *ST_APE_createFromFile(const char *fn) {
    FILE *fp;
    uint8_t buf[8];
    uint32_t sz, count;
    long lsz;
    ST_APE *rv = ST_APE_create();
    uint32_t isz, flags, has_id3v1 = 0, i;
    int key_len;
    char *key;
    uint8_t *val;
    ST_APE_item *item;

    if(!rv)
        return NULL;

    /* Open up the file for reading */
    fp = fopen(fn, "rb");
    if(!fp)
        goto out_free;

    /* Look for the APE Tag footer... */
    if(fseek(fp, -32, SEEK_END))
        goto out_close;

    if(fread(buf, 1, 8, fp) != 8)
        goto out_close;

    if(memcmp("APETAGEX", buf, 8)) {
        /* Skip any ID3v1 tag and try again... */
        if(fseek(fp, -128, SEEK_END))
            goto out_close;

        if(fread(buf, 1, 3, fp) != 3)
            goto out_close;

        if(memcmp("TAG", buf, 3))
            goto out_close;

        if(fseek(fp, -160, SEEK_END))
            goto out_close;

        if(fread(buf, 1, 8, fp) != 8)
            goto out_close;

        if(memcmp("APETAGEX", buf, 8))
            goto out_close;

        has_id3v1 = 1;
    }

    /* Make sure we support the version of the tag */
    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    rv->ver = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    if(rv->ver != 2000)
        goto out_close;

    /* Grab the size of the tag and the flags */
    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    lsz = sz = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    count = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    rv->flags = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

    /* Seek to the beginning of the tag (excluding any header, if present) */
    if(!has_id3v1) {
        if(fseek(fp, -lsz, SEEK_END))
            goto out_close;
    }
    else {
        if(fseek(fp, -(lsz + 128), SEEK_END))
            goto out_close;
    }

    /* Go through all the items in the tag. */
    while(sz > 32) {
        /* Read the length and flags of the item. */
        if(fread(buf, 1, 8, fp) != 8)
            goto out_close;

        isz = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
        flags = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
        key_len = 0;

        /* Figure out how long the key is */
        do {
            if(fread(buf, 1, 1, fp) != 1)
                goto out_close;

            ++key_len;
        } while(buf[0] != 0);

        /* Sanity check. */
        if(key_len < 2 || (sz - 8 - key_len - isz) < 32)
            goto out_close;

        /* Read in the key */
        if(!(key = (char *)malloc(key_len)))
            goto out_close;

        if(fseek(fp, -key_len, SEEK_CUR)) {
            free(key);
            goto out_close;
        }

        if(fread(key, 1, key_len, fp) != key_len) {
            free(key);
            goto out_close;
        }

        /* Convert the whole key to lower-case. */
        for(i = 0; i < key_len; ++i) {
            key[i] = tolower(key[i]);
        }

        /* Read in the value */
        if(!(val = (char *)malloc(isz))) {
            free(key);
            goto out_close;
        }

        if(fread(val, 1, isz, fp) != isz) {
            free(val);
            free(key);
            goto out_close;
        }

        if(!(item = make_item_nc(val, isz, flags))) {
            free(val);
            free(key);
            goto out_close;
        }

        /* Add the item to our list. */
        if(ST_Dict_add(rv->tags, key, item) != ST_Error_None) {
            free_item(item);
            free(key);
            goto out_close;
        }

        /* Clean up... */
        free(key);
        sz -= 8 + key_len + isz;
    }

    /* We're done, so clean up. */
    fclose(fp);

    return rv;

out_close:
    fclose(fp);
out_free:
    ST_APE_free(rv);
    return NULL;
}

ST_FUNC ST_Error ST_APE_itemForKey(const ST_APE *tag, const char *key,
                                   uint8_t *buf, size_t len) {
    const void **value;
    int count;
    ST_APE_item *val;

    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    if((value = ST_Dict_find(tag->tags, key, &count))) {
        if(count) {
            val = (ST_APE_item *)value[0];
            memset(buf, 0, len);
            memcpy(buf, val->data, MIN(len, val->length));
            return ST_Error_None;
        }
    }

    return ST_Error_NotFound;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_APE_copyItemForKey(const ST_APE *tag, const char *key,
                                          ST_Error *err) {
    const void **value;
    int count;
    ST_APE_item *val;

    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;

        return NULL;
    }

    if((value = ST_Dict_find(tag->tags, key, &count))) {
        if(count) {
            val = (ST_APE_item *)value[0];
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

ST_FUNC size_t ST_APE_itemLengthForKey(const ST_APE *tag, const char *key) {
    const void **value;
    int count;
    ST_APE_item *val;

    if(!tag || tag->base.type != ST_TagType_APE)
        return (size_t)-1;

    if((value = ST_Dict_find(tag->tags, key, &count))) {
        if(count) {
            val = (ST_APE_item *)value[0];
            return val->length;
        }
    }

    return 0;
}

ST_FUNC ST_Error ST_APE_title(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "title", buf, len);
}

ST_FUNC ST_Error ST_APE_artist(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "artist", buf, len);
}

ST_FUNC ST_Error ST_APE_album(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "album", buf, len);
}

ST_FUNC ST_Error ST_APE_comment(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "comment", buf, len);
}

ST_FUNC ST_Error ST_APE_date(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "year", buf, len);
}

ST_FUNC ST_Error ST_APE_genre(const ST_APE *tag, uint8_t *buf, size_t len) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_APE_itemForKey(tag, "genre", buf, len);
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_APE_copyTitle(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "title", err);
}

ST_FUNC CFStringRef ST_APE_copyArtist(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "artist", err);
}

ST_FUNC CFStringRef ST_APE_copyAlbum(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "album", err);
}

ST_FUNC CFStringRef ST_APE_copyComment(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "comment", err);
}

ST_FUNC CFStringRef ST_APE_copyDate(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "year", err);
}

ST_FUNC CFStringRef ST_APE_copyGenre(const ST_APE *tag, ST_Error *err) {
    if(!tag || tag->base.type != ST_TagType_APE) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    return ST_APE_copyItemForKey(tag, "genre", err);
}
#endif

ST_FUNC int ST_APE_track(const ST_APE *tag) {
    uint8_t tmp[32];

    if(!tag || tag->base.type != ST_TagType_APE)
        return -1;

    if(ST_APE_itemForKey(tag, "track", tmp, 32) != ST_Error_None) {
        if(ST_APE_itemForKey(tag, "tracknumber", tmp, 32) != ST_Error_None)
            return -1;
    }

    tmp[31] = 0;
    return atoi((const char *)tmp);
}

ST_FUNC int ST_APE_disc(const ST_APE *tag) {
    uint8_t tmp[32];

    if(!tag || tag->base.type != ST_TagType_FLAC)
        return -1;

    if(ST_APE_itemForKey(tag, "disc", tmp, 32) != ST_Error_None) {
        if(ST_APE_itemForKey(tag, "discnumber", tmp, 32) != ST_Error_None)
            return -1;
    }

    tmp[31] = 0;
    return atoi((const char *)tmp);
}

ST_FUNC size_t ST_APE_titleLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "title");
}

ST_FUNC size_t ST_APE_artistLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "artist");
}

ST_FUNC size_t ST_APE_albumLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "album");
}

ST_FUNC size_t ST_APE_commentLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "comment");
}

ST_FUNC size_t ST_APE_dateLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "date");
}

ST_FUNC size_t ST_APE_genreLength(const ST_APE *tag) {
    return ST_APE_itemLengthForKey(tag, "genre");
}

static ST_Error replace_tag(ST_APE *tag, const char *k, const uint8_t *v,
                            size_t len, uint32_t flags, ST_TextEncoding e) {
    ST_Error rv;
    ST_APE_item *c;

    if(!tag || !v || tag->base.type != ST_TagType_APE ||
       e != ST_TextEncoding_UTF8)
        return ST_Error_InvalidArgument;

    if(!(c = make_item(v, len, flags)))
        return ST_Error_errno;

    rv = ST_Dict_replace(tag->tags, k, 0, c);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->tags, k, c);

    if(rv != ST_Error_None)
        free_item(c);

    return rv;
}

ST_FUNC ST_Error ST_APE_setTitle(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, "title", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setArtist(ST_APE *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replace_tag(tag, "artist", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setAlbum(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, "album", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setComment(ST_APE *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replace_tag(tag, "comment", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setDate(ST_APE *tag, const uint8_t *v, size_t len,
                                ST_TextEncoding e) {
    return replace_tag(tag, "year", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setGenre(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, "genre", v, len, 0, e);
}

ST_FUNC ST_Error ST_APE_setTrack(ST_APE *tag, int v) {
    char tmp[32];

    snprintf(tmp, 32, "%d", v);
    return replace_tag(tag, "track", (uint8_t *)tmp, strlen(tmp), 0,
                       ST_TextEncoding_UTF8);
}

ST_FUNC ST_Error ST_APE_setDisc(ST_APE *tag, int v) {
    char tmp[32];

    snprintf(tmp, 32, "%d", v);
    return replace_tag(tag, "disc", (uint8_t *)tmp, strlen(tmp), 0,
                       ST_TextEncoding_UTF8);
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_APE_item *make_item_str(CFStringRef s, uint32_t flags) {
    ST_APE_item *rv;
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
    if((rv = (ST_APE_item *)malloc(sizeof(ST_APE_item)))) {
        rv->data = stmp;
        rv->length = clen;
        rv->flags = flags;
        return rv;
    }

    free(stmp);
    return NULL;
}

static ST_Error replace_tag_str(ST_APE *tag, const char *k, CFStringRef s,
                                uint32_t flags) {
    ST_Error rv;
    ST_APE_item *c;

    if(!tag || !s || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    if(!(c = make_item_str(s, flags)))
        return ST_Error_errno;

    rv = ST_Dict_replace(tag->tags, k, 0, c);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->tags, k, c);

    if(rv != ST_Error_None)
        free_item(c);

    return rv;
}

ST_FUNC ST_Error ST_APE_setTitleStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "title", str, 0);
}

ST_FUNC ST_Error ST_APE_setArtistStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "artist", str, 0);
}

ST_FUNC ST_Error ST_APE_setAlbumStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "album", str, 0);
}

ST_FUNC ST_Error ST_APE_setCommentStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "comment", str, 0);
}

ST_FUNC ST_Error ST_APE_setDateStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "date", str, 0);
}

ST_FUNC ST_Error ST_APE_setGenreStr(ST_APE *tag, CFStringRef str) {
    return replace_tag_str(tag, "genre", str, 0);
}
#endif

ST_FUNC const ST_Dict *ST_APE_itemDictionary(const ST_APE *tag) {
    if(!tag || tag->base.type != ST_TagType_APE)
        return NULL;

    return tag->tags;
}

#ifdef ST_HAVE_COREFOUNDATION
static void dfunc(const ST_Dict *d, void *data, const void *key,
                  const void *v) {
    CFMutableDictionaryRef cfd = (CFMutableDictionaryRef)data;
    CFMutableArrayRef tmp;
    CFStringRef key2, value;
    const ST_APE_item *comment = (const ST_APE_item *)v;

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

ST_FUNC CFDictionaryRef ST_APE_copyDictionary(const ST_APE *tag) {
    CFMutableDictionaryRef rv;

    if(!tag || tag->base.type != ST_TagType_APE)
        return NULL;

    if((rv = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks))) {
        ST_Dict_foreach(tag->tags, rv, &dfunc);
    }

    return rv;
}
#endif

ST_FUNC ST_Error ST_APE_addItem(ST_APE *tag, const char *key,
                                const uint8_t *value, size_t len,
                                uint32_t flags) {
    ST_APE_item *tmp;
    ST_Error rv;

    if(!tag || !key || !value || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    if(!(tmp = make_item(value, len, flags)))
        return ST_Error_errno;

    if((rv = ST_Dict_add(tag->tags, key, tmp)) != ST_Error_None)
        free(tmp);

    return rv;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC ST_Error ST_APE_addItemStr(ST_APE *tag, const char *key,
                                   CFStringRef value, uint32_t flags) {
    ST_APE_item *tmp;
    ST_Error rv;

    if(!tag || !key || !value || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    if(!(tmp = make_item_str(value, flags)))
        return ST_Error_errno;

    if((rv = ST_Dict_add(tag->tags, key, tmp)) != ST_Error_None)
        free(tmp);

    return rv;
}
#endif

ST_FUNC ST_Error ST_APE_removeItem(ST_APE *tag, const char *key) {
    if(!tag || !key || tag->base.type != ST_TagType_APE)
        return ST_Error_InvalidArgument;

    return ST_Dict_remove(tag->tags, key, 0);
}
