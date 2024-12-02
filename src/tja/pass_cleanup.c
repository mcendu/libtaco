// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include <stdlib.h>

// Compare two events, but none events always compare greater than others.
// At the end of sorting, this should cause none events to be all moved to
// the back.
static int cmp(taco_event *a, taco_event *b) {
  int result = (a->type == TACO_EVENT_NONE) - (b->type == TACO_EVENT_NONE);
  if (result == 0)
    result = taco_event_compare(a, b);
  return result;
}

void tja_pass_cleanup_(tja_parser *parser, taco_section *branch) {
  // sort events
  qsort(taco_section_begin_mut_(branch), taco_section_size(branch),
        sizeof(taco_event), (int (*)(const void *, const void *))cmp);

  // pop none events
  int none_count = 0;
  const taco_event *i = taco_section_end(branch);
  while ((--i)->type == TACO_EVENT_NONE)
    none_count += 1;
  taco_section_pop_(branch, none_count);
}
