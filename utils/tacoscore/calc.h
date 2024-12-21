/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef CALC_H
#define CALC_H

#include <taco.h>

typedef int (*score_branch_fn)(const taco_section *branch,
                               const taco_course *meta, int *base, int *bonus,
                               int *ceil, int *combo);

extern int score_set(taco_parser *parser, FILE *f, const char *filename,
                     score_branch_fn score);
extern int score_course(const taco_course *course, int diffclass,
                        score_branch_fn score);

extern int score_branch_ac15(const taco_section *branch,
                             const taco_course *meta, int *base, int *bonus,
                             int *ceil, int *combo);
extern int score_branch_ac15s(const taco_section *branch,
                              const taco_course *meta, int *base, int *bonus,
                              int *ceil, int *combo);
extern int score_branch_ac16(const taco_section *branch,
                             const taco_course *meta, int *base, int *bonus,
                             int *ceil, int *combo);
extern int score_branch_default(const taco_section *branch,
                                const taco_course *meta, int *base, int *bonus,
                                int *ceiling, int *combo);
extern int score_branch_shinuchi(const taco_section *branch,
                                 const taco_course *meta, int *base, int *bonus,
                                 int *ceiling, int *combo);

extern int roll_hits(double duration, double roll_rate);

#endif /* !CALC_H */
