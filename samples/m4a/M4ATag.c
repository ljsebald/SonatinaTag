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
#include <SonatinaTag/Tags/M4A.h>

#ifndef __BLOCKS__
void tag_cb(const ST_Dict *d, void *data, const void *k, const void *v) {
    uint32_t *key = (uint32_t *)k;
    ST_M4A_Atom *atom = (ST_M4A_Atom *)v;
    char fourcc[8] = { *key >> 24, *key >> 16, *key >> 8, *key, 0 };
    size_t size = ST_M4A_Atom_length(atom);
    const uint8_t *data = ST_M4A_Atom_data(atom);
    char str[size + 1];

    memcpy(str, data, size);
    str[size] = 0;

    if(*key != ST_AtomLongName) {
        printf("Key = \"%s\" value = \"%s\"\n", fourcc, str);
    }
    else {
        printf("Key = \"%s\" long name = \"%s\" value=\"%s\"\n", fourcc,
               ST_M4A_Atom_longName(atom), str);
    }
}
#endif

int main(int argc, char *argv[]) {
    ST_M4A *tag;
    const ST_Dict *dict;

    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Grab the tag from the file */
    if(!(tag = ST_M4A_createFromFile(argv[1]))) {
        printf("Cannot read FLAC tag from file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Run through each comment, printing out the key-value pair */
    dict = ST_M4A_atomDictionary(tag);

#ifdef __BLOCKS__
    ST_Dict_foreach_b(dict, ^(const void *k, const void *v) {
        uint32_t *key = (uint32_t *)k;
        ST_M4A_Atom *atom = (ST_M4A_Atom *)v;
        char fourcc[8] = { *key >> 24, *key >> 16, *key >> 8, *key, 0 };
        size_t size = ST_M4A_Atom_length(atom);
        const uint8_t *data = ST_M4A_Atom_data(atom);
        char str[size + 1];

        memcpy(str, data, size);
        str[size] = 0;

        if(*key != ST_AtomLongName) {
            printf("Key = \"%s\" value = \"%s\"\n", fourcc, str);
        }
        else {
            printf("Key = \"%s\" long name = \"%s\" value=\"%s\"\n", fourcc,
                   ST_M4A_Atom_longName(atom), str);
        }
    });
#else
    ST_Dict_foreach(dict, NULL, tag_cb);
#endif

    /* Clean up */
    ST_M4A_free(tag);
    return 0;
}
