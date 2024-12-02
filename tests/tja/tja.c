// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

const char suite_name[] = "libtaco_tja";

extern TCase *case_events();
extern TCase *case_parser();
extern TCase *case_segment();

TCase *(*const cases[])(void) = {
    case_events,
    case_parser,
    case_segment,
    NULL,
};
