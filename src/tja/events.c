// SPDX-License-Identifier: BSD-2-Clause
#include "tja/events.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include "tja/timestamp.h"
#include <string.h>

static const taco_event note_types_[128] = {
    ['0'] = {.type = TACO_EVENT_NONE},
    ['1'] = {.type = TACO_EVENT_DON},
    ['2'] = {.type = TACO_EVENT_KAT},
    ['3'] = {.type = TACO_EVENT_DON_BIG},
    ['4'] = {.type = TACO_EVENT_KAT_BIG},
    ['5'] = {.type = TACO_EVENT_ROLL},
    ['6'] = {.type = TACO_EVENT_ROLL_BIG},
    ['7'] = {.type = TACO_EVENT_BALLOON},
    ['8'] = {.type = TACO_EVENT_ROLL_END},
    ['9'] = {.type = TACO_EVENT_KUSUDAMA},
    ['A'] = {.type = TACO_EVENT_DON_BIG,
             .detail_int = {.value = TACO_DETAIL_HAND}},
    ['B'] = {.type = TACO_EVENT_KAT_BIG,
             .detail_int = {.value = TACO_DETAIL_HAND}},
    ['C'] = {.type = TACO_EVENT_LANDMINE},
};

int tja_events_push_note_(tja_events *restrict events, int note, int line) {
  taco_event e = note_types_[note >= 0 && note < 128 ? note : 0];
  e.line = line;

  int result = tja_events_push_event_(events, &e);
  events->units += 1;
  return result;
}

int tja_events_push_event_(tja_events *restrict events,
                           const taco_event *restrict event) {
  if (event->type == TACO_EVENT_NONE) {
    return 0;
  } else if (event->type == TACO_EVENT_TJA_LEVELHOLD) {
    events->levelhold = true;
    return 0;
  } else if (events->units == 0 && event->type == TACO_EVENT_MEASURE) {
    // no duplicate barlines
    return 0;
  }

  taco_event e;
  memcpy(&e, event, sizeof(e));
  tja_event_set_timestamp_(&e, 0, events->units);
  return taco_section_push_(events->events, &e);
}
