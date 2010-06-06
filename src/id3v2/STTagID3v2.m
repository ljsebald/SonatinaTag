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

#include <stdio.h>
#include <stdlib.h>

#include "STTagID3v2.h"
#include "STTagID3v2Frame.h"
#include "STTagID3v2TextFrame.h"
#include "STTagID3v2URLFrame.h"
#include "NSStringExt.h"

static uint32_t parse_size_22(const uint8_t *buf) {
    /* The size is in big endian order, 3-bytes long */
    return (buf[0] << 16) | (buf[1] << 8) | buf[2];
}

static uint32_t parse_size_23(const uint8_t *buf) {
    /* The size is in big endian order, and formed the normal way */
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static uint32_t parse_size_24(const uint8_t *buf) {
    if(buf[0] > 0x80 || buf[1] > 0x80 || buf[2] > 0x80 || buf[3] > 0x80) {
        return (uint32_t)-1;
    }

    /* The size is in big endian, and the most significant bit of each byte is
       zeroed, leaving us with 28 bits of actual data */
    return (buf[0] << 21) | (buf[1] << 14) | (buf[2] << 7) | buf[3];
}

@interface STTagID3v2 (Internal)

- (id)parseFrames;

@end /* @interface STTagID3v2 */

@implementation STTagID3v2

- (id)initFromFile:(NSString *)file
{
    FILE *fp;
    uint8_t buf[4];
    uint8_t *tag;

    if((self = [super init]) == nil) {
        return nil;
    }

    /* Open up the file for reading */
    fp = fopen([file fileSystemRepresentation], "rb");
    if(!fp) {
        [self release];
        return nil;
    }

    /* Assume for now that ID3v2 tags exist at the beginning of the file... */
    if(fread(buf, 1, 3, fp) != 3) {
        goto out_close;
    }

    /* Check for the ID3 signature */
    if(memcmp("ID3", buf, 3)) {
        /* No ID3 tag at the beginning, fail */
        goto out_close;
    }

    /* We now "know" that there is an ID3 tag here, parse the rest of the ID3
       header to figure out what else we have to do */
    if(fread(buf, 1, 2, fp) != 2) {
        goto out_close;
    }

    /* Support is here for 2.2-2.4 */
    if(buf[0] < 2 || buf[0] > 4) {
        NSLog(@"Unsupported ID3v2 version: %d", buf[0]);
        goto out_close;
    }

    _majorver = buf[0];
    _revision = buf[1];

    /* Grab the flags from the ID3 header */
    if(fread(&_flags, 1, 1, fp) != 1) {
        goto out_close;
    }

    /* Make sure no unknown flags are set */
    if(_majorver == 3 && (_flags & ~(STTAGID3V2_FLAG_MASK_23))) {
        NSLog(@"Unknown flags set: %02x for ID3v2.3", _flags);
        goto out_close;
    }
    else if(_majorver == 4 && (_flags & ~(STTAGID3V2_FLAG_MASK_24))) {
        NSLog(@"Unknown flags set: %02x for ID3v2.4", _flags);
        goto out_close;
    }

    /* We don't handle the unsynchronization setting just yet... */
    if(_flags & STTAGID3V2_FLAG_UNSYNC) {
        NSLog(@"Cannot handle unsynchronization info!");
        goto out_close;
    }

    /* The footer isn't handled either... */
    if(_flags & STTAGID3V2_FLAG_FOOTER) {
        NSLog(@"Ignoring ID3v2.4 footer");
    }

    /* Read in the length of the header */
    if(fread(buf, 1, 4, fp) != 4) {
        goto out_close;
    }

    /* Check the size for validity (no bytes should be 0x80 or greater) */
    if(buf[0] > 0x80 || buf[1] > 0x80 || buf[2] > 0x80 || buf[3] > 0x80) {
        goto out_close;
    }

    /* The size is in big endian, and the most significant bit of each byte is
       zeroed, leaving us with 28 bits of actual data */
    _size = buf[3] | (buf[2] << 7) | (buf[1] << 14) | (buf[0] << 21);

    /* If we have an extended header, read it, but ignore it for now */
    if(_flags & STTAGID3V2_FLAG_EXTHDR) {
        uint32_t sz;

        if(fread(buf, 1, 4, fp) != 4) {
            goto out_close;
        }

        sz = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);

        fseek(fp, (long)sz, SEEK_CUR);

        /* The main header size includes this extended header, so remove that
           part from the value */
        _size -= sz;
    }

    /* We've done all the header parsing, now read in the actual tag data */
    tag = (uint8_t *)malloc((size_t)_size);
    if(!tag) {
        goto out_close;
    }

    if(fread(tag, 1, (size_t)_size, fp) != (size_t)_size) {
        goto out_close;
    }

    /* The _rawtag object will free the memory when its done */
    _rawtag = [[NSData alloc] initWithBytesNoCopy:tag
                                           length:_size
                                     freeWhenDone:YES];
    if(_rawtag == nil) {
        goto out_close;
    }

    /* We're done with the file, so close it */
    fclose(fp);

    /* Allocate the dictionary to hold the frames */
    _frames = [[NSMutableDictionary alloc] initWithCapacity:1];
    if(_frames == nil) {
        [self release];
        return nil;
    }

    /* Parse all frames */
    return [self parseFrames];

out_close:
    [self release];
    fclose(fp);
    return nil;
}

- (void)dealloc
{
    [_rawtag release];
    [_frames release];
    [super dealloc];
}

- (id)frameForKey:(STTagID3v2_FrameCode)fourcc
{
    NSString *str = [NSString stringWith4CC:fourcc];

    return [_frames objectForKey:str];
}

- (int)id3v2MajorVersion
{
    return (int)_majorver;
}

- (int)id3v2Revision
{
    return (int)_revision;
}

- (NSString *)title
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22Title] text];
    }
    else {
        return [[self frameForKey:FrameTitle] text];
    }
}

- (NSString *)artist
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22LeadPerformer] text];
    }
    else {
        return [[self frameForKey:FrameLeadPerformer] text];
    }
}

- (NSString *)album
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22AlbumTitle] text];
    }
    else {
        return [[self frameForKey:FrameAlbumTitle] text];
    }
}

- (NSString *)year
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22Year] text];
    }
    else {
        return [[self frameForKey:FrameYear] text];
    }
}

- (NSString *)comment
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22Comments] text];
    }
    else {
        return [[self frameForKey:FrameComments] text];
    }
}

- (NSString *)genre
{
    if(_majorver == 2) {
        return [[self frameForKey:Frame22ContentType] text];
    }
    else {
        return [[self frameForKey:FrameContentType] text];
    }
}

- (int)trackNumber
{
    NSString *track;

    if(_majorver == 2) {
        track = [[self frameForKey:Frame22TrackNumber] text];
    }
    else {
        track = [[self frameForKey:FrameTrackNumber] text];
    }

    if(track == nil) {
        return 0;
    }

    return [track intValue];
}

- (int)discNumber
{
    NSString *disc;

    if(_majorver == 2) {
        disc = [[self frameForKey:Frame22PartOfSet] text];
    }
    else {
        disc = [[self frameForKey:FramePartOfSet] text];
    }

    if(disc == nil) {
        return 1;
    }

    return [disc intValue];
}

@end /* @implementation STTagID3v2 */

@implementation STTagID3v2 (Internal)

- (id)parseFrames
{
    uint32_t fourcc, sz, start = 0;
    uint16_t flags;
    const uint8_t *buf = [_rawtag bytes];
    NSString *fourccstr;
    NSData *framedata;
    uint32_t (*szf)(const uint8_t *) = &parse_size_23;

    if(_majorver == 4) {
        szf = &parse_size_24;
    }
    else if(_majorver == 2) {
        szf = &parse_size_22;
    }

    while(start < [_rawtag length]) {
        if(_majorver > 2) {
            fourcc = (buf[start] << 24) | (buf[start + 1] << 16) |
                (buf[start + 2] << 8) | buf[start + 3];
            sz = szf(buf + start + 4);
            flags = (buf[start + 8] << 8) | buf[start + 9];
            start += 10;
        }
        else {
            fourcc = (buf[start] << 24) | (buf[start + 1] << 16) |
                (buf[start + 2] < 8) | ' ';
            sz = szf(buf + start + 3);
            flags = 0;
            start += 6;
        }

        /* If we've hit padding, bail */
        if(fourcc == 0) {
            break;
        }

        if(sz == 0 || sz == (uint32_t)-1) {
            goto out_err;
        }

        /* Make sure we only have ID3v2.2 "four" character codes in ID3v2.2 */
        if(_majorver > 2 && (fourcc & 0xFF) == ' ') {
            goto out_err;
        }

        fourccstr = [NSString stringWith4CC:fourcc];
        framedata = [NSData dataWithBytes:buf + start length:(NSUInteger)sz];
        start += sz;

        /* If we have a specialized class for the given type of tag, then handle
           that, otherwise make a generic frame */
        switch(fourcc) {
            case FrameAlbumTitle:
            case FrameBPM:
            case FrameComposer:
            case FrameContentType:
            case FrameCopyrightMsg:
            case FrameDate:
            case FramePlaylistDelay:
            case FrameEncodedBy:
            case FrameLyricist:
            case FrameFileType:
            case FrameTime:
            case FrameContentGrpDesc:
            case FrameTitle:
            case FrameSubtitle:
            case FrameInitialKey:
            case FrameLanguage:
            case FrameLength:
            case FrameMediaType:
            case FrameOriginalAlbumTitle:
            case FrameOriginalFilename:
            case FrameOriginalLyricist:
            case FrameOriginalArtist:
            case FrameOriginalReleaseYr:
            case FrameOwner:
            case FrameLeadPerformer:
            case FrameAccompaniment:
            case FrameConductor:
            case FrameModifiedBy:
            case FramePartOfSet:
            case FramePublisher:
            case FrameTrackNumber:
            case FrameRecordingDates:
            case FrameRadioStationName:
            case FrameRadioStationOwner:
            case FrameSize:
            case FrameISRC:
            case FrameEncodingSettings:
            case FrameYear:
            case FramePartOfCompilation:
            case FrameAlbumArtistSortOrd:
            case FrameComposerSortOrd:
            case FrameEncodingTime:
            case FrameOriginalRelease:
            case FrameRecordingTime:
            case FrameReleaseTime:
            case FrameTaggingTime:
            case FrameInvolvedPeopleList:
            case FrameMusicianCreditList:
            case FrameMood:
            case FrameProducedNotice:
            case FrameAlbumSortOrd:
            case FramePerformerSortOrd:
            case FrameTitleSortOrd:
            case FrameSetSubtitle:
            case Frame22AlbumTitle:
            case Frame22BPM:
            case Frame22Composer:
            case Frame22ContentType:
            case Frame22CopyrightMsg:
            case Frame22Date:
            case Frame22PlaylistDelay:
            case Frame22EncodedBy:
            case Frame22FileType:
            case Frame22Time:
            case Frame22InitialKey:
            case Frame22Language:
            case Frame22Length:
            case Frame22MediaType:
            case Frame22OriginalArtist:
            case Frame22OriginalFilename:
            case Frame22OriginalLyricist:
            case Frame22OriginalRelYear:
            case Frame22OriginalAlbum:
            case Frame22LeadPerformer:
            case Frame22Accompaniment:
            case Frame22Conductor:
            case Frame22ModifiedBy:
            case Frame22PartOfSet:
            case Frame22Publisher:
            case Frame22ISRC:
            case Frame22RecordingDates:
            case Frame22TrackNumber:
            case Frame22Size:
            case Frame22EncodingSettings:
            case Frame22ContentGrpDesc:
            case Frame22Title:
            case Frame22Subtitle:
            case Frame22Lyricist:
            case Frame22Year:
            case Frame22TitleSortOrd:
            case Frame22AlbumSortOrd:
            case Frame22AlbumArtistSortOr:
            case Frame22ComposerSortOrd:
            case Frame22PartOfCompilation:
            {
                STTagID3v2TextFrame *tframe;

                tframe = [STTagID3v2TextFrame frameWithType:fourcc
                                                       size:sz
                                                      flags:flags
                                                       data:framedata];

                if(tframe != nil) {
                    [_frames setObject:tframe forKey:fourccstr];
                }

                break;
            }

            case FrameCommercialInfo:
            case FrameLegalCopyrightInfo:
            case FrameOfficialFilePage:
            case FrameOfficialArtistPage:
            case FrameOfficialSourcePage:
            case FrameOfficialRadioPage:
            case FramePayment:
            case FramePublisherPage:
            case Frame22OfficialFilePg:
            case Frame22OfficialArtistPg:
            case Frame22OfficialSourcePg:
            case Frame22CommercialInfo:
            case Frame22CopyrightInfo:
            case Frame22PublisherPg:
            {
                STTagID3v2URLFrame *uframe;

                uframe = [STTagID3v2URLFrame frameWithType:fourcc
                                                      size:sz
                                                     flags:flags
                                                      data:framedata];

                if(uframe != nil) {
                    [_frames setObject:uframe forKey:fourccstr];
                }

                break;
            }

            default:
            {
                STTagID3v2Frame *frame;

                frame = [STTagID3v2Frame frameWithType:fourcc
                                                  size:sz
                                                 flags:flags
                                                  data:framedata];
                if(frame != nil) {
                    [_frames setObject:frame forKey:fourccstr];
                }
            }
                
        }
    }

    return self;

out_err:
    NSLog(@"Invalid ID3v2 frame");
    [self release];
    return nil;
}

@end /* @implementation STTagID3v2 (Internal) */
