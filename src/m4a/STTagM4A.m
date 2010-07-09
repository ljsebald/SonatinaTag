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

#include <stdio.h>

#include "STTagM4A.h"
#include "NSStringExt.h"

@interface STTagM4A (Internal)

- (id)parseFile:(FILE *)fp;
- (BOOL)findAtom:(uint32_t)atom inFile:(FILE *)fp containerSize:(uint64_t)cz
            size:(uint64_t *)patomsz;

@end /* @interface STTagM4A (Internal) */

@implementation STTagM4A

- (id)initFromFile:(NSString *)filename
{
    FILE *fp;

    if((self = [super init]) == nil) {
        return nil;
    }

    /* Open up the file for reading */
    fp = fopen([filename fileSystemRepresentation], "rb");
    if(!fp) {
        [self release];
        return nil;
    }

    return [self parseFile:fp];
}

- (void)dealloc
{
    [_atoms release];
    [super dealloc];
}

- (NSString *)title
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomTitle]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (NSString *)artist
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomArtist]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (NSString *)album
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomAlbum]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (NSString *)year
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomYear]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (NSString *)comment
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomComment]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (NSData *)artwork
{
    return [self atomForKey:AtomCoverArt];
}

- (NSString *)genre
{
    return [[[NSString alloc] initWithData:[self atomForKey:AtomGenre]
                                  encoding:NSUTF8StringEncoding] autorelease];
}

- (int)trackNumber
{
    NSData *data = [self atomForKey:AtomTrackNumber];
    uint8_t *bytes;

    if(data == nil) {
        return 1;
    }

    bytes = (uint8_t *)[data bytes];
    return (int)bytes[3];
}

- (int)discNumber
{
    NSData *data = [self atomForKey:AtomTrackNumber];
    uint8_t *bytes;

    if(data == nil) {
        return 1;
    }

    bytes = (uint8_t *)[data bytes];
    return (int)bytes[3];
}

- (NSData *)atomForKey:(uint32_t)fourcc
{
    return [_atoms objectForKey:[NSString stringWith4CC:fourcc]];
}

@end /* @implementation STTagM4A */

@implementation STTagM4A (Internal)

- (id)parseFile:(FILE *)fp
{
    uint8_t buf[16];
    uint32_t fourcc;
    uint64_t atomsz, atomread, atomsz2;
    uint64_t moovsz, udtasz, metasz, ilstsz;
    long pos;
    uint8_t *tmp;
    NSData *atomdata;

    /* Read in the first 8 bytes of the file, and make sure it is as we would
       expect it to be */
    if(fread(buf, 1, 8, fp) != 8) {
        goto out_close;
    }

    atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];

    if(fourcc != 'ftyp') {
        goto out_close;
    }

    /* Make sure we have at least one type of information in here */
    if(atomsz < 12) {
        goto out_close;
    }

    /* Read in the type of data contained within */
    if(fread(buf, 1, 4, fp) != 4) {
        goto out_close;
    }

    if(buf[0] != 'M' || buf[1] != '4' || buf[2] != 'A' || buf[3] != ' ') {
        fourcc = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        NSLog(@"Cannot handle %@", [NSString stringWith4CC:fourcc]);
        goto out_close;
    }

    /* We don't care about the rest of the ftyp */
    fseek(fp, atomsz, SEEK_SET);

    /* Next, find the "moov" atom, since its the toplevel container for what the
       tags are in */
    if(![self findAtom:'moov' inFile:fp containerSize:0xFFFFFFFF
                  size:&moovsz]) {
        NSLog(@"Couldn't find moov atom");
        goto out_close;
    }

    /* Now, we need the "udta" atom */
    if(![self findAtom:'udta' inFile:fp containerSize:moovsz size:&udtasz]) {
        NSLog(@"Couldn't find udta atom");
        goto out_close;
    }

    /* Next up is the "meta" atom */
    if(![self findAtom:'meta' inFile:fp containerSize:udtasz size:&metasz]) {
        NSLog(@"Couldn't find meta atom");
        goto out_close;
    }

    /* The meta atom has an extra 4 bytes of version info in the header... */
    fseek(fp, 4, SEEK_CUR);

    /* Finally, the "ilst" atom */
    if(![self findAtom:'ilst' inFile:fp containerSize:metasz size:&ilstsz]) {
        NSLog(@"Couldn't find ilst atom");
        goto out_close;
    }

    _atoms = [[NSMutableDictionary alloc] initWithCapacity:0];
    if(_atoms == nil) {
        goto out_close;
    }

    /* Read in the entire ilst atom */
    while(ilstsz > 8) {
        pos = ftell(fp);

        /* Read in the first 8 bytes of the next atom */
        if(fread(buf, 1, 8, fp) != 8) {
            goto out_close;
        }

        atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
        atomread = 8;

        /* Figure out the real size of the atom */
        if(atomsz == 1) {
            if(fread(buf, 1, 8, fp) != 8) {
                goto out_close;
            }

            atomsz = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
                ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
                ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |
                ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
            atomread += 8;
        }
        else if(atomsz == 0) {
            atomsz = ilstsz;
        }

        /* Ignore the '----' type tags for now, until I come up with a good way
           to deal with them... Also, ignore 'free' tags, as they're just plain
           blank space */
        if(fourcc == '----' || fourcc == 'free') {
            goto doneAtom;
        }

        /* Now that we have that, fetch the data */
        if(![self findAtom:'data' inFile:fp containerSize:atomsz - atomread
                      size:&atomsz2]) {
            goto doneAtom;
        }

        /* Skip the first 8 bytes of any 'data' atom */
        fseek(fp, 8, SEEK_CUR);
        atomsz2 -= 16;

        if(atomsz2 & 0xFFFFFFFF00000000ULL) {
            atomsz2 -= 8;
        }

        /* Save the data in our dictionary */
        tmp = (uint8_t *)malloc(atomsz2);
        if(!tmp) {
            goto doneAtom;
        }

        if(fread(tmp, 1, atomsz2, fp) != atomsz2) {
            goto out_close;
        }

        atomdata = [[NSData alloc] initWithBytesNoCopy:tmp length:atomsz2];
        if(atomdata == nil) {
            goto doneAtom;
        }

        [_atoms setObject:atomdata forKey:[NSString stringWith4CC:fourcc]];
        [atomdata release];

doneAtom:
        fseek(fp, (long)(pos + atomsz), SEEK_SET);
        ilstsz -= atomsz;
    }

    fclose(fp);
    return self;
    
out_close:
    fclose(fp);

out_err:
    [self release];
    return nil;
}

- (BOOL)findAtom:(uint32_t)atom inFile:(FILE *)fp containerSize:(uint64_t)cz
            size:(uint64_t *)patomsz
{
    uint32_t fourcc;
    uint32_t atomsz;
    uint64_t ratomsz;
    uint8_t buf[8];

    while(cz) {
        /* Grab the header of the atom */
        if(fread(buf, 1, 8, fp) != 8) {
            return NO;
        }

        atomsz = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        fourcc = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];

        /* atomsz = 1 -- use 64-bit length */
        if(atomsz == 1) {
            if(fread(buf, 1, 8, fp) != 8) {
                return NO;
            }

            ratomsz = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
                ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
                ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |
                ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
        }
        else {
            ratomsz = atomsz;
        }

        if(fourcc != atom) {
            /* Check the size in the atomsz value */
            if(atomsz == 1) {                
                fseek(fp, (long)ratomsz - 16, SEEK_CUR);
                cz -= ratomsz;
            }
            else if(atomsz == 0) {
                /* atomsz = 0 -- goes to end of file, so we're not going to find
                   what we're looking for */
                return NO;
            }
            else {
                fseek(fp, (long)atomsz - 8, SEEK_CUR);
                cz -= atomsz;
            }
        }
        else {
            *patomsz = ratomsz;
            return YES;
        }
    }

    return NO;
}

@end /* @implementation STTagM4A (Internal) */
