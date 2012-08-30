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

#ifndef SonatinaTag__cdefs_h
#define SonatinaTag__cdefs_h

#include <stddef.h>

#ifdef __cplusplus
    #define ST_BEGIN_DECLS      extern "C" {
    #define ST_END_DECLS        }
#else
    #define ST_BEGIN_DECLS
    #define ST_END_DECLS
#endif

/* Import/export for shared libraries */
#if defined _WIN32 || defined __CYGWIN__
    #define ST_IMPORT   __declspec(dllimport)
    #define ST_EXPORT   __declspec(dllexport)
    #define ST_LOCAL
#elif __GNUC__ >= 4
    #define ST_IMPORT   __attribute__((visibility("default")))
    #define ST_EXPORT   __attribute__((visibility("default")))
    #define ST_LOCAL    __attribute__((visibility("hidden")))
#else
    #warning SonatinaTag: No visibility macros available for your compiler!
    #define ST_IMPORT
    #define ST_EXPORT
    #define ST_LOCAL
#endif

#ifdef ST_SHARED_LIBRARY
    #ifdef ST_BUILD_SHARED
        #define ST_FUNC ST_EXPORT
    #else
        #define ST_FUNC ST_IMPORT
    #endif
#else
    #define ST_FUNC
#endif

#ifdef __APPLE__
#define ST_HAVE_COREFOUNDATION
#include <CoreFoundation/CoreFoundation.h>
#endif

#endif /* !SonatinaTag__cdefs_h */
