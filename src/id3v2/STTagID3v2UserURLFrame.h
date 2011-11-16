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

#ifndef STTagID3v2UserURLFrame_h
#define STTagID3v2UserURLFrame_h

#import <Foundation/Foundation.h>
#include <SonatinaTag/STTagID3v2UserTextFrame.h>

@interface STTagID3v2UserURLFrame : STTagID3v2UserTextFrame {
    int dummy;
}

- (NSURL *)URL;
- (void)setURL:(NSURL *)url;

@end /* @interface STTagID3v2UserURLFrame */

#endif /* !STTagID3v2UserURLFrame_h */
