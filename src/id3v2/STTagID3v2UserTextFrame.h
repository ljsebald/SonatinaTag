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

#ifndef STTagID3v2UserTextFrame_h
#define STTagID3v2UserTextFrame_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTagID3v2Frame.h>
#include <SonatinaTag/STTagID3v2TextFrame.h>

@interface STTagID3v2UserTextFrame : STTagID3v2Frame {
@private
    uint8_t _encoding;
    NSString *_description;
    NSString *_text;
}

- (id)initWithType:(STTagID3v2_FrameCode)type;

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data;
- (void)dealloc;

- (uint8_t)encoding;
- (NSString *)text;
- (NSString *)description;

- (void)setText:(NSString *)s;
- (void)setEncoding:(uint8_t)enc;
- (void)setDescription:(NSString *)d;

@end /* @interface STTagID3v2UserTextFrame */

#endif /* !STTagID3v2UserTextFrame_h */
