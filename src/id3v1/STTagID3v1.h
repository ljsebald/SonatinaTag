/*
    SonatinaTag
    Copyright (C) 2010, 2011 Lawrence Sebald

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

#ifndef STTagID3v1_h
#define STTagID3v1_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>

typedef enum STTagID3v1_Genre_e {
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
    ID3v1GenreMax = ID3v1GenreSynthPop
} STTagID3v1_Genre;

@interface STTagID3v1 : NSObject<STTag> {
@private
    NSString *_title;
    NSString *_artist;
    NSString *_album;
    NSString *_year;
    NSString *_comment;
    uint8_t _genre;
    uint8_t _track;
}

- (id)init;
- (id)initFromFile:(NSString *)file;
- (void)dealloc;

- (BOOL)writeToFile:(NSString *)file error:(NSError **)err;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;
- (NSData *)artwork;
- (uint8_t)genre;
- (int)trackNumber;
- (int)discNumber;

/* Methods for building up new ID3v1 tags. */
- (void)setTitle:(NSString *)title;
- (void)setArtist:(NSString *)artist;
- (void)setAlbum:(NSString *)album;
- (void)setYear:(NSString *)year;
- (void)setComment:(NSString *)comment;
- (void)setGenre:(uint8_t)genre;
- (void)setTrackNumber:(int)trackNumber;

- (id)id3v2FrameForKey:(STTagID3v2_FrameCode)fc;
- (NSDictionary *)tagDictionary;

- (id<STTagPicture>)artworkOfType:(STTagPictureType)type index:(NSUInteger)i;
- (NSArray *)allArtwork;

- (id<STTagComment>)commentAtIndex:(NSUInteger)i;
- (NSArray *)allComments;

+ (NSArray *)genres;
+ (NSString *)stringForGenre:(STTagID3v1_Genre)genre;

@end /* @interface STTagID3v1 */

#endif /* !STTagID3v1_h */
