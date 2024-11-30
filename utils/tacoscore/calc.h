// SPDX-License-Identifier: BSD-2-Clause
#ifndef CALC_H
#define CALC_H

#include <taco.h>

typedef int (*score_branch_fn)(const taiko_section *branch,
                            const taiko_course *meta, int *base, int *bonus,
                            int *ceil, int *combo);

extern int score_set(taiko_parser *parser, FILE *f, const char *filename, score_branch_fn score);
extern int score_course(const taiko_course *course, int diffclass, score_branch_fn score);

extern int score_branch_ac16(const taiko_section *branch,
                             const taiko_course *meta, int *base, int *bonus,
                             int *ceil, int *combo);

extern int roll_hits(double duration, double roll_rate);

#endif /* !CALC_H */