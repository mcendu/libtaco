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

#define STRING_PROPERTY_SETTER(prop)                                           \
  int taco_courseset_set_##prop##_(taco_courseset *restrict set_,              \
                                   const char *restrict prop) {                \
    char *buf_ = taco_strdup_(set_->alloc, prop);                              \
                                                                               \
    if (buf_) {                                                                \
      taco_free_(set_->alloc, set_->prop);                                     \
      set_->prop = buf_;                                                       \
      return 0;                                                                \
    }                                                                          \
                                                                               \
    return -1;                                                                 \
  }

#define STRING_PROPERTY_GETTER(prop, default)                                  \
  const char *taco_courseset_##prop(const taco_courseset *restrict set_) {     \
    if (!(set_->prop))                                                         \
      return (default);                                                        \
    return set_->prop;                                                         \
  }

#define STRING_PROPERTY(prop, default)                                         \
  STRING_PROPERTY_GETTER(prop, default)                                        \
  STRING_PROPERTY_SETTER(prop)

STRING_PROPERTY(title, "Untitled")

STRING_PROPERTY(subtitle, "")

STRING_PROPERTY(genre, "")

STRING_PROPERTY(maker, "")

STRING_PROPERTY(filename, "<unknown>")

STRING_PROPERTY(audio, "")

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
