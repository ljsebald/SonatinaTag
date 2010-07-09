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

#ifndef STTagID3v2PictureFrame_h
#define STTagID3v2PictureFrame_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTagID3v2Frame.h>

typedef enum STTagPictureType_e {
    STTagPictureType_Other              = 0,
    STTagPictureType_SmallIcon          = 1,
    STTagPictureType_OtherIcon          = 2,
    STTagPictureType_FrontCover         = 3,
    STTagPictureType_BackCover          = 4,
    STTagPictureType_Leaflet            = 5,
    STTagPictureType_Media              = 6,
    STTagPictureType_LeadArtist         = 7,
    STTagPictureType_Artist             = 8,
    STTagPictureType_Conductor          = 9,
    STTagPictureType_Band               = 10,
    STTagPictureType_Composer           = 11,
    STTagPictureType_Lyricist           = 12,
    STTagPictureType_RecordingLocation  = 13,
    STTagPictureType_DuringRecording    = 14,
    STTagPictureType_DuringPerformance  = 15,
    STTagPictureType_ScreenCapture      = 16,
    STTagPictureType_ABrightColoredFish = 17,
    STTagPictureType_Illustration       = 18,
    STTagPictureType_ArtistLogo         = 19,
    STTagPictureType_PublisherLogo      = 20,
    STTagPictureType_MAX                = 20
} STTagPictureType;

@interface STTagID3v2PictureFrame : STTagID3v2Frame {
@private
    uint8_t _textEncoding;
    NSString *_mimeType;
    uint8_t _pictureType;
    NSString *_description;
    NSData *_pictureData;
}

- (id)initWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
             flags:(uint16_t)flags data:(NSData *)data;
+ (id)frameWithType:(STTagID3v2_FrameCode)type size:(uint32_t)size
              flags:(uint16_t)flags data:(NSData *)data;
- (void)dealloc;

- (uint8_t)textEncoding;
- (NSString *)mimeType;
- (uint8_t)pictureType;
- (NSString *)description;
- (NSData *)pictureData;

@end /* @interface STTagID3v2PictureFrame */

#endif /* !STTagID3v2PictureFrame_h */
