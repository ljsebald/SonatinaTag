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

#include "STTagID3v2UserTextFrame.h"

static NSStringEncoding encs[4] = {
    NSISOLatin1StringEncoding,
    NSUTF16StringEncoding,
    NSUTF16BigEndianStringEncoding,
    NSUTF8StringEncoding
};

@interface STTagID3v2UserTextFrame (Internal)
- (void)rebuildRawFrame;
@end /* @interface STTAGID3v2UserTextFrame (Internal) */

@implementation STTagID3v2UserTextFrame

- (id)initWithType:(STTagID3v2_FrameCode)type
{
    if((self = [super initWithType:type])) {
        _encoding = STTAGID3V2_ENCODING_ISO8859_1;
    }

    return self;
}

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data
{
    const uint8_t *bytes = [data bytes];
    int enc_len, len;

    if((self = [super initWithType:type size:size flags:flags data:data])) {
        /* Make sure the encoding is valid */
        if(bytes[0] > STTAGID3V2_ENCODING_UTF8) {
            [self release];
            return nil;
        }

        _encoding = bytes[0];
        _description = [[NSString alloc] initWithCString:(char *)bytes + 1
                                                encoding:encs[_encoding]];

        enc_len = ((_encoding == STTAGID3V2_ENCODING_UTF16) ||
                   (_encoding == STTAGID3V2_ENCODING_UTF16BE)) ? 2 : 1;
        len = 1 + ([_description length] + 1) * enc_len;

        _text = [[NSString alloc] initWithBytes:bytes + len
                                         length:size - len
                                       encoding:encs[_encoding]];
    }

    return self;
}

- (void)dealloc
{
    [_text release];
    [_description release];
    [super dealloc];
}

- (uint8_t)encoding
{
    return _encoding;
}

- (NSString *)text
{
    NSCharacterSet *s =
        [NSCharacterSet characterSetWithCharactersInString:@"\0"];
    return [_text stringByTrimmingCharactersInSet:s];
}

- (NSString *)description
{
    NSCharacterSet *s =
        [NSCharacterSet characterSetWithCharactersInString:@"\0"];
    return [_description stringByTrimmingCharactersInSet:s];
}

- (void)setEncoding:(uint8_t)enc
{
    _encoding = enc;
    [self rebuildRawFrame];
}

- (void)setText:(NSString *)s
{
    [_text autorelease];
    _text = [s retain];

    if(_text) {
        [self rebuildRawFrame];
    }
}

- (void)setDescription:(NSString *)s
{
    [_description autorelease];
    _description = [s retain];
    
    if(_description) {
        [self rebuildRawFrame];
    }
}

@end /* @implementation STTagID3v2UserTextFrame */

@implementation STTagID3v2UserTextFrame (Internal)

- (void)rebuildRawFrame
{
    NSData *d;
    NSData *ds = [_text dataUsingEncoding:encs[_encoding]
                     allowLossyConversion:YES];
    NSData *desc = [_description dataUsingEncoding:encs[_encoding]
                              allowLossyConversion:YES];
    int term_len = ((_encoding == 1) || (_encoding == 2)) ? 2 : 1;
    int len = [ds length] + [desc length] + term_len + 1;
    uint8_t tmp[len];

    /* Fill in the data */
    tmp[0] = _encoding;

    [desc getBytes:tmp + 1 length:[desc length]];
    tmp[1 + [desc length]] = 0;

    if(term_len == 2) {
        tmp[1 + [desc length] + 1] = 0;
    }

    [ds getBytes:tmp + 1 + [desc length] + term_len length:[ds length]];

    d = [[NSData alloc] initWithBytes:tmp length:len];
    [self setFrame:d];
}

@end /* @implementation STTagID3v2UserTextFrame (Internal) */
