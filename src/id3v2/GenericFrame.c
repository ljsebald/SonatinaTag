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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SonatinaTag/Error.h"
#include "Frame.h"

static void free_generic(ST_GenericFrame *f) {
    free(f->data);
    free(f);
}

ST_FUNC ST_GenericFrame *ST_ID3v2_GenericFrame_create(uint32_t sz, uint8_t *d) {
    ST_GenericFrame *rv = (ST_GenericFrame *)malloc(sizeof(ST_GenericFrame));

    if(rv) {
        rv->base.type = ST_FrameType_Generic;
        rv->base.dtor = (void (*)(ST_Frame *))free_generic;
        rv->size = sz;
        rv->data = d;
    }

    return rv;
}

ST_FUNC const uint8_t *ST_ID3v2_GenericFrame_data(const ST_GenericFrame *f) {
    if(!f)
        return NULL;

    return f->data;
}

ST_FUNC uint32_t ST_ID3v2_GenericFrame_size(const ST_GenericFrame *f) {
    if(!f)
        return 0;

    return f->size;
}

ST_FUNC ST_Error ST_ID3v2_GenericFrame_setData(ST_GenericFrame *f, uint32_t sz,
                                               uint8_t *d, int own_buf) {
    uint8_t *tmp = d;

    if(!f || !d || !sz)
        return ST_Error_InvalidArgument;

    if(!own_buf) {
        if(!(tmp = (uint8_t *)malloc((size_t)sz)))
            return ST_Error_errno;

        memcpy(tmp, d, sz);
    }

    free(f->data);
    f->data = tmp;
    f->size = sz;

    return ST_Error_None;
}
