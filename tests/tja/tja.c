// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

const char suite_name[] = "libtaco_tja";

extern TCase *case_parser();

TCase *(*const cases[])(void) = {
    case_parser,
    NULL,
};
