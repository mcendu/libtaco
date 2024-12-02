// SPDX-License-Identifier: BSD-2-Clause
#include "tja/metadata.h"

#include "taco.h"
#include <stdlib.h>
#include <string.h>

typedef struct enumerator_ {
  const char text[28];
  int integer;
} enumerator;

#define lenof_(a) (sizeof(a) / sizeof((a)[0]))

static int cmp_enumerator_(const void *restrict kv, const void *restrict vv) {
  const char *key = kv;
  const enumerator *value = vv;
  return strcmp(key, value->text);
}

static const int *lookup_enumerator_(const char *key, const enumerator *list,
                                     size_t size) {
  const enumerator *e =
      bsearch(key, list, size, sizeof(enumerator), cmp_enumerator_);
  if (!e)
    return NULL;
  return &(e->integer);
}

int tja_interpret_side_(const char *str) {
  static const enumerator side_enum[] = {
      {"Both", TACO_CLASS_EASY},   {"Ex", TACO_CLASS_EX_EASY},
      {"Normal", TACO_CLASS_EASY}, {"both", TACO_CLASS_EASY},
      {"ex", TACO_CLASS_EX_EASY},  {"normal", TACO_CLASS_EASY},
  };

  // numeric
  char *end;
  if (strtol(str, &end, 10) == 2 && *end == '\0')
    return TACO_CLASS_EX_EASY;

  // textual
  const int *e = lookup_enumerator_(str, side_enum, lenof_(side_enum));
  if (e)
    return *e;

  return TACO_CLASS_EASY;
}

int tja_interpret_course_(const char *str) {
  static const enumerator course_enum[] = {
      {"Easy", TACO_CLASS_EASY},     {"Edit", TACO_CLASS_EX},
      {"Hard", TACO_CLASS_HARD},     {"Normal", TACO_CLASS_NORMAL},
      {"Oni", TACO_CLASS_ONI},       {"easy", TACO_CLASS_EASY},
      {"edit", TACO_CLASS_EX},       {"hard", TACO_CLASS_HARD},
      {"normal", TACO_CLASS_NORMAL}, {"oni", TACO_CLASS_ONI},
  };

  static const int course_numeric[] = {
      TACO_CLASS_EASY, TACO_CLASS_NORMAL, TACO_CLASS_HARD,
      TACO_CLASS_ONI,  TACO_CLASS_EX,
  };

  // numeric
  int code;
  char *end;
  code = strtol(str, &end, 10);
  if (*end == '\0' && code < 5)
    return course_numeric[code];

  // textual
  const int *e = lookup_enumerator_(str, course_enum, lenof_(course_enum));
  if (e)
    return *e;

  return TACO_CLASS_ONI;
}

int tja_interpret_style_(const char *str) {
  // do not trust the given style, for now
  return TACO_STYLE_SINGLE;
}
