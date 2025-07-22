/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACOASSERT_H
#define TACOASSERT_H

#include "taco.h"

typedef struct assert_section_state_ assert_section_state;

extern assert_section_state *assert_section_setup();
extern void assert_section_teardown(assert_section_state *state);
extern void assert_section_eq(const taco_section *section,
                              const char *expected_path,
                              assert_section_state *restrict state);

#endif
