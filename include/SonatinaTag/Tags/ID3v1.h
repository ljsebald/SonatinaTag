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

#ifndef SonatinaTag__Tags__ID3v1_h
#define SonatinaTag__Tags__ID3v1_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Error.h>

/* Opaque ID3v1 tag structure */
struct ST_ID3v1_struct;
typedef struct ST_ID3v1_struct ST_ID3v1;

/* Defined ID3v1 genres list */
typedef enum ST_ID3v1_GenreCode_e {
    ID3v1GenreMin = 0,
    ID3v1GenreBlues = 0,
    ID3v1GenreClassicRock,
    ID3v1GenreCountry,
    ID3v1GenreDance,
    ID3v1GenreDisco,
    ID3v1GenreFunk,
    ID3v1GenreGrunge,
    ID3v1GenreHipHop,
    ID3v1GenreJazz,
    ID3v1GenreMetal,
    ID3v1GenreNewAge,
    ID3v1GenreOldies,
    ID3v1GenreOther,
    ID3v1GenrePop,
    ID3v1GenreRandB,
    ID3v1GenreRap,
    ID3v1GenreReggae,
    ID3v1GenreRock,
    ID3v1GenreTechno,
    ID3v1GenreIndustrial,
    ID3v1GenreAlternative,
    ID3v1GenreSka,
    ID3v1GenreDeathMetal,
    ID3v1GenrePranks,
    ID3v1GenreSoundtrack,
    ID3v1GenreEuroTechno,
    ID3v1GenreAmbient,
    ID3v1GenreTripHop,
    ID3v1GenreVocal,
    ID3v1GenreJazzFunk,
    ID3v1GenreFusion,
    ID3v1GenreTrance,
    ID3v1GenreClassical,
    ID3v1GenreInstrumental,
    ID3v1GenreAcid,
    ID3v1GenreHouse,
    ID3v1GenreGame,
    ID3v1GenreSoundClip,
    ID3v1GenreGospel,
    ID3v1GenreNoise,
    ID3v1GenreAlternRock,
    ID3v1GenreBass,
    ID3v1GenreSoul,
    ID3v1GenrePunk,
    ID3v1GenreSpace,
    ID3v1GenreMeditative,
    ID3v1GenreInstrumentalPop,
    ID3v1GenreInstrumentalRock,
    ID3v1GenreEthnic,
    ID3v1GenreGothic,
    ID3v1GenreDarkwave,
    ID3v1GenreTechnoIndustrial,
    ID3v1GenreElectronic,
    ID3v1GenrePopFolk,
    ID3v1GenreEurodance,
    ID3v1GenreDream,
    ID3v1GenreSouthernRock,
    ID3v1GenreComedy,
    ID3v1GenreCult,
    ID3v1GenreGangsta,
    ID3v1GenreTop40,
    ID3v1GenreChristianRap,
    ID3v1GenrePopFunk,
    ID3v1GenreJungle,
    ID3v1GenreNativeAmerican,
    ID3v1GenreCabaret,
    ID3v1GenreNewWave,
    ID3v1GenrePsychadelic,
    ID3v1GenreRave,
    ID3v1GenreShowtunes,
    ID3v1GenreTrailer,
    ID3v1GenreLoFi,
    ID3v1GenreTribal,
    ID3v1GenreAcidPunk,
    ID3v1GenreAcidJazz,
    ID3v1GenrePolka,
    ID3v1GenreRetro,
    ID3v1GenreMusical,
    ID3v1GenreRockAndRoll,
    ID3v1GenreHardRock,

    /* The rest of these were Winamp extensions */
    ID3v1GenreFolk,
    ID3v1GenreFolkRock,
    ID3v1GenreNationalFolk,
    ID3v1GenreSwing,
    ID3v1GenreFastFusion,
    ID3v1GenreBebob,
    ID3v1GenreLatin,
    ID3v1GenreRevival,
    ID3v1GenreCeltic,
    ID3v1GenreBluegrass,
    ID3v1GenreAvantgarde,
    ID3v1GenreGothicRock,
    ID3v1GenreProgressiveRock,
    ID3v1GenrePsychadelicRock,
    ID3v1GenreSymphonicRock,
    ID3v1GenreSlowRock,
    ID3v1GenreBigBand,
    ID3v1GenreChorus,
    ID3v1GenreEasyListening,
    ID3v1GenreAcoustic,
    ID3v1GenreHumour,
    ID3v1GenreSpeech,
    ID3v1GenreChanson,
    ID3v1GenreOpera,
    ID3v1GenreChamberMusic,
    ID3v1GenreSonata,
    ID3v1GenreSymphony,
    ID3v1GenreBootyBass,
    ID3v1GenrePrimus,
    ID3v1GenrePornGroove,
    ID3v1GenreSatire,
    ID3v1GenreSlowJam,
    ID3v1GenreClub,
    ID3v1GenreTango,
    ID3v1GenreSamba,
    ID3v1GenreFolklore,
    ID3v1GenreBallad,
    ID3v1GenrePowerBallad,
    ID3v1GenreRhythmicSoul,
    ID3v1GenreFreestyle,
    ID3v1GenreDuet,
    ID3v1GenrePunkRock,
    ID3v1GenreDrumSolo,
    ID3v1GenreAcapella,
    ID3v1GenreEuroHouse,
    ID3v1GenreDanceHall,
    ID3v1GenreGoa,
    ID3v1GenreDrumAndBass,
    ID3v1GenreClubHouse,
    ID3v1GenreHardcore,
    ID3v1GenreTerror,
    ID3v1GenreIndie,
    ID3v1GenreBritPop,
    ID3v1GenreNegerpunk,
    ID3v1GenrePolskPunk,
    ID3v1GenreBeat,
    ID3v1GenreChristianGangsta,
    ID3v1GenreHeavyMetal,
    ID3v1GenreBlackMetal,
    ID3v1GenreCrossover,
    ID3v1GenreContemporary,
    ID3v1GenreChristianRock,
    ID3v1GenreMerengue,
    ID3v1GenreSalsa,
    ID3v1GenreThrashMetal,
    ID3v1GenreAnime,
    ID3v1GenreJPop,
    ID3v1GenreSynthPop,
    ID3v1GenreMax = ID3v1GenreSynthPop,
    ID3v1GenreError = -1
} ST_ID3v1_GenreCode;

/* Create a new blank ID3v1 tag. */
ST_FUNC ST_ID3v1 *ST_ID3v1_create(void);

/* Free an ID3v1 tag. */
ST_FUNC void ST_ID3v1_free(ST_ID3v1 *tag);

/* Create a new ID3v1 tag, reading from a file. */
ST_FUNC ST_ID3v1 *ST_ID3v1_createFromFile(const char *fn);

/* Write an ID3v1 tag to the specified file. This will place the tag at the
   end of the file, overwriting any existing ID3v1 tags that may be there. */
ST_FUNC ST_Error ST_ID3v1_writeToFile(const ST_ID3v1 *tag, const char *fn);

/* Accessors. You must provide a buffer to copy the string into and the length
   of that buffer. If the length of the actual string is greater than the size
   of the buffer, the string WILL NOT be NUL terminated. Also, all strings in
   ID3v1 are ISO-8859-1, always. */
ST_FUNC ST_Error ST_ID3v1_title(const ST_ID3v1 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v1_artist(const ST_ID3v1 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v1_album(const ST_ID3v1 *tag, uint8_t *buf, size_t len);
ST_FUNC ST_Error ST_ID3v1_comment(const ST_ID3v1 *tag, uint8_t *buf,
                                  size_t len);
ST_FUNC ST_Error ST_ID3v1_year(const ST_ID3v1 *tag, uint8_t *buf, size_t len);

ST_FUNC ST_ID3v1_GenreCode ST_ID3v1_genre(const ST_ID3v1 *tag);
ST_FUNC int ST_ID3v1_track(const ST_ID3v1 *tag);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based accessors. These functions will create CFStringRef
   objects for the given data. These accessors follow the "Create Rule" with
   regard to memory management (meaning you must call CFRelease on them).

   Note that these functions MAY return NULL, if the tag has not had a given
   attribute set. Check the value returned in *err to see if there was an error
   in that case. */
ST_FUNC CFStringRef ST_ID3v1_copyTitle(const ST_ID3v1 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v1_copyArtist(const ST_ID3v1 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v1_copyAlbum(const ST_ID3v1 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v1_copyComment(const ST_ID3v1 *tag, ST_Error *err);
ST_FUNC CFStringRef ST_ID3v1_copyYear(const ST_ID3v1 *tag, ST_Error *err);
#endif

/* Return the size of a value in the tag, in bytes. This does not include any
   sort of NUL terminator (so, treat it like strlen() basically). */
ST_FUNC size_t ST_ID3v1_titleLength(const ST_ID3v1 *tag);
ST_FUNC size_t ST_ID3v1_artistLength(const ST_ID3v1 *tag);
ST_FUNC size_t ST_ID3v1_albumLength(const ST_ID3v1 *tag);
ST_FUNC size_t ST_ID3v1_commentLength(const ST_ID3v1 *tag);
ST_FUNC size_t ST_ID3v1_yearLength(const ST_ID3v1 *tag);

/* Mutators. These all copy the strings, so its still your responsibility to
   clean up the values you pass in. You must pass in ISO-8859-1 strings, as that
   is all that ID3v1 actually supports. */
ST_FUNC ST_Error ST_ID3v1_setTitle(ST_ID3v1 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v1_setArtist(ST_ID3v1 *tag, const uint8_t *v, size_t len,
                                    ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v1_setAlbum(ST_ID3v1 *tag, const uint8_t *v, size_t len,
                                   ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v1_setComment(ST_ID3v1 *tag, const uint8_t *v,
                                     size_t len, ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v1_setYear(ST_ID3v1 *tag, const uint8_t *v, size_t len,
                                  ST_TextEncoding e);
ST_FUNC ST_Error ST_ID3v1_setGenre(ST_ID3v1 *tag, ST_ID3v1_GenreCode v);
ST_FUNC ST_Error ST_ID3v1_setTrack(ST_ID3v1 *tag, int v);

#ifdef ST_HAVE_COREFOUNDATION
/* CoreFoundation-based mutators. These functions all make copies of the strings
   passed in (at least conceptually), so you still have to clean them up. You
   can pass in any encoding you want, but be warned it will be harshly converted
   to ISO-8859-1 (since that's all ID3v1 actually supports). Note that harshly,
   in this case, means that if you pass something that can't be converted, these
   functions WILL fail (and return ST_Error_InvalidEncoding). */
ST_FUNC ST_Error ST_ID3v1_setTitleStr(ST_ID3v1 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v1_setArtistStr(ST_ID3v1 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v1_setAlbumStr(ST_ID3v1 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v1_setCommentStr(ST_ID3v1 *tag, CFStringRef str);
ST_FUNC ST_Error ST_ID3v1_setYearStr(ST_ID3v1 *tag, CFStringRef str);
#endif

/* Translate an ID3v1 Genre Code into a human-readable string. These are all
   in English (in whatever native encoding the compiler supports), so you need
   to provide your own translations if you want them. */
ST_FUNC const char *ST_ID3v1_stringForGenre(ST_ID3v1_GenreCode genre);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation string for a given genre code. Like the
   ST_ID3v1_stringForGenre() function, these will all be in English. You are
   responsible for releasing the string when you're done with it. */
ST_FUNC CFStringRef ST_ID3v1_createStringForGenre(ST_ID3v1_GenreCode g);

/* Create an array containing CoreFoundation strings for all genre codes. As
   with the other related functions, all strings will be in English. You are
   responsible for releasing the array when you're done with it. */
ST_FUNC CFArrayRef ST_ID3v1_copyGenres(void);

/* Create a CoreFoundation dictionary of all the data in the tag. You are
   responsible for cleaning up after you're done with the dictionary. */
ST_FUNC CFDictionaryRef ST_ID3v1_copyDictionary(const ST_ID3v1 *tag);
#endif

ST_END_DECLS

#endif /* !SonatinaTag__Tags__ID3v1_h */
