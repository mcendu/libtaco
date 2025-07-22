// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"

void tja_pass_annotate_(tja_parser *parser, taco_section *branch) {
  taco_section_foreach_mut_(i, branch) {
    switch (i->type) {
    case TACO_EVENT_DON:
      i->detail_int.value = TACO_TEXT_DO;
      break;
    case TACO_EVENT_KAT:
      i->detail_int.value = TACO_TEXT_KA;
      break;
    }
  }
}
