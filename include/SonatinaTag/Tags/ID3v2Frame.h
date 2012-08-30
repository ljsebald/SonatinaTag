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

#ifndef SonatinaTag__Tags__ID3v2Frame_h
#define SonatinaTag__Tags__ID3v2Frame_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <stdint.h>
#include <SonatinaTag/basedefs.h>
#include <SonatinaTag/Error.h>
#include <SonatinaTag/Picture.h>

/* Opaque ID3v2 frame structure. All of the specific frame types are
   "subclasses" of this type. */
struct ST_Frame_struct;
typedef struct ST_Frame_struct ST_Frame;

/* Opaque ID3v2 text frame structure. */
struct ST_TextFrame_struct;
typedef struct ST_TextFrame_struct ST_TextFrame;

/* Opaque ID3v2 user-defined text frame structure. */
struct ST_UserTextFrame_struct;
typedef struct ST_UserTextFrame_struct ST_UserTextFrame;

/* Opaque ID3v2 URL frame structure. */
struct ST_URLFrame_struct;
typedef struct ST_URLFrame_struct ST_URLFrame;

/* Opaque ID3v2 user-defined URL frame structure. */
struct ST_UserURLFrame_struct;
typedef struct ST_UserURLFrame_struct ST_UserURLFrame;

/* Opaque ID3v2 comment frame structure. */
struct ST_CommentFrame_struct;
typedef struct ST_CommentFrame_struct ST_CommentFrame;

/* Opaque ID3v2 picture frame structure. */
struct ST_PictureFrame_struct;
typedef struct ST_PictureFrame_struct ST_PictureFrame;

/* Opaque ID3v2 generic frame structure. */
struct ST_GenericFrame_struct;
typedef struct ST_GenericFrame_struct ST_GenericFrame;

/* Frame types */
typedef enum ST_ID3v2_FrameType_e {
    ST_FrameType_Generic    = 0,        /* ST_GenericFrame */
    ST_FrameType_Text       = 1,        /* ST_TextFrame */
    ST_FrameType_UserText   = 2,        /* ST_UserTextFrame */
    ST_FrameType_URL        = 3,        /* ST_URLFrame */
    ST_FrameType_UserURL    = 4,        /* ST_UserURLFrame */
    ST_FrameType_Comment    = 5,        /* ST_CommentFrame */
    ST_FrameType_Picture    = 6,        /* ST_PictureFrame */
    ST_FrameType_Invalid    = -1
} ST_ID3v2_FrameType;

typedef enum ST_ID3v2_FrameCode_e {
    ST_FrameAudioEncryption    = ST_4CC('A', 'E', 'N', 'C'),
    ST_FrameAttachedPicture    = ST_4CC('A', 'P', 'I', 'C'),
    ST_FrameComments           = ST_4CC('C', 'O', 'M', 'M'),
    ST_FrameCommercial         = ST_4CC('C', 'O', 'M', 'R'),
    ST_FrameEncryptionMethod   = ST_4CC('E', 'N', 'C', 'R'),
    ST_FrameEqualization       = ST_4CC('E', 'Q', 'U', 'A'),
    ST_FrameEventTimingCodes   = ST_4CC('E', 'T', 'C', 'O'),
    ST_FrameGeneralObject      = ST_4CC('G', 'E', 'O', 'B'),
    ST_FrameGroupIdent         = ST_4CC('G', 'R', 'I', 'D'),
    ST_FrameInvolvedPeople     = ST_4CC('I', 'P', 'L', 'S'),
    ST_FrameLinkedInfo         = ST_4CC('L', 'I', 'N', 'K'),
    ST_FrameMusicCDIdent       = ST_4CC('M', 'C', 'D', 'I'),
    ST_FrameMPEGLocationLUT    = ST_4CC('M', 'L', 'L', 'T'),
    ST_FrameOwnership          = ST_4CC('O', 'W', 'N', 'E'),
    ST_FramePrivate            = ST_4CC('P', 'R', 'I', 'V'),
    ST_FramePlayCounter        = ST_4CC('P', 'C', 'N', 'T'),
    ST_FramePopularimeter      = ST_4CC('P', 'O', 'P', 'M'),
    ST_FramePositionSync       = ST_4CC('P', 'O', 'S', 'S'),
    ST_FrameRecommendedBufSz   = ST_4CC('R', 'B', 'U', 'F'),
    ST_FrameRelativeVolumeAdj  = ST_4CC('R', 'V', 'A', 'D'),
    ST_FrameReverb             = ST_4CC('R', 'V', 'R', 'B'),
    ST_FrameSyncLyrics         = ST_4CC('S', 'Y', 'L', 'T'),
    ST_FrameSyncTempo          = ST_4CC('S', 'Y', 'T', 'C'),
    ST_FrameAlbumTitle         = ST_4CC('T', 'A', 'L', 'B'),
    ST_FrameBPM                = ST_4CC('T', 'B', 'P', 'M'),
    ST_FrameComposer           = ST_4CC('T', 'C', 'O', 'M'),
    ST_FrameContentType        = ST_4CC('T', 'C', 'O', 'N'),
    ST_FrameCopyrightMsg       = ST_4CC('T', 'C', 'O', 'P'),
    ST_FrameDate               = ST_4CC('T', 'D', 'A', 'T'),
    ST_FramePlaylistDelay      = ST_4CC('T', 'D', 'L', 'Y'),
    ST_FrameEncodedBy          = ST_4CC('T', 'E', 'N', 'C'),
    ST_FrameLyricist           = ST_4CC('T', 'E', 'X', 'T'),
    ST_FrameFileType           = ST_4CC('T', 'F', 'L', 'T'),
    ST_FrameTime               = ST_4CC('T', 'I', 'M', 'E'),
    ST_FrameContentGrpDesc     = ST_4CC('T', 'I', 'T', '1'),
    ST_FrameTitle              = ST_4CC('T', 'I', 'T', '2'),
    ST_FrameSubtitle           = ST_4CC('T', 'I', 'T', '3'),
    ST_FrameInitialKey         = ST_4CC('T', 'K', 'E', 'Y'),
    ST_FrameLanguage           = ST_4CC('T', 'L', 'A', 'N'),
    ST_FrameLength             = ST_4CC('T', 'L', 'E', 'N'),
    ST_FrameMediaType          = ST_4CC('T', 'M', 'E', 'D'),
    ST_FrameOriginalAlbumTitle = ST_4CC('T', 'O', 'A', 'L'),
    ST_FrameOriginalFilename   = ST_4CC('T', 'O', 'F', 'N'),
    ST_FrameOriginalLyricist   = ST_4CC('T', 'O', 'L', 'Y'),
    ST_FrameOriginalArtist     = ST_4CC('T', 'O', 'P', 'E'),
    ST_FrameOriginalReleaseYr  = ST_4CC('T', 'O', 'R', 'Y'),
    ST_FrameOwner              = ST_4CC('T', 'O', 'W', 'N'),
    ST_FrameLeadPerformer      = ST_4CC('T', 'P', 'E', '1'),
    ST_FrameAccompaniment      = ST_4CC('T', 'P', 'E', '2'),
    ST_FrameConductor          = ST_4CC('T', 'P', 'E', '3'),
    ST_FrameModifiedBy         = ST_4CC('T', 'P', 'E', '4'),
    ST_FramePartOfSet          = ST_4CC('T', 'P', 'O', 'S'),
    ST_FramePublisher          = ST_4CC('T', 'P', 'U', 'B'),
    ST_FrameTrackNumber        = ST_4CC('T', 'R', 'C', 'K'),
    ST_FrameRecordingDates     = ST_4CC('T', 'R', 'D', 'A'),
    ST_FrameRadioStationName   = ST_4CC('T', 'R', 'S', 'N'),
    ST_FrameRadioStationOwner  = ST_4CC('T', 'R', 'S', 'O'),
    ST_FrameSize               = ST_4CC('T', 'S', 'I', 'Z'),
    ST_FrameISRC               = ST_4CC('T', 'S', 'R', 'C'),
    ST_FrameEncodingSettings   = ST_4CC('T', 'S', 'E', 'E'),
    ST_FrameYear               = ST_4CC('T', 'Y', 'E', 'R'),
    ST_FrameUserText           = ST_4CC('T', 'X', 'X', 'X'),
    ST_FrameUniqueFileIdent    = ST_4CC('U', 'F', 'I', 'D'),
    ST_FrameTermsOfUse         = ST_4CC('U', 'S', 'E', 'R'),
    ST_FrameUnsyncLyrics       = ST_4CC('U', 'S', 'L', 'T'),
    ST_FrameCommercialInfo     = ST_4CC('W', 'C', 'O', 'M'),
    ST_FrameLegalCopyrightInfo = ST_4CC('W', 'C', 'O', 'P'),
    ST_FrameOfficialFilePage   = ST_4CC('W', 'O', 'A', 'F'),
    ST_FrameOfficialArtistPage = ST_4CC('W', 'O', 'A', 'R'),
    ST_FrameOfficialSourcePage = ST_4CC('W', 'O', 'A', 'S'),
    ST_FrameOfficialRadioPage  = ST_4CC('W', 'O', 'R', 'S'),
    ST_FramePayment            = ST_4CC('W', 'P', 'A', 'Y'),
    ST_FramePublisherPage      = ST_4CC('W', 'P', 'U', 'B'),
    ST_FrameUserLink           = ST_4CC('W', 'X', 'X', 'X'),

    /* The rest of these are "unofficial", but known frame types */
    ST_FramePartOfCompilation  = ST_4CC('T', 'C', 'M', 'P'),
    ST_FrameAlbumArtistSortOrd = ST_4CC('T', 'S', 'O', '2'),
    ST_FrameComposerSortOrd    = ST_4CC('T', 'S', 'O', 'C'),
    ST_FrameReplayGainAdj      = ST_4CC('R', 'G', 'A', 'D'),
    ST_FrameExperimentalRVA2   = ST_4CC('X', 'R', 'V', 'A'),

    /* Added for ID3v2.4 */
    ST_FrameAudioSeekPointIdx  = ST_4CC('A', 'S', 'P', 'I'),
    ST_FrameEqualization2      = ST_4CC('E', 'Q', 'U', '2'),
    ST_FrameRelativeVolumeAdj2 = ST_4CC('R', 'V', 'A', '2'),
    ST_FrameSeek               = ST_4CC('S', 'E', 'E', 'K'),
    ST_FrameSignature          = ST_4CC('S', 'I', 'G', 'N'),
    ST_FrameEncodingTime       = ST_4CC('T', 'D', 'E', 'N'),
    ST_FrameOriginalRelease    = ST_4CC('T', 'D', 'O', 'R'),
    ST_FrameRecordingTime      = ST_4CC('T', 'D', 'R', 'C'),
    ST_FrameReleaseTime        = ST_4CC('T', 'D', 'R', 'L'),
    ST_FrameTaggingTime        = ST_4CC('T', 'D', 'T', 'G'),
    ST_FrameInvolvedPeopleList = ST_4CC('T', 'I', 'P', 'L'),
    ST_FrameMusicianCreditList = ST_4CC('T', 'M', 'C', 'L'),
    ST_FrameMood               = ST_4CC('T', 'M', 'O', 'O'),
    ST_FrameProducedNotice     = ST_4CC('T', 'P', 'R', 'O'),
    ST_FrameAlbumSortOrd       = ST_4CC('T', 'S', 'O', 'A'),
    ST_FramePerformerSortOrd   = ST_4CC('T', 'S', 'O', 'P'),
    ST_FrameTitleSortOrd       = ST_4CC('T', 'S', 'O', 'T'),
    ST_FrameSetSubtitle        = ST_4CC('T', 'S', 'S', 'T'),

    /* ID3v2.2 "four" character codes */
    ST_Frame22RecommendedBufSz = ST_4CC('B', 'U', 'F', ' '),
    ST_Frame22PlayCounter      = ST_4CC('C', 'N', 'T', ' '),
    ST_Frame22Comments         = ST_4CC('C', 'O', 'M', ' '),
    ST_Frame22AudioEncryption  = ST_4CC('C', 'R', 'A', ' '),
    ST_Frame22EncryptedMetaFrm = ST_4CC('C', 'R', 'M', ' '),
    ST_Frame22EventTimingCode  = ST_4CC('E', 'T', 'C', ' '),
    ST_Frame22Equalization     = ST_4CC('E', 'Q', 'U', ' '),
    ST_Frame22GeneralObject    = ST_4CC('G', 'E', 'O', ' '),
    ST_Frame22InvolvedPeopleLst= ST_4CC('I', 'P', 'L', ' '),
    ST_Frame22LinkedInfo       = ST_4CC('L', 'N', 'K', ' '),
    ST_Frame22MusicCDIdent     = ST_4CC('M', 'C', 'I', ' '),
    ST_Frame22MPEGLoccationLUT = ST_4CC('M', 'L', 'L', ' '),
    ST_Frame22AttachedPicture  = ST_4CC('P', 'I', 'C', ' '),
    ST_Frame22Popularimeter    = ST_4CC('P', 'O', 'P', ' '),
    ST_Frame22Reverb           = ST_4CC('R', 'E', 'V', ' '),
    ST_Frame22RelativeVolumeAdj= ST_4CC('R', 'V', 'A', ' '),
    ST_Frame22SyncLyrics       = ST_4CC('S', 'L', 'T', ' '),
    ST_Frame22SyncTempo        = ST_4CC('S', 'T', 'C', ' '),
    ST_Frame22AlbumTitle       = ST_4CC('T', 'A', 'L', ' '),
    ST_Frame22BPM              = ST_4CC('T', 'B', 'P', ' '),
    ST_Frame22Composer         = ST_4CC('T', 'C', 'M', ' '),
    ST_Frame22ContentType      = ST_4CC('T', 'C', 'O', ' '),
    ST_Frame22CopyrightMsg     = ST_4CC('T', 'C', 'R', ' '),
    ST_Frame22Date             = ST_4CC('T', 'D', 'A', ' '),
    ST_Frame22PlaylistDelay    = ST_4CC('T', 'D', 'Y', ' '),
    ST_Frame22EncodedBy        = ST_4CC('T', 'E', 'N', ' '),
    ST_Frame22FileType         = ST_4CC('T', 'F', 'T', ' '),
    ST_Frame22Time             = ST_4CC('T', 'I', 'M', ' '),
    ST_Frame22InitialKey       = ST_4CC('T', 'K', 'E', ' '),
    ST_Frame22Language         = ST_4CC('T', 'L', 'A', ' '),
    ST_Frame22Length           = ST_4CC('T', 'L', 'E', ' '),
    ST_Frame22MediaType        = ST_4CC('T', 'M', 'T', ' '),
    ST_Frame22OriginalArtist   = ST_4CC('T', 'O', 'A', ' '),
    ST_Frame22OriginalFilename = ST_4CC('T', 'O', 'F', ' '),
    ST_Frame22OriginalLyricist = ST_4CC('T', 'O', 'L', ' '),
    ST_Frame22OriginalRelYear  = ST_4CC('T', 'O', 'R', ' '),
    ST_Frame22OriginalAlbum    = ST_4CC('T', 'O', 'T', ' '),
    ST_Frame22LeadPerformer    = ST_4CC('T', 'P', '1', ' '),
    ST_Frame22Accompaniment    = ST_4CC('T', 'P', '2', ' '),
    ST_Frame22Conductor        = ST_4CC('T', 'P', '3', ' '),
    ST_Frame22ModifiedBy       = ST_4CC('T', 'P', '4', ' '),
    ST_Frame22PartOfSet        = ST_4CC('T', 'P', 'A', ' '),
    ST_Frame22Publisher        = ST_4CC('T', 'P', 'B', ' '),
    ST_Frame22ISRC             = ST_4CC('T', 'R', 'C', ' '),
    ST_Frame22RecordingDates   = ST_4CC('T', 'R', 'D', ' '),
    ST_Frame22TrackNumber      = ST_4CC('T', 'R', 'K', ' '),
    ST_Frame22Size             = ST_4CC('T', 'S', 'I', ' '),
    ST_Frame22EncodingSettings = ST_4CC('T', 'S', 'S', ' '),
    ST_Frame22ContentGrpDesc   = ST_4CC('T', 'T', '1', ' '),
    ST_Frame22Title            = ST_4CC('T', 'T', '2', ' '),
    ST_Frame22Subtitle         = ST_4CC('T', 'T', '3', ' '),
    ST_Frame22Lyricist         = ST_4CC('T', 'X', 'T', ' '),
    ST_Frame22Year             = ST_4CC('T', 'Y', 'E', ' '),
    ST_Frame22UserText         = ST_4CC('T', 'X', 'X', ' '),
    ST_Frame22UniqueFileIdent  = ST_4CC('U', 'F', 'I', ' '),
    ST_Frame22UnsyncLyrics     = ST_4CC('U', 'L', 'T', ' '),
    ST_Frame22OfficialFilePg   = ST_4CC('W', 'A', 'F', ' '),
    ST_Frame22OfficialArtistPg = ST_4CC('W', 'A', 'R', ' '),
    ST_Frame22OfficialSourcePg = ST_4CC('W', 'A', 'S', ' '),
    ST_Frame22CommercialInfo   = ST_4CC('W', 'C', 'M', ' '),
    ST_Frame22CopyrightInfo    = ST_4CC('W', 'C', 'P', ' '),
    ST_Frame22PublisherPg      = ST_4CC('W', 'P', 'B', ' '),
    ST_Frame22UserLink         = ST_4CC('W', 'X', 'X', ' '),

    /* iTunes specific v2.2 frames */
    ST_Frame22TitleSortOrd     = ST_4CC('T', 'S', 'T', ' '),
    ST_Frame22AlbumSortOrd     = ST_4CC('T', 'S', 'P', ' '),
    ST_Frame22AlbumArtistSortOr= ST_4CC('T', 'S', '2', ' '),
    ST_Frame22ComposerSortOrd  = ST_4CC('T', 'S', 'C', ' '),
    ST_Frame22PartOfCompilation= ST_4CC('T', 'C', 'P', ' ')
} ST_ID3v2_FrameCode;

/* All Frames *****************************************************************/

/* Free an ID3v2 frame. */
ST_FUNC void ST_ID3v2_Frame_free(ST_Frame *f);

/* Get the type of a frame. */
ST_FUNC ST_ID3v2_FrameType ST_ID3v2_Frame_type(const ST_Frame *f);

/* Retrieve any flags set on the frame. */
ST_FUNC uint16_t ST_ID3v2_Frame_flags(const ST_Frame *f);

/* Set flags on a frame. */
ST_FUNC ST_Error ST_ID3v2_Frame_setFlags(ST_Frame *f, uint16_t flags);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation string representation of the frame. You are
   responsible for cleaning up the string when you're done with it. Returns NULL
   if a description format isn't known (i.e, its a ST_GenericFrame). */
ST_FUNC CFStringRef ST_ID3v2_Frame_createString(const ST_Frame *f);
#endif

/* Text Frames ****************************************************************/

/* Create a frame for text data. The frame DOES NOT take ownership of the
   string. Also, it is your responsibility to figure out the correct
   encoding. */
ST_FUNC ST_TextFrame *ST_ID3v2_TextFrame_create(ST_TextEncoding e, uint32_t len,
                                                const uint8_t *str);

/* Retrieve the text from the frame. It is your responsibility to figure out how
   to interpret the text based on its encoding. */
ST_FUNC const uint8_t *ST_ID3v2_TextFrame_text(const ST_TextFrame *f);

/* Retrieve the length of the text in bytes. */
ST_FUNC uint32_t ST_ID3v2_TextFrame_length(const ST_TextFrame *f);

/* Retrieve the encoding of the text. */
ST_FUNC ST_TextEncoding ST_ID3v2_TextFrame_encoding(const ST_TextFrame *f);

/* Set the text in the frame. Set own_buf to non-zero to have the frame take
   ownership of the buffer, otherwise this function will make a copy. */
ST_FUNC ST_Error ST_ID3v2_TextFrame_setText(ST_TextFrame *f, ST_TextEncoding e,
                                            uint32_t sz, uint8_t *d,
                                            int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a frame for text data from a CoreFoundation string. This function does
   not take ownership of the string passed in. The encoding parameter is the
   desired encoding for the final result. */
ST_FUNC ST_TextFrame *ST_ID3v2_TextFrame_createStr(CFStringRef str,
                                                   ST_TextEncoding enc);

/* Retrieve the text from the frame. It is your responsibility to release the
   returned string. */
ST_FUNC CFStringRef ST_ID3v2_TextFrame_copyText(const ST_TextFrame *f);

/* Set the text in the frame. This function makes a copy of the string passed
   in, so the original is still your responsibility. The text will be converted
   to the encoding that the frame had when it was created. */
ST_FUNC ST_Error ST_ID3v2_TextFrame_setTextStr(ST_TextFrame *f, CFStringRef s);

/* Create a CoreFoundation string representation of the data in a text frame.
   You are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_TextFrame_createString(const ST_TextFrame *f);
#endif

/* User-Defined Text Frames ***************************************************/

/* Create a frame for user-defined text strings. The frame DOES NOT take
   ownership of any strings passed in. Also, it is your responsibility to figure
   out the correct encoding. */
ST_FUNC ST_UserTextFrame *ST_ID3v2_UserTextFrame_create(ST_TextEncoding enc,
                                                        uint32_t sl,
                                                        const uint8_t *str,
                                                        uint32_t dl,
                                                        const uint8_t *desc);

/* Retrieve the text from the comment frame. It is your responsibility to figure
   out how to interpret the text based on its encoding. */
ST_FUNC const uint8_t *ST_ID3v2_UserTextFrame_text(const ST_UserTextFrame *f);

/* Retrieve the length of the text in bytes. */
ST_FUNC uint32_t ST_ID3v2_UserTextFrame_textLength(const ST_UserTextFrame *f);

/* Retrieve the description from the comment frame. It is your responsibility to
   figure out how to interpret the text based on its encoding. */
ST_FUNC const uint8_t *ST_ID3v2_UserTextFrame_desc(const ST_UserTextFrame *f);

/* Retrieve the length of the description in bytes. */
ST_FUNC uint32_t ST_ID3v2_UserTextFrame_descLength(const ST_UserTextFrame *f);

/* Retrieve the encoding of the text and description. */
ST_FUNC ST_TextEncoding ST_ID3v2_UserTextFrame_encoding(const ST_UserTextFrame *f);

/* Set the text in the frame. Set own_buf to non-zero to have the frame take
   ownership of the buffer, otherwise this function will make a copy. */
ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setText(ST_UserTextFrame *f,
                                                ST_TextEncoding enc,
                                                uint32_t str_sz, uint8_t *str,
                                                int own_buf);

/* Set the description of the frame. Set own_buf to non-zero to have the frame
   take ownership of the buffer, otherwise this function will make a copy. It is
   your responsibility to make sure this is in the same encoding as the text. */
ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setDesc(ST_UserTextFrame *f,
                                                uint32_t desc_sz, uint8_t *desc,
                                                int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a frame for user-defined text data from CoreFoundation strings. This
   function does not take ownership of the strings passed in. The encoding
   parameter is the desired encoding for the final result. */
ST_FUNC ST_UserTextFrame *ST_ID3v2_UserTextFrame_createStr(CFStringRef str,
                                                           CFStringRef desc,
                                                           ST_TextEncoding e);

/* Retrieve the text from the frame. It is your responsibility to release the
   returned string. */
ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_copyText(const ST_UserTextFrame *f);

/* Retrieve the description from the frame. It is your responsibility to release
   the returned string. */
ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_copyDesc(const ST_UserTextFrame *f);

/* Set the text in the frame. This function makes a copy of the string passed
   in, so the original is still your responsibility. The text will be converted
   to the encoding that the frame had when it was created. */
ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setTextStr(ST_UserTextFrame *f,
                                                   CFStringRef s);

/* Set the description in the frame. This function makes a copy of the string
   passed in, so the original is still your responsibility. The text will be
   converted to the encoding the frame had when it was created. */
ST_FUNC ST_Error ST_ID3v2_UserTextFrame_setDescStr(ST_UserTextFrame *f,
                                                   CFStringRef d);

/* Create a CoreFoundation string representation of the data in a user-defined
   text frame. You are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_UserTextFrame_createString(const ST_UserTextFrame *f);
#endif

/* URL Frames *****************************************************************/

/* Create a frame for URL data. The frame DOES NOT take ownership of the
   string. Also, it is your responsibility to make sure the URL is in ISO-8859-1
   encoding (as that is all that ID3v2 allows for URLs). */
ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_create(uint32_t len,
                                              const uint8_t *str);

/* Retrieve the URL from the frame. */
ST_FUNC const uint8_t *ST_ID3v2_URLFrame_URL(const ST_URLFrame *f);

/* Retrieve the length of the URL in bytes. */
ST_FUNC uint32_t ST_ID3v2_URLFrame_length(const ST_URLFrame *f);

/* Set the URL in the frame. Set own_buf to non-zero to have the frame take
   ownership of the buffer, otherwise this function will make a copy. */
ST_FUNC ST_Error ST_ID3v2_URLFrame_setURL(ST_URLFrame *f, uint32_t sz,
                                          uint8_t *d, int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a frame for URL data from a CoreFoundation URL object. This function
   does not take ownership of the URL. */
ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_createURL(CFURLRef url);

/* Create a frame for URL data from a CoreFoundation String object. This
   function does not take ownership of the string. */
ST_FUNC ST_URLFrame *ST_ID3v2_URLFrame_createStr(CFStringRef s);

/* Retrieve the URL from the frame. You are responsible for cleaning up the URL
   when you're done with it. */
ST_FUNC CFURLRef ST_ID3v2_URLFrame_copyURL(const ST_URLFrame *f);

/* Retrieve the URL from the frame as a CoreFoundation string. You are
   responsible for cleaning up the string when you're done with it. */
ST_FUNC CFStringRef ST_ID3v2_URLFrame_copyURLString(const ST_URLFrame *f);

/* Set the URL in the frame from a CoreFoundation URL object. This function does
   not take ownership of the URL, so you're still responsible for it. */
ST_FUNC ST_Error ST_ID3v2_URLFrame_setCFURL(ST_URLFrame *f, CFURLRef url);

/* Set the URL in the frame from a CoreFoundation String object. This function
   does not take ownership of the string, so you're still responsible for it. */
ST_FUNC ST_Error ST_ID3v2_URLFrame_setURLStr(ST_URLFrame *f, CFStringRef s);

/* Create a CoreFoundation string representation of the data in a URL frame. You
   are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_URLFrame_createString(const ST_URLFrame *f);
#endif

/* User-defined URL Frames ****************************************************/

/* Create a frame for user-defined URL data. The frame DOES NOT take ownership
   of the buffers. Also, it is your responsibility to make sure the URL is in
   ISO-8859-1 encoding (as that is all that ID3v2 allows for URLs) and that the
   description is in the specified encoding. */
ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_create(ST_TextEncoding enc,
                                                      uint32_t ul,
                                                      const uint8_t *url,
                                                      uint32_t dl,
                                                      const uint8_t *desc);

/* Retrieve the URL from the frame. */
ST_FUNC const uint8_t *ST_ID3v2_UserURLFrame_URL(const ST_UserURLFrame *f);

/* Retrieve the length of the URL in bytes. */
ST_FUNC uint32_t ST_ID3v2_UserURLFrame_URLLength(const ST_UserURLFrame *f);

/* Retrieve the description of the URL. It is your responsibility to deal with
   the encoding of the text. */
ST_FUNC const uint8_t *ST_ID3v2_UserURLFrame_desc(const ST_UserURLFrame *f);

/* Retrieve the length of the description in bytes. */
ST_FUNC uint32_t ST_ID3v2_UserURLFrame_descLength(const ST_UserURLFrame *f);

/* Retrieve the encoding of the description string. */
ST_FUNC ST_TextEncoding ST_ID3v2_UserURLFrame_descEncoding(const ST_UserURLFrame *f);

/* Set the URL in the frame. Set own_buf to non-zero to have the frame take
   ownership of the buffer, otherwise this function will make a copy. */
ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setURL(ST_UserURLFrame *f, uint32_t ul,
                                              uint8_t *url, int own_buf);

/* Set the description of the URL. Set own_buf to non-zero to have the frame
   take ownership of the buffer, otherwise this function will make a copy. It is
   your responsibility to make sure the encoding is correct. */
ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setDesc(ST_UserURLFrame *f,
                                               ST_TextEncoding enc,
                                               uint32_t desc_sz, uint8_t *desc,
                                               int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a frame for User-defined URL data from a CoreFoundation URL object and
   a CoreFoundation String object (for the description). This function does not
   take ownership of either of the objects. The encoding parameter specfies the
   desired encoding of the description (the URL is always ISO-8859-1). */
ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_createURL(CFURLRef url,
                                                         CFStringRef desc,
                                                         ST_TextEncoding e);

/* Create a frame for User-defined URL data from CoreFoundation String objects
   for both the URL and the description. This function does not take ownership
   of the strings. The encoding parameter specfies the desired encoding of the
   description (the URL is always ISO-8859-1).  */
ST_FUNC ST_UserURLFrame *ST_ID3v2_UserURLFrame_createStr(CFStringRef url,
                                                         CFStringRef desc,
                                                         ST_TextEncoding e);

/* Retrieve the URL from the frame. You are responsible for cleaning up the URL
   when you're done with it. */
ST_FUNC CFURLRef ST_ID3v2_UserURLFrame_copyURL(const ST_UserURLFrame *f);

/* Retrieve the URL from the frame as a CoreFoundation string. You are
   responsible for cleaning up the string when you're done with it. */
ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_copyURLString(const ST_UserURLFrame *f);

/* Retrieve the description from the frame as a CoreFoundatio string. You are
   responsible for cleaning up the string when you're done with it. */
ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_copyDesc(const ST_UserURLFrame *f);

/* Set the URL in the frame from a CoreFoundation URL object. This function does
   not take ownership of the URL, so you're still responsible for it. */
ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setCFURL(ST_UserURLFrame *f,
                                                CFURLRef url);

/* Set the URL in the frame from a CoreFoundation String object. This function
   does not take ownership of the string, so you're still responsible for it. */
ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setURLStr(ST_UserURLFrame *f,
                                                 CFStringRef s);

/* Set the description in the frame from a CoreFoundation String object. This
   function does not take ownership of the string. */
ST_FUNC ST_Error ST_ID3v2_UserURLFrame_setDescStr(ST_UserURLFrame *f,
                                                  CFStringRef s);

/* Create a CoreFoundation string representation of the data in a user-defined
   URL frame. You are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_UserURLFrame_createString(const ST_UserURLFrame *f);
#endif

/* Comment Frames *************************************************************/

/* Create a frame for comments. The frame DOES NOT take ownership of any strings
   passed in. Also, it is your responsibility to figure out the correct
   encoding. */
ST_FUNC ST_CommentFrame *ST_ID3v2_CommentFrame_create(ST_TextEncoding enc,
                                                      uint32_t sl,
                                                      const uint8_t *str,
                                                      uint32_t dl,
                                                      const uint8_t *desc,
                                                      const char *lang);

/* Retrieve the text from the comment frame. It is your responsibility to figure
   out how to interpret the text based on its encoding. */
ST_FUNC const uint8_t *ST_ID3v2_CommentFrame_text(const ST_CommentFrame *f);

/* Retrieve the length of the text in bytes. */
ST_FUNC uint32_t ST_ID3v2_CommentFrame_textLength(const ST_CommentFrame *f);

/* Retrieve the description from the comment frame. It is your responsibility to
   figure out how to interpret the text based on its encoding. */
ST_FUNC const uint8_t *ST_ID3v2_CommentFrame_desc(const ST_CommentFrame *f);

/* Retrieve the length of the description in bytes. */
ST_FUNC uint32_t ST_ID3v2_CommentFrame_descLength(const ST_CommentFrame *f);

/* Retrieve the encoding of the text and description. */
ST_FUNC ST_TextEncoding ST_ID3v2_CommentFrame_encoding(const ST_CommentFrame *f);

/* Retrieve the 3-character language code of the comment. */
ST_FUNC const char *ST_ID3v2_CommentFrame_language(const ST_CommentFrame *f);

/* Set the 3-character language code of the comment. */
ST_FUNC ST_Error ST_ID3v2_CommentFrame_setLanguage(ST_CommentFrame *f,
                                                   const char *l);

/* Set the text in the frame. Set own_buf to non-zero to have the frame take
   ownership of the buffer, otherwise this function will make a copy. */
ST_FUNC ST_Error ST_ID3v2_CommentFrame_setText(ST_CommentFrame *f,
                                               ST_TextEncoding enc,
                                               uint32_t str_sz, uint8_t *str,
                                               int own_buf);

/* Set the description of the frame. Set own_buf to non-zero to have the frame
   take ownership of the buffer, otherwise this function will make a copy. It is
   your responsibility to make sure this is in the same encoding as the text. */
ST_FUNC ST_Error ST_ID3v2_CommentFrame_setDesc(ST_CommentFrame *f,
                                               uint32_t desc_sz, uint8_t *desc,
                                               int own_buf);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a frame for comment data from CoreFoundation strings. This function
   does not take ownership of the strings passed in. The encoding parameter is
   the desired encoding for the final result. */
ST_FUNC ST_CommentFrame *ST_ID3v2_CommentFrame_createStr(CFStringRef str,
                                                         CFStringRef desc,
                                                         ST_TextEncoding e,
                                                         const char *lang);

/* Retrieve the text from the frame. It is your responsibility to release the
   returned string. */
ST_FUNC CFStringRef ST_ID3v2_CommentFrame_copyText(const ST_CommentFrame *f);

/* Retrieve the description from the frame. It is your responsibility to release
   the returned string. */
ST_FUNC CFStringRef ST_ID3v2_CommentFrame_copyDesc(const ST_CommentFrame *f);

/* Set the text in the frame. This function makes a copy of the string passed
   in, so the original is still your responsibility. The text will be converted
   to the encoding that the frame had when it was created. */
ST_FUNC ST_Error ST_ID3v2_CommentFrame_setTextStr(ST_CommentFrame *f,
                                                  CFStringRef s);

/* Set the description in the frame. This function makes a copy of the string
   passed in, so the original is still your responsibility. The text will be
   converted to the encoding the frame had when it was created. */
ST_FUNC ST_Error ST_ID3v2_CommentFrame_setDescStr(ST_CommentFrame *f,
                                                  CFStringRef d);

/* Create a CoreFoundation string representation of the data in a comment frame.
   You are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_CommentFrame_createString(const ST_CommentFrame *f);
#endif

/* Picture Frames *************************************************************/

/* Create a frame for picture data. The frame takes ownership of the picture. */
ST_FUNC ST_PictureFrame *ST_ID3v2_PictureFrame_create(ST_Picture *p);

/* Retrieve the picture from the frame. Do not free this yourself, as the frame
   owns the picture data. Also, if you want to keep this after freeing a frame,
   you must create a copy! */
ST_FUNC ST_Picture *ST_ID3v2_PictureFrame_picture(const ST_PictureFrame *f);

/* Set the picture in the frame. The frame takes ownership of the picture. */
ST_FUNC ST_Error ST_ID3v2_PictureFrame_setPicture(ST_PictureFrame *f,
                                                  ST_Picture *p);

#ifdef ST_HAVE_COREFOUNDATION
/* Create a CoreFoundation string representation of the data in a picture frame.
   You are responsible for freeing the returned string. */
ST_FUNC CFStringRef ST_ID3v2_PictureFrame_createString(const ST_PictureFrame *f);
#endif

/* Generic Frames *************************************************************/

/* Create a frame for generic data. The frame takes ownership of the data. */
ST_FUNC ST_GenericFrame *ST_ID3v2_GenericFrame_create(uint32_t sz, uint8_t *d);

/* Retrieve the data from a generic frame. This includes everything except the
   frame's header. */
ST_FUNC const uint8_t *ST_ID3v2_GenericFrame_data(const ST_GenericFrame *f);

/* Retrieve the length of a generic frame. */
ST_FUNC uint32_t ST_ID3v2_GenericFrame_size(const ST_GenericFrame *f);

/* Set the data in a generic frame. Set own_buf to non-zero to have the frame
   take ownership of the buffer passed in (d), otherwise it'll make a copy. */
ST_FUNC ST_Error ST_ID3v2_GenericFrame_setData(ST_GenericFrame *f, uint32_t sz,
                                               uint8_t *d, int own_buf);

ST_END_DECLS

#endif /* !SonatinaTag__Tags__ID3v2__Frame_h */
