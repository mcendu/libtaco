// SPDX-License-Identifier: BSD-2-Clause
#include "courseset.h"

#include "alloc.h"
#include "course.h"
#include "taco.h"
#include <math.h>
#include <string.h>

struct taco_courseset_ {
  taco_allocator *alloc;

  char *title;
  char *subtitle;
  char *genre;
  char *maker;

  char *filename;
  char *audio;
  double demo_time;

  taco_course *courses[8];
};

taco_courseset *taco_courseset_create_(void) {
  return taco_courseset_create2_(&taco_default_allocator_);
}

taco_courseset *taco_courseset_create2_(taco_allocator *a) {
  taco_courseset *set = taco_malloc_(a, sizeof(taco_courseset));
  if (!set)
    return NULL;

  memset(set, 0, sizeof(*set));
  set->alloc = a;
  set->demo_time = NAN;
  return set;
}

void taco_courseset_free(taco_courseset *set) {
  if (!set)
    return;

  taco_free_(set->alloc, set->title);
  taco_free_(set->alloc, set->subtitle);
  taco_free_(set->alloc, set->genre);
  taco_free_(set->alloc, set->maker);
  taco_free_(set->alloc, set->audio);
  taco_free_(set->alloc, set->filename);

  for (int i = 0; i < 8; ++i)
    taco_course_free_(set->courses[i]);

  taco_free_(set->alloc, set);
}

const char *taco_courseset_title(const taco_courseset *restrict set) {
  return set->title;
}

int taco_courseset_set_title_(taco_courseset *restrict set,
                              const char *restrict title) {
  char *buf = taco_strdup_(set->alloc, title);

  if (buf) {
    taco_free_(set->alloc, set->title);
    set->title = buf;
    return 0;
  }

  return -1;
}

const char *taco_courseset_subtitle(const taco_courseset *restrict set) {
  return set->subtitle;
}

int taco_courseset_set_subtitle_(taco_courseset *restrict set,
                                 const char *restrict subtitle) {
  char *buf = taco_strdup_(set->alloc, subtitle);

  if (buf) {
    taco_free_(set->alloc, set->subtitle);
    set->subtitle = buf;
    return 0;
  }

  return -1;
}

const char *taco_courseset_genre(const taco_courseset *restrict set) {
  return set->genre;
}

int taco_courseset_set_genre_(taco_courseset *restrict set,
                              const char *restrict genre) {
  char *buf = taco_strdup_(set->alloc, genre);

  if (buf) {
    taco_free_(set->alloc, set->genre);
    set->genre = buf;
    return 0;
  }

  return -1;
}

const char *taco_courseset_maker(const taco_courseset *restrict set) {
  return set->maker;
}

int taco_courseset_set_maker_(taco_courseset *restrict set,
                              const char *restrict maker) {
  char *buf = taco_strdup_(set->alloc, maker);

  if (buf) {
    taco_free_(set->alloc, set->maker);
    set->maker = buf;
    return 0;
  }

  return -1;
}

const char *taco_courseset_filename(const taco_courseset *restrict set) {
  if (!(set->filename))
    return "<unnamed>";
  return set->filename;
}

int taco_courseset_set_filename_(taco_courseset *restrict set,
                                 const char *restrict filename) {
  char *buf = taco_strdup_(set->alloc, filename);

  if (buf) {
    taco_free_(set->alloc, set->filename);
    set->filename = buf;
    return 0;
  }

  return -1;
}

const char *taco_courseset_audio(const taco_courseset *restrict set) {
  return set->audio;
}

int taco_courseset_set_audio_(taco_courseset *restrict set,
                              const char *restrict path) {
  char *buf = taco_strdup_(set->alloc, path);

  if (buf) {
    taco_free_(set->alloc, set->audio);
    set->audio = buf;
    return 0;
  }

  return -1;
}

double taco_courseset_demo_time(const taco_courseset *restrict set) {
  return set->demo_time;
}

extern void taco_courseset_set_demo_time_(taco_courseset *restrict set,
                                          double time) {
  set->demo_time = time;
}

const taco_course *taco_courseset_get_course(const taco_courseset *restrict set,
                                             int class) {
  if (class >= 8)
    return NULL;
  return set->courses[class];
}

taco_course *taco_courseset_get_course_mut_(taco_courseset *restrict set,
                                            int class) {
  if (class >= 8)
    return NULL;
  return set->courses[class];
}

int taco_courseset_add_course_(taco_courseset *restrict set,
                               taco_course *restrict course) {
  int class = taco_course_class(course);

  if (set->courses[class]) {
    // attempt merger into double/couple course
    return taco_course_merge_(set->courses[class], course);
  }

  set->courses[class] = course;
  return 0;
}

void taco_courseset_delete_course_(taco_courseset *restrict set, int class) {
  taco_course_free_(set->courses[class]);
  set->courses[class] = NULL;
}
