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

#include "STTagID3v2UserURLFrame.h"

static NSStringEncoding encs[4] = {
    NSISOLatin1StringEncoding,
    NSUTF16StringEncoding,
    NSUTF16BigEndianStringEncoding,
    NSUTF8StringEncoding
};

@interface STTagID3v2UserURLFrame (Internal)
- (void)rebuildRawFrame;
@end /* @interface STTAGID3v2UserURLFrame (Internal) */

@implementation STTagID3v2UserURLFrame

- (NSURL *)URL
{
    return [NSURL URLWithString:[self text]];
}

- (void)setURL:(NSURL *)url
{
    [self setText:[url absoluteString]];
}

@end /* @implementation STTagID3v2UserURLFrame */

@implementation STTagID3v2UserURLFrame (Internal)

- (void)rebuildRawFrame
{
    NSData *d;
    int _encoding = [self encoding];
    NSData *ds = [[self text] dataUsingEncoding:encs[_encoding]
                     allowLossyConversion:YES];
    NSData *desc = [[self description] dataUsingEncoding:encs[_encoding]
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

@end /* @implementation STTagID3v2UserURLFrame (Internal) */
