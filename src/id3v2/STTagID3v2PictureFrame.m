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

#include "STTagID3v2PictureFrame.h"
#include "STTagID3v2TextFrame.h"

static NSStringEncoding encs[4] = {
    NSISOLatin1StringEncoding,
    NSUTF16StringEncoding,
    NSUTF16BigEndianStringEncoding,
    NSUTF8StringEncoding
};

@implementation STTagID3v2PictureFrame

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data
{
    const uint8_t *bytes = [data bytes];
    int mimelen, desclen, i, endcode;
    NSRange r;

    if((self = [super initWithType:type size:size flags:flags data:data])) {
        /* Make sure the encoding is valid */
        if(bytes[0] > STTAGID3V2_ENCODING_UTF8) {
            [self release];
            return nil;
        }

        _textEncoding = bytes[0];

        /* Figure out how long the MIME type string is, and allocate the string
           for it (always ISO-8859-1) */
        mimelen = strlen((char *)(bytes + 1));
        _mimeType = [[NSString alloc] initWithBytes:bytes + 1
                                             length:mimelen
                                           encoding:NSISOLatin1StringEncoding];

        /* Next up is the picture type */
        _pictureType = bytes[mimelen + 2];

        /* Description comes next... */
        if(_textEncoding == STTAGID3V2_ENCODING_UTF8 ||
           _textEncoding == STTAGID3V2_ENCODING_ISO8859_1) {
            desclen = strlen((char *)(bytes + mimelen + 3));
            endcode = 1;
        }
        else {
            /* Figure out how long it is the hard way */
            for(i = mimelen + 3, desclen = 0; i < size; i += 2, desclen += 2) {
                if(bytes[i] == 0 && bytes[i + 1] == 0) {
                    break;
                }
            }

            endcode = 2;
        }

        _description = [[NSString alloc] initWithBytes:bytes + mimelen + 3
                                                length:desclen
                                              encoding:encs[_textEncoding]];

        /* Finally, we have the image data */
        r.location = mimelen + desclen + endcode + 3;
        r.length = size - r.location;
        _pictureData = [[data subdataWithRange:r] retain];

        /* Make sure everything got allocated properly */
        if(!_mimeType || !_description || !_pictureData) {
            [self release];
            return nil;
        }
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
    [_mimeType release];
    [_description release];
    [_pictureData release];
    [super dealloc];
}

- (uint8_t)textEncoding
{
    return _textEncoding;
}

- (NSString *)mimeType
{
    return _mimeType;
}

- (uint8_t)pictureType
{
    return _pictureType;
}

- (NSString *)description
{
    return _description;
}

- (NSData *)pictureData
{
    return _pictureData;
}

@end /* @implementation STTagID3v2PictureFrame */
