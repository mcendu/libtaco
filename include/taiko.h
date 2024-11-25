/*
 * Copyright (c) 2024, McEndu et al.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TAIKO_H
#define TAIKO_H

/* Includes */

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/* Attribute syntax */

#if __STDC_VERSION__ >= 202311L || __cplusplus >= 201103L
#define TAIKO_CXX_ATTRIBUTES_
#endif

#ifdef TAIKO_CXX_ATTRIBUTES_
#define TAIKO_ATTRIBUTE_(a) [[a]]
#ifdef __GNUC__
#define TAIKO_ATTRIBUTE_GNU_(a) [[gnu::a]]
#endif
#elif defined(__GNUC__)
#define TAIKO_ATTRIBUTE_GNU_(a) __attribute__((a))
#endif

#ifndef TAIKO_ATTRIBUTE_GNU_
#define TAIKO_ATTRIBUTE_GNU_(a)
#endif

#ifndef TAIKO_ATTRIBUTE_
#define TAIKO_ATTRIBUTE_(a) TAIKO_ATTRIBUTE_GNU_(a)
#endif

/* restrict */

#if defined(__cplusplus) || __STDC_VERSION__ < 199901L
#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif
#endif

/* Attributes */

#ifdef __GNUC__
#define TAIKO_PUBLIC TAIKO_ATTRIBUTE_GNU_(visibility("default")) extern
#elif defined(_MSC_VER) && !defined(TAIKO_STATIC)
#ifdef TAIKO_BUILDING_
#define TAIKO_PUBLIC __declspec(dllexport)
#else
#define TAIKO_PUBLIC __declspec(dllimport)
#endif
#else
#define TAIKO_PUBLIC extern
#endif

#define TAIKO_ALLOC_SIZE(s) TAIKO_ATTRIBUTE_GNU_(alloc_size(s))
#define TAIKO_DEPRECATED TAIKO_ATTRIBUTE_(deprecated)
#define TAIKO_PRINTF(fmt, va) TAIKO_ATTRIBUTE_GNU_(format(printf, (fmt), (va)))
#define TAIKO_MALLOC TAIKO_ATTRIBUTE_GNU_(malloc)

#if __STDC_VERSION__ >= 202311L
#define TAIKO_PURE TAIKO_ATTRIBUTE_(reproducible) TAIKO_ATTRIBUTE_GNU_(pure)
#else
#define TAIKO_PURE TAIKO_ATTRIBUTE_GNU_(pure)
#endif

/* Enums */

#define TAIKO_SIDE_LEFT 0
#define TAIKO_SIDE_RIGHT 1

#define TAIKO_STYLE_SINGLE 0 // singleplayer; both players get the same score
#define TAIKO_STYLE_TJA_L_ 0 // internal, left
#define TAIKO_STYLE_TJA_R_ 1 // internal, right
#define TAIKO_STYLE_COUPLE 2 // multiplayer; each side has a unique score
#define TAIKO_STYLE_DOUBLE 3 // singleplayer; each side has a unique score

#define TAIKO_CLASS_EASY 0
#define TAIKO_CLASS_NORMAL 1
#define TAIKO_CLASS_HARD 2
#define TAIKO_CLASS_ONI 3
#define TAIKO_CLASS_EX_EASY 4
#define TAIKO_CLASS_EX_NORMAL 5
#define TAIKO_CLASS_EX_HARD 6
#define TAIKO_CLASS_EX 7

#define TAIKO_SCOREMODE_DONDERFUL 0
#define TAIKO_SCOREMODE_CLASSIC 1
#define TAIKO_SCOREMODE_357 2
#define TAIKO_SCOREMODE_NIJIIRO 3

#define TAIKO_SCROLLMODE_TAIKO 0
#define TAIKO_SCROLLMODE_BM 1

#define TAIKO_BRANCH_NORMAL 0
#define TAIKO_BRANCH_ADVANCED 1
#define TAIKO_BRANCH_MASTER 2

#define TAIKO_EVENT_NONE 0
#define TAIKO_EVENT_DON 1
#define TAIKO_EVENT_KAT 2
#define TAIKO_EVENT_DON_BIG 3
#define TAIKO_EVENT_KAT_BIG 4
#define TAIKO_EVENT_ROLL 5
#define TAIKO_EVENT_ROLL_BIG 6
#define TAIKO_EVENT_BALLOON 7
#define TAIKO_EVENT_ROLL_END 8
#define TAIKO_EVENT_KUSUDAMA 9
#define TAIKO_EVENT_ROLL_CHECKPOINT 10

#define TAIKO_EVENT_LANDMINE 11

#define TAIKO_EVENT_MEASURE (-1)
#define TAIKO_EVENT_GOGOSTART (-17)
#define TAIKO_EVENT_GOGOEND (-32)
#define TAIKO_EVENT_SCROLL (-33)
#define TAIKO_EVENT_BPM (-49)
#define TAIKO_EVENT_BRANCH_START (-65)
#define TAIKO_EVENT_BRANCH_JUMP (-66)
#define TAIKO_EVENT_BRANCH_CHECK (-67)

#define TAIKO_EVENT_TJA_MEASURE_LENGTH (-0x10001)
#define TAIKO_EVENT_TJA_BRANCH_THRESHOLD (-0x10002)
#define TAIKO_EVENT_TJA_BRANCH_TYPE (-0x10003)
#define TAIKO_EVENT_TJA_LEVELHOLD (-0x10004)

#define TAIKO_BRANCHTYPE_NONE 0
#define TAIKO_BRANCHTYPE_ACCURACY 1
#define TAIKO_BRANCHTYPE_ROLL 2
#define TAIKO_BRANCHTYPE_ACCURACY_BIG 3

#define TAIKO_TEXT(detail) ((detail) & (INT_MIN | 0xf))
#define TAIKO_TEXT_DO 0
#define TAIKO_TEXT_KO 1
#define TAIKO_TEXT_DON 2
#define TAIKO_TEXT_KA 0
#define TAIKO_TEXT_KAT 2

#define TAIKO_DETAIL_HAND 16

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct taiko_allocator_ taiko_allocator;

typedef struct taiko_courseset_ taiko_courseset;
typedef struct taiko_course_ taiko_course;
typedef struct taiko_section_ taiko_section;
typedef struct taiko_event_ taiko_event;

/* Callbacks */

typedef void *taiko_malloc_fn(size_t size, void *restrict heap)
    TAIKO_ALLOC_SIZE(1);
typedef void taiko_free_fn(void *ptr, void *restrict heap);
typedef void *taiko_realloc_fn(void *ptr, size_t size, void *restrict heap)
    TAIKO_ALLOC_SIZE(2);

typedef size_t taiko_read_fn(void *restrict dst, size_t size, size_t count,
                             void *restrict stream);
typedef size_t taiko_write_fn(const void *restrict src, size_t size,
                              size_t count, void *restrict stream);
typedef int taiko_close_fn(void *stream);
typedef int taiko_printf_fn(void *restrict stream, const char *restrict format,
                            va_list ap);

/* User-supplied structures */

struct taiko_allocator_ {
  taiko_malloc_fn *malloc;
  taiko_free_fn *free;
  taiko_realloc_fn *realloc;
  void *heap;
};

/* Core functions */

TAIKO_PUBLIC void taiko_courseset_free(taiko_courseset *set);

TAIKO_PUBLIC const char *
taiko_courseset_title(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC const char *
taiko_courseset_subtitle(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC const char *
taiko_courseset_genre(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC const char *
taiko_courseset_maker(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC const char *
taiko_courseset_filename(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC const char *
taiko_courseset_audio(const taiko_courseset *restrict set) TAIKO_PURE;
TAIKO_PUBLIC double
taiko_courseset_demo_time(const taiko_courseset *restrict set) TAIKO_PURE;

TAIKO_PUBLIC const taiko_course *
taiko_courseset_get_course(const taiko_courseset *restrict set,
                           int class) TAIKO_PURE;

TAIKO_PUBLIC void taiko_course_difficulty(const taiko_course *restrict course,
                                          int *restrict class,
                                          int *restrict level);
TAIKO_PUBLIC int
taiko_course_class(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC double
taiko_course_level(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC int
taiko_course_style(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC double
taiko_course_bpm(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC double
taiko_course_offset(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC int
taiko_course_papamama(const taiko_course *restrict course) TAIKO_PURE;
TAIKO_PUBLIC int
taiko_course_branched(const taiko_course *restrict course) TAIKO_PURE;

TAIKO_PUBLIC const taiko_section *
taiko_course_get_branch(const taiko_course *restrict course, int side,
                        int branch) TAIKO_PURE;

TAIKO_PUBLIC size_t taiko_section_size(const taiko_section *restrict section)
    TAIKO_PURE;
TAIKO_PUBLIC int
taiko_section_tickrate(const taiko_section *restrict section) TAIKO_PURE;

TAIKO_PUBLIC const taiko_event *
taiko_section_begin(const taiko_section *restrict section) TAIKO_PURE;
TAIKO_PUBLIC const taiko_event *
taiko_section_end(const taiko_section *restrict section) TAIKO_PURE;
TAIKO_PUBLIC const taiko_event *
taiko_section_locate(const taiko_section *restrict section,
                     size_t index) TAIKO_PURE;

TAIKO_PUBLIC int taiko_event_time(const taiko_event *restrict event) TAIKO_PURE;
TAIKO_PUBLIC int taiko_event_type(const taiko_event *restrict event) TAIKO_PURE;
TAIKO_PUBLIC int
taiko_event_detail_int(const taiko_event *restrict event) TAIKO_PURE;
TAIKO_PUBLIC double
taiko_event_detail_float(const taiko_event *restrict event) TAIKO_PURE;

TAIKO_PUBLIC int taiko_event_compare(const taiko_event *restrict a,
                                     const taiko_event *restrict b);

TAIKO_PUBLIC const taiko_event *
taiko_event_next(const taiko_event *restrict event) TAIKO_PURE;
TAIKO_PUBLIC const taiko_event *
taiko_event_prev(const taiko_event *restrict event) TAIKO_PURE;

/* Utility macros for public functions */

#define taiko_section_foreach(i, s)                                            \
  for (const taiko_event *i = taiko_section_begin(s);                          \
       i != taiko_section_end(s); i = taiko_event_next(i))

#define taiko_event_notetext(e) (TAIKO_TEXT(taiko_event_detail_int(e)))
#define taiko_event_hits taiko_event_detail_int

#ifdef __cplusplus
}
#endif

#endif /* !TAIKO_H */
