// SPDX-License-Identifier: BSD-2-Clause
#include "tja/branched.h"

#include "section.h"
#include "taco.h"
#include "tja/segment.h"
#include <assert.h>
#include <stdbool.h>

void tja_branched_assign_(tja_branched *restrict branched,
                          tja_segment *restrict segment, int branch) {
  assert((branched->branches[branch]));

  // assign
  if (taco_section_size(branched->branches[branch]))
    taco_section_clear_(branched->branches[branch]);
  taco_section_concat_(branched->branches[branch], segment->segment);
  if (segment->levelhold)
    branched->levelhold |= (segment->levelhold) << branch;

  // the branch with the most measures sets the measure count.
  // if the branches don't have an equal number of measures, we
  // complain later in `pass_check_branches.c`.
  if (branched->measures < segment->measures)
    branched->measures = segment->measures;
}
