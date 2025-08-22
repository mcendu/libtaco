// SPDX-License-Identifier: BSD-2-Clause
#include "tja/parser.h"
#include "tja/postproc.h"

#include "section.h"
#include "taco.h"
#include "tja/timestamp.h"
#include <assert.h>

static int pass_extract_tickrate(tja_parser *parser, taco_section *branch);
static void pass_convert(taco_section *branch);
static void pass_remove_excess_factors(taco_section *branch);

static int gcd(int x, int y);
static int lcm(int x, int y);

int tja_pass_convert_time_(tja_parser *parser, taco_section *branch) {
  if (pass_extract_tickrate(parser, branch) != 0)
    return -1;

  pass_convert(branch);
  pass_remove_excess_factors(branch);
  return 0;
}

static int pass_extract_tickrate(tja_parser *parser, taco_section *branch) {
  // ticks per 4/4 measure
  int tickrate = 96;
  int dividend = 4;
  int divisor = 4;

  taco_section_foreach(i, branch) {
    if (i->type == TACO_EVENT_MEASURE && i->measure.tja_units != 0) {
      // calculate units required for 4/4 signature
      int units = lcm(i->measure.tja_units * divisor, dividend);
      // multiply tickrate with missing factors
      tickrate = lcm(tickrate, units);
    }

    if (i->type == TACO_EVENT_TJA_MEASURE_LENGTH) {
      if (divisor != 0) {
        // update measure length
        dividend = i->tja_measure_length.dividend;
        divisor = i->tja_measure_length.divisor;
      } else {
        tja_parser_diagnose_(parser, i->line, TJA_DIAG_ERROR,
                             "division by zero in #MEASURE");
        return -1;
      }
    }
  }

  taco_section_set_tickrate_(branch, tickrate);
  return 0;
}

static void pass_convert(taco_section *branch) {
  int start = 0;
  int measure_ticks = taco_section_tickrate(branch);
  int current_measure = 0;
  int units = 4;

  taco_section_foreach_mut_(i, branch) {
    // new measure
    if (tja_event_measure_(i) != current_measure) {
      start = start + measure_ticks;
      current_measure = tja_event_measure_(i);
      units = 1;
    }

    // barline
    if (i->type == TACO_EVENT_MEASURE) {
      if (i->measure.tja_units)
        units = i->measure.tja_units;
      i->measure.tja_units = 0;
    }

    int u = tja_event_unit_(i);

    if (i->type == TACO_EVENT_TJA_MEASURE_LENGTH) {
      assert((u == 0));
      measure_ticks =
          (taco_section_tickrate(branch) / i->tja_measure_length.divisor) *
          i->tja_measure_length.dividend;
      i->type = TACO_EVENT_NONE;
    }

    i->time = start + (measure_ticks / units) * u;
  }
}

static void pass_remove_excess_factors(taco_section *branch) {
  int divisor = 0;

  taco_section_foreach(i, branch) divisor =
      gcd(i->time, divisor); // impl detail: gcd(x, 0) = x

  // ensure a minimum tickrate of 96
  if (divisor == 0) {
    // there are no objects other than on time 0
    taco_section_set_tickrate_(branch, 96);
    return;
  } else if (taco_section_tickrate(branch) / divisor < 96) {
    divisor = taco_section_tickrate(branch) / lcm(divisor, 96);
  }

  if (divisor > 1) {
    taco_section_set_tickrate_(branch, taco_section_tickrate(branch) / divisor);
    taco_section_foreach_mut_(i, branch) i->time /= divisor;
  }
}

static int gcd(int x, int y) {
  while (y != 0) {
    int z = y;
    y = x % y;
    x = z;
  }

  return x;
}

static int lcm(int x, int y) { return (x * y) / gcd(x, y); }
