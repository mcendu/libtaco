/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_SECTION_H_
#define TACO_SECTION_H_

#include "note.h" /* IWYU pragma: keep; required by macros */
#include "taco.h"

TACO_MALLOC extern taco_section *taco_section_create_();
TACO_MALLOC extern taco_section *taco_section_create2_(taco_allocator *alloc);
TACO_MALLOC extern taco_section *
taco_section_clone_(const taco_section *restrict other);
extern void taco_section_free_(taco_section *section);

extern taco_event *taco_section_begin_mut_(taco_section *restrict section);
extern taco_event *taco_section_end_mut_(taco_section *restrict section);
extern taco_event *taco_section_locate_mut_(taco_section *restrict s, size_t i);

extern void taco_section_set_tickrate_(taco_section *restrict section,
                                       int tickrate);

extern int taco_section_push_(taco_section *restrict section,
                              const taco_event *restrict event);
extern int taco_section_push_many_(taco_section *restrict section,
                                   const taco_event *restrict events,
                                   size_t count);
extern int taco_section_concat_(taco_section *restrict section,
                                const taco_section *restrict other);
extern int taco_section_pop_(taco_section *restrict section, size_t count);

extern void taco_section_clear_(taco_section *restrict section);
extern int taco_section_trim_(taco_section *restrict section);

extern int taco_section_set_balloons_(taco_section *restrict section,
                                      const int *restrict balloons,
                                      size_t count);

#define taco_section_foreach_mut_(i, s)                                        \
  for (taco_event *i = taco_section_begin_mut_(s); i != taco_section_end(s);   \
       i = taco_event_next_mut_(i))

#endif /* !TACO_SECTION_H_ */
