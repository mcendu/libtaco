/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_SECTION_H_
#define TAIKO_SECTION_H_

#include "note.h" /* IWYU pragma: keep; required by macros */
#include "taco.h"

extern taiko_section *taiko_section_create_() TAIKO_MALLOC;
extern taiko_section *
taiko_section_create2_(taiko_allocator *alloc) TAIKO_MALLOC;
extern taiko_section *
taiko_section_clone_(const taiko_section *restrict other) TAIKO_MALLOC;
extern void taiko_section_free_(taiko_section *section);

extern taiko_event *taiko_section_begin_mut_(taiko_section *restrict section);
extern taiko_event *taiko_section_end_mut_(taiko_section *restrict section);
extern taiko_event *taiko_section_locate_mut_(taiko_section *restrict s,
                                              size_t i);

extern void taiko_section_set_tickrate_(taiko_section *restrict section,
                                        int tickrate);

extern int taiko_section_push_(taiko_section *restrict section,
                               const taiko_event *restrict event);
extern int taiko_section_push_many_(taiko_section *restrict section,
                                    const taiko_event events[restrict],
                                    size_t count);
extern int taiko_section_concat_(taiko_section *restrict section,
                                 const taiko_section *restrict other);

extern int taiko_section_delete_(taiko_section *section, taiko_event *i);
extern void taiko_section_clear_(taiko_section *restrict section);
extern int taiko_section_trim_(taiko_section *restrict section);

extern int taiko_section_set_balloons_(taiko_section *restrict section,
                                       const int balloons[restrict],
                                       size_t count);

#define taiko_section_foreach_mut_(i, s)                                       \
  for (taiko_event *i = taiko_section_begin_mut_(s);                           \
       i != taiko_section_end(s); i = taiko_event_next_mut_(i))

#endif /* !TAIKO_SECTION_H_ */
