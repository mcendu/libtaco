// SPDX-License-Identifier: BSD-2-Clause
#include "tja/segment.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/events.h"
#include "tja/timestamp.h"
#include <stddef.h>

int tja_segment_init_(tja_segment *segment, taco_section *buf) {
  segment->segment = buf;
  segment->measures = 0;
  segment->levelhold = false;
  return buf != NULL;
}

int tja_segment_push_barline_(tja_segment *segment, int units, int line) {
  int retval = 0;
  taco_event barline = {
      .time = 0,
      .type = TACO_EVENT_MEASURE,
      .line = line,
      .measure =
          {
              .real = true,
              .hidden = false,
              .tja_units = units,
          },
  };
  tja_event_set_measure_(&barline, segment->measures);

  retval = taco_section_push_(segment->segment, &barline);
  return retval;
}

void tja_segment_finish_measure_(tja_segment *segment) {
  segment->measures += 1;
}

int tja_segment_push_events_(tja_segment *segment, tja_events *events) {
  taco_section_foreach_mut_(i, events->events) {
    tja_event_set_measure_(i, segment->measures);
  }
  segment->levelhold = segment->levelhold || events->levelhold;
  return taco_section_concat_(segment->segment, events->events);
}
