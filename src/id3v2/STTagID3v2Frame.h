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

#ifndef STTagID3v2Frame_h
#define STTagID3v2Frame_h

#import <Foundation/Foundation.h>

enum STTagID3v2_FrameCode_e {
    FrameAudioEncryption    = 'AENC',
    FrameAttachedPicture    = 'APIC',
    FrameComments           = 'COMM',
    FrameCommercial         = 'COMR',
    FrameEncryptionMethod   = 'ENCR',
    FrameEqualization       = 'EQUA',
    FrameEventTimingCodes   = 'ETCO',
    FrameGeneralObject      = 'GEOB',
    FrameGroupIdent         = 'GRID',
    FrameInvolvedPeople     = 'IPLS',
    FrameLinkedInfo         = 'LINK',
    FrameMusicCDIdent       = 'MCDI',
    FrameMPEGLocationLUT    = 'MLLT',
    FrameOwnership          = 'OWNE',
    FramePrivate            = 'PRIV',
    FramePlayCounter        = 'PCNT',
    FramePopularimeter      = 'POPM',
    FramePositionSync       = 'POSS',
    FrameRecommendedBufSz   = 'RBUF',
    FrameRelativeVolumeAdj  = 'RVAD',
    FrameReverb             = 'RVRB',
    FrameSyncLyrics         = 'SYLT',
    FrameSyncTempo          = 'SYTC',
    FrameAlbumTitle         = 'TALB',
    FrameBPM                = 'TBPM',
    FrameComposer           = 'TCOM',
    FrameContentType        = 'TCON',
    FrameCopyrightMsg       = 'TCOP',
    FrameDate               = 'TDAT',
    FramePlaylistDelay      = 'TDLY',
    FrameEncodedBy          = 'TENC',
    FrameLyricist           = 'TEXT',
    FrameFileType           = 'TFLT',
    FrameTime               = 'TIME',
    FrameContentGrpDesc     = 'TIT1',
    FrameTitle              = 'TIT2',
    FrameSubtitle           = 'TIT3',
    FrameInitialKey         = 'TKEY',
    FrameLanguage           = 'TLAN',
    FrameLength             = 'TLEN',
    FrameMediaType          = 'TMED',
    FrameOriginalAlbumTitle = 'TOAL',
    FrameOriginalFilename   = 'TOFN',
    FrameOriginalLyricist   = 'TOLY',
    FrameOriginalArtist     = 'TOPE',
    FrameOriginalReleaseYr  = 'TORY',
    FrameOwner              = 'TOWN',
    FrameLeadPerformer      = 'TPE1',
    FrameAccompaniment      = 'TPE2',
    FrameConductor          = 'TPE3',
    FrameModifiedBy         = 'TPE4',
    FramePartOfSet          = 'TPOS',
    FramePublisher          = 'TPUB',
    FrameTrackNumber        = 'TRCK',
    FrameRecordingDates     = 'TRDA',
    FrameRadioStationName   = 'TRSN',
    FrameRadioStationOwner  = 'TRSO',
    FrameSize               = 'TSIZ',
    FrameISRC               = 'TSRC',
    FrameEncodingSettings   = 'TSEE',
    FrameYear               = 'TYER',
    FrameUserDefinedTextInf = 'TXXX',
    FrameUniqueFileIdent    = 'UFID',
    FrameTermsOfUse         = 'USER',
    FrameUnsyncLyrics       = 'USLT',
    FrameCommercialInfo     = 'WCOM',
    FrameLegalCopyrightInfo = 'WCOP',
    FrameOfficialFilePage   = 'WOAF',
    FrameOfficialArtistPage = 'WOAR',
    FrameOfficialSourcePage = 'WOAS',
    FrameOfficialRadioPage  = 'WORS',
    FramePayment            = 'WPAY',
    FramePublisherPage      = 'WPUB',
    FrameUserLink           = 'WXXX',

    /* The rest of these are "unofficial", but known frame types */
    FramePartOfCompilation  = 'TCMP',
    FrameAlbumArtistSortOrd = 'TSO2',
    FrameComposerSortOrd    = 'TSOC',
    FrameReplayGainAdj      = 'RGAD',
    FrameExperimentalRVA2   = 'XRVA',

    /* Added for ID3v2.4 */
    FrameAudioSeekPointIdx  = 'ASPI',
    FrameEqualization2      = 'EQU2',
    FrameRelativeVolumeAdj2 = 'RVA2',
    FrameSeek               = 'SEEK',
    FrameSignature          = 'SIGN',
    FrameEncodingTime       = 'TDEN',
    FrameOriginalRelease    = 'TDOR',
    FrameRecordingTime      = 'TDRC',
    FrameReleaseTime        = 'TDRL',
    FrameTaggingTime        = 'TDTG',
    FrameInvolvedPeopleList = 'TIPL',
    FrameMusicianCreditList = 'TMCL',
    FrameMood               = 'TMOO',
    FrameProducedNotice     = 'TPRO',
    FrameAlbumSortOrd       = 'TSOA',
    FramePerformerSortOrd   = 'TSOP',
    FrameTitleSortOrd       = 'TSOT',
    FrameSetSubtitle        = 'TSST',

    /* ID3v2.2 "four" character codes */
    Frame22RecommendedBufSz = 'BUF ',
    Frame22PlayCounter      = 'CNT ',
    Frame22Comments         = 'COM ',
    Frame22AudioEncryption  = 'CRA ',
    Frame22EncryptedMetaFrm = 'CRM ',
    Frame22EventTimingCode  = 'ETC ',
    Frame22Equalization     = 'EQU ',
    Frame22GeneralObject    = 'GEO ',
    Frame22InvolvedPeopleLst= 'IPL ',
    Frame22LinkedInfo       = 'LNK ',
    Frame22MusicCDIdent     = 'MCI ',
    Frame22MPEGLoccationLUT = 'MLL ',
    Frame22AttachedPicture  = 'PIC ',
    Frame22Popularimeter    = 'POP ',
    Frame22Reverb           = 'REV ',
    Frame22RelativeVolumeAdj= 'RVA ',
    Frame22SyncLyrics       = 'SLT ',
    Frame22SyncTempo        = 'STC ',
    Frame22AlbumTitle       = 'TAL ',
    Frame22BPM              = 'TBP ',
    Frame22Composer         = 'TCM ',
    Frame22ContentType      = 'TCO ',
    Frame22CopyrightMsg     = 'TCR ',
    Frame22Date             = 'TDA ',
    Frame22PlaylistDelay    = 'TDY ',
    Frame22EncodedBy        = 'TEN ',
    Frame22FileType         = 'TFT ',
    Frame22Time             = 'TIM ',
    Frame22InitialKey       = 'TKE ',
    Frame22Language         = 'TLA ',
    Frame22Length           = 'TLE ',
    Frame22MediaType        = 'TMT ',
    Frame22OriginalArtist   = 'TOA ',
    Frame22OriginalFilename = 'TOF ',
    Frame22OriginalLyricist = 'TOL ',
    Frame22OriginalRelYear  = 'TOR ',
    Frame22OriginalAlbum    = 'TOT ',
    Frame22LeadPerformer    = 'TP1 ',
    Frame22Accompaniment    = 'TP2 ',
    Frame22Conductor        = 'TP3 ',
    Frame22ModifiedBy       = 'TP4 ',
    Frame22PartOfSet        = 'TPA ',
    Frame22Publisher        = 'TPB ',
    Frame22ISRC             = 'TRC ',
    Frame22RecordingDates   = 'TRD ',
    Frame22TrackNumber      = 'TRK ',
    Frame22Size             = 'TSI ',
    Frame22EncodingSettings = 'TSS ',
    Frame22ContentGrpDesc   = 'TT1 ',
    Frame22Title            = 'TT2 ',
    Frame22Subtitle         = 'TT3 ',
    Frame22Lyricist         = 'TXT ',
    Frame22Year             = 'TYE ',
    Frame22UserDefinedText  = 'TXX ',
    Frame22UniqueFileIdent  = 'UFI ',
    Frame22UnsyncLyrics     = 'ULT ',
    Frame22OfficialFilePg   = 'WAF ',
    Frame22OfficialArtistPg = 'WAR ',
    Frame22OfficialSourcePg = 'WAS ',
    Frame22CommercialInfo   = 'WCM ',
    Frame22CopyrightInfo    = 'WCP ',
    Frame22PublisherPg      = 'WPB ',
    Frame22UserLink         = 'WXX ',

    /* iTunes specific v2.2 frames */
    Frame22TitleSortOrd     = 'TST ',
    Frame22AlbumSortOrd     = 'TSP ',
    Frame22AlbumArtistSortOr= 'TS2 ',
    Frame22ComposerSortOrd  = 'TSC ',
    Frame22PartOfCompilation= 'TCP '
};

#ifndef SONATINATAG__FRAMECODES_DEFINED
#define SONATINATAG__FRAMECODES_DEFINED
typedef enum STTagID3v2_FrameCode_e STTagID3v2_FrameCode;
#endif

@interface STTagID3v2Frame : NSObject {
@private
    STTagID3v2_FrameCode _frameType;
    uint32_t _size;
    uint16_t _flags;
    NSData *_rawFrame;
}

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data;
+ (id)frameWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
              flags:(uint16_t)flags data:(NSData *)data;

- (void)dealloc;

- (STTagID3v2_FrameCode)type;
- (uint32_t)size;
- (uint16_t)flags;
- (NSData *)frame;

@end /* @interface STTagID3v2Frame */

#endif /* !STTagID3v2Frame_h */
