// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include <stdbool.h>

int tja_pass_barlines_(tja_parser *parser, taco_section *branch) {
  bool barline_on = true;

  taco_section_foreach_mut_(i, branch) {
    switch (i->type) {
    case TACO_EVENT_TJA_BARLINEOFF:
      barline_on = false;
      i->type = TACO_EVENT_NONE;
      break;
    case TACO_EVENT_TJA_BARLINEON:
      barline_on = true;
      i->type = TACO_EVENT_NONE;
      break;
    case TACO_EVENT_MEASURE:
      i->measure.hidden = !barline_on;
      break;
    }
  }

  return 0;
}
