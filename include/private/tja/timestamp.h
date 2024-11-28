/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_TIMESTAMP_H_
#define TJA_TIMESTAMP_H_

#include "taco.h"

extern void tja_event_get_timestamp_(const taiko_event *restrict event,
                                     int *restrict measure, int *restrict unit);
extern int tja_event_measure_(const taiko_event *restrict event);
extern int tja_event_unit_(const taiko_event *restrict event);

extern void tja_event_set_timestamp_(taiko_event *restrict event, int measure,
                                     int unit);
extern void tja_event_set_measure_(taiko_event *restrict event, int measure);
extern void tja_event_set_unit_(taiko_event *restrict event, int unit);

#endif /* TJA_TIMESTAMP_H_ */
