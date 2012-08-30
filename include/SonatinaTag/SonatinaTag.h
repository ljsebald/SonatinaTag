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

#ifndef SonatinaTag__SonatinaTag_h
#define SonatinaTag__SonatinaTag_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <stddef.h>
#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Error.h>
#include <SonatinaTag/Picture.h>

/* Opaque tag type. All tags are "subclasses" of this type. */
struct ST_Tag_struct;
typedef struct ST_Tag_struct ST_Tag;

/* Retrieve the type of a tag. */
ST_FUNC ST_TagType ST_Tag_type(const ST_Tag *tag);

/* Create a tag from a file. This will attempt to automatically determine the
   "best" type of tag to use for the file. */
ST_FUNC ST_Tag *ST_Tag_createFromFile(const char *fn);

ST_FUNC void ST_Tag_free(ST_Tag *tag);

ST_FUNC int ST_Tag_track(const ST_Tag *tag);
ST_FUNC int ST_Tag_disc(const ST_Tag *tag);

ST_FUNC const ST_Picture *ST_Tag_picture(const ST_Tag *tag, ST_PictureType pt,
                                         int index);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_Tag_copyTitle(const ST_Tag *tag, ST_Error *err);
ST_FUNC CFStringRef ST_Tag_copyArtist(const ST_Tag *tag, ST_Error *err);
ST_FUNC CFStringRef ST_Tag_copyAlbum(const ST_Tag *tag, ST_Error *err);
ST_FUNC CFStringRef ST_Tag_copyComment(const ST_Tag *tag, ST_Error *err);
ST_FUNC CFStringRef ST_Tag_copyDate(const ST_Tag *tag, ST_Error *err);
ST_FUNC CFStringRef ST_Tag_copyGenre(const ST_Tag *tag, ST_Error *err);

/* Retrieve a dictionary of all the data in a tag. You are responsible for
   cleaning up the dictionary when you're done with it. */
ST_FUNC CFDictionaryRef ST_Tag_copyDictionary(const ST_Tag *tag);
#endif

ST_END_DECLS

#endif /* !SonatinaTag__SonatinaTag_h */
