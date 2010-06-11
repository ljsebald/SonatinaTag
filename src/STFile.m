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

- (int)trackNumber;
- (int)discNumber;

@end /* @interface STTagDummy */

@implementation STFile

- (id)initWithFile:(NSString *)filename
{
    STTagID3v1 *id3v1;
    STTagID3v2 *id3v2;
    STTagM4A *m4a;
    STTagDummy *dummy;

    if((self = [super init]) != nil) {
        _filename = [filename retain];

        /* Make our storage for tags */
        _tags = [[NSMutableDictionary alloc] initWithCapacity:0];
        if(_tags == nil) {
            [self release];
        }

        id3v1 = [[STTagID3v1 alloc] initFromFile:filename];
        if(id3v1) {
            [_tags setObject:id3v1 forKey:STFileID3v1Type];
        }

        id3v2 = [[STTagID3v2 alloc] initFromFile:filename];
        if(id3v2) {
            [_tags setObject:id3v2 forKey:STFileID3v2Type];
        }

        dummy = [[STTagDummy alloc] initFromFile:filename];
        if(dummy) {
            [_tags setObject:dummy forKey:@"__dummy__"];
        }

        m4a = [[STTagM4A alloc] initFromFile:filename];
        if(m4a) {
            [_tags setObject:m4a forKey:STFileM4AType];
        }

        [id3v1 release];
        [id3v2 release];
        [dummy release];
        [m4a release];
    }

    return self;

out_err:
    [self release];
    return nil;
}

- (void)dealloc
{
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
    /* Prefer ID3v2 here over ID3v1 */
    id<STTag> rv;

    rv = [_tags objectForKey:STFileID3v2Type];
    if(rv == nil) {
        rv = [_tags objectForKey:STFileM4AType];
        if(rv == nil) {
            rv = [_tags objectForKey:STFileID3v1Type];
            if(rv == nil) {
                rv = [_tags objectForKey:@"__dummy__"];
            }
        }
    }

    return rv;
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

- (int)trackNumber
{
    return 1;
}

- (int)discNumber
{
    return 1;
}

@end /* @implementation STTagDummy */
