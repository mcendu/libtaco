// SPDX-License-Identifier: BSD-2-Clause
#include "tja/coursebody.h"

#include "course.h"
#include "note.h"
#include "section.h"
#include "taiko.h"
#include "tja/segment.h"
#include "tja/timestamp.h"

int tja_coursebody_init_(tja_coursebody *restrict c, taiko_allocator *a) {
  taiko_course *course = taiko_course_create2_(a);
  taiko_section *default_branch = taiko_section_create2_(a);

  if (!course || !default_branch) {
    taiko_course_free_(course);
    taiko_section_free_(default_branch);
    return -1;
  }

  c->course = course;
  c->measures = 0;
  c->levelhold = 0;
  taiko_course_attach_branch_(course, default_branch, TAIKO_SIDE_LEFT,
                              TAIKO_BRANCH_NORMAL);
  return 0;
}

int tja_coursebody_append_common_(tja_coursebody *restrict c,
                                  tja_segment *restrict common) {
  taiko_section_foreach_mut_ (i, common->segment) {
    tja_event_set_measure_(i, tja_event_measure_(i) + c->measures);
  }
  c->measures += common->measures;

  if (common->levelhold) {
    c->levelhold = ~0;
  }

  if (!taiko_course_branched(c->course)) {
    taiko_section_concat_(taiko_course_get_branch_mut_(
                              c->course, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL),
                          common->segment);
  } else {
    for (int b = TAIKO_BRANCH_NORMAL; b <= TAIKO_BRANCH_MASTER; ++b) {
      taiko_section_concat_(
          taiko_course_get_branch_mut_(c->course, TAIKO_SIDE_LEFT, b),
          common->segment);
    }
  }

  return 0;
}

int tja_coursebody_append_branched_(tja_coursebody *restrict c,
                                    tja_branched *restrict branched) {
  taiko_course_setup_branching_(c->course);

  for (int b = TAIKO_BRANCH_NORMAL; b <= TAIKO_BRANCH_MASTER; ++b) {
    taiko_section *section = branched->branches[b];
    taiko_section_foreach_mut_ (i, section) {
      tja_event_set_measure_(i, tja_event_measure_(i) + c->measures);
    }

    taiko_section *branch =
        taiko_course_get_branch_mut_(c->course, TAIKO_SIDE_LEFT, b);

    if (!(c->levelhold & (1 << b))) {
      taiko_event condition[2] = {
          {
              .time = 0,
              .type = TAIKO_EVENT_TJA_BRANCH_TYPE,
              .detail_int = {branched->branchtype},
          },
          {
              .time = 0,
              .type = TAIKO_EVENT_TJA_BRANCH_THRESHOLD,
              .branch_cond =
                  {
                      .advanced = branched->threshold_advanced,
                      .master = branched->threshold_master,
                  },
          },
      };
      tja_event_set_measure_(&condition[0], c->measures);
      condition[1].time = condition[0].time;

      taiko_section_push_many_(branch, condition, 2);
    }

    taiko_section_concat_(branch, section);
  }

  c->measures += branched->measures;
  return 0;
}
