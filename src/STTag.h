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

#ifndef STTag_h
#define STTag_h

#import <Foundation/Foundation.h>

#include <SonatinaTag/STTagID3v2Frame.h>

@protocol STTag <NSObject>
@required
- (id)initFromFile:(NSString *)filename;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;

- (NSData *)artwork;

- (int)trackNumber;
- (int)discNumber;

@optional
- (id)id3v2FrameForKey:(STTagID3v2_FrameCode)fc;

@end /* @protocol STTag */

#endif /* !STTag_h */
