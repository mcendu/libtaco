// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"

extern void tja_pass_checkpoint_rolls_(tja_parser *parser,
                                       taiko_section *branch) {
  taiko_event *head = NULL;

  taiko_section_foreach_mut_ (i, branch) {
    if (taiko_event_type(i) <= 0)
      continue;

    if (head) {
      switch (taiko_event_type(i)) {
      case TAIKO_EVENT_ROLL:
      case TAIKO_EVENT_ROLL_BIG:
      case TAIKO_EVENT_BALLOON:
        i->type = TAIKO_EVENT_NONE;
        break;
      case TAIKO_EVENT_ROLL_CHECKPOINT:
      case TAIKO_EVENT_KUSUDAMA:
        i->type = TAIKO_EVENT_ROLL_CHECKPOINT;
        break;
      case TAIKO_EVENT_ROLL_END:
        head = NULL;
        break;
      default:
        // warn and delete the drum roll head
        tja_parser_diagnose_(parser, head->line, TJA_DIAG_WARN,
                             "drum roll does not terminate");
        head->type = TAIKO_EVENT_NONE;
        head = NULL;
        break;
      }
    } else {
      switch (taiko_event_type(i)) {
      case TAIKO_EVENT_ROLL:
      case TAIKO_EVENT_ROLL_BIG:
      case TAIKO_EVENT_BALLOON:
      case TAIKO_EVENT_KUSUDAMA:
        head = i;
        break;
      }
    }
  }
}
