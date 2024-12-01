// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include <stdlib.h>

// Compare two events, but none events always compare greater than others.
// At the end of sorting, this should cause none events to be all moved to
// the back.
static int cmp(taiko_event *a, taiko_event *b) {
  int result = (a->type == TAIKO_EVENT_NONE) - (b->type == TAIKO_EVENT_NONE);
  if (result == 0)
    result = taiko_event_compare(a, b);
  return result;
}

void tja_pass_cleanup_(tja_parser *parser, taiko_section *branch) {
  // sort events
  qsort(taiko_section_begin_mut_(branch), taiko_section_size(branch),
        sizeof(taiko_event), (int (*)(const void *, const void *))cmp);

  // pop none events
  int none_count = 0;
  const taiko_event *i = taiko_section_end(branch);
  while ((--i)->type == TAIKO_EVENT_NONE)
    none_count += 1;
  taiko_section_pop_(branch, none_count);
}
