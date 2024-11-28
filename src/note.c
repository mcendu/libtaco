// SPDX-License-Identifier: BSD-2-Clause
#include "note.h"

#include "taco.h"
#include <math.h>

int taiko_event_time(const taiko_event *event) { return event->time; }

int taiko_event_type(const taiko_event *event) { return event->type; }

int taiko_event_detail_int(const taiko_event *event) {
  switch (event->type) {
  case TAIKO_EVENT_DON:
  case TAIKO_EVENT_DON_BIG:
  case TAIKO_EVENT_KAT:
  case TAIKO_EVENT_KAT_BIG:
  case TAIKO_EVENT_BALLOON:
  case TAIKO_EVENT_KUSUDAMA:
  case TAIKO_EVENT_LANDMINE:
    return event->detail_int.value;
  default:
    return -1;
  }
}

double taiko_event_detail_float(const taiko_event *event) {
  switch (event->type) {
  case TAIKO_EVENT_BPM:
    return event->detail_float.value;
  default:
    return NAN;
  }
}

int taiko_event_compare(const taiko_event *a, const taiko_event *b) {
  int result = (a->time > b->time) - (a->time < b->time);

  if (result == 0)
    result = (a->type > b->type) - (a->type < b->type);

  return result;
}

const taiko_event *taiko_event_next(const taiko_event *event) {
  return event + 1;
}

taiko_event *taiko_event_next_mut_(taiko_event *event) { return event + 1; }

const taiko_event *taiko_event_prev(const taiko_event *event) {
  return event - 1;
}

taiko_event *taiko_event_prev_mut_(taiko_event *event) { return event - 1; }
