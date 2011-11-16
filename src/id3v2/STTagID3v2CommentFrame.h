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

#ifndef STTagID3v2CommentFrame_h
#define STTagID3v2CommentFrame_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>
#include <SonatinaTag/STTagID3v2Frame.h>

/* Grab the encoding list */
#include <SonatinaTag/STTagID3v2TextFrame.h>

@interface STTagID3v2CommentFrame : STTagID3v2Frame <STTagComment> {
@private
    uint8_t _encoding;
    NSString *_language;
    NSString *_description;
    NSString *_text;
}

- (id)initWithType:(STTagID3v2_FrameCode)type;

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data;
- (void)dealloc;

- (uint8_t)encoding;
- (NSString *)language;
- (NSString *)description;
- (NSString *)comment;

- (BOOL)setEncoding:(uint8_t)enc;
- (BOOL)setComment:(NSString *)s;
- (BOOL)setLanguage:(NSString *)language;
- (BOOL)setDescription:(NSString *)description;

@end /* @interface STTagID3v2CommentFrame */

#endif /* !STTagID3v2CommentFrame_h */
