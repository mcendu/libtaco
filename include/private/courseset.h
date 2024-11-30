/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_COURSESET_H_
#define TAIKO_COURSESET_H_

#include "taco.h"

extern taiko_courseset *taiko_courseset_create_() TAIKO_MALLOC;
extern taiko_courseset *
taiko_courseset_create2_(taiko_allocator *alloc) TAIKO_MALLOC;

extern int taiko_courseset_set_title_(taiko_courseset *restrict set,
                                      const char *restrict title);
extern int taiko_courseset_set_subtitle_(taiko_courseset *restrict set,
                                         const char *restrict subtitle);
extern int taiko_courseset_set_genre_(taiko_courseset *restrict set,
                                      const char *restrict genre);
extern int taiko_courseset_set_maker_(taiko_courseset *restrict set,
                                      const char *restrict maker);

extern int taiko_courseset_set_filename_(taiko_courseset *restrict set,
                                         const char *restrict filename);
extern int taiko_courseset_set_audio_(taiko_courseset *restrict set,
                                      const char *restrict path);

extern void taiko_courseset_set_demo_time_(taiko_courseset *restrict set,
                                           double time);

extern taiko_course *
taiko_courseset_get_course_mut_(taiko_courseset *restrict set, int class);
extern int taiko_courseset_add_course_(taiko_courseset *restrict set,
                                       taiko_course *restrict course);
extern void taiko_courseset_delete_course_(taiko_courseset *restrict set,
                                           int class);

#endif /* !TAIKO_COURSESET_H_ */
