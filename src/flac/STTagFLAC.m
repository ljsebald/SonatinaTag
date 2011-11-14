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

#include <stdio.h>
#include <stdlib.h>

#include "STTagFLAC.h"
#include "STTagFLACPicture.h"
#include "NSStringExt.h"

#include "STTagID3v2TextFrame.h"
#include "STTagID3v2CommentFrame.h"

#define METADATA_TYPE_VORBIS_COMMENT    4
#define METADATA_TYPE_PICTURE           6

@interface STTagFLAC (internal)
- (BOOL)parseComments;
@end /* @interface STTagFLAC (internal) */

@implementation STTagFLAC

- (id)init
{
    if((self = [super init])) {
        _vorbisComments = [[NSMutableDictionary alloc] init];
        _pictures = [[NSMutableArray alloc] init];

        [_vorbisComments setObject:@"SonatinaTag" forKey:@"vendor"];
    }

    return self;
}

- (id)initFromFile:(NSString *)file
{
    FILE *fp;
    uint8_t buf[4];
    uint8_t *tag;
    int done = 0;
    uint8_t block_type;
    uint32_t block_len;
    NSData *tmp;
    STTagFLACPicture *pic;

    if((self = [super init]) == nil) {
        return nil;
    }

    /* Open up the file for reading */
    fp = fopen([file fileSystemRepresentation], "rb");
    if(!fp) {
        [self release];
        return nil;
    }

    /* Check for the fLaC that starts FLAC files. */
    if(fread(buf, 1, 4, fp) != 4) {
        goto out_close;
    }

    /* Check for the signature */
    if(memcmp("fLaC", buf, 4)) {
        goto out_close;
    }

    /* Create the storage for any pictures */
    _pictures = [[NSMutableArray alloc] initWithCapacity:0];
    if(!_pictures) {
        goto out_close;
    }

    /* Loop through the metadata blocks until we find a VORBIS_COMMENT one. */
    while(!done) {
        if(fread(buf, 1, 4, fp) != 4) {
            goto out_close;
        }

        block_type = buf[0] & 0x7F;
        block_len = (buf[1] << 16) | (buf[2] << 8) | (buf[3]);

        /* See if this is the last one */
        done = buf[0] & 0x80;

        /* If this isn't a type we care about, skip it. */
        if(block_type != METADATA_TYPE_VORBIS_COMMENT &&
           block_type != METADATA_TYPE_PICTURE) {
            fseek(fp, (long)block_len, SEEK_CUR);
            continue;
        }

        /* Since we're looking at the metadata block we want, allocate the space
           to store it. */
        tag = (uint8_t *)malloc((size_t)block_len);
        if(!tag) {
            goto out_close;
        }

        if(fread(tag, 1, (size_t)block_len, fp) != (size_t)block_len) {
            goto out_close;
        }

        if(block_type == METADATA_TYPE_VORBIS_COMMENT) {
            /* The _rawtag object will free the memory when its done */
            _rawtag = [[NSData alloc] initWithBytesNoCopy:tag
                                                   length:block_len
                                             freeWhenDone:YES];
            if(_rawtag == nil) {
                goto out_close;
            }
        }
        else if(block_type == METADATA_TYPE_PICTURE) {
            /* Make the data object we need to parse the picture */
            tmp = [[NSData alloc] initWithBytesNoCopy:tag
                                               length:block_len
                                         freeWhenDone:YES];

            if(tmp == nil) {
                goto out_close;
            }

            /* Parse it out */
            pic = [[STTagFLACPicture alloc] initWithData:tmp];

            if(pic) {
                [_pictures addObject:pic];
                [pic release];
            }

            [tmp release];
        }
    }

    /* We're done with the file at this point, no matter what. */
    fclose(fp);

    /* If we don't have a vorbis comment to work with, then we're kinda screwed
       at this point. */
    if(!_rawtag) {
        [self release];
        return nil;
    }

    /* Allocate the dictionary to hold comments. */
    _vorbisComments = [[NSMutableDictionary alloc] initWithCapacity:1];

    if([self parseComments]) {
        return self;
    }
    else {
        [self release];
        return nil;
    }

out_close:
    [self release];
    fclose(fp);
    return nil;
}

- (void)dealloc
{
    [_rawtag release];
    [_pictures release];
    [_vorbisComments release];
    [super dealloc];
}

- (NSString *)title
{
    return [self commentForKey:@"title"];
}

- (NSString *)artist
{
    return [self commentForKey:@"artist"];
}

- (NSString *)album
{
    return [self commentForKey:@"album"];
}

- (NSString *)year
{
    return [self commentForKey:@"date"];
}

- (NSString *)comment
{
    return [self commentForKey:@"description"];
}

- (NSData *)artwork
{
    if([_pictures count] > 0) {
        return [[_pictures objectAtIndex:0] pictureData];
    }

    return nil;
}

- (int)trackNumber
{
    NSString *n = [self commentForKey:@"tracknumber"];

    if(n) {
        return [n intValue];
    }
    else {
        return 0;
    }
}

- (int)discNumber
{
    NSString *n = [self commentForKey:@"discnumber"];

    if(n) {
        return [n intValue];
    }
    else {
        return 1;
    }
}

- (id)commentForKey:(NSString *)key index:(NSUInteger)i
{    
    return [[_vorbisComments objectForKey:key] objectAtIndex:i];
}

- (id)commentForKey:(NSString *)key
{
    return [self commentForKey:key index:0];
}

- (NSUInteger)commentCountForKey:(NSString *)key
{
    return [[_vorbisComments objectForKey:key] count];
}

- (void)addPicture:(STTagFLACPicture *)p
{
    [_pictures addObject:p];
}

- (void)addComment:(NSString *)comment key:(NSString *)key
{
    NSString *lkey = [key lowercaseString];
    NSMutableArray *a = [_vorbisComments objectForKey:lkey];

    /* Did the object already exist in the dictionary? */
    if(a) {
        /* Append this new item to the end. */
        [a addObject:comment];
    }
    else {
        /* Create a new item and add it to the dictionary */
        a = [NSMutableArray arrayWithCapacity:1];
        [a addObject:comment];
        [_vorbisComments setObject:a forKey:lkey];
    }
}

- (BOOL)writeToData:(NSMutableData *)d lastMeta:(BOOL)l error:(NSError **)err
{
    uint8_t hdr[4] = { METADATA_TYPE_VORBIS_COMMENT, 0, 0, 0 }, buf[4];
    uint32_t size = 0, count = 0, countpos, tmp;
    NSString *vendor = [_vorbisComments objectForKey:@"vendor"];
    NSData *tmpdata = [vendor dataUsingEncoding:NSUTF8StringEncoding];
    uint32_t start;

    /* Write out all of the picture data first */
    for(STTagFLACPicture *p in _pictures) {
        if(![p appendToData:d error:err]) {
            return NO;
        }
    }

    /* Record where we're starting from... */
    start = [d length];

    /* Is this the last metadata block? */
    if(l) {
        hdr[0] |= 0x80;
    }

    /* Write the metadata block header with a blank size for now */
    [d appendBytes:hdr length:4];

    /* Write the header of the vorbis comment (bleh! little endian stuff) */
    size = [tmpdata length];
    buf[0] = size;
    buf[1] = size >> 8;
    buf[2] = size >> 16;
    buf[3] = size >> 24;
    [d appendBytes:buf length:4];
    [d appendData:tmpdata];

    /* Save where we need to write the number of metadata strings... */
    countpos = start + size + 8;
    size += 8;
    [d appendBytes:&count length:4];

    /* Put each comment in the file */
    for(NSString *key in _vorbisComments) {
        if(![key isEqualToString:@"vendor"]) {
            NSArray *array = [_vorbisComments objectForKey:key];

            for(id comment in array) {
                NSString *str = [NSString stringWithFormat:@"%@=%@",
                                 [key uppercaseString], comment];
                const char *sbuf = [str UTF8String];

                /* Figure out the length (little endian) */
                tmp = [str length];
                buf[0] = tmp;
                buf[1] = tmp >> 8;
                buf[2] = tmp >> 16;
                buf[3] = tmp >> 24;

                /* Write the length and the comment */
                [d appendBytes:buf length:4];
                [d appendBytes:sbuf length:tmp];

                size += tmp + 4;
                ++count;
            }
        }
    }

    /* We now have the total size, so write it in the header */
    hdr[1] = size >> 16;
    hdr[2] = size >> 8;
    hdr[3] = size;
    [d replaceBytesInRange:NSMakeRange(start + 1, 3) withBytes:hdr + 1];

    /* Write the count of items... */
    buf[0] = count;
    buf[1] = count >> 8;
    buf[2] = count >> 16;
    buf[3] = count >> 24;
    [d replaceBytesInRange:NSMakeRange(countpos, 4) withBytes:buf];

    return YES;
}

- (id)id3v2FrameForKey:(STTagID3v2_FrameCode)fc
{
    STTagID3v2TextFrame *tf;
    STTagID3v2CommentFrame *cf;
    NSString *val;

    switch(fc) {
        case FrameLeadPerformer:
            val = [self commentForKey:@"artist"];
            goto text_frame;

        case FrameAlbumTitle:
            val = [self commentForKey:@"album"];
            goto text_frame;

        case FrameTitle:
            val = [self commentForKey:@"title"];
            goto text_frame;

        case FrameTrackNumber:
            /* XXXX */
            val = [self commentForKey:@"tracknumber"];
            goto text_frame;

        case FrameReleaseTime:
            val = [self commentForKey:@"date"];
            goto text_frame;

        case FrameComments:
            cf = [[STTagID3v2CommentFrame alloc] initWithType:fc];

            if(!cf) {
                return nil;
            }

            [cf setText:[self commentForKey:@"comment"]];
            return [cf autorelease];

        case FrameAccompaniment:
            val = [self commentForKey:@"albumartist"];
            goto text_frame;

        case FrameComposer:
            val = [self commentForKey:@"composer"];
            goto text_frame;

        case FrameContentGrpDesc:
            val = [self commentForKey:@"grouping"];
            goto text_frame;

        case FramePartOfSet:
            /* XXXX */
            val = [self commentForKey:@"discnumber"];
            goto text_frame;

        case FrameBPM:
            val = [self commentForKey:@"bpm"];
            goto text_frame;

        case FramePartOfCompilation:
            val = [self commentForKey:@"compilation"];
            goto text_frame;

        case FramePerformerSortOrd:
            val = [self commentForKey:@"artistsort"];
            goto text_frame;

        case FrameAlbumArtistSortOrd:
            val = [self commentForKey:@"albumartistsort"];
            goto text_frame;

        case FrameAlbumSortOrd:
            val = [self commentForKey:@"albumsort"];
            goto text_frame;

        case FrameTitleSortOrd:
            val = [self commentForKey:@"titlesort"];
            goto text_frame;

        case FrameComposerSortOrd:
            val = [self commentForKey:@"composersort"];
            goto text_frame;

        case FrameEncodedBy:
            val = [self commentForKey:@"vendor"];
            goto text_frame;

        case FrameISRC:
            val = [self commentForKey:@"isrc"];
            goto text_frame;

        case FramePublisher:
            /* XXXX */
            val = [self commentForKey:@"label"];
            goto text_frame;

        case FrameLyricist:
            val = [self commentForKey:@"lyricist"];
            goto text_frame;

        case FrameConductor:
            val = [self commentForKey:@"conductor"];
            goto text_frame;

        case FrameModifiedBy:
            val = [self commentForKey:@"remixer"];
            goto text_frame;

        case FrameMood:
            val = [self commentForKey:@"mood"];
            goto text_frame;

        case FrameMediaType:
            val = [self commentForKey:@"media"];
            goto text_frame;

        case FrameLanguage:
            val = [self commentForKey:@"language"];
            goto text_frame;

        case FrameInitialKey:
            val = [self commentForKey:@"key"];
            goto text_frame;

        case FrameCopyrightMsg:
            val = [self commentForKey:@"copyright"];
            goto text_frame;

        case FrameContentType:
            val = [self commentForKey:@"genre"];
            goto text_frame;
    }

    /* Nothing that we know about, so return nil */
    return nil;

text_frame:
    if(val) {
        tf = [[STTagID3v2TextFrame alloc] initWithType:fc];

        if(!tf) {
            return nil;
        }

        [tf setText:val encoding:STTAGID3V2_ENCODING_UTF8];
        return [tf autorelease];
    }

    return nil;
}

@end /* @implementation STTagFLAC */

@implementation STTagFLAC (internal)

- (BOOL)parseComments
{
    const uint8_t *buf = [_rawtag bytes];
    uint32_t start = 0, sz, count;
    NSString *tmp;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    unsigned int loc;

    /* The first part of the Vorbis Comment is the vendor of the encoder. */
    sz = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    tmp = [[NSString alloc] initWithBytes:buf + 4
                                   length:sz
                                 encoding:NSUTF8StringEncoding];
    [self addComment:tmp key:@"vendor"];
    [tmp release];

    /* Set up the rest of the parsing */
    start = sz + 4;
    count = buf[start] | (buf[start + 1] << 8) | (buf[start + 2] << 16) |
        (buf[start + 3] << 24);
    start += 4;

    while(start < [_rawtag length] && count--) {
        /* Read the size of the next comment */
        sz = buf[start] | (buf[start + 1] << 8) | (buf[start + 2] << 16) |
            (buf[start + 3] << 24);

        /* Read in the comment, and parse it */
        tmp = [[NSString alloc] initWithBytes:buf + start + 4
                                       length:sz
                                     encoding:NSUTF8StringEncoding];

        loc = [tmp indexOfCharacter:(unichar)'='];

        /* Add the comment to the list */
        if(loc != (unsigned int)NSNotFound) {
            [self addComment:[tmp substringFromIndex:loc + 1]
                         key:[tmp substringToIndex:loc]];
        }

        [tmp release];
        start += sz + 4;
    }

    [pool drain];
    return YES;
}

@end /* @implementation STTagFLAC (internal) */
