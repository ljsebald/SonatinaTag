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

#ifndef STTagID3v2_h
#define STTagID3v2_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>

#define STTAGID3V2_FLAG_UNSYNC  (1 << 7)
#define STTAGID3V2_FLAG_EXTHDR  (1 << 6)
#define STTAGID3V2_FLAG_EXP     (1 << 5)
#define STTAGID3V2_FLAG_FOOTER  (1 << 4)

#define STTAGID3V2_FLAG_MASK_23 0xE0
#define STTAGID3V2_FLAG_MASK_24 0xF0

/* Forward declarations */
@class STTagID3v2Frame;

#ifndef SONATINATAG__FRAMECODES_DEFINED
#define SONATINATAG__FRAMECODES_DEFINED
typedef enum STTagID3v2_FrameCode_e STTagID3v2_FrameCode;
#endif

@interface STTagID3v2 : NSObject<STTag> {
@private
    NSMutableDictionary *_frames;
    NSData *_rawtag;
    uint8_t _majorver;
    uint8_t _revision;
    uint8_t _flags;
    uint32_t _size;
}

- (id)initFromFile:(NSString *)file;
- (void)dealloc;

- (id)frameForKey:(STTagID3v2_FrameCode)fourcc;

- (int)id3v2MajorVersion;
- (int)id3v2Revision;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;

- (NSData *)artwork;

- (NSString *)genre;
- (int)trackNumber;
- (int)discNumber;

@end /* @interface STTagID3v2 */

#endif /* !STTagID3v2_h */
