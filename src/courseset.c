// SPDX-License-Identifier: BSD-2-Clause
#include "courseset.h"

#include "alloc.h"
#include "course.h"
#include "taco.h"
#include <math.h>
#include <string.h>

struct taiko_courseset_ {
  taiko_allocator *alloc;

  char *title;
  char *subtitle;
  char *genre;
  char *maker;

  char *filename;
  char *audio;
  double demo_time;

  taiko_course *courses[8];
};

taiko_courseset *taiko_courseset_create_(void) {
  return taiko_courseset_create2_(&taiko_default_allocator_);
}

taiko_courseset *taiko_courseset_create2_(taiko_allocator *a) {
  taiko_courseset *set = taiko_malloc_(a, sizeof(taiko_courseset));
  if (!set)
    return NULL;

  memset(set, 0, sizeof(*set));
  set->alloc = a;
  set->demo_time = NAN;
  return set;
}

void taiko_courseset_free(taiko_courseset *set) {
  if (!set)
    return;

  taiko_free_(set->alloc, set->title);
  taiko_free_(set->alloc, set->subtitle);
  taiko_free_(set->alloc, set->genre);
  taiko_free_(set->alloc, set->maker);
  taiko_free_(set->alloc, set->audio);
  taiko_free_(set->alloc, set->filename);

  for (int i = 0; i < 8; ++i)
    taiko_course_free_(set->courses[i]);

  taiko_free_(set->alloc, set);
}

const char *taiko_courseset_title(const taiko_courseset *restrict set) {
  return set->title;
}

int taiko_courseset_set_title_(taiko_courseset *restrict set,
                               const char *restrict title) {
  char *buf = taiko_strdup_(set->alloc, title);

  if (buf) {
    taiko_free_(set->alloc, set->title);
    set->title = buf;
    return 0;
  }

  return -1;
}

const char *taiko_courseset_subtitle(const taiko_courseset *restrict set) {
  return set->subtitle;
}

int taiko_courseset_set_subtitle_(taiko_courseset *restrict set,
                                  const char *restrict subtitle) {
  char *buf = taiko_strdup_(set->alloc, subtitle);

  if (buf) {
    taiko_free_(set->alloc, set->subtitle);
    set->subtitle = buf;
    return 0;
  }

  return -1;
}

const char *taiko_courseset_genre(const taiko_courseset *restrict set) {
  return set->genre;
}

int taiko_courseset_set_genre_(taiko_courseset *restrict set,
                               const char *restrict genre) {
  char *buf = taiko_strdup_(set->alloc, genre);

  if (buf) {
    taiko_free_(set->alloc, set->genre);
    set->genre = buf;
    return 0;
  }

  return -1;
}

const char *taiko_courseset_maker(const taiko_courseset *restrict set) {
  return set->maker;
}

int taiko_courseset_set_maker_(taiko_courseset *restrict set,
                               const char *restrict maker) {
  char *buf = taiko_strdup_(set->alloc, maker);

  if (buf) {
    taiko_free_(set->alloc, set->maker);
    set->maker = buf;
    return 0;
  }

  return -1;
}

const char *taiko_courseset_filename(const taiko_courseset *restrict set) {
  if (!(set->filename))
    return "<unnamed>";
  return set->filename;
}

int taiko_courseset_set_filename_(taiko_courseset *restrict set,
                                  const char *restrict filename) {
  char *buf = taiko_strdup_(set->alloc, filename);

  if (buf) {
    taiko_free_(set->alloc, set->filename);
    set->filename = buf;
    return 0;
  }

  return -1;
}

const char *taiko_courseset_audio(const taiko_courseset *restrict set) {
  return set->audio;
}

int taiko_courseset_set_audio_(taiko_courseset *restrict set,
                               const char *restrict path) {
  char *buf = taiko_strdup_(set->alloc, path);

  if (buf) {
    taiko_free_(set->alloc, set->audio);
    set->audio = buf;
    return 0;
  }

  return -1;
}

double taiko_courseset_demo_time(const taiko_courseset *restrict set) {
  return set->demo_time;
}

extern void taiko_courseset_set_demo_time_(taiko_courseset *restrict set,
                                           double time) {
  set->demo_time = time;
}

const taiko_course *
taiko_courseset_get_course(const taiko_courseset *restrict set, int class) {
  if (class >= 8)
    return NULL;
  return set->courses[class];
}

taiko_course *taiko_courseset_get_course_mut_(taiko_courseset *restrict set,
                                              int class) {
  if (class >= 8)
    return NULL;
  return set->courses[class];
}

int taiko_courseset_add_course_(taiko_courseset *restrict set,
                                taiko_course *restrict course) {
  int class = taiko_course_class(course);

  if (set->courses[class]) {
    // attempt merger into double/couple course
    return taiko_course_merge_(set->courses[class], course);
  }

  set->courses[class] = course;
  return 0;
}

void taiko_courseset_delete_course_(taiko_courseset *restrict set, int class) {
  taiko_course_free_(set->courses[class]);
  set->courses[class] = NULL;
}
