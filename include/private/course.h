/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_COURSE_H_
#define TACO_COURSE_H_

#include "taco.h"

TACO_MALLOC extern taco_course *taco_course_create_();
TACO_MALLOC extern taco_course *taco_course_create2_(taco_allocator *alloc);
extern void taco_course_free_(taco_course *restrict course);

extern void taco_course_set_class_(taco_course *restrict course, int class);
extern void taco_course_set_level_(taco_course *restrict course, double level);
extern void taco_course_set_style_(taco_course *restrict course, int style);
extern void taco_course_set_papamama_(taco_course *restrict course,
                                      int papamama);
extern void taco_course_set_bpm_(taco_course *restrict course, double bpm);
extern void taco_course_set_offset_(taco_course *restrict course,
                                    double offset);
extern void taco_course_set_score_base_(taco_course *restrict course, int base);
extern void taco_course_set_score_tournament_(taco_course *restrict course,
                                              int score);
extern void taco_course_set_score_bonus_(taco_course *restrict course,
                                         int bonus);

extern taco_section *taco_course_get_branch_mut_(taco_course *restrict course,
                                                 int side, int branch);
extern taco_section *taco_course_attach_branch_(taco_course *restrict course,
                                                taco_section *restrict data,
                                                int side, int branch);

extern int taco_course_setup_branching_(taco_course *restrict course);

extern int taco_course_set_balloons_(taco_course *restrict course,
                                     const int *restrict balloons, size_t count,
                                     int side, int branch);

extern int taco_course_merge_(taco_course *restrict destination,
                              taco_course *restrict other);

#endif /* !TACO_COURSE_H_ */
