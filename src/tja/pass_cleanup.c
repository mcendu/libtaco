// SPDX-License-Identifier: BSD-2-Clause
#include "note.h"
#include "section.h"
#include "taco.h"
#include <stdlib.h>

taiko_section *tja_pass_cleanup_(taiko_section *branch) {
  // delete none events from previous passes
  taiko_section_foreach_mut_ (i, branch) {
    while (i->type == TAIKO_EVENT_NONE)
      taiko_section_delete_(branch, i);
  }

  // sort events
  qsort(taiko_section_begin_mut_(branch), taiko_section_size(branch),
        sizeof(taiko_event),
        (int (*)(const void *, const void *))taiko_event_compare);

  return branch;
}
