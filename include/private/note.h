/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_NOTE_H_
#define TAIKO_NOTE_H_

#include "taiko.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct taiko_event_ taiko_event;

struct int_event_ {
  uint32_t value;
  uint8_t reserved[4];
};

struct float_event_ {
  double value;
};

struct branch_start_event_ {
  int8_t good;
  int8_t good_big;
  int8_t ok;
  int8_t ok_big;
  int8_t roll;
  int8_t bad;
  uint8_t reserved_1;
  uint8_t reserved_2;
};

struct branch_condition_event_ {
  int32_t advanced;
  int32_t master;
};

struct measure_event_ {
  bool real : 1;
  bool hidden : 1;
  int32_t tja_units;
};

struct measure_length_event_ {
  int32_t dividend;
  int32_t divisor;
};

struct taiko_event_ {
  uint32_t time;
  int32_t type;
  union {
    uint8_t raw_params[8];
    struct int_event_ detail_int;
    struct float_event_ detail_float;
    struct branch_start_event_ branch_start;
    struct branch_condition_event_ branch_cond;
    struct measure_event_ measure;
    struct measure_length_event_ tja_measure_length;
  };
};

extern taiko_event *taiko_event_next_mut_(taiko_event *event);
extern taiko_event *taiko_event_prev_mut_(taiko_event *event);

#endif /* !TAIKO_NOTE_H_ */
