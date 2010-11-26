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

#include "STTagFLAC.h"
#include "STTagFLACPicture.h"

#define METADATA_TYPE_VORBIS_COMMENT    4
#define METADATA_TYPE_PICTURE           6

@interface STTagFLAC (internal)
- (BOOL)parseComments;
@end /* @interface STTagFLAC (internal) */

@implementation STTagFLAC

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
    return [_vorbisComments objectForKey:@"title"];
}

- (NSString *)artist
{
    return [_vorbisComments objectForKey:@"artist"];
}

- (NSString *)album
{
    return [_vorbisComments objectForKey:@"album"];
}

- (NSString *)year
{
    return [_vorbisComments objectForKey:@"date"];
}

- (NSString *)comment
{
    return [_vorbisComments objectForKey:@"description"];
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
    NSString *n = [_vorbisComments objectForKey:@"tracknumber"];

    if(n) {
        return [n intValue];
    }
    else {
        return 0;
    }
}

- (int)discNumber
{
    NSString *n = [_vorbisComments objectForKey:@"discnumber"];

    if(n) {
        return [n intValue];
    }
    else {
        return 1;
    }
}

@end /* @implementation STTagFLAC */

@implementation STTagFLAC (internal)

- (BOOL)parseComments
{
    const uint8_t *buf = [_rawtag bytes];
    uint32_t start = 0, sz, count;
    NSString *tmp;
    NSArray *kv;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    /* The first part of the Vorbis Comment is the vendor of the encoder. */
    sz = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    tmp = [[NSString alloc] initWithBytes:buf + 4
                                   length:sz
                                 encoding:NSUTF8StringEncoding];
    [_vorbisComments setValue:tmp forKey:@"vendor"];
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
        kv = [tmp componentsSeparatedByString:@"="];

        /* Add the comment to the list */
        if([kv count] == 2) {
            [_vorbisComments setValue:[kv objectAtIndex:1]
                               forKey:[[kv objectAtIndex:0] lowercaseString]];
        }

        [tmp release];
        start += sz + 4;
    }

    [pool drain];
    return YES;
}

@end /* @implementation STTagFLAC (internal) */
