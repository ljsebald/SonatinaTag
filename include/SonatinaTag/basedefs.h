/*
    SonatinaTag
    Copyright (C) 2011, 2012 Lawrence Sebald

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

#ifndef SonatinaTag__basedefs_h
#define SonatinaTag__basedefs_h

/* Tag types supported by the library */
typedef enum ST_TagType_e {
    ST_TagType_Invalid          = -1,
    ST_TagType_ID3v1            = 1,
    ST_TagType_ID3v2            = 2,
    ST_TagType_FLAC             = 3,
    ST_TagType_M4A              = 4
} ST_TagType;

/* Valid Encoding types */
typedef enum ST_TextEncoding_e {
    ST_TextEncoding_Invalid     = -1,
    ST_TextEncoding_ISO8859_1   = 0,
    ST_TextEncoding_UTF16       = 1,
    ST_TextEncoding_UTF16BE     = 2,
    ST_TextEncoding_UTF8        = 3
} ST_TextEncoding;

#define ST_4CC(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

#endif /* !SonatinaTag__basedefs_h */
