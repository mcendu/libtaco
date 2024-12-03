/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_COURSESET_H_
#define TACO_COURSESET_H_

#include "taco.h"

extern taco_courseset *taco_courseset_create_() TACO_MALLOC;
extern taco_courseset *
taco_courseset_create2_(taco_allocator *alloc) TACO_MALLOC;

extern int taco_courseset_set_title_(taco_courseset *restrict set,
                                     const char *restrict title);
extern int taco_courseset_set_subtitle_(taco_courseset *restrict set,
                                        const char *restrict subtitle);
extern int taco_courseset_set_genre_(taco_courseset *restrict set,
                                     const char *restrict genre);
extern int taco_courseset_set_maker_(taco_courseset *restrict set,
                                     const char *restrict maker);

extern int taco_courseset_set_filename_(taco_courseset *restrict set,
                                        const char *restrict filename);
extern int taco_courseset_set_audio_(taco_courseset *restrict set,
                                     const char *restrict path);

extern void taco_courseset_set_demo_time_(taco_courseset *restrict set,
                                          double time);

extern taco_course *taco_courseset_get_course_mut_(taco_courseset *restrict set,
                                                   int class);
extern int taco_courseset_add_course_(taco_courseset *restrict set,
                                      taco_course *restrict course);
extern void taco_courseset_delete_course_(taco_courseset *restrict set,
                                          int class);

#endif /* !TACO_COURSESET_H_ */
