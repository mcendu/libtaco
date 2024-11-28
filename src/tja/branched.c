// SPDX-License-Identifier: BSD-2-Clause
#include "tja/branched.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/segment.h"
#include "tja/timestamp.h"
#include <assert.h>
#include <stdbool.h>

static void pad_measures_(tja_branched *restrict branched, int branch_measures,
                          int branch);
static void section_pad_measures_(taiko_section *restrict section, int from,
                                  int to);

void tja_branched_assign_(tja_branched *restrict branched,
                          tja_segment *restrict segment, int branch) {
  assert((branched->branches[branch]));

  // assign
  if (taiko_section_size(branched->branches[branch]))
    taiko_section_clear_(branched->branches[branch]);
  taiko_section_concat_(branched->branches[branch], segment->segment);
  if (segment->levelhold)
    branched->levelhold |= (segment->levelhold) << branch;

  pad_measures_(branched, segment->measures, branch);
}

static void pad_measures_(tja_branched *restrict branched, int branch_measures,
                          int branch) {
  if (branched->measures == branch_measures) {
    return;
  } else if (branched->measures > branch_measures) {
    section_pad_measures_(branched->branches[branch], branch_measures,
                          branched->measures);
  } else {
    for (int b = 0; b < 3; ++b) {
      if (branch == b || !branched->branches[b])
        continue;
      section_pad_measures_(branched->branches[b], branched->measures,
                            branch_measures);
    }

    branched->measures = branch_measures;
  }
}

static void section_pad_measures_(taiko_section *restrict section, int from,
                                  int to) {
  for (int m = from; m < to; ++m) {
    taiko_event e = {
        .time = 0,
        .type = TAIKO_EVENT_MEASURE,
        .measure =
            {
                .hidden = false,
                .tja_units = 0,
            },
    };
    tja_event_set_measure_(&e, m);
    taiko_section_push_(section, &e);
  }
}
