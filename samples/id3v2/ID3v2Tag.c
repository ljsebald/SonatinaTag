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
#include <SonatinaTag/Tags/ID3v2.h>

#ifndef __BLOCKS__
void tag_cb(const ST_Dict *d, void *data, const void *k, const void *v) {
    uint32_t *key = (uint32_t *)k;
    char fourcc[8] = { *key >> 24, *key >> 16, *key >> 8, *key, 0 };
    ST_Frame *f = (ST_Frame *)v;

    printf("Key = \"%s\"", fourcc);

    if(ST_Frame_type(f) == ST_FrameType_Text) {
        ST_TextFrame *tf = (ST_TextFrame *)v;

        if(ST_ID3v2_TextFrame_encoding(tf) == ST_TextEncoding_ISO8859_1 ||
           ST_ID3v2_TextFrame_encoding(tf) == ST_TextEncoding_UTF8) {
            size_t len = ST_ID3v2_TextFrame_length(tf);
            uint8_t buf[len + 1];

            memcpy(buf, ST_ID3v2_TextFrame_text(tf), len);
            printf("Value = \"%s\"", (char *)buf);
        }
    }
    else if(ST_Frame_type(f) == ST_FrameType_UserText) {
        ST_UserTextFrame *utf = (ST_UserTextFrame *)v;

        if(ST_ID3v2_UserTextFrame_encoding(utf) == ST_TextEncoding_ISO8859_1 ||
           ST_ID3v2_UserTextFrame_encoding(utf) == ST_TextEncoding_UTF8) {
            size_t len = ST_ID3v2_UserTextFrame_textlength(utf);
            size_t len2 = ST_ID3v2_UserTextFrame_descLength(utf);
            uint8_t buf[len + 1];
            uint8_t buf2[len2 + 1];

            memcpy(buf, ST_ID3v2_UserTextFrame_text(utf), len);
            memcpy(buf2, ST_ID3v2_UserTextFrame_desc(utf), len2);
            printf("Description = \"%s\" Value = \"%s\"", (char *)buf2,
                   (char *)buf);
        }
    }

    printf("\n");
}
#endif

int main(int argc, char *argv[]) {
    ST_ID3v2 *tag;
    const ST_Dict *dict;

    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Grab the tag from the file */
    if(!(tag = ST_ID3v2_createFromFile(argv[1]))) {
        printf("Cannot read ID3v2 tag from file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Run through each frame */
    dict = ST_ID3v2_frameDictionary(tag);

#ifdef __BLOCKS__
    ST_Dict_foreach_b(dict, ^(const void *k, const void *v) {
        uint32_t *key = (uint32_t *)k;
        char fourcc[8] = { *key >> 24, *key >> 16, *key >> 8, *key, 0 };
        ST_Frame *f = (ST_Frame *)v;

        printf("Key = \"%s\" ", fourcc);

        if(ST_ID3v2_Frame_type(f) == ST_FrameType_Text) {
            ST_TextFrame *tf = (ST_TextFrame *)v;

            if(ST_ID3v2_TextFrame_encoding(tf) == ST_TextEncoding_ISO8859_1 ||
               ST_ID3v2_TextFrame_encoding(tf) == ST_TextEncoding_UTF8) {
                size_t len = ST_ID3v2_TextFrame_length(tf);
                uint8_t buf[len + 1];

                memcpy(buf, ST_ID3v2_TextFrame_text(tf), len);
                buf[len] = 0;

                printf("Value = \"%s\"", (char *)buf);
            }
        }
        else if(ST_ID3v2_Frame_type(f) == ST_FrameType_UserText) {
            ST_UserTextFrame *utf = (ST_UserTextFrame *)v;
            ST_TextEncoding e = ST_ID3v2_UserTextFrame_encoding(utf);

            if(e == ST_TextEncoding_ISO8859_1 || e == ST_TextEncoding_UTF8) {
                size_t len = ST_ID3v2_UserTextFrame_textLength(utf);
                size_t len2 = ST_ID3v2_UserTextFrame_descLength(utf);
                uint8_t buf[len + 1];
                uint8_t buf2[len2 + 1];

                memcpy(buf, ST_ID3v2_UserTextFrame_text(utf), len);
                memcpy(buf2, ST_ID3v2_UserTextFrame_desc(utf), len2);
                buf[len] = 0;
                buf2[len2] = 0;
                printf("Description = \"%s\" Value = \"%s\"", (char *)buf2,
                       (char *)buf);
            }
        }
        else if(ST_ID3v2_Frame_type(f) == ST_FrameType_URL) {
            ST_URLFrame *uf = (ST_URLFrame *)v;
            size_t len = ST_ID3v2_URLFrame_length(uf);
            uint8_t buf[len + 1];

            memcpy(buf, ST_ID3v2_URLFrame_URL(uf), len);
            buf[len] = 0;

            printf("Value = \"%s\"", (char *)buf);
        }
        else if(ST_ID3v2_Frame_type(f) == ST_FrameType_UserURL) {
            ST_UserURLFrame *uuf = (ST_UserURLFrame *)v;
            ST_TextEncoding e = ST_ID3v2_UserURLFrame_descEncoding(uuf);
            size_t len = ST_ID3v2_UserURLFrame_URLLength(uuf);
            uint8_t buf[len + 1];

            memcpy(buf, ST_ID3v2_UserURLFrame_URL(uuf), len);
            buf[len] = 0;

            if(e == ST_TextEncoding_ISO8859_1 || e == ST_TextEncoding_UTF8) {
                size_t len2 = ST_ID3v2_UserURLFrame_descLength(uuf);
                uint8_t buf2[len2 + 1];

                memcpy(buf2, ST_ID3v2_UserURLFrame_desc(uuf), len2);
                buf2[len2] = 0;

                printf("Description = \"%s\" ", (char *)buf2);
            }

            printf("Value = \"%s\"", (char *)buf);
        }

        printf("\n");
    });
#else
    ST_Dict_foreach(dict, NULL, tag_cb);
#endif

    /* Clean up */
    ST_ID3v2_free(tag);
    return 0;
}
