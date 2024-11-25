// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

const char suite_name[] = "libtaiko";

extern TCase *case_course();
extern TCase *case_section();

TCase *(*const cases[])(void) = {
    case_course,
    case_section,
    NULL,
};
