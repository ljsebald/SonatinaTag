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

#ifndef STTagFLACPicture_h
#define STTagFLACPicture_h

#import <Foundation/Foundation.h>

/* Grab the picture types */
#include <SonatinaTag/STTagID3v2PictureFrame.h>

@interface STTagFLACPicture : NSObject {
@private
    uint32_t _pictureType;
    NSString *_mimeType;
    NSString *_description;
    uint32_t _width;
    uint32_t _height;
    uint32_t _bitDepth;
    uint32_t _indexUsed;
    NSData *_pictureData;
}

- (id)initWithData:(NSData *)data;
- (void)dealloc;

- (uint32_t)pictureType;
- (NSString *)mimeType;
- (NSString *)description;
- (uint32_t)width;
- (uint32_t)height;
- (uint32_t)bitDepth;
- (uint32_t)indexUsed;

- (NSData *)pictureData;

@end /* @interface STTagFLACPicture */

#endif /* !STTagFLACPicture_h */
