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

#include "STTagFLACPicture.h"
#include <arpa/inet.h>

#define METADATA_TYPE_PICTURE           6

@implementation STTagFLACPicture

- (id)init
{
    if((self = [super init])) {
        _description = [[NSString alloc] init];
        _mimeType = [[NSString alloc] init];
    }

    return self;
}

+ (id)flacPicture
{
    return [[[self alloc] init] autorelease];
}

- (id)initWithData:(NSData *)data
{
    const uint8_t *bytes = [data bytes];
    uint32_t start, sz;

    if((self = [super init])) {
        /* The picture type is the first thing, make sure its valid */
        _pictureType = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) |
            bytes[3];

        if(_pictureType > STTagPictureType_MAX) {
            [self release];
            return nil;
        }

        /* Next up is the mime type string (ASCII) */
        sz = (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];
        _mimeType = [[NSString alloc] initWithBytes:bytes + 8
                                             length:sz
                                           encoding:NSISOLatin1StringEncoding];
        if(!_mimeType) {
            [self release];
            return nil;
        }

        /* Next up is the description (UTF-8) */
        start = sz + 8;
        sz = (bytes[start] << 24) | (bytes[start + 1] << 16) |
            (bytes[start + 2] << 8) | bytes[start + 3];
        _description = [[NSString alloc] initWithBytes:bytes + start + 4
                                                length:sz
                                              encoding:NSUTF8StringEncoding];

        if(!_description) {
            [self release];
            return nil;
        }

        /* Next up is the width/height/color depth/index info */
        start += sz + 4;
        _width = (bytes[start] << 24) | (bytes[start + 1] << 16) |
            (bytes[start + 2] << 8) | bytes[start + 3];
        _height = (bytes[start + 4] << 24) | (bytes[start + 5] << 16) |
            (bytes[start + 6] << 8) | bytes[start + 7];
        _bitDepth = (bytes[start + 8] << 24) | (bytes[start + 9] << 16) |
            (bytes[start + 10] << 8) | bytes[start + 11];
        _indexUsed = (bytes[start + 12] << 24) | (bytes[start + 13] << 16) |
            (bytes[start + 14] << 8) | bytes[start + 15];

        /* Finally is the picture data */
        sz = (bytes[start + 16] << 24) | (bytes[start + 17] << 16) |
            (bytes[start + 18] << 8) | bytes[start + 19];
        _pictureData = [[NSData alloc] initWithBytes:bytes + start + 20
                                              length:sz];

        if(!_pictureData) {
            [self release];
            return nil;
        }
    }

    return self;
}

- (void)dealloc
{
    [_mimeType release];
    [_description release];
    [_pictureData release];
    [super dealloc];
}

- (uint32_t)pictureType
{
    return _pictureType;
}

- (NSString *)mimeType
{
    return _mimeType;
}

- (NSString *)description
{
    return _description;
}

- (uint32_t)width
{
    return _width;
}

- (uint32_t)height
{
    return _height;
}

- (uint32_t)bitDepth
{
    return _bitDepth;
}

- (uint32_t)indexUsed
{
    return _indexUsed;
}

- (NSData *)pictureData
{
    return _pictureData;
}

- (void)setPictureData:(NSData *)d
                ofType:(uint32_t)type
                  mime:(NSString *)mime
           description:(NSString *)desc
                 width:(uint32_t)w
                height:(uint32_t)h
                   bpp:(uint32_t)bpp
             indexUsed:(uint32_t)index
{
    _pictureData = [d retain];
    _pictureType = type;
    _mimeType = [mime retain];
    _description = [desc retain];
    _width = w;
    _height = h;
    _bitDepth = bpp;
    _indexUsed = index;
}

- (BOOL)appendToData:(NSMutableData *)d error:(NSError **)err
{
    uint32_t tmp;
    NSData *tmpdata = [_mimeType dataUsingEncoding:NSASCIIStringEncoding
                              allowLossyConversion:YES];
    uint32_t size = 0;
    uint32_t start = [d length];
    uint8_t hdr[4] = { METADATA_TYPE_PICTURE, 0, 0, 0 };

    /* Write out the header with a blank size for now */
    [d appendBytes:hdr length:4];

    /* Write out the type... */
    tmp = htonl(_pictureType);
    [d appendBytes:&tmp length:4];
    size += 4;

    /* MIME type */
    tmp = htonl([tmpdata length]);
    [d appendBytes:&tmp length:4];
    [d appendData:tmpdata];
    size += 4 + [tmpdata length];

    /* Description */
    tmpdata = [_description dataUsingEncoding:NSUTF8StringEncoding];
    tmp = htonl([tmpdata length]);
    [d appendBytes:&tmp length:4];
    [d appendData:tmpdata];
    size += 4 + [tmpdata length];

    /* Image characteristics */
    tmp = htonl(_width);
    [d appendBytes:&tmp length:4];
    tmp = htonl(_height);
    [d appendBytes:&tmp length:4];
    tmp = htonl(_bitDepth);
    [d appendBytes:&tmp length:4];
    tmp = htonl(_indexUsed);
    [d appendBytes:&tmp length:4];
    size += 16;

    /* Image data */
    tmp = htonl([_pictureData length]);
    [d appendBytes:&tmp length:4];
    [d appendData:_pictureData];
    size += 4 + [_pictureData length];

    /* Fix the size in the header */
    hdr[1] = size >> 16;
    hdr[2] = size >> 8;
    hdr[3] = size;
    [d replaceBytesInRange:NSMakeRange(start + 1, 3) withBytes:hdr + 1];
    return YES;
}

@end /* @implementation STTagFLACPicture */
