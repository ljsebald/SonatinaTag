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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "SonatinaTag/SonatinaTag.h"
#include "SonatinaTag/Tags/M4A.h"
#include "../base/Tag.h"

struct ST_M4A_struct {
    ST_Tag base;
    ST_Dict *atoms;
    uint32_t picture_count;
    ST_Picture **pictures;
};

struct ST_M4A_Atom_struct {
    char *long_name;
    size_t data_sz;
    uint8_t *data;
};


#ifdef MIN
#undef MIN
#endif

#define MIN(x, y) ((x < y) ? x : y)

/* Forward declarations */
static int parse_file(ST_M4A *tag, FILE *fp);

static void free_atom(void *a) {
    ST_M4A_Atom *atom = (ST_M4A_Atom *)a;

    free(atom->long_name);
    free(atom->data);
    free(atom);
}

static ST_M4A_Atom *create_atom(size_t sz, uint8_t *data, char *long_name) {
    ST_M4A_Atom *a = (ST_M4A_Atom *)malloc(sizeof(ST_M4A_Atom));

    if(a) {
        a->data = data;
        a->long_name = long_name;
        a->data_sz = sz;
    }

    return a;
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_M4A_Atom *create_atom_str(CFStringRef str, char *long_name) {
    CFIndex slen = CFStringGetLength(str);
    CFIndex l = CFStringGetMaximumSizeForEncoding(slen, kCFStringEncodingUTF8);
    uint8_t *s;
    void *tmp;
    ST_M4A_Atom *a;

    /* Make space for the string... */
    if(!(s = (uint8_t *)malloc(l))) {
        return NULL;
    }

    /* Convert the string, and clean it up a bit... */
    CFStringGetBytes(str, CFRangeMake(0, slen), kCFStringEncodingUTF8, 0, false,
                     s, l, &slen);

    if(slen != l) {
        if((tmp = realloc(s, slen)))
            s = (uint8_t *)tmp;
    }

    /* Make the atom, or die trying */
    if((a = (ST_M4A_Atom *)malloc(sizeof(ST_M4A_Atom)))) {
        a->data = s;
        a->long_name = long_name;
        a->data_sz = slen;
    }
    else {
        free(s);
    }

    return a;
}

static ST_M4A_Atom *create_atom_data(CFDataRef d, char *long_name) {
    CFIndex slen = CFDataGetLength(d);
    uint8_t *s;
    ST_M4A_Atom *a;

    /* Make space for the data... */
    if(!(s = (uint8_t *)malloc(slen))) {
        return NULL;
    }

    /* Copy the data out... */
    CFDataGetBytes(d, CFRangeMake(0, slen), s);

    /* Make the atom, or die trying */
    if((a = (ST_M4A_Atom *)malloc(sizeof(ST_M4A_Atom)))) {
        a->data = s;
        a->long_name = long_name;
        a->data_sz = slen;
    }
    else {
        free(s);
    }

    return a;
}
#endif

ST_FUNC const char *ST_M4A_Atom_longName(const ST_M4A_Atom *atom) {
    if(!atom)
        return NULL;

    return atom->long_name;
}

ST_FUNC const uint8_t *ST_M4A_Atom_data(const ST_M4A_Atom *atom) {
    if(!atom)
        return NULL;

    return atom->data;
}

ST_FUNC size_t ST_M4A_Atom_length(const ST_M4A_Atom *atom) {
    if(!atom)
        return (size_t)-1;

    return atom->data_sz;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_M4A_Atom_copyDataStr(const ST_M4A_Atom *atom) {
    if(!atom)
        return NULL;

    return CFStringCreateWithBytes(kCFAllocatorDefault, atom->data,
                                   atom->data_sz, kCFStringEncodingUTF8,
                                   false);
}

ST_FUNC CFDataRef ST_M4A_Atom_copyData(const ST_M4A_Atom *atom) {
    if(!atom)
        return NULL;

    return CFDataCreate(kCFAllocatorDefault, atom->data, atom->data_sz);
}
#endif

ST_FUNC ST_M4A *ST_M4A_create(void) {
    ST_M4A *rv = (ST_M4A *)malloc(sizeof(ST_M4A));

    if(rv) {
        if(!(rv->atoms = ST_Dict_createUint32(10, free_atom))) {
            free(rv);
            return NULL;
        }

        rv->picture_count = 0;
        rv->pictures = NULL;
        rv->base.type = ST_TagType_M4A;
    }

    return rv;
}

ST_FUNC void ST_M4A_free(ST_M4A *tag) {
    uint32_t i;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return;

    /* Clean up the dictionary. This will free all the values in them too. */
    ST_Dict_free(tag->atoms);

    /* Free all pictures */
    for(i = 0; i < tag->picture_count; ++i) {
        ST_Picture_free(tag->pictures[i]);
    }

    free(tag->pictures);
    free(tag);
}

ST_FUNC ST_M4A *ST_M4A_createFromFile(const char *fn) {
    ST_M4A *rv = ST_M4A_create();
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
    ST_M4A_free(rv);
    return NULL;
}

ST_FUNC ST_Error ST_M4A_atomForKey(const ST_M4A *tag, ST_M4A_AtomCode code,
                                   int index, uint8_t *buf, size_t len) {
    const void **value;
    int count;
    const ST_M4A_Atom *atom;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if((value = ST_Dict_find(tag->atoms, &code, &count))) {
        if(index < count) {
            atom = (const ST_M4A_Atom *)value[index];

            memset(buf, 0, len);
            memcpy(buf, atom->data, MIN(len, atom->data_sz));
            return ST_Error_None;
        }
    }

    return ST_Error_NotFound;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_M4A_copyAtomForKey(const ST_M4A *tag,
                                          ST_M4A_AtomCode code, int index,
                                          ST_Error *err) {
    const void **value;
    int count;
    const ST_M4A_Atom *atom;
    CFStringRef rv;

    if(!tag || tag->base.type != ST_TagType_M4A) {
        if(err)
            *err = ST_Error_InvalidArgument;
        return NULL;
    }

    if((value = ST_Dict_find(tag->atoms, &code, &count))) {
        if(index < count) {
            atom = (const ST_M4A_Atom *)value[index];

            rv = CFStringCreateWithBytes(kCFAllocatorDefault, atom->data,
                                         atom->data_sz, kCFStringEncodingUTF8,
                                         false);
            if(!rv && err)
                *err = ST_Error_Unknown;

            return rv;
        }
    }

    if(err)
        *err = ST_Error_NotFound;
    return NULL;
}
#endif

ST_FUNC int ST_M4A_atomCountForKey(const ST_M4A *tag, ST_M4A_AtomCode code) {
    const void **value;
    int count;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return -1;

    if((value = ST_Dict_find(tag->atoms, &code, &count)))
        return count;

    return 0;
}

ST_FUNC size_t ST_M4A_atomLengthForKey(const ST_M4A *tag, ST_M4A_AtomCode code,
                                       int index) {
    const void **value;
    int count;
    const ST_M4A_Atom *atom;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return (size_t)-1;

    if((value = ST_Dict_find(tag->atoms, &code, &count))) {
        if(index < count) {
            return atom->data_sz;
        }
    }

    return 0;
}

ST_FUNC ST_Error ST_M4A_title(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomTitle, 0, buf, len);
}

ST_FUNC ST_Error ST_M4A_artist(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomArtist, 0, buf, len);
}

ST_FUNC ST_Error ST_M4A_album(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomAlbum, 0, buf, len);
}

ST_FUNC ST_Error ST_M4A_comment(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomComment, 0, buf, len);
}

ST_FUNC ST_Error ST_M4A_date(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomYear, 0, buf, len);
}

ST_FUNC ST_Error ST_M4A_genre(const ST_M4A *tag, uint8_t *buf, size_t len) {
    return ST_M4A_atomForKey(tag, ST_AtomGenre, 0, buf, len);
}

ST_FUNC int ST_M4A_track(const ST_M4A *tag) {
    uint8_t buf[32] = { 0 };

    if(!tag || tag->base.type != ST_TagType_M4A)
        return -1;

    if(ST_M4A_atomForKey(tag, ST_AtomTrackNumber, 0, buf, 32) != ST_Error_None)
        return -1;

    return (int)(buf[3] | (buf[2] << 8));
}

ST_FUNC int ST_M4A_disc(const ST_M4A *tag) {
    uint8_t buf[32] = { 0 };

    if(!tag || tag->base.type != ST_TagType_M4A)
        return -1;

    if(ST_M4A_atomForKey(tag, ST_AtomDiscNumber, 0, buf, 32) != ST_Error_None)
        return -1;

    return (int)(buf[3] | (buf[2] << 8));
}

ST_FUNC const ST_Picture *ST_M4A_picture(const ST_M4A *tag, int index) {
    if(!tag || tag->base.type != ST_TagType_M4A || index >= tag->picture_count)
        return NULL;

    return tag->pictures[index];
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC CFStringRef ST_M4A_copyTitle(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomTitle, 0, err);
}

ST_FUNC CFStringRef ST_M4A_copyArtist(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomArtist, 0, err);
}

ST_FUNC CFStringRef ST_M4A_copyAlbum(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomAlbum, 0, err);
}

ST_FUNC CFStringRef ST_M4A_copyComment(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomComment, 0, err);
}

ST_FUNC CFStringRef ST_M4A_copyDate(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomYear, 0, err);
}

ST_FUNC CFStringRef ST_M4A_copyGenre(const ST_M4A *tag, ST_Error *err) {
    return ST_M4A_copyAtomForKey(tag, ST_AtomGenre, 0, err);
}
#endif

ST_FUNC size_t ST_M4A_titleLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomTitle, 0);
}

ST_FUNC size_t ST_M4A_artistLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomArtist, 0);
}

ST_FUNC size_t ST_M4A_albumLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomAlbum, 0);
}

ST_FUNC size_t ST_M4A_commentLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomComment, 0);
}

ST_FUNC size_t ST_M4A_dateLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomYear, 0);
}

ST_FUNC size_t ST_M4A_genreLength(const ST_M4A *tag) {
    return ST_M4A_atomLengthForKey(tag, ST_AtomGenre, 0);
}

ST_FUNC ST_Error ST_M4A_addPicture(ST_M4A *tag, ST_Picture *picture) {
    void *tmp;
    uint32_t count;

    if(!tag || !picture || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    count = tag->picture_count;
    if(!(tmp = realloc(tag->pictures, (count + 1) * sizeof(ST_Picture *))))
        return ST_Error_errno;

    tag->pictures = (ST_Picture **)tmp;
    tag->pictures[tag->picture_count++] = picture;

    return ST_Error_None;
}

ST_FUNC ST_Error ST_M4A_removePicture(ST_M4A *tag, int index) {
    void *tmp;
    uint32_t count, i;

    if(!tag || index < 0 || tag->base.type != ST_TagType_M4A ||
       index >= tag->picture_count)
        return ST_Error_InvalidArgument;

    /* Clean up the requested picture, and move everything down in the array to
       take its place. */
    count = tag->picture_count;
    ST_Picture_free(tag->pictures[index]);

    for(i = index; i < count - 1; ++i) {
        tag->pictures[i] = tag->pictures[i + 1];
    }

    if(!(tmp = realloc(tag->pictures, (count - 1) * sizeof(ST_Picture *)))) {
        /* Uhh... This shouldn't happen, since we're making it smaller... */
    }
    else {
        tag->pictures = (ST_Picture **)tmp;
    }

    --tag->picture_count;

    return ST_Error_None;
}

ST_FUNC const ST_Dict *ST_M4A_atomDictionary(const ST_M4A *tag) {
    if(!tag || tag->base.type != ST_TagType_M4A)
        return NULL;

    return tag->atoms;
}

#ifdef ST_HAVE_COREFOUNDATION
static void dfunc(const ST_Dict *d, void *data, const void *key,
                  const void *v) {
    CFMutableDictionaryRef cfd = (CFMutableDictionaryRef)data;
    CFMutableArrayRef tmp;
    CFStringRef key2, value;
    const ST_M4A_Atom *atom = (const ST_M4A_Atom *)v;
    ST_M4A_AtomCode c = *((const ST_M4A_AtomCode *)key);
    char key_str[5] = { c >> 24, c >> 16, c >> 8, c, 0 }; 

    /* Create CoreFoundation strings of the key and value */
    key2 = CFStringCreateWithCString(kCFAllocatorDefault, key_str,
                                     kCFStringEncodingISOLatin1);
    if(!key2)
        return;

    value = CFStringCreateWithBytes(kCFAllocatorDefault,
                                    (const UInt8 *)atom->data,
                                    atom->data_sz, kCFStringEncodingUTF8,
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

ST_FUNC CFDictionaryRef ST_M4A_copyDictionary(const ST_M4A *tag) {
    CFMutableDictionaryRef rv;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return NULL;

    if((rv = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                       &kCFTypeDictionaryKeyCallBacks,
                                       &kCFTypeDictionaryValueCallBacks))) {
        ST_Dict_foreach(tag->atoms, rv, &dfunc);
    }

    return rv;
}
#endif

ST_FUNC ST_Error ST_M4A_addAtom(ST_M4A *tag, ST_M4A_AtomCode code,
                                const char *lname, uint8_t *value, size_t len,
                                int ownbuf) {
    uint8_t *tmp = value;
    char *ln = NULL;
    ST_M4A_Atom *atom;
    ST_Error rv;

    if(!tag || !value || !len || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(!ownbuf && !(tmp = (uint8_t *)malloc(len)))
        return ST_Error_errno;

    if(lname && !(ln = strdup(lname))) {
        free(tmp);
        return ST_Error_errno;
    }

    if(!ownbuf)
        memcpy(tmp, value, len);

    if(!(atom = create_atom(len, tmp, ln))) {
        free(tmp);
        free(ln);
        return ST_Error_errno;
    }

    if((rv = ST_Dict_add(tag->atoms, &code, atom)) != ST_Error_None)
        free_atom(atom);

    return rv;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC ST_Error ST_M4A_addAtomStr(ST_M4A *tag, ST_M4A_AtomCode code,
                                   const char *lname, CFStringRef value) {
    char *ln = NULL;
    ST_M4A_Atom *atom;
    ST_Error rv;

    if(!tag || !value || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(lname && !(ln = strdup(lname)))
        return ST_Error_errno;

    if(!(atom = create_atom_str(value, ln))) {
        free(ln);
        return ST_Error_errno;
    }

    if((rv = ST_Dict_add(tag->atoms, &code, atom)) != ST_Error_None)
        free_atom(atom);

    return rv;
}

ST_FUNC ST_Error ST_M4A_addAtomData(ST_M4A *tag, ST_M4A_AtomCode code,
                                    const char *lname, CFDataRef value) {
    char *ln = NULL;
    ST_M4A_Atom *atom;
    ST_Error rv;

    if(!tag || !value || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(lname && !(ln = strdup(lname)))
        return ST_Error_errno;

    if(!(atom = create_atom_data(value, ln))) {
        free(ln);
        return ST_Error_errno;
    }

    if((rv = ST_Dict_add(tag->atoms, &code, atom)) != ST_Error_None)
        free_atom(atom);

    return rv;
}
#endif

ST_FUNC ST_Error ST_M4A_removeAtom(ST_M4A *tag, ST_M4A_AtomCode code,
                                   int index) {
    if(!tag || index < -1 || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    return ST_Dict_remove(tag->atoms, &code, index);
}

static ST_Error replace_tag(ST_M4A *tag, ST_M4A_AtomCode k, const uint8_t *v,
                            size_t len, ST_TextEncoding e) {
    uint8_t *tmp;
    ST_Error rv;
    ST_M4A_Atom *atom;

    if(!tag || !v || tag->base.type != ST_TagType_M4A ||
       e != ST_TextEncoding_UTF8)
        return ST_Error_InvalidArgument;

    if(!(tmp = (uint8_t *)malloc(len)))
        return ST_Error_errno;

    memcpy(tmp, v, len);

    if(!(atom = create_atom(len, tmp, NULL))) {
        free(tmp);
        return ST_Error_errno;
    }

    rv = ST_Dict_replace(tag->atoms, &k, 0, atom);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->atoms, &k, atom);

    if(rv != ST_Error_None)
        free_atom(atom);

    return rv;
}

#ifdef ST_HAVE_COREFOUNDATION
static ST_Error replace_tag_str(ST_M4A *tag, ST_M4A_AtomCode k, CFStringRef v) {
    ST_Error rv;
    ST_M4A_Atom *atom;

    if(!tag || !v || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(!(atom = create_atom_str(v, NULL)))
        return ST_Error_errno;

    rv = ST_Dict_replace(tag->atoms, &k, 0, atom);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->atoms, &k, atom);

    if(rv != ST_Error_None)
        free_atom(atom);

    return rv;
}
#endif

ST_FUNC ST_Error ST_M4A_setTitle(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomTitle, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setArtist(ST_M4A *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomArtist, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setAlbum(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomAlbum, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setComment(ST_M4A *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomComment, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setDate(ST_M4A *tag, const uint8_t *v, size_t len,
                                ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomYear, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setGenre(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e) {
    return replace_tag(tag, ST_AtomGenre, v, len, e);
}

ST_FUNC ST_Error ST_M4A_setTrack(ST_M4A *tag, int v) {
    uint8_t buf[32] = { 0 };
    uint8_t *nv;
    size_t sz;
    uint32_t atom_type = (uint32_t)ST_AtomTrackNumber;
    ST_M4A_Atom *atom;
    ST_Error rv;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(!(nv = (uint8_t *)malloc(8)))
        return ST_Error_errno;

    ST_M4A_atomForKey(tag, atom_type, 0, buf, 32);
    sz = ST_M4A_atomLengthForKey(tag, atom_type, 0);

    /* Did we have something? If so, copy it. */
    if(sz == 8) {
        memcpy(nv, buf, 8);
    }
    else {
        memset(nv, 0, 8);
        nv[2] = (uint8_t)(v >> 8);
        nv[3] = (uint8_t)v;
    }

    if(!(atom = create_atom(8, nv, NULL))) {
        free(nv);
        return ST_Error_errno;
    }

    /* Replace the old one, if there is one. */
    rv = ST_Dict_replace(tag->atoms, &atom_type, 0, atom);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->atoms, &atom_type, atom);

    if(rv != ST_Error_None)
        free_atom(atom);

    return rv;
}

ST_FUNC ST_Error ST_M4A_setDisc(ST_M4A *tag, int v) {
    uint8_t buf[32] = { 0 };
    uint8_t *nv;
    size_t sz;
    uint32_t atom_type = (uint32_t)ST_AtomDiscNumber;
    ST_M4A_Atom *atom;
    ST_Error rv;

    if(!tag || tag->base.type != ST_TagType_M4A)
        return ST_Error_InvalidArgument;

    if(!(nv = (uint8_t *)malloc(6)))
        return ST_Error_errno;

    ST_M4A_atomForKey(tag, atom_type, 0, buf, 32);
    sz = ST_M4A_atomLengthForKey(tag, atom_type, 0);

    /* Did we have something? If so, copy it. */
    if(sz == 6) {
        memcpy(nv, buf, 6);
    }
    else {
        memset(nv, 0, 6);
        nv[2] = (uint8_t)(v >> 8);
        nv[3] = (uint8_t)v;
    }

    if(!(atom = create_atom(6, nv, NULL))) {
        free(nv);
        return ST_Error_errno;
    }

    /* Replace the old one, if there is one. */
    rv = ST_Dict_replace(tag->atoms, &atom_type, 0, atom);

    if(rv == ST_Error_NotFound)
        rv = ST_Dict_add(tag->atoms, &atom_type, atom);

    if(rv != ST_Error_None)
        free_atom(atom);

    return rv;
}

#ifdef ST_HAVE_COREFOUNDATION
ST_FUNC ST_Error ST_M4A_setTitleStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomTitle, v);
}

ST_FUNC ST_Error ST_M4A_setArtistStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomArtist, v);
}

ST_FUNC ST_Error ST_M4A_setAlbumStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomAlbum, v);
}

ST_FUNC ST_Error ST_M4A_setCommentStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomComment, v);
}

ST_FUNC ST_Error ST_M4A_setDateStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomYear, v);
}

ST_FUNC ST_Error ST_M4A_setGenreStr(ST_M4A *tag, CFStringRef v) {
    return replace_tag_str(tag, ST_AtomGenre, v);
}
#endif

static long find_atom(ST_M4A_AtomCode atom, FILE *fp, uint64_t container,
                     uint64_t *atom_sz) {
    uint32_t fourcc;
    uint32_t atomsz;
    uint64_t ratomsz;
    uint8_t buf[8];
    long cur, tmp;

    while(container) {
        if((cur = ftell(fp)) < 0)
            return -1;

        if(fread(buf, 1, 8, fp) != 8)
            return -1;

        atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];

        /* If the atom size is 1, the real atom size is stored in the next 8
           bytes. */
        if(atomsz == 1) {
            if(fread(buf, 1, 8, fp) != 8)
                return -1;

            ratomsz = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
                ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
                ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |
                ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
        }
        else if(atomsz == 0) {
            fseek(fp, 0, SEEK_END);
            tmp = ftell(fp);
            ratomsz = (uint64_t)(tmp - cur);
            fseek(fp, cur, SEEK_SET);
        }
        else {
            ratomsz = (uint64_t)atomsz;
        }

        if(fourcc != (uint32_t)atom) {
            /* Check the size to see where to go next */
            if(atomsz == 1) {
                fseek(fp, (long)ratomsz - 16, SEEK_CUR);
                container -= ratomsz;
            }
            else if(atomsz == 0) {
                /* If the atom size is 0, it goes to the end of the file, so
                   we're never going to find it. */
                return -2;
            }
            else {
                fseek(fp, (long)atomsz - 8, SEEK_CUR);
                container -= atomsz;
            }
        }
        else {
            *atom_sz = ratomsz;
            return cur;
        }
    }

    return -2;
}

static int parse_file(ST_M4A *tag, FILE *fp) {
    uint8_t buf[16];
    uint32_t fourcc;
    uint64_t atomsz, atomread, atomsz2;
    uint64_t moovsz, udtasz, metasz, ilstsz, meansz, namesz;
    long pos, meanp, namep;
    uint8_t *tmp;
    char *mean = NULL;
    long size;
    ST_Picture *pic;

    /* Figure out how long the file is */
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* Read in the first 8 bytes of the file, and make sure it is as we would
       expect it to be */
    if(fread(buf, 1, 8, fp) != 8)
        goto out_close;

    atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];

    if(fourcc != ST_AtomFileType)
        goto out_close;

    /* Make sure we have at least one type of information in here */
    if(atomsz < 12)
        goto out_close;

    /* Read in the type of data contained within */
    if(fread(buf, 1, 4, fp) != 4)
        goto out_close;

    if(buf[0] != 'M' || buf[1] != '4' || buf[2] != 'A' || buf[3] != ' ')
        goto out_close;

    /* We don't care about the rest of the ftyp */
    fseek(fp, atomsz, SEEK_SET);

    /* Next, find the "moov" atom, since its the toplevel container for what the
       tags are in */
    if(find_atom(ST_AtomMovieData, fp, size - atomsz, &moovsz) < 0)
        goto out_close;

    /* Now, we need the "udta" atom */
    if(find_atom(ST_AtomUserData, fp, moovsz, &udtasz) < 0)
        goto out_close;

    /* Next up is the "meta" atom */
    if(find_atom(ST_AtomMetadata, fp, udtasz, &metasz) < 0)
        goto out_close;

    /* The meta atom has an extra 4 bytes of version info in the header... */
    fseek(fp, 4, SEEK_CUR);

    /* Finally, the "ilst" atom */
    if(find_atom(ST_AtomItemList, fp, metasz, &ilstsz) < 0)
        goto out_close;

    /* Read in the entire ilst atom */
    while(ilstsz > 8) {
        pos = ftell(fp);

        /* Read in the first 8 bytes of the next atom */
        if(fread(buf, 1, 8, fp) != 8)
            goto out_close;

        atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
        atomread = 8;

        /* Figure out the real size of the atom */
        if(atomsz == 1) {
            if(fread(buf, 1, 8, fp) != 8)
                goto out_close;

            atomsz = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
                ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
                ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |
                ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
            atomread += 8;
        }
        else if(atomsz == 0) {
            atomsz = ilstsz;
        }

        /* Ignore free atoms, since they're just empty space... */
        if(fourcc == ST_AtomFreeSpace)
            goto doneAtom;

        /* Do we have a '----' atom? */
        if(fourcc == ST_AtomLongName) {
            /* Find the sizes of both the name and mean atom, if we have them
               both. */
            if((namep = find_atom(ST_AtomName, fp, atomsz - atomread,
                                  &namesz)) < 0) {
                namesz = 0;
            }
            else {
                namesz -= 12;
            }

            fseek(fp, pos + atomread, SEEK_SET);

            if((meanp = find_atom(ST_AtomMeaning, fp, atomsz - atomread,
                                  &meansz)) < 0)
                goto doneAtom;

            meansz -= 12;

            /* Allocate space, and read them in */
            if(!(mean = (char *)malloc(namesz + meansz + 2)))
                goto doneAtom;

            /* Read them in */
            fseek(fp, 4, SEEK_CUR);
            if(fread(mean, 1, meansz, fp) != meansz)
                goto out_close;

            if(namep >= 0) {
                mean[meansz] = '.';
                fseek(fp, namep + 12, SEEK_SET);
                if(fread(mean + meansz + 1, 1, namesz, fp) != namesz)
                    goto out_close;
            }

            mean[meansz + namesz + 1] = 0;

            /* Go back to the normal flow of things... */
            fseek(fp, pos + atomread, SEEK_SET);
        }

        /* Now that we have that, fetch the data */
        if(find_atom(ST_AtomData, fp, atomsz - atomread, &atomsz2) < 0)
            goto doneAtom;

        /* Skip the first 8 bytes of any 'data' atom */
        fseek(fp, 8, SEEK_CUR);
        atomsz2 -= 16;

        if(atomsz2 & 0xFFFFFFFF00000000ULL)
            atomsz2 -= 8;

        /* Save the data in our dictionary */
        if(!(tmp = (uint8_t *)malloc(atomsz2)))
            goto out_close;

        if(fread(tmp, 1, atomsz2, fp) != atomsz2)
            goto out_close;

        if(fourcc != ST_AtomCoverArt) {
            if(ST_M4A_addAtom(tag, fourcc, mean, tmp, atomsz2,
                              1) != ST_Error_None)
                goto out_close;
        }
        else {
            if(!(pic = ST_Picture_create()))
                goto out_close;

            if(ST_Picture_setData(pic, tmp, (uint32_t)atomsz2,
                                  1) != ST_Error_None)
                goto out_close;

            if(ST_M4A_addPicture(tag, pic) != ST_Error_None)
                goto out_close;
        }

    doneAtom:
        fseek(fp, (long)(pos + atomsz), SEEK_SET);
        ilstsz -= atomsz;
        tmp = NULL;

        if(mean) {
            free(mean);
            mean = NULL;
        }
    }

    fclose(fp);
    return 0;

out_close:
    free(mean);
    free(tmp);
    fclose(fp);
    return -1;
}
