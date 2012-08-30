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

#ifndef SonatinaTag__Tags__M4A_h
#define SonatinaTag__Tags__M4A_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Picture.h>
#include <SonatinaTag/Dictionary.h>

typedef enum ST_M4A_AtomCode_e {
    ST_AtomAlbum                = ST_4CC('\251', 'a', 'l', 'b'),
    ST_AtomAlbumArtist          = ST_4CC('a', 'A', 'R', 'T'),
    ST_AtomAppleID              = ST_4CC('a', 'p', 'I', 'D'), 
    ST_AtomArtist               = ST_4CC('\251', 'A', 'R', 'T'),
    ST_AtomCategory             = ST_4CC('c', 'a', 't', 'g'),
    ST_AtomComment              = ST_4CC('\251', 'c', 'm', 't'),
    ST_AtomComposer             = ST_4CC('\251', 'w', 'r', 't'),
    ST_AtomCopyright            = ST_4CC('c', 'p', 'r', 't'),
    ST_AtomCoverArt             = ST_4CC('c', 'o', 'v', 'r'),
    ST_AtomDescription          = ST_4CC('d', 'e', 's', 'c'),
    ST_AtomDiscNumber           = ST_4CC('d', 'i', 's', 'k'),
    ST_AtomEncoder              = ST_4CC('\251', 't', 'o', 'o'),
    ST_AtomEpisodeGUID          = ST_4CC('e', 'g', 'i', 'd'),
    ST_AtomGenre                = ST_4CC('\251', 'g', 'e', 'n'),
    ST_AtomGenreID              = ST_4CC('g', 'n', 'r', 'e'),
    ST_AtomGrouping             = ST_4CC('\251', 'g', 'r', 'p'),
    ST_AtomKeyword              = ST_4CC('k', 'e', 'y', 'w'),
    ST_AtomPartOfCompilation    = ST_4CC('c', 'p', 'i', 'l'),
    ST_AtomPartOfGaplessAlbum   = ST_4CC('p', 'g', 'a', 'p'),
    ST_AtomPodcast              = ST_4CC('p', 'c', 's', 't'),
    ST_AtomPodcastURL           = ST_4CC('p', 'u', 'r', 'l'),
    ST_AtomPurchaseDate         = ST_4CC('p', 'u', 'r', 'd'),
    ST_AtomRating               = ST_4CC('r', 't', 'n', 'g'),
    ST_AtomSortAlbum            = ST_4CC('s', 'o', 'a', 'l'),
    ST_AtomSortAlbumArtist      = ST_4CC('s', 'o', 'a', 'a'),
    ST_AtomSortArtist           = ST_4CC('s', 'o', 'a', 'r'),
    ST_AtomSortComposer         = ST_4CC('s', 'o', 'c', 'o'),
    ST_AtomSortShow             = ST_4CC('s', 'o', 's', 'n'),
    ST_AtomSortTitle            = ST_4CC('s', 'o', 'n', 'm'),
    ST_AtomTempo                = ST_4CC('t', 'm', 'p', 'o'),
    ST_AtomTitle                = ST_4CC('\251', 'n', 'a', 'm'),
    ST_AtomTrackNumber          = ST_4CC('t', 'r', 'k', 'n'),
    ST_AtomTVEpisodeID          = ST_4CC('t', 'v', 'e', 'n'),
    ST_AtomTVEpisodeNumber      = ST_4CC('t', 'v', 'e', 's'),
    ST_AtomTVNetworkName        = ST_4CC('t', 'v', 'n', 'n'),
    ST_AtomTVSeason             = ST_4CC('t', 'v', 's', 'n'),
    ST_AtomTVShowName           = ST_4CC('t', 'v', 's', 'h'),
    ST_AtomUnsyncLyrics         = ST_4CC('\251', 'l', 'y', 'r'),
    ST_AtomYear                 = ST_4CC('\251','d', 'a', 'y'),
    ST_AtomLongName             = ST_4CC('-', '-', '-', '-'),

    /* These ones are used internally. You shouldn't need them yourself. */
    ST_AtomFreeSpace            = ST_4CC('f', 'r', 'e', 'e'),
    ST_AtomData                 = ST_4CC('d', 'a', 't', 'a'),
    ST_AtomMovieData            = ST_4CC('m', 'o', 'o', 'v'),
    ST_AtomUserData             = ST_4CC('u', 'd', 't', 'a'),
    ST_AtomItemList             = ST_4CC('i', 'l', 's', 't'),
    ST_AtomFileType             = ST_4CC('f', 't', 'y', 'p'),
    ST_AtomMetadata             = ST_4CC('m', 'e', 't', 'a'),
    ST_AtomMeaning              = ST_4CC('m', 'e', 'a', 'n'),
    ST_AtomName                 = ST_4CC('n', 'a', 'm', 'e')
} ST_M4A_AtomCode;

/* Opaque M4A tag structure */
struct ST_M4A_struct;
typedef struct ST_M4A_struct ST_M4A;

struct ST_M4A_Atom_struct;
typedef struct ST_M4A_Atom_struct ST_M4A_Atom;

/* Create a new blank M4A tag. */
ST_FUNC ST_M4A *ST_M4A_create(void);

/* Free a M4A tag. */
ST_FUNC void ST_M4A_free(ST_M4A *tag);

/* Create a new M4A tag, reading from a file. */
ST_FUNC ST_M4A *ST_M4A_createFromFile(const char *fn);

/* Retrieve the value of an arbitrary atom from the tag. Cover art atoms are
   only accessible with the special function for them (ST_M4A_picture). */
ST_FUNC ST_Error ST_M4A_atomForKey(const ST_M4A *tag, ST_M4A_AtomCode code,
                                   int index, uint8_t *buf, size_t len);

#ifdef ST_HAVE_COREFOUNDATION
/* Retrieve the value of an arbitrary atom from the tag as a CoreFoundation
   string. It is only safe to use this with atoms containing UTF-8 strings. */
ST_FUNC CFStringRef ST_M4A_copyAtomForKey(const ST_M4A *tag,
                                          ST_M4A_AtomCode code, int index,
                                          ST_Error *err);
#endif

/* Retrieve the number of values for an arbitrary atom type. */
ST_FUNC int ST_M4A_atomCountForKey(const ST_M4A *tag, ST_M4A_AtomCode code);

/* Retrieve the length (in bytes) of an arbitrary atom from the tag. This count
   DOES NOT include any sort of NUL terminator if applicable. */
ST_FUNC size_t ST_M4A_atomLengthForKey(const ST_M4A *tag, ST_M4A_AtomCode code,
                                       int index);

/* Retrieve a dictionary of all of the atoms in the tag. Each of the entries in
   this will be a pointer to a ST_M4A_Atom. */
ST_FUNC const ST_Dict *ST_M4A_atomDictionary(const ST_M4A *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation dictionary of all the atoms in the tag. Keys will be
   CFStringRefs and values will be CFArrays of CFStringRefs. You are responsible
   for cleaning up the dictionary when you're done with it. */
ST_FUNC CFDictionaryRef ST_M4A_copyDictionary(const ST_M4A *tag);
#endif

/* Add an arbitrary key-value pair to the atoms in the tag. */
ST_FUNC ST_Error ST_M4A_addAtom(ST_M4A *tag, ST_M4A_AtomCode code,
                                const char *lname, uint8_t *value, size_t len,
                                int ownbuf);

#ifdef ST_HAVE_COREFOUNDATION
/* Add an arbitrary key-value pair to the atoms in the tag, with the value
   represented as a CoreFoundation string. */
ST_FUNC ST_Error ST_M4A_addAtomStr(ST_M4A *tag, ST_M4A_AtomCode code,
                                   const char *lname, CFStringRef value);

/* Same as the above, but with a CFData */
ST_FUNC ST_Error ST_M4A_addAtomData(ST_M4A *tag, ST_M4A_AtomCode code,
                                    const char *lname, CFDataRef value);
#endif

/* Remove a key-value pair from the atoms in the tag. */
ST_FUNC ST_Error ST_M4A_removeAtom(ST_M4A *tag, ST_M4A_AtomCode code,
                                   int index);

/* Accessors. You must provide a buffer to copy the string into and the length
   of that buffer. If the length of the actual string is greater than the size
   of the buffer, the string WILL NOT be NUL terminated. Also, all strings in
   M4A files should be UTF-8. */
ST_FUNC ST_Error ST_M4A_title(const ST_M4A *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_M4A_artist(const ST_M4A *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_M4A_album(const ST_M4A *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_M4A_comment(const ST_M4A *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_M4A_date(const ST_M4A *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_M4A_genre(const ST_M4A *tag, uint8_t *buf, size_t len);

ST_FUNC int ST_M4A_track(const ST_M4A *tag);
ST_FUNC int ST_M4A_disc(const ST_M4A *tag);

ST_FUNC const ST_Picture *ST_M4A_picture(const ST_M4A *tag, int index);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_M4A_copyTitle(const ST_M4A *tag, ST_Error *err);
ST_FUNC CFStringRef ST_M4A_copyArtist(const ST_M4A *tag, ST_Error *err);
ST_FUNC CFStringRef ST_M4A_copyAlbum(const ST_M4A *tag, ST_Error *err);
ST_FUNC CFStringRef ST_M4A_copyComment(const ST_M4A *tag, ST_Error *err);
ST_FUNC CFStringRef ST_M4A_copyDate(const ST_M4A *tag, ST_Error *err);
ST_FUNC CFStringRef ST_M4A_copyGenre(const ST_M4A *tag, ST_Error *err);
#endif

/* Return the size of a value in the tag, in bytes. This does not include any
   sort of NUL terminator (so, treat it like strlen() basically). */
ST_FUNC size_t ST_M4A_titleLength(const ST_M4A *tag);
ST_FUNC size_t ST_M4A_artistLength(const ST_M4A *tag);
ST_FUNC size_t ST_M4A_albumLength(const ST_M4A *tag);
ST_FUNC size_t ST_M4A_commentLength(const ST_M4A *tag);
ST_FUNC size_t ST_M4A_dateLength(const ST_M4A *tag);
ST_FUNC size_t ST_M4A_genreLength(const ST_M4A *tag);

/* Mutators. These all copy the strings, so its still your responsibility to
   clean up the values you pass in. */
ST_FUNC ST_Error ST_M4A_setTitle(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);
ST_FUNC ST_Error ST_M4A_setArtist(ST_M4A *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_M4A_setAlbum(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);
ST_FUNC ST_Error ST_M4A_setComment(ST_M4A *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_M4A_setDate(ST_M4A *tag, const uint8_t *v, size_t len,
                                ST_TextEncoding e);
ST_FUNC ST_Error ST_M4A_setGenre(ST_M4A *tag, const uint8_t *v, size_t len,
                                 ST_TextEncoding e);

ST_FUNC ST_Error ST_M4A_setTrack(ST_M4A *tag, int v);
ST_FUNC ST_Error ST_M4A_setDisc(ST_M4A *tag, int v);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based mutators. These all copy the strings, so its still your
   responsibility to clean up the values you pass in. */
ST_FUNC ST_Error ST_M4A_setTitleStr(ST_M4A *tag, CFStringRef v);
ST_FUNC ST_Error ST_M4A_setArtistStr(ST_M4A *tag, CFStringRef v);
ST_FUNC ST_Error ST_M4A_setAlbumStr(ST_M4A *tag, CFStringRef v);
ST_FUNC ST_Error ST_M4A_setCommentStr(ST_M4A *tag, CFStringRef v);
ST_FUNC ST_Error ST_M4A_setDateStr(ST_M4A *tag, CFStringRef v);
ST_FUNC ST_Error ST_M4A_setGenreStr(ST_M4A *tag, CFStringRef v);
#endif

/* Add or remove a picture from the tag. This takes ownership of the ST_Picture
   object. */
ST_FUNC ST_Error ST_M4A_addPicture(ST_M4A *tag, ST_Picture *picture);
ST_FUNC ST_Error ST_M4A_removePicture(ST_M4A *tag, int index);

/* Accessors for the information in a M4A Atom. You must not change any of the
   values returned by the these functions. */
ST_FUNC const char *ST_M4A_Atom_longName(const ST_M4A_Atom *atom);
ST_FUNC const uint8_t *ST_M4A_Atom_data(const ST_M4A_Atom *atom);
ST_FUNC size_t ST_M4A_Atom_length(const ST_M4A_Atom *atom);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessor for M4A Atom data. This assumes the atom
   actually contains UTF-8 string data. */
ST_FUNC CFStringRef ST_M4A_Atom_copyDataStr(const ST_M4A_Atom *atom);

/* The same as above, but as a CFData instead of a CFString */
ST_FUNC CFDataRef ST_M4A_Atom_copyData(const ST_M4A_Atom *atom);
#endif

ST_END_DECLS

#endif /* !SonatinaTag__Tags__M4A_h */
