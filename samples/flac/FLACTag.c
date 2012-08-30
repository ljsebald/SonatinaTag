/*
    This file is a sample application of SonatinaTag.

    Copyright (C) 2011 Lawrence Sebald

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
*/

/*  This example program reads the tags from a FLAC file, and uses a block to
    print them out. If blocks are not supported by the compiler, it will do so
    using a callback instead.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SonatinaTag/Dictionary.h>
#include <SonatinaTag/Tags/FLAC.h>

#ifndef __BLOCKS__
void tag_cb(const ST_Dict *d, void *data, const void *k, const void *v) {
    ST_FLAC_vcomment *value = (ST_FLAC_vcomment *)v;
    size_t len = ST_FLAC_vcomment_length(value);
    uint8_t buf[len + 1];

    memcpy(buf, ST_FLAC_vcomment_data(value), len);
    buf[len] = 0;

    printf("Key = \"%s\" value = \"%s\"\n", (const char *)k, (const char *)buf);
}
#endif

int main(int argc, char *argv[]) {
    ST_FLAC *tag;
    const ST_Dict *dict;

    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Grab the tag from the file */
    if(!(tag = ST_FLAC_createFromFile(argv[1]))) {
        printf("Cannot read FLAC tag from file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Run through each comment, printing out the key-value pair */
    dict = ST_FLAC_commentDictionary(tag);

#ifdef __BLOCKS__
    ST_Dict_foreach_b(dict, ^(const void *k, const void *v) {
        ST_FLAC_vcomment *value = (ST_FLAC_vcomment *)v;
        size_t len = ST_FLAC_vcomment_length(value);
        uint8_t buf[len + 1];

        memcpy(buf, ST_FLAC_vcomment_data(value), len);
        buf[len] = 0;

        printf("Key = \"%s\" value = \"%s\"\n", (const char *)k,
               (const char *)buf);
    });
#else
    ST_Dict_foreach(dict, NULL, tag_cb);
#endif

    /* Clean up */
    ST_FLAC_free(tag);
    return 0;
}
