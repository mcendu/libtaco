// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

const char suite_name[] = "libtaco";

extern TCase *case_course();
extern TCase *case_note();
extern TCase *case_section();

TCase *(*const cases[])(void) = {
    case_course,
    case_note,
    case_section,
    NULL,
};
