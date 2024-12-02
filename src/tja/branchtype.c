// SPDX-License-Identifier: BSD-2-Clause
#include "tja/branchtype.h"

#include "taco.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const struct branchtype_info_ {
  const char *code;
  int type;
} branchtype_names[] = {
    {"b", TACO_BRANCHTYPE_ACCURACY_BIG},
    {"p", TACO_BRANCHTYPE_ACCURACY},
    {"r", TACO_BRANCHTYPE_ROLL},
};

static const int branchtype_classes_[] = {
    [TACO_BRANCHTYPE_NONE] = BRANCHTYPE_CLASS_INTEGER,
    [TACO_BRANCHTYPE_ACCURACY] = BRANCHTYPE_CLASS_PERCENTAGE,
    [TACO_BRANCHTYPE_ACCURACY_BIG] = BRANCHTYPE_CLASS_PERCENTAGE,
    [TACO_BRANCHTYPE_ROLL] = BRANCHTYPE_CLASS_INTEGER,
};

static int cmp_branchtype_info_(const void *key, const void *entry_void) {
  const struct branchtype_info_ *entry = entry_void;
  return strcmp(key, entry->code);
}

int tja_branch_type_(const char *code) {
  const struct branchtype_info_ *entry =
      bsearch(code, branchtype_names,
              sizeof(branchtype_names) / sizeof(struct branchtype_info_),
              sizeof(struct branchtype_info_), cmp_branchtype_info_);
  return entry ? entry->type : TACO_BRANCHTYPE_NONE;
}

int tja_branchtype_convert_threshold_(int type, double value) {
  int class = tja_branchtype_class(type);

  switch (class) {
  default:
  case BRANCHTYPE_CLASS_INTEGER:
    return (int)value;

  case BRANCHTYPE_CLASS_PERCENTAGE: {
    // convert percentage to a fixed-point format for internal use, with
    // 24 bits for the fractional part and 8 bits for the integer part.
    // The range of numbers representable is from -128 to about 127.9999999,
    // where 0.0 means 0% and 1.0 represents 100%.
    double fixedpoint = ((value / 100) * 0x1000000);
    return fixedpoint >= (double)INT32_MAX ? INT32_MAX : (int32_t)fixedpoint;
  }
  }
}

int tja_branchtype_class(int type) { return branchtype_classes_[type]; }
