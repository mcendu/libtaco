// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"

int tja_pass_checkpoint_rolls_(tja_parser *parser, taco_section *branch) {
  taco_event *head = NULL;

  taco_section_foreach_mut_(i, branch) {
    if (taco_event_type(i) <= 0)
      continue;

    if (head) {
      switch (taco_event_type(i)) {
      case TACO_EVENT_ROLL:
      case TACO_EVENT_ROLL_BIG:
      case TACO_EVENT_BALLOON:
      case TACO_EVENT_LANDMINE_ROLL:
      case TACO_EVENT_DONROLL:
      case TACO_EVENT_KATROLL:
        i->type = TACO_EVENT_NONE;
        break;
      case TACO_EVENT_ROLL_CHECKPOINT:
      case TACO_EVENT_KUSUDAMA:
        i->type = TACO_EVENT_ROLL_CHECKPOINT;
        break;
      case TACO_EVENT_ROLL_END:
        head = NULL;
        break;
      default:
        // warn and delete the drum roll head
        tja_parser_diagnose_(parser, head->line, TJA_DIAG_WARN,
                             "drum roll does not terminate");
        head->type = TACO_EVENT_NONE;
        head = NULL;
        break;
      }
    } else {
      switch (taco_event_type(i)) {
      case TACO_EVENT_ROLL:
      case TACO_EVENT_ROLL_BIG:
      case TACO_EVENT_BALLOON:
      case TACO_EVENT_KUSUDAMA:
      case TACO_EVENT_LANDMINE_ROLL:
      case TACO_EVENT_DONROLL:
      case TACO_EVENT_KATROLL:
        head = i;
        break;
      }
    }
  }

  return 0;
}
