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

#ifndef TACO_H
#define TACO_H

/* ## Required headers */

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* ## Attribute macros */

/* Check support for C++-style attributes. */
#if __STDC_VERSION__ >= 202311L || __cplusplus >= 201103L
#define TACO_CXX_ATTRIBUTES_
#endif

/* Determine the right attribute syntax to use. */
#ifdef TACO_CXX_ATTRIBUTES_
#define TACO_ATTRIBUTE_(a) [[a]]
#ifdef __GNUC__
#define TACO_ATTRIBUTE_GNU_(a) [[gnu::a]]
#endif
#elif defined(__GNUC__)
#define TACO_ATTRIBUTE_GNU_(a) __attribute__((a))
#endif

/* Fallback definitions. */
#ifndef TACO_ATTRIBUTE_GNU_
#define TACO_ATTRIBUTE_GNU_(a)
#endif

#ifndef TACO_ATTRIBUTE_
#define TACO_ATTRIBUTE_(a) TACO_ATTRIBUTE_GNU_(a)
#endif

/* C++ don't support restrict without extensions. Define it. */
#if defined(__cplusplus) || __STDC_VERSION__ < 199901L
#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif
#endif

/* TACO_PUBLIC declares things usable for programs. */
#if defined(_WIN32) && !defined(TACO_TESTS_)
#ifdef TACO_BUILDING_
#define TACO_PUBLIC __declspec(dllexport)
#else
#define TACO_PUBLIC __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#define TACO_PUBLIC TACO_ATTRIBUTE_GNU_(visibility("default")) extern
#else
#define TACO_PUBLIC extern
#endif

/* Miscellaneous attributes for easier compile-time checks. */
#define TACO_ALLOC_SIZE(s) TACO_ATTRIBUTE_GNU_(alloc_size(s))
#define TACO_DEPRECATED TACO_ATTRIBUTE_(deprecated)
#define TACO_PRINTF(fmt, va) TACO_ATTRIBUTE_GNU_(format(printf, (fmt), (va)))
#define TACO_MALLOC TACO_ATTRIBUTE_GNU_(malloc)

#if __STDC_VERSION__ >= 202311L
#define TACO_PURE TACO_ATTRIBUTE_(reproducible) TACO_ATTRIBUTE_GNU_(pure)
#else
#define TACO_PURE TACO_ATTRIBUTE_GNU_(pure)
#endif

/* ## Enums */

/* Player's side. */
#define TACO_SIDE_LEFT 0
#define TACO_SIDE_RIGHT 1

/* Play style. */
#define TACO_STYLE_SINGLE 0
#define TACO_STYLE_2P_ONLY 1
#define TACO_STYLE_COUPLE 2
#define TACO_STYLE_DOUBLE 3

/* Difficulty class. */
#define TACO_CLASS_EASY 0
#define TACO_CLASS_NORMAL 1
#define TACO_CLASS_HARD 2
#define TACO_CLASS_ONI 3
#define TACO_CLASS_EX_EASY 4
#define TACO_CLASS_EX_NORMAL 5
#define TACO_CLASS_EX_HARD 6
#define TACO_CLASS_EX 7

/* Scroll style. */
#define TACO_SCROLLMODE_TAIKO 0
#define TACO_SCROLLMODE_BM 1

/* Branch. */
#define TACO_BRANCH_NORMAL 0
#define TACO_BRANCH_ADVANCED 1
#define TACO_BRANCH_MASTER 2

/* Event types. Positive numbers indicate interactive events, ones the
   players can react to; this include normal notes and drum roll notes.
   Negative numbers are for things that are not interacted with, like
   measure lines and speed changes. Event IDs fall between -32768 and
   32767.  */
#define TACO_EVENT_NONE 0
#define TACO_EVENT_DON 1
#define TACO_EVENT_KAT 2
#define TACO_EVENT_DON_BIG 3
#define TACO_EVENT_KAT_BIG 4
#define TACO_EVENT_ROLL 5
#define TACO_EVENT_ROLL_BIG 6
#define TACO_EVENT_BALLOON 7
#define TACO_EVENT_ROLL_END 8
#define TACO_EVENT_KUSUDAMA 9
#define TACO_EVENT_ROLL_CHECKPOINT 10

#define TACO_EVENT_LANDMINE 11

#define TACO_EVENT_MEASURE (-1)
#define TACO_EVENT_GOGOSTART (-17)
#define TACO_EVENT_GOGOEND (-32)
#define TACO_EVENT_SCROLL (-33)
#define TACO_EVENT_SCROLL_COMPLEX (-34)
#define TACO_EVENT_BPM (-49)
#define TACO_EVENT_DELAY (-50)
#define TACO_EVENT_BRANCH_START (-65)
#define TACO_EVENT_BRANCH_JUMP (-66)
#define TACO_EVENT_BRANCH_CHECK (-67)

/* Branch conditions. */
#define TACO_BRANCHTYPE_NONE 0
#define TACO_BRANCHTYPE_ACCURACY 1
#define TACO_BRANCHTYPE_ROLL 2
#define TACO_BRANCHTYPE_ACCURACY_BIG 3
#define TACO_BRANCHTYPE_MAX 4

/* Note annotations. */
#define TACO_TEXT(detail) ((detail) & (INT_MIN | 0xf))
#define TACO_TEXT_DO 0
#define TACO_TEXT_KO 1
#define TACO_TEXT_DON 2
#define TACO_TEXT_KA 4
#define TACO_TEXT_KAT 6

#define TACO_DETAIL_HAND 16

/* Allow trivial calling of libtaco functions from C++. */
#ifdef __cplusplus
extern "C" {
#endif

/* ## Types */

/* Types for user-visible and user-supplied stuff. */

/* An object that allocates memory for other objects. */
typedef struct taco_allocator_ taco_allocator;
/* Provides I/O support for an object. */
typedef struct taco_io_ taco_io;
/* The scoring mode of a branched section. */
typedef struct taco_branch_scoring_ taco_branch_scoring;

/* Types for libtaco managed stuff. Everything here are opaque. */

/* Reads coursesets from a disk or other input. */
typedef struct taco_parser_ taco_parser;
/* A set of game levels synced to the same piece of music. */
typedef struct taco_courseset_ taco_courseset;
/* A single game level. */
typedef struct taco_course_ taco_course;
/*
 * A branch of a level.
 *
 * Internally also used for sections of a branch.
 */
typedef struct taco_section_ taco_section;
/* An event in a level. */
typedef struct taco_event_ taco_event;

/* ## Callbacks */

/* Allocates memory. */
typedef void *taco_malloc_fn(size_t size, void *restrict heap)
    TACO_ALLOC_SIZE(1);
/* Frees memory. */
typedef void taco_free_fn(void *ptr, void *restrict heap);
/* Moves and resizes memory */
typedef void *taco_realloc_fn(void *ptr, size_t size, void *restrict heap)
    TACO_ALLOC_SIZE(2);

/* Read data from stream. (cf. fread) */
typedef size_t taco_read_fn(void *restrict dst, size_t size, size_t count,
                            void *restrict stream);
/* Write data to stream. (cf. fwrite) */
typedef size_t taco_write_fn(const void *restrict src, size_t size,
                             size_t count, void *restrict stream);
/* Close stream. Not used if you pass in the stream. (cf. fclose) */
typedef int taco_close_fn(void *stream);
/* Write formatted output to stream. (cf. vfprintf) */
typedef int taco_printf_fn(void *restrict stream, const char *restrict format,
                           va_list ap);
/* Change the position of a stream. (cf. fseek) */
typedef int taco_seek_fn(void *restrict stream, uint64_t offset, int whence);

/* ## Struct definitions */

struct taco_branch_scoring_ {
  int good;
  int good_big;
  int ok;
  int ok_big;
  int roll;
  int bad;
};

struct taco_allocator_ {
  taco_malloc_fn *malloc;
  taco_free_fn *free;
  taco_realloc_fn *realloc;
  void *heap;
};

struct taco_io_ {
  /*
   * Version field, for backwards compatibility. you should always say
   * `sizeof(taco_io)` here.
   */
  size_t version;
  taco_read_fn *read;
  taco_write_fn *write;
  taco_close_fn *close;
  taco_printf_fn *printf;
  taco_seek_fn *seek;
  /* Do not modify any fields above. Add new fields below. */
};

/* Functions */

/* Creates a TJA parser. */
TACO_PUBLIC taco_parser *taco_parser_tja_create();
/* Creates a TJA parser, with the specified allocator. */
TACO_PUBLIC taco_parser *taco_parser_tja_create2(taco_allocator *allocator);
/* Destroys a parser. */
TACO_PUBLIC void taco_parser_free(taco_parser *parser);
/* Parse a courseset from the filesystem. */
TACO_PUBLIC taco_courseset *taco_parser_parse_file(taco_parser *restrict parser,
                                                   const char *restrict file);
/* Parse a courseset from an open <stdio.h> stream. */
TACO_PUBLIC taco_courseset *
taco_parser_parse_stdio(taco_parser *restrict parser, FILE *file);

TACO_PUBLIC int taco_parser_set_error_stdio(taco_parser *restrict parser,
                                            FILE *file);

/* Destroys a courseset. */
TACO_PUBLIC void taco_courseset_free(taco_courseset *set);

/* Gets the song title. */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_title(const taco_courseset *restrict set);
/* Gets the song subtitle or artist info. */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_subtitle(const taco_courseset *restrict set);
/* Gets the song genre. */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_genre(const taco_courseset *restrict set);
/* Gets the maker of the courseset. */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_maker(const taco_courseset *restrict set);
/* Gets the filename of the set. (e.g. rot.tja) */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_filename(const taco_courseset *restrict set);
/* Gets the path to the audio, relative to the set's directory. */
TACO_PURE TACO_PUBLIC const char *
taco_courseset_audio(const taco_courseset *restrict set);
/* Gets the start time of the song's preview, in seconds.. */
TACO_PURE TACO_PUBLIC double
taco_courseset_demo_time(const taco_courseset *restrict set);
/* Gets a course with the difficulty class specified. */
TACO_PURE TACO_PUBLIC const taco_course *
taco_courseset_get_course(const taco_courseset *restrict set, int diffclass);

/* Gets difficulty info. */
TACO_PUBLIC void taco_course_difficulty(const taco_course *restrict course,
                                        int *restrict diffclass,
                                        int *restrict level);
/* Gets the difficulty class. */
TACO_PURE TACO_PUBLIC int taco_course_class(const taco_course *restrict course);
/* Gets the level. */
TACO_PURE TACO_PUBLIC double
taco_course_level(const taco_course *restrict course);
/* Gets the playstyle. */
TACO_PURE TACO_PUBLIC int taco_course_style(const taco_course *restrict course);
/* Gets the main tempo. */
TACO_PURE TACO_PUBLIC double
taco_course_bpm(const taco_course *restrict course);
/* Gets when the audio should start playing in seconds. */
TACO_PURE TACO_PUBLIC double
taco_course_offset(const taco_course *restrict course);
/* Gets whether multiple players can play the same side of a course. */
TACO_PURE TACO_PUBLIC int
taco_course_papamama(const taco_course *restrict course);
/* Gets whether the course branch into different variants. */
TACO_PURE TACO_PUBLIC int
taco_course_branched(const taco_course *restrict course);
/* Gets the base component of the per-note score. */
TACO_PUBLIC int taco_course_score_base(const taco_course *restrict course);
/* Gets the bonus component of the per note score. */
TACO_PUBLIC int taco_course_score_bonus(const taco_course *restrict course);
/* Gets the per-note score under tournament scoring. */
TACO_PUBLIC int
taco_course_score_tournament(const taco_course *restrict course);

/* Gets the branch of a course. */
TACO_PURE TACO_PUBLIC const taco_section *
taco_course_get_branch(const taco_course *restrict course, int side,
                       int branch);

/* Gets the count of events. */
TACO_PURE TACO_PUBLIC size_t
taco_section_size(const taco_section *restrict section);
/* Gets the number of ticks per 4/4 measure. */
TACO_PURE TACO_PUBLIC int
taco_section_tickrate(const taco_section *restrict section);

/* Gets an iterator to the first event. */
TACO_PURE TACO_PUBLIC const taco_event *
taco_section_begin(const taco_section *restrict section);
/* Gets an iterator to after the last event. It cannot be dereferenced. */
TACO_PURE TACO_PUBLIC const taco_event *
taco_section_end(const taco_section *restrict section);
/* Gets an iterator to an arbitrary event. Returns NULL if out of bounds. */
TACO_PURE TACO_PUBLIC const taco_event *
taco_section_locate(const taco_section *restrict section, size_t index);

/* Print a section in an unstable, human-readable format. */
TACO_PUBLIC void taco_section_print(const taco_section *restrict section,
                                    FILE *restrict file);

/* Gets the time of an event in ticks. */
TACO_PURE TACO_PUBLIC int taco_event_time(const taco_event *restrict event);
/* Gets the event type. */
TACO_PURE TACO_PUBLIC int taco_event_type(const taco_event *restrict event);
/* Gets the annotation and appearance of a normal note. */
TACO_PURE TACO_PUBLIC int
taco_event_detail_int(const taco_event *restrict event);
/* Gets the parameter of an event. */
TACO_PURE TACO_PUBLIC double
taco_event_detail_float(const taco_event *restrict event);
/* Get the scrolling multiplier of a scroll event. */
TACO_PUBLIC int taco_event_scroll(const taco_event *restrict event,
                                  double *restrict x, double *restrict y);
/* Gets the scoring parameters of a branching section. */
TACO_PUBLIC int
taco_event_branch_scoring(const taco_event *restrict event,
                          taco_branch_scoring *restrict scoring);
/* Gets the branch thresholds of a branching section. */
TACO_PUBLIC int taco_event_branch_thresholds(const taco_event *restrict event,
                                             int *restrict advanced,
                                             int *restrict master);

/* Check if the event is interactive, e.g. like a Don/Kat or a drum roll. */
TACO_PURE TACO_PUBLIC int taco_event_is_note(const taco_event *restrict event);
/* Check if the event is a Don or a Kat. */
TACO_PURE TACO_PUBLIC int
taco_event_is_normal_note(const taco_event *restrict event);
/* Check if the event starts a drumroll. */
TACO_PURE TACO_PUBLIC int taco_event_is_roll(const taco_event *restrict event);

/* Compares two events for sorting. */
TACO_PURE TACO_PUBLIC int taco_event_compare(const taco_event *restrict a,
                                             const taco_event *restrict b);

/* Gets the time of an event in seconds. */
TACO_PURE TACO_PUBLIC double
taco_event_seconds(const taco_event *restrict event,
                   const taco_section *restrict section);

/* Gets the next event. */
TACO_PURE TACO_PUBLIC const taco_event *
taco_event_next(const taco_event *restrict event);
/* Gets the previous event. */
TACO_PURE TACO_PUBLIC const taco_event *
taco_event_prev(const taco_event *restrict event);

/* ## Aliases */

/* Gets the number of hits required for a balloon. */
#define taco_event_hits taco_event_detail_int
/* Gets the new tempo. */
#define taco_event_bpm taco_event_detail_float

/* ## Shorthands */

/* Iterates over a section. Replace `i` with an unused variable name. */
#define taco_section_foreach(i, s)                                             \
  for (const taco_event *i = taco_section_begin(s); i != taco_section_end(s);  \
       i = taco_event_next(i))

/* Gets the annotation of a note. */
#define taco_event_notetext(e) (TACO_TEXT(taco_event_detail_int(e)))

#ifdef __cplusplus
}
#endif

#endif /* !TACO_H */
