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
#include <stdlib.h>
#include <string.h>

#include "SonatinaTag/SonatinaTag.h"
#include "SonatinaTag/Tags/ID3v2.h"
#include "Frame.h"
#include "../base/Tag.h"

struct ST_ID3v2_struct {
    ST_Tag base;
    uint8_t majorver;
    uint8_t revision;
    uint8_t flags;
    ST_Dict *frames;
};

#define STTAGID3V2_FLAG_UNSYNC  (1 << 7)
#define STTAGID3V2_FLAG_EXTHDR  (1 << 6)
#define STTAGID3V2_FLAG_EXP     (1 << 5)
#define STTAGID3V2_FLAG_FOOTER  (1 << 4)

#define STTAGID3V2_FLAG_MASK_22 0x80
#define STTAGID3V2_FLAG_MASK_23 0xE0
#define STTAGID3V2_FLAG_MASK_24 0xF0

#ifdef MIN
#undef MIN
#endif

#define MIN(x, y) ((x < y) ? x : y)

/* Forward declarations */
static int parse_file(ST_ID3v2 *tag, FILE *fp);

ST_FUNC ST_ID3v2 *ST_ID3v2_create(void) {
    ST_ID3v2 *rv = (ST_ID3v2 *)malloc(sizeof(ST_ID3v2));
    void (*f)(void *) = (void (*)(void *))ST_ID3v2_Frame_free;

    if(rv) {
        if(!(rv->frames = ST_Dict_createUint32(10, f))) {
            free(rv);
            return NULL;
        }

        rv->base.type = ST_TagType_ID3v2;
    }

    return rv;
}

ST_FUNC void ST_ID3v2_free(ST_ID3v2 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return;

    /* Clean up the dictionary. This will free all the values in it too. */
    ST_Dict_free(tag->frames);

    free(tag);
}

ST_FUNC ST_ID3v2 *ST_ID3v2_createFromFile(const char *fn) {
    ST_ID3v2 *rv = ST_ID3v2_create();
    FILE *fp;

    if(!rv)
        return NULL;

    /* Open up the file for reading */
    fp = fopen(fn, "rb");
    if(!fp) {
        goto out_free;
    }

    if(!parse_file(rv, fp)) {
        return rv;
    }

out_free:
    ST_ID3v2_free(rv);
    return NULL;
}

ST_FUNC const ST_Frame *ST_ID3v2_frameForKey(const ST_ID3v2 *tag,
                                             ST_ID3v2_FrameCode code,
                                             int index) {
    const void **value;
    int count;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return NULL;

    if((value = ST_Dict_find(tag->frames, &code, &count))) {
        if(index < count) {
            return (const ST_Frame *)value[index];
        }
    }

    return NULL;
}

ST_FUNC int ST_ID3v2_frameCountForKey(const ST_ID3v2 *tag,
                                      ST_ID3v2_FrameCode code) {
    const void **value;
    int count;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return -1;

    if((value = ST_Dict_find(tag->frames, &code, &count)))
        return count;

    return 0;
}

ST_FUNC const ST_Dict *ST_ID3v2_frameDictionary(const ST_ID3v2 *tag) {
    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return NULL;

    return tag->frames;
}

#ifdef ST_HAVE_COREFOUNDATION
static void dfunc(const ST_Dict *d, void *data, const void *key,
                  const void *v) {
    CFMutableDictionaryRef cfd = (CFMutableDictionaryRef)data;
    CFMutableArrayRef tmp;
    CFStringRef key2, value;
    const ST_Frame *frame = (const ST_Frame *)v;
    ST_ID3v2_FrameCode c = *((const ST_ID3v2_FrameCode *)key);
    char key_str[5] = { c >> 24, c >> 16, c >> 8, c, 0 }; 

    /* Create CoreFoundation strings of the key and value */
    key2 = CFStringCreateWithCString(kCFAllocatorDefault, key_str,
                                     kCFStringEncodingISOLatin1);
    if(!key2)
        return;

    if(!(value = ST_ID3v2_Frame_createString(frame))) {
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

ST_FUNC CFDictionaryRef ST_ID3v2_copyDictionary(const ST_ID3v2 *tag) {
    CFMutableDictionaryRef rv;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return NULL;

    if((rv = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks))) {
        ST_Dict_foreach(tag->frames, rv, &dfunc);
    }

    return rv;
}
#endif

ST_FUNC ST_Error ST_ID3v2_addFrame(ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                   ST_Frame *frame) {
    if(!tag || !frame || tag->base.type != ST_TagType_ID3v2)
        return ST_Error_InvalidArgument;

    return ST_Dict_add(tag->frames, &code, frame);
}

ST_FUNC ST_Error ST_ID3v2_removeFrame(ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                      int index) {
    if(!tag || index < -1 || tag->base.type != ST_TagType_ID3v2)
        return ST_Error_InvalidArgument;

    return ST_Dict_remove(tag->frames, &code, index);
}

static ST_Error frameText(const ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                          ST_ID3v2_FrameCode code22, uint8_t *buf, size_t len) {
    const ST_Frame *frame;
    const ST_TextFrame *tframe;
    const ST_CommentFrame *cframe;

    if(!tag || tag->base.type != ST_TagType_ID3v2 || (!buf && len))
        return ST_Error_InvalidArgument;

    if(tag->majorver == 2) {
        if(!(frame = ST_ID3v2_frameForKey(tag, code22, 0)))
            return ST_Error_NotFound;
    }
    else {
        if(!(frame = ST_ID3v2_frameForKey(tag, code, 0)))
            return ST_Error_NotFound;
    }

    if(frame->type == ST_FrameType_Text) {
        tframe = (const ST_TextFrame *)frame;

        memset(buf, 0, len);
        memcpy(buf, tframe->string, MIN(len, tframe->size));
        return ST_Error_None;
    }
    else if(frame->type == ST_FrameType_Comment) {
        cframe = (const ST_CommentFrame *)frame;

        memset(buf, 0, len);
        memcpy(buf, cframe->string, MIN(len, cframe->string_size));
        return ST_Error_None;
    }
    else {
        return ST_Error_InvalidArgument;
    }
}

#ifdef ST_HAVE_COREFOUNDATION
static CFStringEncoding encs[4] = {
    kCFStringEncodingISOLatin1,
    kCFStringEncodingUTF16,
    kCFStringEncodingUTF16BE,
    kCFStringEncodingUTF8
};

static CFStringRef frameTextStr(const ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                ST_ID3v2_FrameCode code22, ST_Error *err) {
    const ST_Frame *frame;
    const ST_TextFrame *tframe;
    const ST_CommentFrame *cframe;
    ST_Error tmp, *e = err;

    if(!err)
        e = &tmp;

    if(!tag || tag->base.type != ST_TagType_ID3v2) {
        *e = ST_Error_InvalidArgument;
        return NULL;
    }

    if(tag->majorver == 2) {
        if(!(frame = ST_ID3v2_frameForKey(tag, code22, 0))) {
            *e = ST_Error_NotFound;
            return NULL;
        }
    }
    else {
        if(!(frame = ST_ID3v2_frameForKey(tag, code, 0))) {
            *e = ST_Error_NotFound;
            return NULL;
        }
    }

    if(frame->type == ST_FrameType_Text) {
        tframe = (const ST_TextFrame *)frame;
        *e = ST_Error_None;

        return CFStringCreateWithBytes(kCFAllocatorDefault, tframe->string,
                                       tframe->size, encs[tframe->encoding],
                                       tframe->encoding ==
                                       ST_TextEncoding_UTF16);
    }
    else if(frame->type == ST_FrameType_Comment) {
        cframe = (const ST_CommentFrame *)frame;
        *e = ST_Error_None;

        return CFStringCreateWithBytes(kCFAllocatorDefault, cframe->string,
                                       cframe->string_size,
                                       encs[cframe->encoding],
                                       cframe->encoding ==
                                       ST_TextEncoding_UTF16);
    }
    else {
        *e = ST_Error_InvalidArgument;
        return NULL;
    }
}
#endif

static size_t frameTextLength(const ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                              ST_ID3v2_FrameCode code22) {
    const ST_Frame *frame;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return (size_t)-1;

    if(tag->majorver == 2) {
        if(!(frame = ST_ID3v2_frameForKey(tag, code22, 0)))
            return 0;
    }
    else {
        if(!(frame = ST_ID3v2_frameForKey(tag, code, 0)))
            return 0;
    }

    if(frame->type == ST_FrameType_Text)
        return ((const ST_TextFrame *)frame)->size;
    else if(frame->type == ST_FrameType_Comment)
        return ((const ST_CommentFrame *)frame)->string_size;
    else
        return (size_t)-1;
}

static ST_TextEncoding frameEnc(const ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                ST_ID3v2_FrameCode code22) {
    const ST_Frame *frame;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return ST_TextEncoding_Invalid;

    if(tag->majorver == 2) {
        if(!(frame = ST_ID3v2_frameForKey(tag, code22, 0)))
            return ST_TextEncoding_Invalid;
    }
    else {
        if(!(frame = ST_ID3v2_frameForKey(tag, code, 0)))
            return ST_TextEncoding_Invalid;
    }

    if(frame->type == ST_FrameType_Text)
        return ((const ST_TextFrame *)frame)->encoding;
    else if(frame->type == ST_FrameType_Comment)
        return ((const ST_CommentFrame *)frame)->encoding;
    else
        return ST_TextEncoding_Invalid;
}

ST_FUNC ST_Error ST_ID3v2_title(const ST_ID3v2 *tag, uint8_t *buf, size_t len) {
    return frameText(tag, ST_FrameTitle, ST_Frame22Title, buf, len);
}

ST_FUNC ST_Error ST_ID3v2_artist(const ST_ID3v2 *tag, uint8_t *buf, size_t len)
{
    return frameText(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer, buf,
                     len);
}

ST_FUNC ST_Error ST_ID3v2_album(const ST_ID3v2 *tag, uint8_t *buf, size_t len) {
    return frameText(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle, buf, len);
}

ST_FUNC ST_Error ST_ID3v2_comment(const ST_ID3v2 *tag, uint8_t *buf,
                                  size_t len) {
    return frameText(tag, ST_FrameComments, ST_Frame22Comments, buf, len);
}

ST_FUNC ST_Error ST_ID3v2_date(const ST_ID3v2 *tag, uint8_t *buf, size_t len) {
    return frameText(tag, ST_FrameDate, ST_Frame22Date, buf, len);
}

ST_FUNC ST_Error ST_ID3v2_genre(const ST_ID3v2 *tag, uint8_t *buf, size_t len) {
    return frameText(tag, ST_FrameContentType, ST_Frame22ContentType, buf, len);
}

/* Just in case someone gets the brilliant idea to use this on a braindead
   system... */
static int my_atoi(uint8_t *buf) {
    int value = 0;

    while(*buf >= 0x30 && *buf <= 0x39) {
        value = (value * 10) + (*buf - 0x30);
        ++buf;
    }

    return value;
}

static int my_atoi16le(uint8_t *buf) {
    int value = 0;

    while(*buf >= 0x30 && *buf <= 0x39) {
        value = (value * 10) + (*buf - 0x30);
        buf += 2;
    }

    return value;
}

static int my_atoi16be(uint8_t *buf) {
    return my_atoi16le(++buf);
}

static int frameNumber(const ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                       ST_ID3v2_FrameCode code22) {
    const ST_Frame *f;
    const ST_TextFrame *tf;

    if(!tag || tag->base.type != ST_TagType_ID3v2)
        return -1;

    if(tag->majorver == 2)
        code = code22;

    if(!(f = ST_ID3v2_frameForKey(tag, code, 0)))
        return -1;

    if(f->type != ST_FrameType_Text)
        return -1;

    tf = (const ST_TextFrame *)f;

    if(tf->encoding == ST_TextEncoding_UTF8 ||
       tf->encoding == ST_TextEncoding_ISO8859_1) {
        uint8_t buf[tf->size + 1];

        memcpy(buf, tf->string, tf->size);
        buf[tf->size] = 0;
        return my_atoi(buf);
    }
    else if(tf->encoding == ST_TextEncoding_UTF16BE) {
        uint8_t buf[tf->size + 2];

        memcpy(buf, tf->string, tf->size);
        buf[tf->size] = buf[tf->size + 1] = 0;
        return my_atoi16be(buf);
    }
    else if(tf->encoding == ST_TextEncoding_UTF16) {
        uint8_t buf[tf->size + 2];

        memcpy(buf, tf->string, tf->size);
        buf[tf->size] = buf[tf->size + 1] = 0;

        if(buf[0] == 0xFF && buf[1] == 0xFE)
            return my_atoi16le(buf + 2);
        else if(buf[0] == 0xFE && buf[1] == 0xFF)
            return my_atoi16be(buf + 2);
        else
            return -1;
    }
    else {
        return -1;
    }
}

ST_FUNC int ST_ID3v2_track(const ST_ID3v2 *tag) {
    return frameNumber(tag, ST_FrameTrackNumber, ST_Frame22TrackNumber);
}
    
ST_FUNC int ST_ID3v2_disc(const ST_ID3v2 *tag) {
    return frameNumber(tag, ST_FramePartOfSet, ST_Frame22PartOfSet);
}

ST_FUNC const ST_Picture *ST_ID3v2_picture(const ST_ID3v2 *tag,
                                           ST_PictureType pt, int index) {
    const void **value;
    int count, i;
    uint32_t key = ST_FrameAttachedPicture;
    const ST_Picture *p;
    const ST_PictureFrame *pf;

    if(!tag || tag->base.type != ST_TagType_ID3v2 || index < 0 ||
       pt < ST_PictureType_Other || pt > ST_PictureType_Any)
        return NULL;

    if(tag->majorver == 2)
        key = ST_Frame22AttachedPicture;

    if((value = ST_Dict_find(tag->frames, &key, &count)) && count > index) {
        pf = (const ST_PictureFrame *)value[index];

        if(pt == ST_PictureType_Any)
            return ST_ID3v2_PictureFrame_picture(pf);

        for(i = 0; i < count; ++i) {
            p = ST_ID3v2_PictureFrame_picture(pf);

            if(ST_Picture_type(p) == pt) {
                if(!index)
                    return p;

                --index;
            }
        }
    }

    return NULL;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_ID3v2_copyTitle(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameTitle, ST_Frame22Title, err);
}

ST_FUNC CFStringRef ST_ID3v2_copyArtist(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer,
                        err);
}

ST_FUNC CFStringRef ST_ID3v2_copyAlbum(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle, err);
}

ST_FUNC CFStringRef ST_ID3v2_copyComment(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameComments, ST_Frame22Comments, err);
}

ST_FUNC CFStringRef ST_ID3v2_copyDate(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameDate, ST_Frame22Date, err);
}

ST_FUNC CFStringRef ST_ID3v2_copyGenre(const ST_ID3v2 *tag, ST_Error *err) {
    return frameTextStr(tag, ST_FrameContentType, ST_Frame22ContentType, err);
}
#endif

ST_FUNC size_t ST_ID3v2_titleLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameTitle, ST_Frame22Title);
}

ST_FUNC size_t ST_ID3v2_artistLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer);
}

ST_FUNC size_t ST_ID3v2_albumLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle);
}

ST_FUNC size_t ST_ID3v2_commentLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameComments, ST_Frame22Comments);
}

ST_FUNC size_t ST_ID3v2_dateLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameDate, ST_Frame22Date);
}

ST_FUNC size_t ST_ID3v2_genreLength(const ST_ID3v2 *tag) {
    return frameTextLength(tag, ST_FrameContentType, ST_Frame22ContentType);
}

ST_FUNC ST_TextEncoding ST_ID3v2_titleEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameTitle, ST_Frame22Title);
}

ST_FUNC ST_TextEncoding ST_ID3v2_artistEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer);
}

ST_FUNC ST_TextEncoding ST_ID3v2_albumEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle);
}

ST_FUNC ST_TextEncoding ST_ID3v2_commentEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameComments, ST_Frame22Comments);
}

ST_FUNC ST_TextEncoding ST_ID3v2_dateEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameDate, ST_Frame22Date);
}

ST_FUNC ST_TextEncoding ST_ID3v2_genreEncoding(const ST_ID3v2 *tag) {
    return frameEnc(tag, ST_FrameContentType, ST_Frame22ContentType);
}

static ST_Error replaceText(ST_ID3v2 *tag, ST_ID3v2_FrameCode k,
                            ST_ID3v2_FrameCode k22, const uint8_t *v,
                            size_t len, ST_TextEncoding e) {
    ST_Error rv;
    ST_Frame *frame;

    if(!tag || !v || tag->base.type != ST_TagType_ID3v2 ||
       e < ST_TextEncoding_ISO8859_1 || e > ST_TextEncoding_UTF8)
        return ST_Error_InvalidArgument;

    if(tag->majorver < 4 && e > ST_TextEncoding_UTF16)
        return ST_Error_InvalidEncoding;

    if(k != ST_FrameComments) {
        if(!(frame = (ST_Frame *)ST_ID3v2_TextFrame_create(e, len, v)))
            return ST_Error_errno;
    }
    else {
        const uint8_t *desc = (const uint8_t *)"";
        const char lang[4] = "eng";

        if(!(frame = (ST_Frame *)ST_ID3v2_CommentFrame_create(e, len, v, 0,
                                                              desc, lang)))
            return ST_Error_errno;
    }

    if(tag->majorver == 2)
        k = k22;

    rv = ST_Dict_replace(tag->frames, &k, 0, frame);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->frames, &k, frame);

    if(rv != ST_Error_None)
        ST_ID3v2_Frame_free(frame);

    return rv;
}

ST_FUNC ST_Error ST_ID3v2_setTitle(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replaceText(tag, ST_FrameTitle, ST_Frame22Title, v, len, e);
}

ST_FUNC ST_Error ST_ID3v2_setArtist(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                    ST_TextEncoding e) {
    return replaceText(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer, v,
                       len, e);
}

ST_FUNC ST_Error ST_ID3v2_setAlbum(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replaceText(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle, v, len,
                       e);
}

ST_FUNC ST_Error ST_ID3v2_setComment(ST_ID3v2 *tag, const uint8_t *v,
                                     size_t len, ST_TextEncoding e) {
    return replaceText(tag, ST_FrameComments, ST_Frame22Comments, v, len, e);
}

ST_FUNC ST_Error ST_ID3v2_setDate(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replaceText(tag, ST_FrameDate, ST_Frame22Date, v, len, e);
}

ST_FUNC ST_Error ST_ID3v2_setGenre(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replaceText(tag, ST_FrameContentType, ST_Frame22ContentType, v, len,
                       e);
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_Error replaceTextStr(ST_ID3v2 *tag, ST_ID3v2_FrameCode k,
                               ST_ID3v2_FrameCode k22, CFStringRef s) {
    ST_Error rv;
    ST_Frame *frame;
    ST_TextEncoding e = ST_TextEncoding_UTF8;

    if(!tag || !s || tag->base.type != ST_TagType_ID3v2)
        return ST_Error_InvalidArgument;

    /* ID3v2.2 and v2.3 don't support UTF-8, so use UTF-16 instead. */
    if(tag->majorver < 4)
        e = ST_TextEncoding_UTF16;

    if(k != ST_FrameComments) {
        if(!(frame = (ST_Frame *)ST_ID3v2_TextFrame_createStr(s, e)))
            return ST_Error_errno;
    }
    else {
        static const CFStringRef desc = CFSTR("");
        static const char lang[4] = "eng";

        if(!(frame = (ST_Frame *)ST_ID3v2_CommentFrame_createStr(s, desc, e,
                                                                 lang)))
            return ST_Error_errno;
    }

    if(tag->majorver == 2)
        k = k22;

    rv = ST_Dict_replace(tag->frames, &k, 0, frame);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->frames, &k, frame);

    if(rv != ST_Error_None)
        ST_ID3v2_Frame_free(frame);

    return rv;
}

ST_FUNC ST_Error ST_ID3v2_setTitleStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameTitle, ST_Frame22Title, str);
}

ST_FUNC ST_Error ST_ID3v2_setArtistStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameLeadPerformer, ST_Frame22LeadPerformer,
                          str);
}

ST_FUNC ST_Error ST_ID3v2_setAlbumStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameAlbumTitle, ST_Frame22AlbumTitle, str);
}

ST_FUNC ST_Error ST_ID3v2_setCommentStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameComments, ST_Frame22Comments, str);
}

ST_FUNC ST_Error ST_ID3v2_setDateStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameDate, ST_Frame22Date, str);
}

ST_FUNC ST_Error ST_ID3v2_setGenreStr(ST_ID3v2 *tag, CFStringRef str) {
    return replaceTextStr(tag, ST_FrameContentType, ST_Frame22ContentType, str);
}
#endif

ST_FUNC ST_Error ST_ID3v2_addPicture(ST_ID3v2 *tag, ST_Picture *p) {
    ST_PictureFrame *f;
    ST_Error rv;

    if(!tag || !p || tag->base.type != ST_TagType_ID3v2)
        return ST_Error_InvalidArgument;

    if(!(f = ST_ID3v2_PictureFrame_create(p)))
        return ST_Error_errno;

    if(tag->majorver == 2)
        rv = ST_ID3v2_addFrame(tag, ST_Frame22AttachedPicture, &f->base);
    else
        rv = ST_ID3v2_addFrame(tag, ST_FrameAttachedPicture, &f->base);

    if(rv != ST_Error_None)
        ST_ID3v2_Frame_free(&f->base);

    return rv;
}

ST_FUNC ST_Error ST_ID3v2_removePicture(ST_ID3v2 *tag, ST_PictureType pt,
                                        int index) {
    const void **value;
    int count, i;
    uint32_t key = ST_FrameAttachedPicture;
    const ST_Picture *p;

    if(!tag || tag->base.type != ST_TagType_ID3v2 || index < -1 ||
       pt < ST_PictureType_Other || pt > ST_PictureType_Any)
        return ST_Error_InvalidArgument;

    if(tag->majorver == 2)
        key = ST_Frame22AttachedPicture;

    /* Handle the easy case first... */
    if(pt == ST_PictureType_Any) {
        return ST_Dict_remove(tag->frames, &key, index);
    }

    if((value = ST_Dict_find(tag->frames, &key, &count)) && count > index) {
        for(i = 0; i < count; ++i) {
            p = (const ST_Picture *)value[index];

            if(ST_Picture_type(p) == pt) {
                if(!index)
                    return ST_Dict_remove(tag->frames, &key, i);

                --index;
            }
        }
    }

    return ST_Error_NotFound;
}

static uint32_t parse_size_22(const uint8_t *buf) {
    /* The size is in big endian order, 3-bytes long */
    return (buf[0] << 16) | (buf[1] << 8) | buf[2];
}

static uint32_t parse_size_23(const uint8_t *buf) {
    /* The size is in big endian order, and formed the normal way */
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static uint32_t parse_size_24(const uint8_t *buf) {
    if(buf[0] > 0x80 || buf[1] > 0x80 || buf[2] > 0x80 || buf[3] > 0x80) {
        return (uint32_t)-1;
    }

    /* The size is in big endian, and the most significant bit of each byte is
       zeroed, leaving us with 28 bits of actual data */
    return (buf[0] << 21) | (buf[1] << 14) | (buf[2] << 7) | buf[3];
}

static int parse_file(ST_ID3v2 *tag, FILE *fp) {
    uint32_t fcc, sz, start = 0;
    uint16_t flags;
    uint8_t buf[10];
    int majorver, revision;
    uint32_t size;
    uint32_t (*szf)(const uint8_t *) = &parse_size_23;
    uint8_t *frame;
    int shouldfree;

    /* Assume for now that ID3v2 tags exist at the beginning of the file... */
    if(fread(buf, 1, 3, fp) != 3)
        goto out_close;

    /* Check for the ID3 signature */
    if(memcmp("ID3", buf, 3))
        /* No ID3 tag at the beginning, fail */
        goto out_close;

    /* We now "know" that there is an ID3 tag here, parse the rest of the ID3
       header to figure out what else we have to do */
    if(fread(buf, 1, 2, fp) != 2)
        goto out_close;

    /* Support is here for 2.2-2.4 */
    if(buf[0] < 2 || buf[0] > 4)
        goto out_close;

    tag->majorver = majorver = buf[0];
    tag->revision = revision = buf[1];

    if(majorver == 4)
        szf = &parse_size_24;
    else if(majorver == 2)
        szf = &parse_size_22;

    /* Grab the flags from the ID3 header */
    if(fread(&tag->flags, 1, 1, fp) != 1)
        goto out_close;

    /* Make sure no unknown flags are set */
    if((majorver == 2 && (tag->flags & ~(STTAGID3V2_FLAG_MASK_22))) ||
       (majorver == 3 && (tag->flags & ~(STTAGID3V2_FLAG_MASK_23))) ||
       (majorver == 4 && (tag->flags & ~(STTAGID3V2_FLAG_MASK_24))))
        goto out_close;

#if 0
    /* We don't handle the unsynchronization setting just yet... */
    if(tag->flags & STTAGID3V2_FLAG_UNSYNC)
        goto out_close;
#endif

    /* The footer isn't handled either... */
    if(tag->flags & STTAGID3V2_FLAG_FOOTER) {
        /* Silently ignore... */
    }

    /* Read in the length of the header */
    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    /* The length is always encoded in the same way as lengths in v2.4 */
    size = parse_size_24(buf);

    /* If we have an extended header, skip it for now */
    if(tag->flags & STTAGID3V2_FLAG_EXTHDR) {
        uint32_t sz2;

        if(fread(buf, 1, 4, fp) != 4)
            goto out_close;

        sz2 = szf(buf);

        fseek(fp, (long)sz2, SEEK_CUR);

        /* The main header size includes this extended header, so remove that
           part from the value */
        size -= sz2;
    }

    while(start < size) {
        if(majorver > 2) {
            if(fread(buf, 1, 10, fp) != 10)
                goto out_close;

            fcc = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
            sz = szf(buf + 4);
            flags = (buf[8] << 8) | buf[9];
            start += 10;
        }
        else {
            if(fread(buf, 1, 6, fp) != 6)
                goto out_close;

            if(buf[0] || buf[1] || buf[2])
                fcc = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | ' ';
            else
                fcc = 0;

            sz = szf(buf + 3);
            flags = 0;
            start += 6;
        }

        /* If we've hit padding, bail */
        if(fcc == 0)
            break;

        if(sz == 0 || sz == (uint32_t)-1)
            goto out_close;

        /* Make sure we only have ID3v2.2 "four" character codes in ID3v2.2 */
        if(majorver > 2 && (fcc & 0xFF) == ' ')
            goto out_close;

        /* Allocate space for the raw frame and read it in*/
        if(!(frame = (uint8_t *)malloc((size_t)sz)))
            goto out_close;

        if(fread(frame, 1, (size_t)sz, fp) != (size_t)sz)
            goto out_close;

        start += sz;

        /* If we have a specialized class for the given type of tag, then handle
           that, otherwise make a generic frame */
        if(buf[0] == 'T' && fcc != ST_FrameUserText &&
           fcc != ST_Frame22UserText) {
            ST_TextFrame *tframe;

            if(!(tframe = ST_ID3v2_TextFrame_create_buf(frame, sz)))
                goto out_close;

            tframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &tframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else if(buf[0] == 'T') {
            ST_UserTextFrame *utframe;

            if(!(utframe = ST_ID3v2_UserTextFrame_create_buf(frame, sz)))
                goto out_close;

            utframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &utframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else if(buf[0] == 'W' && fcc != ST_FrameUserLink &&
                fcc != ST_Frame22UserLink) {
            ST_URLFrame *uframe;

            if(!(uframe = ST_ID3v2_URLFrame_create_buf(frame, sz)))
                goto out_close;

            uframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &uframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else if(buf[0] == 'W') {
            ST_UserURLFrame *uuframe;

            if(!(uuframe = ST_ID3v2_UserURLFrame_create_buf(frame, sz)))
                goto out_close;

            uuframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &uuframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else if(fcc == ST_FrameAttachedPicture ||
                fcc == ST_Frame22AttachedPicture) {
            ST_PictureFrame *pframe;

            if(majorver > 2) {
                if(!(pframe = ST_ID3v2_PictureFrame_create_buf(frame, sz)))
                    goto out_close;
            }
            else {
                if(!(pframe = ST_ID3v2_PictureFrame_create_buf2(frame, sz)))
                    goto out_close;
            }

            pframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &pframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else if(fcc == ST_FrameComments || fcc == ST_Frame22Comments) {
            ST_CommentFrame *cframe;

            if(!(cframe = ST_ID3v2_CommentFrame_create_buf(frame, sz)))
                goto out_close;

            cframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &cframe->base) != ST_Error_None)
                goto out_close;

            shouldfree = 1;
        }
        else {
            ST_GenericFrame *gframe;

            if(!(gframe = ST_ID3v2_GenericFrame_create(sz, frame)))
                goto out_free;

            gframe->base.flags = flags;

            if(ST_ID3v2_addFrame(tag, fcc, &gframe->base) != ST_Error_None)
                goto out_free;

            shouldfree = 0;
        }

        if(shouldfree)
            free(frame);
    }

    fclose(fp);
    return 0;

out_free:
    free(frame);
out_close:
    fclose(fp);
    return -1;
}
