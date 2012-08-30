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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "SonatinaTag/Dictionary.h"
#include "SonatinaTag/queue.h"

typedef struct dict_kv_s {
    TAILQ_ENTRY(dict_kv_s) qentry;
    void *key;
    int num_values;
    void **values;
} dict_kv_t;

TAILQ_HEAD(dict_bucket, dict_kv_s);
    
struct ST_Dict_struct {
    unsigned long (*hash)(const void *);
    int (*compare)(const void *, const void *);
    void *(*copy_key)(const void *);
    void (*dtor_key)(void *);
    void (*dtor_val)(void *);
    int num_buckets;
    struct dict_bucket *buckets;
};

ST_FUNC ST_Dict *ST_Dict_create(int nb, unsigned long (*hash)(const void *),
                                int (*compare)(const void *, const void *),
                                void *(*copy_key)(const void *),
                                void (*dtor_key)(void *),
                                void (*dtor_val)(void *)) {
    ST_Dict *rv;
    int i;

    /* Make sure they didn't do something stupid... */
    if(!nb || !hash || !compare || !copy_key || !dtor_key) {
        return NULL;
    }

    rv = (ST_Dict *)malloc(sizeof(ST_Dict));
    if(!rv) {
        return NULL;
    }

    /* Allocate space for the buckets */
    rv->buckets = (struct dict_bucket *)malloc(sizeof(struct dict_bucket) * nb);
    if(!rv->buckets) {
        free(rv);
        return NULL;
    }

    /* Fill in the the structure */
    rv->hash = hash;
    rv->compare = compare;
    rv->copy_key = copy_key;
    rv->dtor_key = dtor_key;
    rv->dtor_val = dtor_val;
    rv->num_buckets = nb;

    for(i = 0; i < nb; ++i) {
        TAILQ_INIT(&rv->buckets[i]);
    }

    return rv;
}

/* Simple DJB hash for strings... */
static unsigned long sh(const void *s) {
    char *str = (char *)s;
    unsigned long hash = 5381;
    char c;

    while((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/* Hash function for integers, borrowed from:
   http://burtleburtle.net/bob/hash/integer.html
 */
static unsigned long ih(const void *k) {
    unsigned long a = (unsigned long)(*((uint32_t *)k));
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

static int ic(const void *k1, const void *k2) {
    uint32_t a = *((uint32_t *)k1);
    uint32_t b = *((uint32_t *)k2);

    return a - b;
}

static void *id(const void *k) {
    uint32_t *a = (uint32_t *)k;
    uint32_t *tmp = (uint32_t *)malloc(sizeof(uint32_t));

    if(tmp) {
        *tmp = *a;
    }

    return tmp;
}

ST_FUNC ST_Dict *ST_Dict_createString(int nb, void (*dtor_val)(void *)) {
    return ST_Dict_create(nb, sh, (int (*)(const void *, const void *))strcmp,
                          (void *(*)(const void *))strdup, free, dtor_val);
}

ST_FUNC ST_Dict *ST_Dict_createUint32(int nb, void (*dtor_val)(void *)) {
    return ST_Dict_create(nb, ih, ic, id, free, dtor_val);
}

ST_FUNC void ST_Dict_free(ST_Dict *d) {
    void (*kd)(void *) = d->dtor_key;
    void (*vd)(void *) = d->dtor_val;
    int i, k, nb = d->num_buckets;
    dict_kv_t *j, *tmp;

    for(i = 0; i < nb; ++i) {
        j = TAILQ_FIRST(&d->buckets[i]);
        while(j) {
            tmp = TAILQ_NEXT(j, qentry);

            if(vd) {
                for(k = 0; k < j->num_values; ++k) {
                    vd(j->values[k]);
                }
            }

            kd(j->key);
            free(j->values);
            free(j);
            j = tmp;
        }
    }

    free(d->buckets);
    free(d);
}

ST_FUNC void ST_Dict_foreach(const ST_Dict *d, void *data,
                             void (*f)(const ST_Dict *d, void *data,
                                       const void *k, const void *v)) {
    int i, k, nb = d->num_buckets;
    dict_kv_t *j;
    
    for(i = 0; i < nb; ++i) {
        TAILQ_FOREACH(j, &d->buckets[i], qentry) {
            for(k = 0; k < j->num_values; ++k) {
                f(d, data, j->key, j->values[k]);
            }
        }
    }
}

#ifdef __BLOCKS__
ST_FUNC void ST_Dict_foreach_b(const ST_Dict *d,
                               void (^f)(const void *k, const void *v)) {
    int i, k, nb = d->num_buckets;
    dict_kv_t *j;
    
    for(i = 0; i < nb; ++i) {
        TAILQ_FOREACH(j, &d->buckets[i], qentry) {
            for(k = 0; k < j->num_values; ++k) {
                f(j->key, j->values[k]);
            }
        }
    }
}
#endif

/* Return the <key, value> pair structure for the given key, if it exists.
   Otherwise, put the bucket number in rb. */
static dict_kv_t *find_kv(const ST_Dict *d, const void *key, int *rb) {
    unsigned long hash = d->hash(key);
    int bucket = hash % d->num_buckets;
    dict_kv_t *i;

    TAILQ_FOREACH(i, &d->buckets[bucket], qentry) {
        if(!d->compare(key, i->key)) {
            return i;
        }
    }

    *rb = bucket;
    return NULL;
}

ST_FUNC const void **ST_Dict_find(const ST_Dict *d, const void *key,
                                  int *value_count) {
    int rb;
    dict_kv_t *kv;

    /* Make sure they aren't doing anything nutty... */
    if(!d || !key) {
        return NULL;
    }

    /* Find the <key, value> pair */
    if((kv = find_kv(d, key, &rb)) && value_count) {
        *value_count = kv->num_values;
        return (const void **)kv->values;
    }

    return NULL;
}

ST_FUNC ST_Error ST_Dict_add(ST_Dict *d, const void *key, void *value) {
    int rb;
    dict_kv_t *kv;
    void *t;

    /* Make sure they aren't doing anything nutty... */
    if(!d || !key || !value) {
        return ST_Error_InvalidArgument;
    }

    /* Find the <key, value> pair, if there is one already. */
    if((kv = find_kv(d, key, &rb))) {
        /* Make space for the new value */
        if(!(t = realloc(kv->values, (kv->num_values + 1) * sizeof(void *)))) {
            return ST_Error_errno;
        }

        kv->values = (void **)t;
        kv->values[kv->num_values++] = value;
        return ST_Error_None;
    }

    /* We don't have an old entry, we have to add a new one */
    if(!(kv = (dict_kv_t *)malloc(sizeof(dict_kv_t)))) {
        return ST_Error_errno;
    }

    memset(kv, 0, sizeof(dict_kv_t));

    if(!(kv->values = (void **)malloc(sizeof(void *)))) {
        free(kv);
        return ST_Error_errno;
    }

    if(!(kv->key = d->copy_key(key))) {
        free(kv->values);
        free(kv);
        return ST_Error_Unknown;
    }

    kv->values[0] = value;
    kv->num_values = 1;
    TAILQ_INSERT_TAIL(&d->buckets[rb], kv, qentry);
    
    return ST_Error_None;
}

ST_FUNC ST_Error ST_Dict_remove(ST_Dict *d, const void *key, int index) {
    dict_kv_t *kv;
    int rb, i, free_key = 0;
    void (*dv)(void *);
    void (*dk)(void *);
    void *tmp;

    /* Make sure they aren't doing anything stupid... */
    if(!d || !key || index < -1) {
        return ST_Error_InvalidArgument;
    }

    /* Find the key */
    if(!(kv = find_kv(d, key, &rb))) {
        return ST_Error_NotFound;
    }

    /* Make sure its in range */
    if(index >= kv->num_values) {
        return ST_Error_NotFound;
    }

    dv = d->dtor_val;
    dk = d->dtor_key;
    
    /* Are we freeing all of the items? */
    if(index == -1) {
        if(dv) {
            for(i = 0; i < kv->num_values; ++i) {
                dv(kv->values[i]);
            }
        }

        free_key = 1;
    }
    /* Or, are we freeing the only item? */
    else if(index == 0 && kv->num_values == 1) {
        if(dv) {
            dv(kv->values[0]);
        }

        free_key = 1;
    }
    /* Or, are we just freeing something in the list */
    else {
        if(dv) {
            dv(kv->values[index]);
        }

        if(index < kv->num_values - 1) {
            memmove(kv->values + index, kv->values + index + 1,
                    (kv->num_values - index - 1) * sizeof(void *));
        }

        /* Resize the values array */
        if((tmp = realloc(kv->values, (kv->num_values - 1) * sizeof(void *)))) {
            kv->values = (void **)tmp;
        }

        --kv->num_values;
    }

    /* Free the key, if we're supposed to */
    if(free_key) {
        free(kv->values);
        dk(kv->key);
        TAILQ_REMOVE(&d->buckets[rb], kv, qentry);
        free(kv);
    }

    return ST_Error_None;
}

ST_FUNC ST_Error ST_Dict_replace(ST_Dict *d, const void *key, int index,
                                 void *value) {
    int rb;
    dict_kv_t *kv;

    /* Make sure they aren't doing anything nutty... */
    if(!d || !key || !value || index < 0) {
        return ST_Error_InvalidArgument;
    }

    /* Find the <key, value> pair. */
    if((kv = find_kv(d, key, &rb)) && index < kv->num_values) {
        /* Clean up the old value, and set our new one in place */
        if(d->dtor_val) {
            d->dtor_val(kv->values[index]);
        }

        kv->values[index] = value;

        return ST_Error_None;
    }

    return ST_Error_NotFound;
}
