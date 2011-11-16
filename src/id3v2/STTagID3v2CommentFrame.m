/*
    SonatinaTag
    Copyright (C) 2011 Lawrence Sebald

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

#include "STTagID3v2CommentFrame.h"

static NSStringEncoding encs[4] = {
    NSISOLatin1StringEncoding,
    NSUTF16StringEncoding,
    NSUTF16BigEndianStringEncoding,
    NSUTF8StringEncoding
};

@interface STTagID3v2CommentFrame (Internal)
- (void)rebuildRawFrame;
@end /* @interface STTAGID3v2CommentFrame (Internal) */

@implementation STTagID3v2CommentFrame

- (id)initWithType:(STTagID3v2_FrameCode)type
{
    if((self = [super initWithType:type])) {
        _encoding = STTAGID3V2_ENCODING_ISO8859_1;

        if(!(_language = [[NSString alloc] initWithString:@"eng"])) {
            [self release];
            self = nil;
        }
    }

    return self;
}

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data
{
    const uint8_t *bytes = [data bytes];
    int len;
    int enc_len;

    if((self = [super initWithType:type size:size flags:flags data:data])) {
        /* Make sure the encoding is valid */
        if(bytes[0] > STTAGID3V2_ENCODING_UTF8) {
            [self release];
            return nil;
        }


        _encoding = bytes[0];
        _language = [[NSString alloc] initWithFormat:@"%c%c%c", bytes[1],
                     bytes[2], bytes[3]];
        _description = [[NSString alloc] initWithCString:(char *)bytes + 4
                                                encoding:encs[_encoding]];

        enc_len = ((_encoding == STTAGID3V2_ENCODING_UTF16) ||
                   (_encoding == STTAGID3V2_ENCODING_UTF16BE)) ? 2 : 1;
        len = 4 + ([_description length] + 1) * enc_len;

        _text = [[NSString alloc] initWithBytes:bytes + len
                                         length:size - len
                                       encoding:encs[_encoding]];
    }

    return self;
}

- (void)dealloc
{
    [_language release];
    [_description release];
    [super dealloc];
}

- (uint8_t)encoding
{
    return _encoding;
}

- (NSString *)language
{
    return [NSString stringWithString:_language];
}

- (NSString *)description
{
    return [NSString stringWithString:_description];
}

- (NSString *)comment
{
    NSCharacterSet *s =
        [NSCharacterSet characterSetWithCharactersInString:@"\0"];
    return [_text stringByTrimmingCharactersInSet:s];
}

- (BOOL)setEncoding:(uint8_t)enc
{
    if(_encoding > STTAGID3V2_ENCODING_UTF8) {
        return NO;
    }

    _encoding = enc;
    [self rebuildRawFrame];
    return YES;
}

- (BOOL)setComment:(NSString *)s
{
    if(!s) {
        return NO;
    }

    [_text autorelease];
    _text = [s retain];
    [self rebuildRawFrame];
    return YES;
}

- (BOOL)setLanguage:(NSString *)language
{
    if(!language || [language length] != 3) {
        return NO;
    }

    [_language autorelease];
    _language = [language retain];
    [self rebuildRawFrame];
    return YES;
}

- (BOOL)setDescription:(NSString *)description
{
    if(!description) {
        return NO;
    }

    [_description autorelease];
    _description = [description retain];
    [self rebuildRawFrame];
    return YES;
}

@end /* @implementation STTagID3v2CommentFrame */

@implementation STTagID3v2CommentFrame (Internal)

- (void)rebuildRawFrame
{
    NSData *d;
    int encoding = (int)[self encoding];
    NSData *ds = [_text dataUsingEncoding:encs[encoding]
                     allowLossyConversion:YES];
    NSData *desc = [_description dataUsingEncoding:encs[encoding]
                              allowLossyConversion:YES];
    int term_len = ((encoding == 1) || (encoding == 2)) ? 2 : 1;
    int len = [ds length] + [desc length] + term_len + 3;
    uint8_t tmp[len];

    /* Fill in the data */
    tmp[0] = encoding;
    [_language getCString:(char *)tmp + 1
                maxLength:4
                 encoding:NSASCIIStringEncoding];

    [desc getBytes:tmp + 4 length:[desc length]];
    tmp[4 + [desc length]] = 0;

    if(term_len == 2) {
        tmp[4 + [desc length] + 1] = 0;
    }

    [ds getBytes:tmp + 4 + [desc length] + term_len length:[ds length]];

    d = [[NSData alloc] initWithBytes:tmp length:len];
    [self setFrame:d];
}

@end /* @implementation STTagID3v2CommentFrame (Internal) */
