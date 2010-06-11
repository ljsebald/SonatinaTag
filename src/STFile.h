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

#ifndef STFile_h
#define STFile_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>

#define STFileID3v1Type     @"ID3v1"
#define STFileID3v2Type     @"ID3v2"
#define STFileM4AType       @"M4A"

@interface STFile : NSObject {
@private
    NSString *_filename;
    NSMutableDictionary *_tags;
}

- (id)initWithFile:(NSString *)filename;
- (void)dealloc;

- (id<STTag>)getTagForType:(NSString *)type;
- (id<STTag>)tag;

@end /* @interface STFile */

#endif /* !STFile_h */
