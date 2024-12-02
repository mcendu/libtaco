// SPDX-License-Identifier: BSD-2-Clause
#include "calc.h"

#include <math.h>
#include <taco.h>

int score_set(taco_parser *parser, FILE *f, const char *filename, score_branch_fn score) {
  taco_courseset *set = taco_parser_parse_stdio(parser, f);
  if (!set) {
    printf("## %s ##\n", filename);
    printf("Failed to read courseset.\n");
    return -1;
  }

  const char *title = taco_courseset_title(set);
  if (title) {
    printf("## %s (%s) ##\n", title, filename);
  } else {
    printf("## %s ##\n", filename);
  }

  for (int i = 0; i < 8; ++i) {
    const taco_course *course = taco_courseset_get_course(set, i);
    if (!course)
      continue;
    score_course(course, i, score);
  }

  taco_courseset_free(set);
  return 0;
}

int score_course(const taco_course *c, int cls, score_branch_fn score) {
  static const char *const class_names[8] = {
      "Easy",       "Normal",       "Hard",       "Oni",
      "Inner Easy", "Inner Normal", "Inner Hard", "Inner Oni",
  };

  int base, bonus, ceil, combo;

  if (taco_course_style(c) == TACO_STYLE_SINGLE) {
    const taco_section *b = taco_course_get_branch(c, 0, TACO_BRANCH_MASTER);
    score_branch_ac16(b, c, &base, &bonus, &ceil, &combo);
    printf("%s: base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], base, bonus, ceil, combo);
  } else {
    const taco_section *l =
        taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_MASTER);
    const taco_section *r =
        taco_course_get_branch(c, TACO_SIDE_RIGHT, TACO_BRANCH_MASTER);
    score(l, c, &base, &bonus, &ceil, &combo);
    printf("%s (%s): base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], "P1", base, bonus, ceil, combo);
    score(r, c, &base, &bonus, &ceil, &combo);
    printf("%s (%s): base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], "P2", base, bonus, ceil, combo);
  }

  return 0;
}

int roll_hits(double duration, double roll_rate) {
  return ceil(duration * roll_rate);
}
