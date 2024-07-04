/*
 * SPDX-FileCopyrightText: 2024 Alain Emilia Anna Zscheile <fogti+devel@ytrizja.de>
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include <stddef.h>

// find last file extension
size_t mmzx_find_ext(const char *name);

// normalize file extension (upper to lower case)
void mmzx_normalize_ext(char *ext);

// check if the file extension is known
bool mmzx_has_known_ext(const char *ext);

// update leading common substring
void mmzx_update_llcs(size_t *retlen, const char *first, const char *next);

typedef struct {
    const char *orig_name;
    const char *name;
} mmzx_name_ent_t;

// note: this doesn't allocate any memory
static inline void mmzx_copy_name_ent(mmzx_name_ent_t *dst, mmzx_name_ent_t *src) {
    dst->orig_name = src->orig_name;
    dst->name = src->name;
}

void mmzx_deinit_name_ent(mmzx_name_ent_t *self);

typedef struct {
    mmzx_name_ent_t *names;
    size_t length;
    size_t capacity;
} mmzx_names_t;

void mmzx_deinit_names(mmzx_names_t *self);
