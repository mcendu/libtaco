/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_COURSE_H_
#define TAIKO_COURSE_H_

#include "taiko.h"

extern taiko_course *taiko_course_create_() TAIKO_MALLOC;
extern taiko_course *taiko_course_create2_(taiko_allocator *alloc) TAIKO_MALLOC;
extern void taiko_course_free_(taiko_course *restrict course);

extern void taiko_course_set_class_(taiko_course *restrict course, int class);
extern void taiko_course_set_level_(taiko_course *restrict course,
                                    double level);
extern void taiko_course_set_style_(taiko_course *restrict course, int style);
extern void taiko_course_set_papamama_(taiko_course *restrict course,
                                       int papamama);
extern void taiko_course_set_bpm_(taiko_course *restrict course, double bpm);
extern void taiko_course_set_offset_(taiko_course *restrict course,
                                     double offset);

extern taiko_section *
taiko_course_get_branch_mut_(taiko_course *restrict course, int side,
                             int branch);
extern taiko_section *taiko_course_attach_branch_(taiko_course *restrict course,
                                                  taiko_section *restrict data,
                                                  int side, int branch);

extern int taiko_course_setup_branching_(taiko_course *restrict course);

extern int taiko_course_set_balloons_(taiko_course *restrict course,
                                      const int balloons[restrict],
                                      size_t count, int side, int branch);

extern int taiko_course_merge_(taiko_course *restrict destination,
                               taiko_course *restrict other);

#endif /* !TAIKO_COURSE_H_ */
