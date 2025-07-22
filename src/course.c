// SPDX-License-Identifier: BSD-2-Clause
#include "course.h"

#include "alloc.h"
#include "section.h"
#include "taco.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

typedef struct balloon_data_ balloon_data_;

struct balloon_data_ {
  int *data;
  size_t count;
};

struct taco_course_ {
  taco_allocator *alloc;

  double basebpm;
  double offset;
  double level;

  int class;
  int style;
  int scrollmode;

  int score_base;
  int score_tournament;
  int score_bonus;

  bool papamama : 1;
  bool branched : 1;

  taco_section *branches[2][3];
  balloon_data_ balloons[2][3];
};

taco_course *taco_course_create_(void) {
  return taco_course_create2_(&taco_default_allocator_);
}

taco_course *taco_course_create2_(taco_allocator *a) {
  taco_course *c = taco_malloc_(a, sizeof(taco_course));
  if (!c)
    return NULL;

  memset(c, 0, sizeof(*c));
  c->alloc = a;
  c->basebpm = 130;
  return c;
}

void taco_course_free_(taco_course *restrict c) {
  if (!c)
    return;

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      taco_section_free_(c->branches[i][j]);
      taco_free_(c->alloc, c->balloons[i][j].data);
    }
  }

  taco_free_(c->alloc, c);
}

void taco_course_difficulty(const taco_course *restrict course,
                            int *restrict class, int *restrict level) {
  if (class)
    *class = course->class;
  if (level)
    *level = course->level;
}

int taco_course_class(const taco_course *restrict course) {
  return course->class;
}

double taco_course_level(const taco_course *restrict course) {
  return course->level;
}

int taco_course_style(const taco_course *restrict course) {
  return course->style;
}

int taco_course_papamama(const taco_course *restrict course) {
  return course->papamama;
}

int taco_course_branched(const taco_course *restrict course) {
  return course->branched;
}

double taco_course_bpm(const taco_course *restrict course) {
  return course->basebpm;
}

double taco_course_offset(const taco_course *restrict course) {
  return course->offset;
}

int taco_course_score_base(const taco_course *restrict course) {
  return course->score_base;
}

int taco_course_score_tournament(const taco_course *restrict course) {
  return course->score_tournament;
}

int taco_course_score_bonus(const taco_course *restrict course) {
  return course->score_bonus;
}

void taco_course_set_class_(taco_course *restrict course, int class) {
  course->class = class;
}

void taco_course_set_level_(taco_course *restrict course, double level) {
  course->level = level;
}

void taco_course_set_style_(taco_course *restrict course, int style) {
  course->style = style;
}

void taco_course_set_papamama_(taco_course *restrict course, int papamama) {
  course->papamama = papamama;
}

void taco_course_set_bpm_(taco_course *restrict course, double bpm) {
  course->basebpm = bpm;
}

void taco_course_set_offset_(taco_course *restrict course, double offset) {
  course->offset = offset;
}

void taco_course_set_score_base_(taco_course *restrict course, int base) {
  course->score_base = base;
}

void taco_course_set_score_tournament_(taco_course *restrict course,
                                       int score) {
  course->score_tournament = score;
}

void taco_course_set_score_bonus_(taco_course *restrict course, int bonus) {
  course->score_bonus = bonus;
}

const taco_section *taco_course_get_branch(const taco_course *restrict course,
                                           int side, int branch) {
  if (side < 2 && branch < 3) {
    if (course->style == TACO_STYLE_SINGLE)
      side = TACO_SIDE_LEFT;
    if (!course->branched)
      branch = TACO_BRANCH_NORMAL;
    return course->branches[side][branch];
  }
  return NULL;
}

taco_section *taco_course_get_branch_mut_(taco_course *restrict course,
                                          int side, int branch) {
  if (side < 2 && branch < 3) {
    if (course->style == TACO_STYLE_SINGLE)
      side = TACO_SIDE_LEFT;
    if (!course->branched)
      branch = TACO_BRANCH_NORMAL;
    return course->branches[side][branch];
  }
  return NULL;
}

int taco_course_setup_branching_(taco_course *restrict course) {
  if (course->branched)
    return 0;

  taco_section **branches = course->branches[TACO_SIDE_LEFT];

  const taco_section *normal = branches[TACO_BRANCH_NORMAL];
  taco_section *advanced = taco_section_clone_(normal);
  taco_section *master = taco_section_clone_(normal);

  if (!advanced || !master) {
    taco_section_free_(advanced);
    taco_section_free_(master);
    return -1;
  }

  branches[TACO_BRANCH_ADVANCED] = advanced;
  branches[TACO_BRANCH_MASTER] = master;

  course->branched = true;
  return 0;
}

taco_section *taco_course_attach_branch_(taco_course *restrict course,
                                         taco_section *restrict content,
                                         int side, int branch) {
  assert((side < 2 && branch < 3));

  // attach
  taco_section **target = &(course->branches[side][branch]);
  taco_section *old = *target;
  *target = content;

  // apply bpm data
  taco_section_set_bpm_(content, course->basebpm);

  // apply balloon hitcount if stored
  balloon_data_ *balloons = &(course->balloons[side][branch]);
  if (balloons) {
    taco_section_set_balloons_(content, balloons->data, balloons->count);
    taco_free_(course->alloc, balloons->data);
    balloons->data = NULL;
    balloons->count = 0;
  }

  return old;
}

int taco_course_set_balloons_(taco_course *restrict course,
                              const int *restrict balloons, size_t count,
                              int side, int branch) {
  assert((balloons));

  // apply hitcount immediately if section is attached
  taco_section *s = course->branches[side][branch];
  if (s) {
    taco_section_set_balloons_(s, balloons, count);
    return 0;
  }

  // copy balloon hitcount array for later use
  int *b = taco_malloc_(course->alloc, count * sizeof(int));
  if (!b)
    return -1;

  memcpy(b, balloons, count * sizeof(int));
  course->balloons[side][branch].data = b;
  course->balloons[side][branch].count = count;
  return 0;
}

int taco_course_merge_(taco_course *restrict s, taco_course *restrict other) {
  int src_side;
  int dst_side;

  if (s->style != TACO_STYLE_2P_ONLY && other->style == TACO_STYLE_2P_ONLY) {
    src_side = TACO_SIDE_LEFT;
    dst_side = TACO_SIDE_RIGHT;
  } else if (s->style == TACO_STYLE_2P_ONLY &&
             other->style != TACO_STYLE_2P_ONLY) {
    src_side = TACO_SIDE_RIGHT;
    dst_side = TACO_SIDE_LEFT;
    s->style = other->style;
  } else {
    return -1;
  }

  for (int i = 0; i < 3; ++i) {
    // move data of this side
    if (src_side != TACO_SIDE_LEFT) {
      s->branches[src_side][i] = s->branches[TACO_SIDE_LEFT][i];
      memcpy(&s->balloons[src_side][i], &s->balloons[TACO_SIDE_LEFT][i],
             sizeof(balloon_data_));
    }

    // move data in from the other side course
    s->branches[dst_side][i] = other->branches[TACO_SIDE_LEFT][i];
    memcpy(&s->balloons[dst_side][i], &other->balloons[TACO_SIDE_LEFT][i],
           sizeof(balloon_data_));
    other->branches[TACO_SIDE_LEFT][i] = NULL;
    other->balloons[TACO_SIDE_LEFT][i].data = NULL;
  }

  if (!s->style)
    s->style = TACO_STYLE_COUPLE;
  taco_course_free_(other);
  return 0;
}
