/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_EVENTS_H_
#define TJA_EVENTS_H_

#include "taco.h"
#include <stdbool.h>

typedef struct tja_events_ tja_events;

extern int tja_events_push_note_(tja_events *restrict events, int note,
                                 int line);
extern int tja_events_push_event_(tja_events *restrict events,
                                  const taiko_event *restrict event);

struct tja_events_ {
  int units;
  bool levelhold : 1;
  taiko_section *events;
};

#endif /* TJA_EVENTS_H_ */
