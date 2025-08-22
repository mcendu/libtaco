// SPDX-License-Identifier: BSD-2-Clause
#include "tja/coursebody.h"

#include "course.h"
#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include "tja/segment.h"
#include "tja/timestamp.h"

int tja_coursebody_init_(tja_coursebody *restrict c, taco_allocator *a) {
  taco_course *course = taco_course_create2_(a);
  taco_section *default_branch = taco_section_create2_(a);

  if (!course || !default_branch) {
    taco_course_free_(course);
    taco_section_free_(default_branch);
    return -1;
  }

  c->course = course;
  c->measures = 0;
  c->levelhold = 0;
  taco_course_attach_branch_(course, default_branch, TACO_SIDE_LEFT,
                             TACO_BRANCH_NORMAL);
  return 0;
}

int tja_coursebody_append_common_(tja_coursebody *restrict c,
                                  tja_segment *restrict common) {
  taco_section_foreach_mut_(i, common->segment) {
    tja_event_set_measure_(i, tja_event_measure_(i) + c->measures);
  }
  c->measures += common->measures;

  if (common->levelhold) {
    c->levelhold = ~0;
  }

  if (!taco_course_branched(c->course)) {
    taco_section_concat_(taco_course_get_branch_mut_(c->course, TACO_SIDE_LEFT,
                                                     TACO_BRANCH_NORMAL),
                         common->segment);
  } else {
    for (int b = TACO_BRANCH_NORMAL; b <= TACO_BRANCH_MASTER; ++b) {
      taco_section_concat_(
          taco_course_get_branch_mut_(c->course, TACO_SIDE_LEFT, b),
          common->segment);
    }
  }

  return 0;
}

int tja_coursebody_append_branched_(tja_coursebody *restrict c,
                                    tja_branched *restrict branched) {
  taco_course_setup_branching_(c->course);

  for (int b = TACO_BRANCH_NORMAL; b <= TACO_BRANCH_MASTER; ++b) {
    taco_section *section = branched->branches[b];
    taco_section_foreach_mut_(i, section) {
      tja_event_set_measure_(i, tja_event_measure_(i) + c->measures);
    }

    taco_section *branch =
        taco_course_get_branch_mut_(c->course, TACO_SIDE_LEFT, b);

    if (!(c->levelhold & (1 << b))) {
      taco_event condition[2] = {
          {
              .time = 0,
              .type = TACO_EVENT_TJA_BRANCH_TYPE,
              .line = branched->line,
              .detail_int = {branched->branchtype},
          },
          {
              .time = 0,
              .type = TACO_EVENT_TJA_BRANCH_THRESHOLD,
              .line = branched->line,
              .branch_cond =
                  {
                      .advanced = branched->threshold_advanced,
                      .master = branched->threshold_master,
                  },
          },
      };
      tja_event_set_measure_(&condition[0], c->measures);
      condition[1].time = condition[0].time;

      taco_section_push_many_(branch, condition, 2);
    }

    taco_section_concat_(branch, section);
  }

  c->measures += branched->measures;
  return 0;
}
