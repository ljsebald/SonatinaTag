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

#include <ctype.h>

#include "STTagID3v1.h"

struct __ID3v1Tag {
    char magic[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    union {
        char comment[30];
        struct {
            char comment[28];
            uint8_t zero;
            uint8_t track;
        } v1_1;
    } comment_field;
    uint8_t genre;
} __attribute__((packed));

@implementation STTagID3v1

- (id)initFromFile:(NSString *)file
{
    FILE *fp;
    struct __ID3v1Tag tag;
    NSString *tmp;
    NSCharacterSet *set = [NSCharacterSet whitespaceCharacterSet];
    const char *yr;

    if((self = [super init]) == nil) {
        return nil;
    }

    /* Open up the file for reading. */
    fp = fopen([file fileSystemRepresentation], "rb");
    if(!fp) {
        goto out_rel;
    }

    /* Go to the position where the ID3v1 should be in the file. */
    if(fseek(fp, -128, SEEK_END)) {
        goto out_close;
    }

    /* Read in the whole tag area for checking */
    if(fread(&tag, 1, 128, fp) != 128) {
        goto out_close;
    }

    fclose(fp);

    /* Look for the magic value */
    if(tag.magic[0] != 'T' || tag.magic[1] != 'A' || tag.magic[2] != 'G') {
        goto out_rel;
    }

    /* Parse out the strings */
    tmp = [[NSString alloc] initWithBytes:tag.title
                                   length:30
                                 encoding:NSISOLatin1StringEncoding];
    _title = [[tmp stringByTrimmingCharactersInSet:set] retain];
    [tmp release];

    tmp = [[NSString alloc] initWithBytes:tag.artist
                                   length:30
                                 encoding:NSISOLatin1StringEncoding];
    _artist = [[tmp stringByTrimmingCharactersInSet:set] retain];
    [tmp release];

    tmp = [[NSString alloc] initWithBytes:tag.album
                                   length:30
                                 encoding:NSISOLatin1StringEncoding];
    _album = [[tmp stringByTrimmingCharactersInSet:set] retain];
    [tmp release];

    _year = [[NSString alloc] initWithBytes:tag.year
                                     length:4
                                   encoding:NSISOLatin1StringEncoding];

    tmp = [[NSString alloc] initWithBytes:tag.comment_field.comment
                                   length:30
                                 encoding:NSISOLatin1StringEncoding];
    _comment = [[tmp stringByTrimmingCharactersInSet:set] retain];
    [tmp release];

    _genre = tag.genre;

    /* Check for the track number */
    if(tag.comment_field.v1_1.zero == 0) {
        _track = tag.comment_field.v1_1.track;
    }
    else {
        _track = 0;
    }

    /* Check the year for validity */
    yr = [_year cStringUsingEncoding:NSISOLatin1StringEncoding];
    
    if(strlen(yr) != 4 || !isnumber(yr[0]) || !isnumber(yr[1]) ||
       !isnumber(yr[2]) || !isnumber(yr[3])) {
        goto out_rel;
    }

    /* Check the genre for validity */
    if(_genre > ID3v1GenreMax) {
        goto out_rel;
    }

    return self;

out_close:
    fclose(fp);
out_rel:
    [self release];
    return nil;
}

- (void)dealloc
{
    [_title release];
    [_artist release];
    [_album release];
    [_comment release];
    [_year release];
    [super dealloc];
}

- (NSString *)title
{
    return _title;
}

- (NSString *)artist
{
    return _artist;
}

- (NSString *)album
{
    return _album;
}

- (NSString *)year
{
    return _year;
}

- (NSString *)comment
{
    return _comment;
}

- (uint8_t)genre
{
    return _genre;
}

- (int)trackNumber
{
    return (int)_track;
}

- (int)discNumber
{
    return 1;
}

@end /* @implementation STTagID3v1 */
