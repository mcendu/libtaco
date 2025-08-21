// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "tja/metadata.h"
#include "tja/parser.h"
#include "tja.tab.h"

void tja_pass_prepend_bgm_(tja_parser *parser, taco_section *branch) {
  double bpm = parser->metadata->bpm;
  taco_event e = {
    .time = 0,
    .type = TACO_EVENT_BPM,
    .line = 0,
    .detail_float = { .value = bpm },
  };
  taco_section_push_(branch, &e);
}
