/*
 * SPDX-FileCopyrightText: 2024 Alain Emilia Anna Zscheile <fogti+devel@ytrizja.de>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>

Test(mmzx_tests, find_ext) {
    const char *e = "this.is.an.file.name";
    cr_assert(eq(u64, 16, mmzx_find_ext(e)));
}

Test(mmzx_tests, normalize) {
    char e[] = "EveryThingMightBe Haa.-";
    mmzx_normalize_ext(e);
    cr_assert(eq(str, e, "everythingmightbe haa.-"));
}

Test(mmzx_tests, lcs) {
    static const char *const a[] = {
        "allemam",
        "allem",
        "allex",
        "allexifiy",
        "allexa",
        0
    };

    cr_assert(eq(u64, 4, mmzx_lcs(a)));

    static const char *const b[] = {
        "allemam",
        "allex",
        "b",
        "bonk",
        0
    };

    cr_assert(eq(u64, 0, mmzx_lcs(b)));
}
