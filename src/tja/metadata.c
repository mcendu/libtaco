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
  taiko_allocator *alloc;
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
  bool papamama;
};

typedef int (*metadata_setter)(tja_metadata *meta, tja_metadata_field *field);

static const metadata_setter setters[];

tja_metadata *tja_metadata_create2_(taiko_allocator *a) {
  tja_metadata *m = taiko_malloc_(a, sizeof(tja_metadata));
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
  return m;
}

void tja_metadata_free_(tja_metadata *meta) {
  if (!meta)
    return;

  taiko_free_(meta->alloc, meta->title);
  taiko_free_(meta->alloc, meta->subtitle);
  taiko_free_(meta->alloc, meta->genre);
  taiko_free_(meta->alloc, meta->maker);
  taiko_free_(meta->alloc, meta->audio);
  taiko_free_(meta->alloc, meta->balloon_n);
  taiko_free_(meta->alloc, meta->balloon_a);
  taiko_free_(meta->alloc, meta->balloon_m);
  taiko_free_(meta->alloc, meta);
}

int tja_metadata_add_field_(tja_metadata *meta, tja_metadata_field *field) {
  if (field->key >= TJA_METADATA_MAX)
    return -1;

  return setters[field->key](meta, field);
}

static inline void add_text_field_fn_(taiko_allocator *alloc, char **dst,
                                      char **src) {
  taiko_free_(alloc, *dst);
  *dst = *src;
  *src = NULL;
}

#define add_text_field(meta, updates, field)                                   \
  add_text_field_fn_((meta)->alloc, &(meta)->field, &(updates)->field)

static inline void move_balloon_fn_(taiko_allocator *alloc, tja_balloon **dst,
                                    tja_balloon **src) {
  taiko_free_(alloc, *dst);
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
  move_balloon(meta, updates, balloon_n);
  move_balloon(meta, updates, balloon_a);
  move_balloon(meta, updates, balloon_m);

  if (updates->course != -1)
    meta->papamama = updates->papamama;

  return 0;
}

int tja_courseset_apply_metadata_(taiko_courseset *set, tja_metadata *meta) {
  if (meta->title && !taiko_courseset_title(set))
    taiko_courseset_set_title_(set, meta->title);
  if (meta->subtitle && !taiko_courseset_subtitle(set))
    taiko_courseset_set_subtitle_(set, meta->subtitle);
  if (meta->genre && !taiko_courseset_genre(set))
    taiko_courseset_set_genre_(set, meta->genre);
  if (meta->maker && !taiko_courseset_maker(set))
    taiko_courseset_set_maker_(set, meta->maker);
  if (meta->audio && !taiko_courseset_audio(set))
    taiko_courseset_set_audio_(set, meta->audio);
  if (isnan(taiko_courseset_demo_time(set)))
    taiko_courseset_set_demo_time_(set, meta->demostart);
  return 0;
}

int tja_course_apply_metadata_(taiko_course *course, tja_metadata *meta) {
  taiko_course_set_class_(course, meta->course);
  taiko_course_set_level_(course, meta->level);
  taiko_course_set_papamama_(course, meta->papamama);

  if (meta->balloon_n)
    taiko_course_set_balloons_(course, tja_balloon_data_(meta->balloon_n),
                               tja_balloon_size_(meta->balloon_n),
                               TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  if (meta->balloon_a)
    taiko_course_set_balloons_(course, tja_balloon_data_(meta->balloon_a),
                               tja_balloon_size_(meta->balloon_a),
                               TAIKO_SIDE_LEFT, TAIKO_BRANCH_ADVANCED);
  if (meta->balloon_m)
    taiko_course_set_balloons_(course, tja_balloon_data_(meta->balloon_m),
                               tja_balloon_size_(meta->balloon_m),
                               TAIKO_SIDE_LEFT, TAIKO_BRANCH_MASTER);

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
MAKE_SETTER_(papamama, integer);

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
    [TJA_METADATA_PAPAMAMA] = SETTER_(papamama),
};
