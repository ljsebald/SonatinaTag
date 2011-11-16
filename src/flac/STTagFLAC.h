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

#ifndef STTagFLAC_h
#define STTagFLAC_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTag.h>

@class STTagFLACPicture;

@interface STTagFLAC : NSObject<STTag> {
@private
    NSMutableDictionary *_vorbisComments;
    NSMutableArray *_pictures;
    NSData *_rawtag;
}

- (id)init;

- (id)initFromFile:(NSString *)file;
- (void)dealloc;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;

- (NSData *)artwork;

- (int)trackNumber;
- (int)discNumber;

- (id)commentForKey:(NSString *)key index:(NSUInteger)i;
- (id)commentForKey:(NSString *)key;
- (NSUInteger)commentCountForKey:(NSString *)key;

- (void)addPicture:(STTagFLACPicture *)p;
- (void)addComment:(NSString *)comment key:(NSString *)key;
- (BOOL)writeToData:(NSMutableData *)d lastMeta:(BOOL)l error:(NSError **)err;

- (id)id3v2FrameForKey:(STTagID3v2_FrameCode)fc;
- (NSDictionary *)tagDictionary;

- (id<STTagPicture>)artworkOfType:(STTagPictureType)type index:(NSUInteger)i;
- (NSArray *)allArtwork;

- (id<STTagComment>)commentAtIndex:(NSUInteger)i;
- (NSArray *)allComments;

@end /* @interface STTagFLAC */

#endif /* !STTagFLAC_h */
