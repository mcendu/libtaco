// SPDX-License-Identifier: BSD-2-Clause
#include "note.h"

#include "taco.h"
#include "tja/parser.h"
#include <math.h>

int taco_event_time(const taco_event *event) { return event->time; }

int taco_event_type(const taco_event *event) { return event->type; }

int taco_event_detail_int(const taco_event *event) {
  switch (event->type) {
  case TACO_EVENT_DON:             // annotation
  case TACO_EVENT_DON_BIG:         // hand
  case TACO_EVENT_KAT:             // annotation
  case TACO_EVENT_KAT_BIG:         // hand
  case TACO_EVENT_BALLOON:         // hits required
  case TACO_EVENT_KUSUDAMA:        // hits required
  case TACO_EVENT_LANDMINE:        // annotation
  case TACO_EVENT_TJA_BRANCH_TYPE: // branch type
    return event->detail_int.value;
  default:
    return -1;
  }
}

double taco_event_detail_float(const taco_event *event) {
  switch (event->type) {
  case TACO_EVENT_BPM:    // bpm
  case TACO_EVENT_SCROLL: // scroll speed multiplier
    return event->detail_float.value;
  default:
    return NAN;
  }
}

int taco_event_branch_scoring(const taco_event *restrict event,
                              taco_branch_scoring *restrict scoring) {
  if (event->type != TACO_EVENT_BRANCH_START)
    return -1;

  if (scoring) {
    scoring->good = event->branch_start.good;
    scoring->good_big = event->branch_start.good_big;
    scoring->ok = event->branch_start.ok;
    scoring->ok_big = event->branch_start.ok_big;
    scoring->roll = event->branch_start.roll;
    scoring->bad = event->branch_start.bad;
  }

  return 0;
}

int taco_event_branch_thresholds(const taco_event *restrict event,
                                 int *restrict advanced, int *restrict master) {
  if (event->type != TACO_EVENT_BRANCH_CHECK)
    return -1;

  if (advanced)
    *advanced = event->branch_cond.advanced;
  if (master)
    *master = event->branch_cond.master;

  return 0;
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
