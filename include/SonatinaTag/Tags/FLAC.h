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

#ifndef SonatinaTag__Tags__FLAC_h
#define SonatinaTag__Tags__FLAC_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Picture.h>
#include <SonatinaTag/Dictionary.h>

/* Opaque FLAC tag structure */
struct ST_FLAC_struct;
typedef struct ST_FLAC_struct ST_FLAC;

/* Opaque FLAC vorbis comment structure */
struct ST_FLAC_vcomment_struct;
typedef struct ST_FLAC_vcomment_struct ST_FLAC_vcomment;

/* Get the length of data in a FLAC comment. */
ST_FUNC size_t ST_FLAC_vcomment_length(const ST_FLAC_vcomment *c);

/* Get the data from a FLAC comment. You must not change the value returned by
   this function! */
ST_FUNC const uint8_t *ST_FLAC_vcomment_data(const ST_FLAC_vcomment *c);

/* Create a new blank FLAC tag. */
ST_FUNC ST_FLAC *ST_FLAC_create(void);

/* Free a FLAC tag. */
ST_FUNC void ST_FLAC_free(ST_FLAC *tag);

/* Create a new FLAC tag, reading from a file. */
ST_FUNC ST_FLAC *ST_FLAC_createFromFile(const char *fn);

/* Retrieve the value of an arbitrary Vorbis comment from the tag. */
ST_FUNC ST_Error ST_FLAC_commentForKey(const ST_FLAC *tag, const char *key,
                                       int index, uint8_t *buf, size_t len);

#ifdef ST_HAVE_COREFOUNDATION
/* Retrieve a CoreFoundation string holding the value of an arbitrary Vorbis
   comment from the tag. You are responsible for releasing the returned string
   when you're done with it. */
ST_FUNC CFStringRef ST_FLAC_copyCommentForKey(const ST_FLAC *tag,
                                              const char *key, int index,
                                              ST_Error *err);
#endif

/* Retrieve the number of values for an arbitrary Vorbis comment. */
ST_FUNC int ST_FLAC_commentCountForKey(const ST_FLAC *tag, const char *key);

/* Retrieve the length (in bytes) of the value for an arbitrary Vorbis comment.
   This does not include any sort of NUL terminators, if applicable. */
ST_FUNC size_t ST_FLAC_commentLengthForKey(const ST_FLAC *tag, const char *key,
                                           int index);

/* Retrieve a dictionary of all of the Vorbis comments in the tag. These are
   pointers to a ST_FLAC_vcomment. */
ST_FUNC const ST_Dict *ST_FLAC_commentDictionary(const ST_FLAC *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation dictionary of all the Vorbis comments in the tag.
   Keys will be CFStringRefs and values will be CFArrays of CFStringRefs. You
   are responsible for cleaning up the dictionary when you're done with it. */
ST_FUNC CFDictionaryRef ST_FLAC_copyDictionary(const ST_FLAC *tag);
#endif

/* Add an arbitrary key-value pair to the Vorbis comments in the tag. */
ST_FUNC ST_Error ST_FLAC_addComment(ST_FLAC *tag, const char *key,
                                    const uint8_t *value, size_t len);

#ifdef ST_HAVE_COREFOUNDATION
/* Add an arbitrary key-value pair to the Vorbis comments in the tag (with a
   CoreFoundation string value). */
ST_FUNC ST_Error ST_FLAC_addCommentStr(ST_FLAC *tag, const char *key,
                                       CFStringRef value);
#endif

/* Remove a key-value pair from the Vorbis comments in the tag. */
ST_FUNC ST_Error ST_FLAC_removeComment(ST_FLAC *tag, const char *key,
                                       int index);

/* Accessors. You must provide a buffer to copy the string into and the length
   of that buffer. If the length of the actual string is greater than or equal
   to the size of the buffer, the string WILL NOT be NUL terminated. Also, all
   strings in FLAC tags are always UTF-8. */
ST_FUNC ST_Error ST_FLAC_title(const ST_FLAC *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_FLAC_artist(const ST_FLAC *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_FLAC_album(const ST_FLAC *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_FLAC_comment(const ST_FLAC *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_FLAC_date(const ST_FLAC *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_FLAC_genre(const ST_FLAC *tag, uint8_t *buf, size_t len);

ST_FUNC int ST_FLAC_track(const ST_FLAC *tag);
ST_FUNC int ST_FLAC_disc(const ST_FLAC *tag);

ST_FUNC const ST_Picture *ST_FLAC_picture(const ST_FLAC *tag, ST_PictureType pt,
                                          int index);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_FLAC_copyTitle(const ST_FLAC *tag, ST_Error *err);
ST_FUNC CFStringRef ST_FLAC_copyArtist(const ST_FLAC *tag, ST_Error *err);
ST_FUNC CFStringRef ST_FLAC_copyAlbum(const ST_FLAC *tag, ST_Error *err);
ST_FUNC CFStringRef ST_FLAC_copyComment(const ST_FLAC *tag, ST_Error *err);
ST_FUNC CFStringRef ST_FLAC_copyDate(const ST_FLAC *tag, ST_Error *err);
ST_FUNC CFStringRef ST_FLAC_copyGenre(const ST_FLAC *tag, ST_Error *err);
#endif

/* Return the size of a value in the tag, in bytes. This does not include any
   sort of NUL terminator (so, treat it like strlen() basically). */
ST_FUNC size_t ST_FLAC_titleLength(const ST_FLAC *tag);
ST_FUNC size_t ST_FLAC_artistLength(const ST_FLAC *tag);
ST_FUNC size_t ST_FLAC_albumLength(const ST_FLAC *tag);
ST_FUNC size_t ST_FLAC_commentLength(const ST_FLAC *tag);
ST_FUNC size_t ST_FLAC_dateLength(const ST_FLAC *tag);
ST_FUNC size_t ST_FLAC_genreLength(const ST_FLAC *tag);

/* Mutators. These all copy the strings, so its still your responsibility to
   clean up the values you pass in. You must pass in UTF-8 strings, as that is
   all that FLAC actually supports in its tags. */
ST_FUNC ST_Error ST_FLAC_setTitle(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_FLAC_setArtist(ST_FLAC *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_FLAC_setAlbum(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_FLAC_setComment(ST_FLAC *tag, const uint8_t *v, size_t len,
                                    ST_TextEncoding e);
ST_FUNC ST_Error ST_FLAC_setDate(ST_FLAC *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);
ST_FUNC ST_Error ST_FLAC_setGenre(ST_FLAC *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);

ST_FUNC ST_Error ST_FLAC_setTrack(ST_FLAC *tag, int v);
ST_FUNC ST_Error ST_FLAC_setDisc(ST_FLAC *tag, int v);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based mutators. These functions all make copies of the strings
   passed in (at least conceptually), so you still have to clean them up. You
   can pass in any encoding you want, but be warned it will be harshly converted
   to UTF-8 (since that's all FLAC actually supports). */
ST_FUNC ST_Error ST_FLAC_setTitleStr(ST_FLAC *tag, CFStringRef str);
ST_FUNC ST_Error ST_FLAC_setArtistStr(ST_FLAC *tag, CFStringRef str);
ST_FUNC ST_Error ST_FLAC_setAlbumStr(ST_FLAC *tag, CFStringRef str);
ST_FUNC ST_Error ST_FLAC_setCommentStr(ST_FLAC *tag, CFStringRef str);
ST_FUNC ST_Error ST_FLAC_setDateStr(ST_FLAC *tag, CFStringRef str);
ST_FUNC ST_Error ST_FLAC_setGenreStr(ST_FLAC *tag, CFStringRef str);
#endif

/* Add a picture to the tag. The tag takes ownership of the picture passed in,
   so you mustn't free it yourself! */
ST_FUNC ST_Error ST_FLAC_addPicture(ST_FLAC *tag, ST_Picture *p);

/* Remove a picture from the tag. */
ST_FUNC ST_Error ST_FLAC_removePicture(ST_FLAC *tag, ST_PictureType pt,
                                       int index);

ST_END_DECLS

#endif /* !SonatinaTag__Tags__FLAC_h */
