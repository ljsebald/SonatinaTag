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

#include "STTagID3v2Frame.h"
#include "NSErrorExt.h"

@implementation STTagID3v2Frame

- (id)initWithType:(STTagID3v2_FrameCode)type
{
    if((self = [super init])) {
        _frameType = type;
    }

    return self;
}

+ (id)frameWithType:(STTagID3v2_FrameCode)type
{
    return [[[self alloc] initWithType:type] autorelease];
}

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data
{
    if((self = [super init])) {
        _frameType = type;
        _size = size;
        _flags = flags;
        _rawFrame = [data retain];
    }

    return self;
}

+ (id)frameWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
              flags:(uint16_t)flags data:(NSData *)data
{
    return [[[self alloc] initWithType:type
                                  size:size
                                 flags:flags
                                  data:data] autorelease];
}

- (void)dealloc
{
    [_rawFrame release];
    [super dealloc];
}

- (STTagID3v2_FrameCode)type
{
    return _frameType;
}

- (uint32_t)size
{
    return _size;
}

- (uint16_t)flags
{
    return _flags;
}

- (NSData *)frame
{
    return _rawFrame;
}

- (void)setFlags:(uint16_t)f
{
    _flags = f;
}

- (void)setFrame:(NSData *)d
{
    [_rawFrame autorelease];
    _rawFrame = [d retain];
    _size = [d length];
}

- (BOOL)appendToData:(NSMutableData *)d
         withVersion:(int)version
               error:(NSError **)err
{
    int hdrlen = version > 2 ? 10 : 6;
    char hdr[10] = { _frameType >> 24, _frameType >> 16, _frameType >> 8,
        _frameType };

    /* Build the header */
    switch(version) {
        case 2:
            /* Three byte type (so overwrite the last byte written in the header
               from above), three byte size (no flags) */
            hdr[3] = _size >> 16;
            hdr[4] = _size >> 8;
            hdr[5] = _size;
            break;

        case 3:
            /* The size is in big endian order, and formed the normal way */
            hdr[4] = _size >> 24;
            hdr[5] = _size >> 16;
            hdr[6] = _size >> 8;
            hdr[7] = _size;
            hdr[8] = _flags >> 8;
            hdr[9] = _flags;
            break;

        case 4:
            /* The size is in big endian, and the most significant bit of each
               byte is zeroed, leaving us with 28 bits of actual data */
            hdr[4] = (_size >> 21) & 0x7F;
            hdr[5] = (_size >> 14) & 0x7F;
            hdr[6] = (_size >> 7) & 0x7F;
            hdr[7] = _size & 0x7F;
            hdr[8] = _flags >> 8;
            hdr[9] = _flags;
            break;

        default:
            if(err) {
                *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                           code:STError_InvalidParam
                                         reason:@"Invalid ID3v2 Version"];
            }

            return NO;
    }

    [d appendBytes:hdr length:hdrlen];
    [d appendData:_rawFrame];
    return YES;
}

@end /* @implementation STTagID3v2Frame */
