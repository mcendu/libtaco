// SPDX-License-Identifier: BSD-2-Clause
#include "tja/branchtype.h"

#include "taco.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BRANCHTYPE_CLASS_INTEGER 0
#define BRANCHTYPE_CLASS_PERCENTAGE 1

static const struct branchtype_info_ {
  const char *code;
  int type;
  int class;
} branchtype_info[] = {
    {"b", TAIKO_BRANCHTYPE_ACCURACY_BIG, BRANCHTYPE_CLASS_PERCENTAGE},
    {"p", TAIKO_BRANCHTYPE_ACCURACY, BRANCHTYPE_CLASS_PERCENTAGE},
    {"r", TAIKO_BRANCHTYPE_ROLL, BRANCHTYPE_CLASS_INTEGER},
};

static int cmp_branchtype_info_(const void *key, const void *entry_void) {
  const struct branchtype_info_ *entry = entry_void;
  return strcmp(key, entry->code);
}

int tja_branch_type_(const char *code) {
  const struct branchtype_info_ *entry =
      bsearch(code, branchtype_info,
              sizeof(branchtype_info) / sizeof(struct branchtype_info_),
              sizeof(struct branchtype_info_), cmp_branchtype_info_);
  return entry ? entry->type : TAIKO_BRANCHTYPE_NONE;
}

int tja_branch_type_convert_threshold_(const char *code, double value) {
  const struct branchtype_info_ *entry =
      bsearch(code, branchtype_info,
              sizeof(branchtype_info) / sizeof(struct branchtype_info_),
              sizeof(struct branchtype_info_), cmp_branchtype_info_);
  int class = entry ? entry->class : BRANCHTYPE_CLASS_INTEGER;

  switch (class) {
  default:
  case BRANCHTYPE_CLASS_INTEGER:
    return (int)value;

  case BRANCHTYPE_CLASS_PERCENTAGE: {
    double fixedpoint = ((value / 100) * exp2(24));
    return fixedpoint >= (double)INT_MAX ? INT_MAX : (int)fixedpoint;
  }
  }
}
