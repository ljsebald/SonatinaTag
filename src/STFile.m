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

#include "STFile.h"
#include "STTagID3v1.h"
#include "STTagID3v2.h"
#include "STTagM4A.h"
#include "STTagFLAC.h"

/* A dummy tag to reply with something at least somewhat valid when no other
   tags are actually available */
@interface STTagDummy : NSObject<STTag> {
@private
    NSString *_filename;
}

- (id)initFromFile:(NSString *)filename;
- (void)dealloc;

- (NSString *)title;
- (NSString *)artist;
- (NSString *)album;
- (NSString *)year;
- (NSString *)comment;

- (NSData *)artwork;

- (int)trackNumber;
- (int)discNumber;

@end /* @interface STTagDummy */

@implementation STFile

- (id)initWithFile:(NSString *)filename
{
    STTagID3v1 *id3v1 = nil;
    STTagID3v2 *id3v2 = nil;
    STTagM4A *m4a = nil;
    STTagDummy *dummy = nil;
    STTagFLAC *flac = nil;
    NSString *ext;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    if((self = [super init])) {
        _filename = [filename retain];
        ext = [filename pathExtension];

        /* Make our storage for tags */
        _tags = [[NSMutableDictionary alloc] initWithCapacity:0];
        if(_tags == nil) {
            goto out_err;
        }

        /* Allocate a dummy tag, in case all else fails... */
        dummy = [[[STTagDummy alloc] initFromFile:filename] autorelease];
        if(dummy) {
            [_tags setObject:dummy forKey:@"__dummy__"];
        }

        _defaultTag = [@"__dummy__" retain];

        /* Assume for now that MP3 files are all that contain ID3 tags... */
        if([ext caseInsensitiveCompare:@"mp3"] == 0) {
            id3v1 = [[[STTagID3v1 alloc] initFromFile:filename] autorelease];
            if(id3v1) {
                [_tags setObject:id3v1 forKey:STFileID3v1Type];
                [_defaultTag release];
                _defaultTag = [STFileID3v1Type retain];
            }

            id3v2 = [[[STTagID3v2 alloc] initFromFile:filename] autorelease];
            if(id3v2) {
                [_tags setObject:id3v2 forKey:STFileID3v2Type];
                [_defaultTag release];
                _defaultTag = [STFileID3v2Type retain];
            }
        }

        /* M4A/MP4/M4P files are all that should contain these... */
        if([ext caseInsensitiveCompare:@"m4a"] == 0 ||
           [ext caseInsensitiveCompare:@"mp4"] == 0 ||
           [ext caseInsensitiveCompare:@"m4p"] == 0) {
            m4a = [[[STTagM4A alloc] initFromFile:filename] autorelease];
            if(m4a) {
                [_tags setObject:m4a forKey:STFileM4AType];
                [_defaultTag release];
                _defaultTag = [STFileM4AType retain];
            }
        }

        /* FLAC files are going to be all that will work here... */
        if([ext caseInsensitiveCompare:@"flac"] == 0 ||
           [ext caseInsensitiveCompare:@"fla"] == 0) {
            flac = [[[STTagFLAC alloc] initFromFile:filename] autorelease];
            if(flac) {
                [_tags setObject:flac forKey:STFileFLACType];
                [_defaultTag release];
                _defaultTag = [STFileFLACType retain];
            }
        }
    }

    [pool drain];
    return self;

out_err:
    [pool drain];
    [self release];
    return nil;
}

- (void)dealloc
{
    [_defaultTag release];
    [_filename release];
    [_tags release];
    [super dealloc];
}

- (id<STTag>)getTagForType:(NSString *)type
{
    return [_tags objectForKey:type];
}

- (id<STTag>)tag
{
    return [_tags objectForKey:_defaultTag];
}

@end /* @implementation STFile */

@implementation STTagDummy

- (id)initFromFile:(NSString *)filename
{
    if((self = [super init])) {
        _filename =
            [[NSString alloc] initWithString:[filename lastPathComponent]];
    }

   return self;
}

- (void)dealloc
{
    [_filename release];
    [super dealloc];
}

- (NSString *)title
{
    return _filename;
}

- (NSString *)artist
{
    return @"";
}

- (NSString *)album
{
    return @"";
}

- (NSString *)year
{
    return @"";
}

- (NSString *)comment
{
    return @"";
}

- (NSData *)artwork
{
    return nil;
}

- (int)trackNumber
{
    return 1;
}

- (int)discNumber
{
    return 1;
}

@end /* @implementation STTagDummy */
