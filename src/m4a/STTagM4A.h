/*
    SonatinaTag
    Copyright (C) 2010 Lawrence Sebald

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

#ifndef STTagM4A_h
#define STTagM4A_h

#import <Foundation/Foundation.h>

#include <SonatinaTag/STTag.h>

typedef enum STTagM4A_AtomCode_e {
    AtomAlbum = '\251alb',
    AtomAlbumArtist = 'aART',
    AtomAppleID = 'apID', 
    AtomArtist = '\251ART',
    AtomCategory = 'catg',
    AtomComment = '\251cmt',
    AtomComposer = '\251wrt',
    AtomCopyright = 'cprt',
    AtomCoverArt = 'covr',
    AtomDescription = 'desc',
    AtomDiscNumber = 'disk',
    AtomEncoder = '\251too',
    AtomEpisodeGUID = 'egid',
    AtomGenre = '\251gen',
    AtomGenreID = 'gnre',
    AtomGrouping = '\251grp',
    AtomKeyword = 'keyw',
    AtomPartOfCompilation = 'cpil',
    AtomPartOfGaplessAlbum = 'pgap',
    AtomPodcast = 'pcst',
    AtomPodcastURL = 'purl',
    AtomPurchaseDate = 'purd',
    AtomRating = 'rtng',
    AtomSortAlbum = 'soal',
    AtomSortAlbumArtist = 'soaa',
    AtomSortArtist = 'soar',
    AtomSortComposer = 'soco',
    AtomSortShow = 'sosn',
    AtomSortTitle = 'sonm',
    AtomTempo = 'tmpo',
    AtomTitle = '\251nam',
    AtomTrackNumber = 'trkn',
    AtomTVEpisodeID = 'tven',
    AtomTVEpisodeNumber = 'tves',
    AtomTVNetworkName = 'tvnn',
    AtomTVSeason = 'tvsn',
    AtomTVShowName = 'tvsh',
    AtomUnsyncLyrics = '\251lyr',
    AtomYear = '\251day'
} STTagM4A_AtomCode;

@interface STTagM4A : NSObject<STTag> {
@private
    NSMutableDictionary *_atoms;
}

- (id)initFromFile:(NSString *)filename;
- (void)dealloc;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;

- (NSString *)genre;
- (int)trackNumber;
- (int)discNumber;

- (NSData *)atomForKey:(uint32_t)fourcc;

@end /* @interface STTagM4A */

#endif /* !STTagM4A_h */
