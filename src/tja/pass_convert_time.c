// SPDX-License-Identifier: BSD-2-Clause
#include "section.h"
#include "taiko.h"
#include "tja/timestamp.h"
#include <assert.h>

static taiko_section *pass_extract_tickrate(taiko_section *branch);
static taiko_section *pass_convert(taiko_section *branch);
static taiko_section *pass_remove_excess_factors(taiko_section *branch);

static int gcd(int x, int y);
static int lcm(int x, int y);

taiko_section *tja_pass_convert_time_(taiko_section *branch) {
  branch = pass_extract_tickrate(branch);
  branch = pass_convert(branch);
  branch = pass_remove_excess_factors(branch);
  return branch;
}

taiko_section *pass_extract_tickrate(taiko_section *branch) {
  // ticks per 4/4 measure
  int tickrate = 96;
  int dividend = 4;
  int divisor = 4;

  taiko_section_foreach (i, branch) {
    if (i->type == TAIKO_EVENT_MEASURE) {
      // calculate units required for 4/4 signature
      int units = lcm(i->measure.tja_units * divisor, dividend);
      // multiply tickrate with missing factors
      tickrate = lcm(tickrate, units);
    }

    if (i->type == TAIKO_EVENT_TJA_MEASURE_LENGTH) {
      dividend = i->tja_measure_length.dividend;
      divisor = i->tja_measure_length.divisor;
    }
  }

  taiko_section_set_tickrate_(branch, tickrate);
  return branch;
}

static taiko_section *pass_convert(taiko_section *branch) {
  int start = 0;
  int measure_ticks = taiko_section_tickrate(branch);
  int current_measure = 0;
  int units = 4;

  taiko_section_foreach_mut_ (i, branch) {
    if (tja_event_measure_(i) != current_measure) {
      start = start + measure_ticks;
      current_measure = tja_event_measure_(i);
      units = 1;
    }

    if (i->type == TAIKO_EVENT_MEASURE) {
      if (i->measure.tja_units)
        units = i->measure.tja_units;
      i->measure.tja_units = 0;
    }

    int u = tja_event_unit_(i);

    if (i->type == TAIKO_EVENT_TJA_MEASURE_LENGTH) {
      assert((u == 0));
      measure_ticks = (measure_ticks / i->tja_measure_length.divisor) *
                      i->tja_measure_length.dividend;
      i->type = TAIKO_EVENT_NONE;
    }

    i->time = start + (measure_ticks / units) * u;
  }

  return branch;
}

taiko_section *pass_remove_excess_factors(taiko_section *branch) {
  int divisor = 0;

  taiko_section_foreach (i, branch)
    divisor = gcd(i->time, divisor);

  if (divisor > 1) {
    taiko_section_set_tickrate_(branch,
                                taiko_section_tickrate(branch) / divisor);
    taiko_section_foreach_mut_ (i, branch)
      i->time /= divisor;
  }

  return branch;
}

int gcd(int x, int y) {
  while (y != 0) {
    x = y;
    y = x % y;
  }

  return x;
}

int lcm(int x, int y) { return (x * y) / gcd(x, y); }
