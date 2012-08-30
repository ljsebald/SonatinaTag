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

#ifndef SonatinaTag__Error_h
#define SonatinaTag__Error_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

/* Error codes that are returned by various SonatinaTag functions. */
typedef enum ST_Error_e {
    /* No error */
    ST_Error_None = 0,

    /* This one is special... This basically means that you should consult
       errno for what the actual error was. Essentially, this means that
       something in libc returned an error. */
    ST_Error_errno = -1,

    /* The rest should be self-explanatory... */
    ST_Error_InvalidArgument = -2,
    ST_Error_Unknown = -3,
    ST_Error_NotFound = -4,
    ST_Error_InvalidEncoding = -5
} ST_Error;

ST_END_DECLS

#endif /* !SonatinaTag__Error_h */
