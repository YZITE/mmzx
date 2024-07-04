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
    size_t llcs = 7;
    mmzx_update_llcs(&llcs, "allemam", "allem");
    mmzx_update_llcs(&llcs, "allemam", "allex");
    mmzx_update_llcs(&llcs, "allemam", "allexifiy");
    mmzx_update_llcs(&llcs, "allemam", "allexa");
    cr_assert(eq(u64, 4, llcs));

    mmzx_update_llcs(&llcs, "allemam", "b");
    mmzx_update_llcs(&llcs, "allemam", "bonk");
    cr_assert(eq(u64, 0, llcs));
}
