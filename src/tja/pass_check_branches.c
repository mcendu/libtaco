// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "section.h"
#include "taco.h"
#include "tja.tab.h"
#include "tja/parser.h"

static const char *branch_names_[] = {
    [TACO_BRANCH_NORMAL] = "normal",
    [TACO_BRANCH_ADVANCED] = "advanced",
};

int tja_pass_check_branches_(tja_parser *parser, taco_course *course) {
  if (!taco_course_branched(course))
    return 0;

  taco_section *timing = taco_section_create2_(parser->alloc);

  // extract a list of timing events (namely BPM and real measures) from master
  // branch
  const taco_section *master =
      taco_course_get_branch(course, TACO_SIDE_LEFT, TACO_BRANCH_MASTER);

  taco_section_foreach(i, master) {
    if (taco_event_type(i) == TACO_EVENT_BPM)
      taco_section_push_(timing, i);
    else if (taco_event_type(i) == TACO_EVENT_DELAY)
      taco_section_push_(timing, i);
    else if (taco_event_type(i) == TACO_EVENT_MEASURE && i->measure.real)
      taco_section_push_(timing, i);
  }

  // check other branches for inconsistencies
  int error = 0;

  for (int b = TACO_BRANCH_NORMAL; b <= TACO_BRANCH_ADVANCED; ++b) {
    const taco_section *s = taco_course_get_branch(course, TACO_SIDE_LEFT, b);
    const taco_event *j = taco_section_begin(timing);

    taco_section_foreach(i, s) {
      switch (taco_event_type(i)) {
      case TACO_EVENT_BPM:
      case TACO_EVENT_DELAY:
        if (j == taco_section_end(timing) || i->time != j->time ||
            i->type != j->type ||
            i->detail_float.value != j->detail_float.value) {
          tja_parser_diagnose_(parser, i->line, TJA_DIAG_ERROR,
                               "timing of branch %s diverges from master",
                               branch_names_[b]);
          if (j != taco_section_end(timing))
            tja_parser_diagnose_(parser, j->line, TJA_DIAG_NOTE,
                                 "last timing event in master here");
          error = -1;
          goto check_loop_end;
        }
        j = taco_event_next(j);
        break;
      case TACO_EVENT_MEASURE:
        if (j == taco_section_end(timing) || i->time != j->time ||
            i->type != j->type || !i->measure.real) {
          tja_parser_diagnose_(parser, i->line, TJA_DIAG_ERROR,
                               "timing of branch %s diverges from master",
                               branch_names_[b]);
          if (j != taco_section_end(timing))
            tja_parser_diagnose_(parser, j->line, TJA_DIAG_NOTE,
                                 "last timing event in master here");
          error = -1;
          goto check_loop_end;
        }
        j = taco_event_next(j);
        break;
      }
    }
  check_loop_end:
    if (j != taco_section_end(timing)) {
      tja_parser_diagnose_(parser, j->line, TJA_DIAG_ERROR,
                           "timing of branch master diverges from %s",
                           branch_names_[b]);
      error = -1;
    }
  }

  taco_section_free_(timing);
  return error;
}
