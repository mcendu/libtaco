// SPDX-License-Identifier: BSD-2-Clause
#include "note.h"

#include "taco.h"
#include <math.h>

int taco_event_time(const taco_event *event) { return event->time; }

int taco_event_type(const taco_event *event) { return event->type; }

int taco_event_detail_int(const taco_event *event) {
  switch (event->type) {
  case TACO_EVENT_DON:
  case TACO_EVENT_DON_BIG:
  case TACO_EVENT_KAT:
  case TACO_EVENT_KAT_BIG:
  case TACO_EVENT_BALLOON:
  case TACO_EVENT_KUSUDAMA:
  case TACO_EVENT_LANDMINE:
    return event->detail_int.value;
  default:
    return -1;
  }
}

double taco_event_detail_float(const taco_event *event) {
  switch (event->type) {
  case TACO_EVENT_BPM:
    return event->detail_float.value;
  default:
    return NAN;
  }
}

int taco_event_is_note(const taco_event *restrict event) {
  return event->type > 0;
}

int taco_event_is_normal_note(const taco_event *restrict event) {
  switch (event->type) {
  case TACO_EVENT_DON:
  case TACO_EVENT_DON_BIG:
  case TACO_EVENT_KAT:
  case TACO_EVENT_KAT_BIG:
    return 1;
  default:
    return 0;
  }
}

int taco_event_is_roll(const taco_event *restrict event) {
  switch (event->type) {
  case TACO_EVENT_ROLL:
  case TACO_EVENT_ROLL_BIG:
  case TACO_EVENT_BALLOON:
  case TACO_EVENT_KUSUDAMA:
    return 1;
  default:
    return 0;
  }
}

int taco_event_compare(const taco_event *a, const taco_event *b) {
  int result = (a->time > b->time) - (a->time < b->time);

  if (result == 0)
    result = (a->type > b->type) - (a->type < b->type);

  return result;
}

const taco_event *taco_event_next(const taco_event *event) { return event + 1; }

taco_event *taco_event_next_mut_(taco_event *event) { return event + 1; }

const taco_event *taco_event_prev(const taco_event *event) { return event - 1; }

taco_event *taco_event_prev_mut_(taco_event *event) { return event - 1; }
