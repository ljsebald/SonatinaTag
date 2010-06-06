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

#include "STTagID3v2URLFrame.h"

@implementation STTagID3v2URLFrame
- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data
{
    const char *bytes = [data bytes];
    NSString *str = [[NSString alloc]initWithBytes:bytes
                                            length:size
                                          encoding:NSISOLatin1StringEncoding];

    self = [super initWithType:type size:size flags:flags data:data];

    if(self != nil) {
        _url = [[NSURL alloc] initWithString:str];
    }

    [str release];
    return self;
}

+ (id)frameWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
              flags:(uint16_t)flags data:(NSData *)data;
{
    return [[[self alloc] initWithType:type
                                  size:size
                                 flags:flags
                                  data:data] autorelease];
}

- (void)dealloc
{
    [_url release];
    [super dealloc];
}

- (NSURL *)URL
{
    return _url;
}

@end /* @implementation STTagID3v2URLFrame */
