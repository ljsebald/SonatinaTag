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

#ifndef SonatinaTag__Dictionary_h
#define SonatinaTag__Dictionary_h

#include <SonatinaTag/cdefs.h>

ST_BEGIN_DECLS

#include <SonatinaTag/Error.h>

/* Opaque dictionary type */
struct ST_Dict_struct;
typedef struct ST_Dict_struct ST_Dict;

/* Create a general dictionary. */
ST_FUNC ST_Dict *ST_Dict_create(int nb, unsigned long (*hash)(const void *),
                                int (*compare)(const void *, const void *),
                                void *(*copy_key)(const void *),
                                void (*dtor_key)(void *),
                                void (*dtor_val)(void *));

/* Create a dictionary with string (const char *) keys. */
ST_FUNC ST_Dict *ST_Dict_createString(int nb, void (*dtor_val)(void *));

/* Create a dictionary with integral (unsigned long) keys. You should always
   pass a pointer to an unsigned long as your key! */
ST_FUNC ST_Dict *ST_Dict_createUint32(int nb, void (*dtor_val)(void *));

/* Free a dictionary, calling the destructors on all of the keys and values. */
ST_FUNC void ST_Dict_free(ST_Dict *d);

/* Foreach loop */
ST_FUNC void ST_Dict_foreach(const ST_Dict *d, void *data,
                             void (*f)(const ST_Dict *d, void *data,
                                       const void *k, const void *v));

#ifdef __BLOCKS__
/* Foreach loop, but using a block, if the compiler supports it */
ST_FUNC void ST_Dict_foreach_b(const ST_Dict *d,
                               void (^f)(const void *k, const void *v));
#endif

/* Find a key, return its value(s). */
ST_FUNC const void **ST_Dict_find(const ST_Dict *d, const void *key,
                                  int *value_count);

/* Add a <key, value> pair to the dictionary. Duplicate keys are supported, and
   will all be stored, in the order they are given. */
ST_FUNC ST_Error ST_Dict_add(ST_Dict *d, const void *key, void *data);

/* Remove a value from the dictionary. */
ST_FUNC ST_Error ST_Dict_remove(ST_Dict *d, const void *key, int index);

/* Replace a value in the dictionary. */
ST_FUNC ST_Error ST_Dict_replace(ST_Dict *d, const void *key, int index,
                                 void *value);

ST_END_DECLS

#endif /* !SonatinaTag__Dictionary_h */
