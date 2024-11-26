// SPDX-License-Identifier: BSD-2-Clause
#include "course.h"

#include "alloc.h"
#include "section.h"
#include "taiko.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

typedef struct balloon_data_ balloon_data_;

struct balloon_data_ {
  int *data;
  size_t count;
};

struct taiko_course_ {
  taiko_allocator *alloc;

  double basebpm;
  double offset;
  double level;

  int class;
  int style;
  int scrollmode;

  bool papamama : 1;
  bool branched : 1;

  taiko_section *branches[2][3];
  balloon_data_ balloons[2][3];
};

taiko_course *taiko_course_create_(void) {
  return taiko_course_create2_(&taiko_default_allocator_);
}

taiko_course *taiko_course_create2_(taiko_allocator *a) {
  taiko_course *c = taiko_malloc_(a, sizeof(taiko_course));
  if (!c)
    return NULL;

  memset(c, 0, sizeof(*c));
  c->alloc = a;
  c->basebpm = 130;
  return c;
}

void taiko_course_free_(taiko_course *restrict c) {
  if (!c)
    return;

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      taiko_section_free_(c->branches[i][j]);
      taiko_free_(c->alloc, c->balloons[i][j].data);
    }
  }

  taiko_free_(c->alloc, c);
}

extern void taiko_course_difficulty(const taiko_course *restrict course,
                                    int *restrict class, int *restrict level) {
  if (class)
    *class = course->class;
  if (level)
    *level = course->level;
}

int taiko_course_class(const taiko_course *restrict course) {
  return course->class;
}

double taiko_course_level(const taiko_course *restrict course) {
  return course->level;
}

int taiko_course_style(const taiko_course *restrict course) {
  return course->style;
}

int taiko_course_papamama(const taiko_course *restrict course) {
  return course->papamama;
}

int taiko_course_branched(const taiko_course *restrict course) {
  return course->branched;
}

double taiko_course_bpm(const taiko_course *restrict course) {
  return course->basebpm;
}

double taiko_course_offset(const taiko_course *restrict course) {
  return course->offset;
}

void taiko_course_set_class_(taiko_course *restrict course, int class) {
  course->class = class;
}

void taiko_course_set_level_(taiko_course *restrict course, double level) {
  course->level = level;
}

void taiko_course_set_style_(taiko_course *restrict course, int style) {
  course->style = style;
}

void taiko_course_set_papamama_(taiko_course *restrict course, int papamama) {
  course->papamama = papamama;
}

void taiko_course_set_bpm_(taiko_course *restrict course, double bpm) {
  course->basebpm = bpm;
}

void taiko_course_set_offset_(taiko_course *restrict course, double offset) {
  course->offset = offset;
}

const taiko_section *
taiko_course_get_branch(const taiko_course *restrict course, int side,
                        int branch) {
  if (side < 2 && branch < 3) {
    if (course->style == TAIKO_STYLE_SINGLE)
      side = TAIKO_SIDE_LEFT;
    if (!course->branched)
      branch = TAIKO_BRANCH_NORMAL;
    return course->branches[side][branch];
  }
  return NULL;
}

taiko_section *taiko_course_get_branch_mut_(taiko_course *restrict course,
                                            int side, int branch) {
  if (side < 2 && branch < 3) {
    if (course->style == TAIKO_STYLE_SINGLE)
      side = TAIKO_SIDE_LEFT;
    if (!course->branched)
      branch = TAIKO_BRANCH_NORMAL;
    return course->branches[side][branch];
  }
  return NULL;
}

int taiko_course_setup_branching_(taiko_course *restrict course) {
  if (course->branched)
    return 0;

  taiko_section **branches = course->branches[TAIKO_SIDE_LEFT];

  const taiko_section *normal = branches[TAIKO_BRANCH_NORMAL];
  taiko_section *advanced = taiko_section_clone_(normal);
  taiko_section *master = taiko_section_clone_(normal);

  if (!advanced || !master) {
    taiko_section_free_(advanced);
    taiko_section_free_(master);
    return -1;
  }

  branches[TAIKO_BRANCH_ADVANCED] = advanced;
  branches[TAIKO_BRANCH_MASTER] = master;

  course->branched = true;
  return 0;
}

taiko_section *taiko_course_attach_branch_(taiko_course *restrict course,
                                           taiko_section *restrict content,
                                           int side, int branch) {
  assert((side < 2 && branch < 3));

  // attach
  taiko_section **target = &(course->branches[side][branch]);
  taiko_section *old = *target;
  *target = content;

  // apply balloon hitcount if stored
  balloon_data_ *balloons = &(course->balloons[side][branch]);
  if (balloons) {
    taiko_section_set_balloons_(content, balloons->data, balloons->count);
    taiko_free_(course->alloc, balloons->data);
    balloons->data = NULL;
    balloons->count = 0;
  }

  return old;
}

int taiko_course_set_balloons_(taiko_course *restrict course,
                               const int balloons[restrict], size_t count,
                               int side, int branch) {
  assert((balloons));

  // apply hitcount immediately if section is attached
  taiko_section *s = course->branches[side][branch];
  if (s) {
    taiko_section_set_balloons_(s, balloons, count);
    return 0;
  }

  // copy balloon hitcount array for later use
  int *b = taiko_malloc_(course->alloc, count * sizeof(int));
  if (!b)
    return -1;

  memcpy(b, balloons, count * sizeof(int));
  course->balloons[side][branch].data = b;
  course->balloons[side][branch].count = count;
  return 0;
}

int taiko_course_merge_(taiko_course *restrict s,
                        taiko_course *restrict other) {
  int src_side;
  int dst_side;

  if (s->style == TAIKO_STYLE_TJA_L_ && other->style == TAIKO_STYLE_TJA_R_) {
    src_side = TAIKO_SIDE_LEFT;
    dst_side = TAIKO_SIDE_RIGHT;
  } else if (s->style == TAIKO_STYLE_TJA_R_ &&
             other->style == TAIKO_STYLE_TJA_L_) {
    src_side = TAIKO_SIDE_RIGHT;
    dst_side = TAIKO_SIDE_LEFT;
  } else {
    return -1;
  }

  for (int i = 0; i < 3; ++i) {
    // move data of this side
    if (src_side != TAIKO_SIDE_LEFT) {
      s->branches[src_side][i] = s->branches[TAIKO_SIDE_LEFT][i];
      memcpy(&s->balloons[src_side][i], &s->balloons[TAIKO_SIDE_LEFT][i],
             sizeof(balloon_data_));
    }

    // move data in from the other side course
    s->branches[dst_side][i] = other->branches[TAIKO_SIDE_LEFT][i];
    memcpy(&s->balloons[dst_side][i], &other->balloons[TAIKO_SIDE_LEFT][i],
           sizeof(balloon_data_));
    other->branches[TAIKO_SIDE_LEFT][i] = NULL;
    other->balloons[TAIKO_SIDE_LEFT][i].data = NULL;
  }

  s->style = TAIKO_STYLE_COUPLE;
  taiko_course_free_(other);
  return 0;
}
