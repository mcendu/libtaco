// SPDX-License-Identifier: BSD-2-Clause
#include "tja/timestamp.h"

#include "note.h"
#include <stdint.h>

typedef struct tja_timestamp_ tja_timestamp;

struct tja_timestamp_ {
  uint16_t measure;
  uint16_t unit;
};

void tja_event_set_timestamp_(taco_event *restrict event, int measure,
                              int dividend) {
  tja_timestamp *t = (tja_timestamp *)&event->time;
  t->measure = measure;
  t->unit = dividend;
}

void tja_event_set_measure_(taco_event *restrict event, int measure) {
  tja_timestamp *t = (tja_timestamp *)&event->time;
  t->measure = measure;
}

void tja_event_set_unit_(taco_event *restrict event, int unit) {
  tja_timestamp *t = (tja_timestamp *)&event->time;
  t->unit = unit;
}

void tja_event_get_timestamp_(const taco_event *restrict event,
                              int *restrict measure, int *restrict dividend) {
  const tja_timestamp *t = (const tja_timestamp *)&event->time;
  if (measure)
    *measure = t->measure;
  if (dividend)
    *dividend = t->unit;
}

int tja_event_measure_(const taco_event *restrict event) {
  const tja_timestamp *t = (const tja_timestamp *)&event->time;
  return t->measure;
}

int tja_event_unit_(const taco_event *restrict event) {
  const tja_timestamp *t = (const tja_timestamp *)&event->time;
  return t->unit;
}
