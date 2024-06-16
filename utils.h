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

// find leading common substring length
size_t mmzx_lcs(const char *const *items);

// find leading common substring length, on mapped entries
size_t mmzx_lcs_map(const void *items, size_t itemlen, const char *mapfn(const void *));

typedef struct {
    const char *orig_name;
    const char *name;
} mmzx_name_ent_t;

void mmzx_deinit_name_ent(mmzx_name_ent_t *self);

typedef struct {
    mmzx_name_ent_t *names;
    size_t length;
    size_t capacity;
} mmzx_names_t;

void mmzx_deinit_names(mmzx_names_t *self);

