// SPDX-License-Identifier: BSD-2-Clause
#include "tja/metadata.h"

#include "alloc.h"
#include "course.h"
#include "courseset.h"
#include "taco.h"
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct tja_metadata_ {
  taco_allocator *alloc;
  double bpm;
  double offset;
  double demostart;
  double level;
  char *title;
  char *subtitle;
  char *genre;
  char *maker;
  char *audio;
  tja_balloon *balloon_n;
  tja_balloon *balloon_a;
  tja_balloon *balloon_m;
  int side;
  int course;
  int style;
  int scoreinit;
  int scorediff;
  int scoreinit_s;
  bool papamama;
};

typedef int (*metadata_setter)(tja_metadata *meta, tja_metadata_field *field);

static metadata_setter setter(int index);

tja_metadata *tja_metadata_create2_(taco_allocator *a) {
  tja_metadata *m = taco_malloc_(a, sizeof(tja_metadata));
  if (!m)
    return NULL;
  memset(m, 0, sizeof(*m));
  m->alloc = a;
  m->bpm = NAN;
  m->offset = NAN;
  m->demostart = NAN;
  m->level = NAN;
  m->side = -1;
  m->course = -1;
  m->style = -1;
  m->scoreinit = -1;
  m->scoreinit_s = -1;
  m->scorediff = -1;
  return m;
}

void tja_metadata_free_(tja_metadata *meta) {
  if (!meta)
    return;

  taco_free_(meta->alloc, meta->title);
  taco_free_(meta->alloc, meta->subtitle);
  taco_free_(meta->alloc, meta->genre);
  taco_free_(meta->alloc, meta->maker);
  taco_free_(meta->alloc, meta->audio);
  taco_free_(meta->alloc, meta->balloon_n);
  taco_free_(meta->alloc, meta->balloon_a);
  taco_free_(meta->alloc, meta->balloon_m);
  taco_free_(meta->alloc, meta);
}

int tja_metadata_add_field_(tja_metadata *meta, tja_metadata_field *field) {
  if (field->key >= TJA_METADATA_MAX)
    return -1;

  return setter(field->key)(meta, field);
}

static inline void add_text_field_fn_(taco_allocator *alloc, char **dst,
                                      char **src) {
  taco_free_(alloc, *dst);
  *dst = *src;
  *src = NULL;
}

#define add_text_field(meta, updates, field)                                   \
  add_text_field_fn_((meta)->alloc, &(meta)->field, &(updates)->field)

static inline void move_balloon_fn_(taco_allocator *alloc, tja_balloon **dst,
                                    tja_balloon **src) {
  taco_free_(alloc, *dst);
  *dst = *src;
  *src = NULL;
}

#define move_balloon(meta, updates, field)                                     \
  move_balloon_fn_((meta)->alloc, &(meta)->field, &(updates)->field)

#define update_real(meta, updates, field)                                      \
  if (!isnan((updates)->field))                                                \
  (meta)->field = (updates)->field

#define update_int(meta, updates, field)                                       \
  if ((updates)->field != -1)                                                  \
  (meta)->field = (updates)->field

int tja_metadata_update_(tja_metadata *meta, tja_metadata *updates) {
  add_text_field(meta, updates, title);
  add_text_field(meta, updates, subtitle);
  add_text_field(meta, updates, genre);
  add_text_field(meta, updates, maker);
  add_text_field(meta, updates, audio);
  update_real(meta, updates, demostart);
  update_real(meta, updates, bpm);
  update_real(meta, updates, offset);
  update_real(meta, updates, level);
  update_int(meta, updates, side);
  update_int(meta, updates, course);
  update_int(meta, updates, style);
  update_int(meta, updates, scoreinit);
  update_int(meta, updates, scoreinit_s);
  update_int(meta, updates, scorediff);
  move_balloon(meta, updates, balloon_n);
  move_balloon(meta, updates, balloon_a);
  move_balloon(meta, updates, balloon_m);

  if (updates->course != -1)
    meta->papamama = updates->papamama;

  return 0;
}

int tja_courseset_apply_metadata_(taco_courseset *set, tja_metadata *meta) {
  if (meta->title)
    taco_courseset_set_title_(set, meta->title);
  if (meta->subtitle)
    taco_courseset_set_subtitle_(set, meta->subtitle);
  if (meta->genre)
    taco_courseset_set_genre_(set, meta->genre);
  if (meta->maker)
    taco_courseset_set_maker_(set, meta->maker);
  if (meta->audio)
    taco_courseset_set_audio_(set, meta->audio);
  if (isnan(taco_courseset_demo_time(set)))
    taco_courseset_set_demo_time_(set, meta->demostart);
  return 0;
}

int tja_course_apply_metadata_(taco_course *course, tja_metadata *meta) {
  taco_course_set_class_(course, meta->course);
  if (!isnan(meta->level))
    taco_course_set_level_(course, meta->level);
  taco_course_set_papamama_(course, meta->papamama);

  if (meta->scoreinit != 0) {
    taco_course_set_score_base_(course, meta->scoreinit);
    if (meta->scorediff != 0)
      taco_course_set_score_bonus_(course, meta->scorediff);
  }
  if (meta->scoreinit_s != 0) {
    taco_course_set_score_tournament_(course, meta->scoreinit_s);
  }

  if (meta->balloon_n)
    taco_course_set_balloons_(course, tja_balloon_data_(meta->balloon_n),
                              tja_balloon_size_(meta->balloon_n),
                              TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  if (meta->balloon_a)
    taco_course_set_balloons_(course, tja_balloon_data_(meta->balloon_a),
                              tja_balloon_size_(meta->balloon_a),
                              TACO_SIDE_LEFT, TACO_BRANCH_ADVANCED);
  if (meta->balloon_m)
    taco_course_set_balloons_(course, tja_balloon_data_(meta->balloon_m),
                              tja_balloon_size_(meta->balloon_m),
                              TACO_SIDE_LEFT, TACO_BRANCH_MASTER);

  return 0;
}

#define SETTER_(field) set_##field##_
#define MAKE_SETTER_(field, type)                                              \
  static int set_##field##_(tja_metadata *m, tja_metadata_field *f) {          \
    m->field = f->type;                                                        \
    return 0;                                                                  \
  }                                                                            \
  static int set_##field##_(tja_metadata *, tja_metadata_field *)

int SETTER_(unrecognized)(tja_metadata *m, tja_metadata_field *f) { return 0; }

MAKE_SETTER_(title, text);
MAKE_SETTER_(subtitle, text);
MAKE_SETTER_(genre, text);
MAKE_SETTER_(maker, text);
MAKE_SETTER_(audio, text);
MAKE_SETTER_(bpm, real);
MAKE_SETTER_(offset, real);
MAKE_SETTER_(demostart, real);
MAKE_SETTER_(side, integer);
MAKE_SETTER_(course, integer);
MAKE_SETTER_(level, real);
MAKE_SETTER_(style, integer);
MAKE_SETTER_(balloon_n, balloon);
MAKE_SETTER_(balloon_a, balloon);
MAKE_SETTER_(balloon_m, balloon);
MAKE_SETTER_(scorediff, integer);
MAKE_SETTER_(papamama, integer);

static int SETTER_(scoreinit)(tja_metadata *m, tja_metadata_field *f) {
  m->scoreinit = f->scoreinit.casual;
  m->scoreinit_s = f->scoreinit.tournament;
  return 0;
}

static const metadata_setter setters[] = {
    [TJA_METADATA_UNRECOGNIZED] = SETTER_(unrecognized),
    [TJA_METADATA_TITLE] = SETTER_(title),
    [TJA_METADATA_SUBTITLE] = SETTER_(subtitle),
    [TJA_METADATA_GENRE] = SETTER_(genre),
    [TJA_METADATA_MAKER] = SETTER_(maker),
    [TJA_METADATA_AUDIO] = SETTER_(audio),
    [TJA_METADATA_BPM] = SETTER_(bpm),
    [TJA_METADATA_OFFSET] = SETTER_(offset),
    [TJA_METADATA_DEMOSTART] = SETTER_(demostart),
    [TJA_METADATA_SIDE] = SETTER_(side),
    [TJA_METADATA_COURSE] = SETTER_(course),
    [TJA_METADATA_LEVEL] = SETTER_(level),
    [TJA_METADATA_STYLE] = SETTER_(style),
    [TJA_METADATA_BALLOON] = SETTER_(balloon_n),
    [TJA_METADATA_BALLOONEXP] = SETTER_(balloon_a),
    [TJA_METADATA_BALLOONMAS] = SETTER_(balloon_m),
    [TJA_METADATA_SCOREINIT] = SETTER_(scoreinit),
    [TJA_METADATA_SCOREDIFF] = SETTER_(scorediff),
    [TJA_METADATA_PAPAMAMA] = SETTER_(papamama),
};

static metadata_setter setter(int index) { return setters[index]; };
