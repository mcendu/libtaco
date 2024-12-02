/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_COURSEBODY_H_
#define TJA_COURSEBODY_H_

#include "taco.h"
#include "tja/branched.h"
#include "tja/segment.h"

typedef struct tja_coursebody_ tja_coursebody;

struct tja_coursebody_ {
  taco_course *course;
  int measures;
  unsigned char levelhold;
};

extern int tja_coursebody_init_(tja_coursebody *restrict c,
                                taco_allocator *allocator);
extern int tja_coursebody_append_common_(tja_coursebody *restrict c,
                                         tja_segment *restrict common);
extern int tja_coursebody_append_branched_(tja_coursebody *restrict c,
                                           tja_branched *restrict branched);

#endif /* TJA_COURSEBODY_H_ */
