/*
 * SPDX-FileCopyrightText: 2024 Alain Emilia Anna Zscheile <fogti+devel@ytrizja.de>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// find last file extension
size_t mmzx_find_ext(const char *name) {
    size_t start = 0;
    for(size_t i = 0; *name; ++i, ++name) {
        if(*name == '.') {
            start = i + 1;
        }
    }
    return start;
}

// normalize file extension (upper to lower case)
void mmzx_normalize_ext(char *ext) {
    for(; *ext; ++ext) {
        *ext = tolower(*ext);
    }
}

// check if the file extension is known
bool mmzx_has_known_ext(const char *ext) {
    static const char *const knownExts[] = {
        "avi",
        "jpg",
        "mts",
        "pdf",
        0
    };

    for(const char *const *ext2 = knownExts; *ext2; ++ext2) {
        if(strcmp(ext, *ext2) == 0)
            return true;
    }
    return false;
}

// update leading common substring
void mmzx_update_llcs(size_t *retlen, const char *first, const char *next) {
    if((!retlen) || (!first) || (!next)) return;

    size_t i = 0;
    for(; i < *retlen; ++i, ++first, ++next) {
        assert(*first);
        if(*next != *first) {
            *retlen = i;
            break;
        } else if(!*next) {
            break;
        }
    }
}

void mmzx_deinit_name_ent(mmzx_name_ent_t *self) {
    if (self->orig_name != self->name) {
        free((char*)self->orig_name);
        self->orig_name = 0;
    }
    free((char*)self->name);
    self->name = 0;
}

void mmzx_deinit_names(mmzx_names_t *self) {
    mmzx_name_ent_t *names = self->names;
    for(size_t i = 0; i < self->length; ++i) {
        mmzx_deinit_name_ent(&names[i]);
    }
    free((void*) self->names);
    self->names = 0;
}
