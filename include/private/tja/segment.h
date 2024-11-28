/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_SEGMENT_H_
#define TJA_SEGMENT_H_

#include "taco.h"
#include "tja/events.h"
#include <stdbool.h>

typedef struct tja_segment_ tja_segment;

extern int tja_segment_init_(tja_segment *segment, taiko_section *buf);
extern int tja_segment_push_barline_(tja_segment *segment, int units);
extern void tja_segment_finish_measure_(tja_segment *segment);
extern int tja_segment_push_events_(tja_segment *segment, tja_events *events);

struct tja_segment_ {
  taiko_section *segment;
  int measures;
  bool levelhold : 1;
};

#endif /* !TJA_SEGMENT_H_ */
