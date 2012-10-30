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

#ifndef SonatinaTag__Tags__APE_h
#define SonatinaTag__Tags__APE_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <stdint.h>
#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Dictionary.h>

/* Opaque APE tag structure */
struct ST_APE_struct;
typedef struct ST_APE_struct ST_APE;

/* Opaque APE tag item structure */
struct ST_APE_item_struct;
typedef struct ST_APE_item_struct ST_APE_item;

/* Get the length of data in a APE tag item. */
ST_FUNC size_t ST_APE_item_length(const ST_APE_item *c);

/* Get the flags from an APE tag item. */
ST_FUNC uint32_t ST_APE_item_flags(const ST_APE_item *c);

/* Get the data from a APE tag item. You must not change the value returned by
   this function! */
ST_FUNC const uint8_t *ST_APE_item_data(const ST_APE_item *c);

/* Create a new blank APE tag. */
ST_FUNC ST_APE *ST_APE_create(void);

/* Free a APE tag. */
ST_FUNC void ST_APE_free(ST_APE *tag);

/* Create a new APE tag, reading from a file. */
ST_FUNC ST_APE *ST_APE_createFromFile(const char *fn);

/* Retrieve the value of an arbitrary item from the tag. */
ST_FUNC ST_Error ST_APE_itemForKey(const ST_APE *tag, const char *key,
                                   uint8_t *buf, size_t len);

#ifdef ST_HAVE_COREFOUNDATION
/* Retrieve a CoreFoundation string holding the value of an arbitrary item from
   the tag. You are responsible for releasing the returned string when you're
   done with it. */
ST_FUNC CFStringRef ST_APE_copyItemForKey(const ST_APE *tag, const char *key,
                                          ST_Error *err);
#endif

/* Retrieve the length (in bytes) of the value for an arbitrary tag item. This
   does not include any sort of NUL terminators, if applicable. */
ST_FUNC size_t ST_APE_itemLengthForKey(const ST_APE *tag, const char *key);

/* Retrieve a dictionary of all of the items in the tag. These are pointers to a
   ST_APE_item. */
ST_FUNC const ST_Dict *ST_APE_itemDictionary(const ST_APE *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation dictionary of all the tag items in the tag. Keys will
   be CFStringRefs and values will be CFArrays of CFStringRefs. You are
   responsible for cleaning up the dictionary when you're done with it. */
ST_FUNC CFDictionaryRef ST_APE_copyDictionary(const ST_APE *tag);
#endif

/* Add an arbitrary key-value pair to the items in the tag. */
ST_FUNC ST_Error ST_APE_addItem(ST_APE *tag, const char *key,
                                const uint8_t *value, size_t len,
                                uint32_t flags);

#ifdef ST_HAVE_COREFOUNDATION
/* Add an arbitrary key-value pair to the items in the tag (with a
   CoreFoundation string value). */
ST_FUNC ST_Error ST_APE_addItemStr(ST_APE *tag, const char *key,
                                   CFStringRef value, uint32_t flags);
#endif

/* Remove a key-value pair from the items in the tag. */
ST_FUNC ST_Error ST_APE_removeItem(ST_APE *tag, const char *key);

/* Accessors. You must provide a buffer to copy the string into and the length
   of that buffer. If the length of the actual string is greater than or equal
   to the size of the buffer, the string WILL NOT be NUL terminated. Also, all
   strings in APEv2 tags are always UTF-8. */
ST_FUNC ST_Error ST_APE_title(const ST_APE *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_APE_artist(const ST_APE *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_APE_album(const ST_APE *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_APE_comment(const ST_APE *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_APE_date(const ST_APE *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_APE_genre(const ST_APE *tag, uint8_t *buf, size_t len);

ST_FUNC int ST_APE_track(const ST_APE *tag);
ST_FUNC int ST_APE_disc(const ST_APE *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_APE_copyTitle(const ST_APE *tag, ST_Error *err);
ST_FUNC CFStringRef ST_APE_copyArtist(const ST_APE *tag, ST_Error *err);
ST_FUNC CFStringRef ST_APE_copyAlbum(const ST_APE *tag, ST_Error *err);
ST_FUNC CFStringRef ST_APE_copyComment(const ST_APE *tag, ST_Error *err);
ST_FUNC CFStringRef ST_APE_copyDate(const ST_APE *tag, ST_Error *err);
ST_FUNC CFStringRef ST_APE_copyGenre(const ST_APE *tag, ST_Error *err);
#endif

/* Return the size of a value in the tag, in bytes. This does not include any
   sort of NUL terminator (so, treat it like strlen() basically). */
ST_FUNC size_t ST_APE_titleLength(const ST_APE *tag);
ST_FUNC size_t ST_APE_artistLength(const ST_APE *tag);
ST_FUNC size_t ST_APE_albumLength(const ST_APE *tag);
ST_FUNC size_t ST_APE_commentLength(const ST_APE *tag);
ST_FUNC size_t ST_APE_dateLength(const ST_APE *tag);
ST_FUNC size_t ST_APE_genreLength(const ST_APE *tag);

/* Mutators. These all copy the strings, so its still your responsibility to
   clean up the values you pass in. You must pass in UTF-8 strings, as that is
   all that APEv2 actually supports in its tags. */
ST_FUNC ST_Error ST_APE_setTitle(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);
ST_FUNC ST_Error ST_APE_setArtist(ST_APE *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_APE_setAlbum(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);
ST_FUNC ST_Error ST_APE_setComment(ST_APE *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_APE_setDate(ST_APE *tag, const uint8_t *v, size_t len,
                                ST_TextEncoding e);
ST_FUNC ST_Error ST_APE_setGenre(ST_APE *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);

ST_FUNC ST_Error ST_APE_setTrack(ST_APE *tag, int v);
ST_FUNC ST_Error ST_APE_setDisc(ST_APE *tag, int v);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based mutators. These functions all make copies of the strings
   passed in (at least conceptually), so you still have to clean them up. You
   can pass in any encoding you want, but be warned it will be harshly converted
   to UTF-8 (since that's all APE actually supports). */
ST_FUNC ST_Error ST_APE_setTitleStr(ST_APE *tag, CFStringRef str);
ST_FUNC ST_Error ST_APE_setArtistStr(ST_APE *tag, CFStringRef str);
ST_FUNC ST_Error ST_APE_setAlbumStr(ST_APE *tag, CFStringRef str);
ST_FUNC ST_Error ST_APE_setCommentStr(ST_APE *tag, CFStringRef str);
ST_FUNC ST_Error ST_APE_setDateStr(ST_APE *tag, CFStringRef str);
ST_FUNC ST_Error ST_APE_setGenreStr(ST_APE *tag, CFStringRef str);
#endif

ST_END_DECLS

#endif /* !SonatinaTag__Tags__FLAC_h */
