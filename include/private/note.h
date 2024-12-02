/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_NOTE_H_
#define TACO_NOTE_H_

#include "taco.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct taco_event_ taco_event;

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

struct taco_event_ {
  uint32_t time;
  int16_t type;
  uint16_t line;
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

extern taco_event *taco_event_next_mut_(taco_event *event);
extern taco_event *taco_event_prev_mut_(taco_event *event);

#endif /* !TACO_NOTE_H_ */
