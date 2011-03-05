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

#include <ctype.h>

#include "STTagID3v1.h"
#include "NSStringExt.h"
#include "NSErrorExt.h"

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

/* Completely pointless initializer, just for the sake of completeness. */
- (id)init
{
    return (self = [super init]);
}

- (id)initFromFile:(NSString *)file
{
    FILE *fp;
    struct __ID3v1Tag tag;
    NSString *tmp;
    NSCharacterSet *set = [NSCharacterSet whitespaceCharacterSet];

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
    if(![_year isValidYear] && [_year UTF8String][0]) {
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

- (BOOL)writeToFile:(NSString *)file error:(NSError **)err
{
    FILE *fp;
    struct __ID3v1Tag tag = { 0 };
    BOOL rv = YES;

    /* Open up the file for reading and writing. */
    fp = fopen([file fileSystemRepresentation], "r+b");
    if(!fp) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_OpeningFile
                                     reason:@"Cannot open file"];
        }

        rv = NO;
        goto out;
    }

    /* Go to the position where the ID3v1 should be in the file. */
    if(fseek(fp, -128, SEEK_END)) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_IOError
                                     reason:@"I/O Error: Cannot seek"];
        }

        rv = NO;
        goto out_close;
    }

    /* Read in what should be the magic header, if its there */
    if(fread(tag.magic, 1, 3, fp) != 3) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_IOError
                                     reason:@"I/O Error: Cannot read"];
        }

        rv = NO;
        goto out_close;
    }

    if(tag.magic[0] != 'T' || tag.magic[1] != 'A' || tag.magic[2] != 'G') {
        /* No magic value, so we assume the tag isn't actually present. */
        if(fseek(fp, 0, SEEK_END)) {
            if(err) {
                *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                           code:STError_IOError
                                         reason:@"I/O Error: Cannot seek"];
            }

            rv = NO;
            goto out_close;
        }

        tag.magic[0] = 'T';
        tag.magic[1] = 'A';
        tag.magic[2] = 'G';
    }
    else {
        /* We have a tag, reposition to the end so we overwrite the old tag. */
        if(fseek(fp, -128, SEEK_END)) {
            if(err) {
                *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                           code:STError_IOError
                                         reason:@"I/O Error: Cannot seek"];
            }

            rv = NO;
            goto out_close;
        }
    }

    /* Fill in the tag. */
    if(![_title getCString:tag.title maxLength:30
                  encoding:NSISOLatin1StringEncoding]) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_CharConvert
                                     reason:@"Invalid character in title"];
        }

        rv = NO;
        goto out_close;
    }

    if(![_artist getCString:tag.artist maxLength:30
                   encoding:NSISOLatin1StringEncoding]) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_CharConvert
                                     reason:@"Invalid character in artist"];
        }

        rv = NO;
        goto out_close;
    }

    if(![_album getCString:tag.album maxLength:30
                  encoding:NSISOLatin1StringEncoding]) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_CharConvert
                                     reason:@"Invalid character in album"];
        }

        rv = NO;
        goto out_close;
    }

    if(![_comment getCString:tag.comment_field.comment maxLength:30
                    encoding:NSISOLatin1StringEncoding]) {
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_CharConvert
                                     reason:@"Invalid character in comment"];
        }

        rv = NO;
        goto out_close;
    }

    if(_year) {
        if(![_year isValidYear]) {
            if(err) {
                *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                           code:STError_CharConvert
                                         reason:@"Invalid character in year"];
            }

            rv = NO;
            goto out_close;
        }

        memcpy(tag.year, [_year UTF8String], 4);
    }
    
    tag.genre = _genre;

    /* Fill in the track if we have one */
    if(_track) {
        tag.comment_field.v1_1.zero = 0;
        tag.comment_field.v1_1.track = _track;
    }

    /* Write it out to the file... */
    if(fwrite(&tag, 1, 128, fp) != 128) {
        /* Uhh... *gulp* This is bad! */
        if(err) {
            *err = [NSError errorWithDomain:SonatinaTagErrorDomain
                                       code:STError_IOError
                                     reason:@"I/O Error: Cannot write"];
        }

        rv = NO;
        goto out_close;
    }

    /* If we got here, we have no errors, so note that if the user sent in a
       NSError pointer */
    if(err) {
        *err = nil;
    }

out_close:
    fclose(fp);
out:
    return rv;
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

- (NSData *)artwork
{
    return nil;
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

/* Methods for building up new ID3v1 tags. */
- (void)setTitle:(NSString *)title
{
    _title = [title retain];
}

- (void)setArtist:(NSString *)artist
{
    _artist = [artist retain];
}

- (void)setAlbum:(NSString *)album
{
    _album = [album retain];
}

- (void)setYear:(NSString *)year
{
    _year = [year retain];
}

- (void)setComment:(NSString *)comment
{
    _comment = [comment retain];
}

- (void)setGenre:(uint8_t)genre
{
    _genre = genre;
}

- (void)setTrackNumber:(int)trackNumber
{
    _track = (uint8_t)trackNumber;
}

@end /* @implementation STTagID3v1 */
