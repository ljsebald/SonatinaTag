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

#ifndef SonatinaTag__Tags__ID3v2_h
#define SonatinaTag__Tags__ID3v2_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Picture.h>
#include <SonatinaTag/Dictionary.h>
#include <SonatinaTag/Tags/ID3v2Frame.h>

/* Opaque ID3v2 tag structure */
struct ST_ID3v2_struct;
typedef struct ST_ID3v2_struct ST_ID3v2;

/* Create a new blank ID3v2 tag. */
ST_FUNC ST_ID3v2 *ST_ID3v2_create(void);

/* Free an ID3v2 tag. */
ST_FUNC void ST_ID3v2_free(ST_ID3v2 *tag);

/* Create a new ID3v2 tag, reading from a file. */
ST_FUNC ST_ID3v2 *ST_ID3v2_createFromFile(const char *fn);

/* Retrieve an arbitrary frame from the tag. */
ST_FUNC const ST_Frame *ST_ID3v2_frameForKey(const ST_ID3v2 *tag,
                                             ST_ID3v2_FrameCode code,
                                             int index);

/* Retrieve the number of values for an arbitrary frame type. */
ST_FUNC int ST_ID3v2_frameCountForKey(const ST_ID3v2 *tag,
                                      ST_ID3v2_FrameCode code);

/* Retrieve a dictionary of all of the frames in the tag. Each of the entries in
   this will be of type ST_ID3v2_Frame (or one of its descendants). */
ST_FUNC const ST_Dict *ST_ID3v2_frameDictionary(const ST_ID3v2 *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* Retrieve a dictionary of string representations of all of the frames in this
   tag. Each of the entries will be a CFArray of CFStringRef objects. You are
   responsible for releasing the dictionary when you're done with it. */
ST_FUNC CFDictionaryRef ST_ID3v2_copyDictionary(const ST_ID3v2 *tag);
#endif

/* Add an arbitrary key-value pair to the frames in the tag. The tag takes
   ownership of the frame. */
ST_FUNC ST_Error ST_ID3v2_addFrame(ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                   ST_Frame *frame);

/* Remove a key-value pair from the frames in the tag. */
ST_FUNC ST_Error ST_ID3v2_removeFrame(ST_ID3v2 *tag, ST_ID3v2_FrameCode code,
                                      int index);

/* Accessors. You must provide a buffer to copy the string into and the length
   of that buffer. If the length of the actual string is greater than the size
   of the buffer, the string WILL NOT be NUL terminated. You must handle the
   encoding of the strings properly! */
ST_FUNC ST_Error ST_ID3v2_title(const ST_ID3v2 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v2_artist(const ST_ID3v2 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v2_album(const ST_ID3v2 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v2_comment(const ST_ID3v2 *tag, uint8_t *buf,
                                  size_t len);
ST_FUNC ST_Error ST_ID3v2_date(const ST_ID3v2 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v2_genre(const ST_ID3v2 *tag, uint8_t *buf, size_t len);

ST_FUNC int ST_ID3v2_track(const ST_ID3v2 *tag);
ST_FUNC int ST_ID3v2_disc(const ST_ID3v2 *tag);

ST_FUNC const ST_Picture *ST_ID3v2_picture(const ST_ID3v2 *tag,
                                           ST_PictureType pt, int index);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_ID3v2_copyTitle(const ST_ID3v2 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v2_copyArtist(const ST_ID3v2 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v2_copyAlbum(const ST_ID3v2 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v2_copyComment(const ST_ID3v2 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v2_copyDate(const ST_ID3v2 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v2_copyGenre(const ST_ID3v2 *tag, ST_Error *err);
#endif

/* Return the size of a value in the tag, in bytes. This does not include any
   sort of NUL terminator (so, treat it like strlen() basically). */
ST_FUNC size_t ST_ID3v2_titleLength(const ST_ID3v2 *tag);
ST_FUNC size_t ST_ID3v2_artistLength(const ST_ID3v2 *tag);
ST_FUNC size_t ST_ID3v2_albumLength(const ST_ID3v2 *tag);
ST_FUNC size_t ST_ID3v2_commentLength(const ST_ID3v2 *tag);
ST_FUNC size_t ST_ID3v2_dateLength(const ST_ID3v2 *tag);
ST_FUNC size_t ST_ID3v2_genreLength(const ST_ID3v2 *tag);

/* Return the encoding of a value in the tag. */
ST_FUNC ST_TextEncoding ST_ID3v2_titleEncoding(const ST_ID3v2 *tag);
ST_FUNC ST_TextEncoding ST_ID3v2_artistEncoding(const ST_ID3v2 *tag);
ST_FUNC ST_TextEncoding ST_ID3v2_albumEncoding(const ST_ID3v2 *tag);
ST_FUNC ST_TextEncoding ST_ID3v2_commentEncoding(const ST_ID3v2 *tag);
ST_FUNC ST_TextEncoding ST_ID3v2_dateEncoding(const ST_ID3v2 *tag);
ST_FUNC ST_TextEncoding ST_ID3v2_genreEncoding(const ST_ID3v2 *tag);

/* Mutators. These all copy the strings, so its still your responsibility to
   clean up the values you pass in. */
ST_FUNC ST_Error ST_ID3v2_setTitle(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v2_setArtist(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                    ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v2_setAlbum(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v2_setComment(ST_ID3v2 *tag, const uint8_t *v,
                                     size_t len, ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v2_setDate(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v2_setGenre(ST_ID3v2 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based mutators. These functions all make copies of the strings
   passed in (at least conceptually), so you still have to clean them up. The
   strings passed in will be converted to UTF-8 (ID3v2.4) or UTF-16 (ID3v2.2 or
   v2.3). */
ST_FUNC ST_Error ST_ID3v2_setTitleStr(ST_ID3v2 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v2_setArtistStr(ST_ID3v2 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v2_setAlbumStr(ST_ID3v2 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v2_setCommentStr(ST_ID3v2 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v2_setDateStr(ST_ID3v2 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v2_setGenreStr(ST_ID3v2 *tag, CFStringRef str);
#endif

/* Add a picture to the tag. The tag takes ownership of the picture passed in,
   so you mustn't free it yourself! */
ST_FUNC ST_Error ST_ID3v2_addPicture(ST_ID3v2 *tag, ST_Picture *p);

/* Remove a picture from the tag. */
ST_FUNC ST_Error ST_ID3v2_removePicture(ST_ID3v2 *tag, ST_PictureType pt,
                                        int index);

ST_END_DECLS

#endif /* !SonatinaTag__Tags__ID3v2_h */
