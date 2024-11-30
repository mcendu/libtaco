// SPDX-License-Identifier: BSD-2-Clause
#include "calc.h"

#include <math.h>
#include <taco.h>

int score_set(taiko_parser *parser, FILE *f, const char *filename, score_branch_fn score) {
  taiko_courseset *set = taiko_parser_parse_stdio(parser, f);
  if (!set) {
    printf("## %s ##\n", filename);
    printf("Failed to read courseset.\n");
    return -1;
  }

  const char *title = taiko_courseset_title(set);
  if (title) {
    printf("## %s (%s) ##\n", title, filename);
  } else {
    printf("## %s ##\n", filename);
  }

  for (int i = 0; i < 8; ++i) {
    const taiko_course *course = taiko_courseset_get_course(set, i);
    if (!course)
      continue;
    score_course(course, i, score);
  }

  taiko_courseset_free(set);
  return 0;
}

int score_course(const taiko_course *c, int cls, score_branch_fn score) {
  static const char *const class_names[8] = {
      "Easy",       "Normal",       "Hard",       "Oni",
      "Inner Easy", "Inner Normal", "Inner Hard", "Inner Oni",
  };

  int base, bonus, ceil, combo;

  if (taiko_course_style(c) == TAIKO_STYLE_SINGLE) {
    const taiko_section *b = taiko_course_get_branch(c, 0, TAIKO_BRANCH_MASTER);
    score_branch_ac16(b, c, &base, &bonus, &ceil, &combo);
    printf("%s: base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], base, bonus, ceil, combo);
  } else {
    const taiko_section *l =
        taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_MASTER);
    const taiko_section *r =
        taiko_course_get_branch(c, TAIKO_SIDE_RIGHT, TAIKO_BRANCH_MASTER);
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
