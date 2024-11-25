/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_BRANCHES_H_
#define TJA_BRANCHES_H_

#include "taiko.h"
#include "tja/segment.h"
typedef struct tja_branched_ tja_branched;

struct tja_branched_ {
  taiko_section *common;
  taiko_section *branches[3];
  int branchtype;
  int threshold_advanced;
  int threshold_master;
  int measures;
  unsigned char levelhold;
};

extern void tja_branched_assign_(tja_branched *restrict branched,
                                 tja_segment *restrict segment, int branch);

#endif /* !TJA_BRANCHES_H_ */
