// SPDX-License-Identifier: BSD-2-Clause
#include "getopt.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taco.h>

static const struct option options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0},
};

static void help(const char *arg0);
static void version();

static int score_set(taiko_parser *parser, FILE *f, const char *filename);
static int score_course(const taiko_course *course, int diffclass);
static int score_branch(const taiko_section *branch, const taiko_course *meta,
                        int *base, int *bonus, int *ceil, int *combo);

static int roll_hits(double duration, double roll_rate);

int main(int argc, char **argv) {
  int opt;
  int options_valid = 1;
  FILE *input = stdin;
  const char *filename = "<stdin>";

  while ((opt = getopt_long(argc, argv, "hv", options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      help(argv[0]);
      exit(0);
    case 'v':
      version();
      exit(0);
    case '?':
      options_valid = 0;
      break;
    }
  }

  if (!options_valid)
    exit(1);

  if (argc > optind && strcmp(argv[optind], "-") != 0) {
    input = fopen(argv[optind], "r");
    filename = argv[optind];
  }

  taiko_parser *parser = taiko_parser_tja_create();
  int result = score_set(parser, input, filename);
  taiko_parser_free(parser);

  exit(result);
}

static int score_set(taiko_parser *parser, FILE *f, const char *filename) {
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
    score_course(course, i);
  }

  taiko_courseset_free(set);
  return 0;
}

static int score_course(const taiko_course *c, int cls) {
  static const char *const class_names[8] = {
      "Easy",       "Normal",       "Hard",       "Oni",
      "Inner Easy", "Inner Normal", "Inner Hard", "Inner Oni",
  };

  int base, bonus, ceil, combo;

  if (taiko_course_style(c) == TAIKO_STYLE_SINGLE) {
    const taiko_section *b = taiko_course_get_branch(c, 0, TAIKO_BRANCH_MASTER);
    score_branch(b, c, &base, &bonus, &ceil, &combo);
    printf("%s: base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], base, bonus, ceil, combo);
  } else {
    const taiko_section *l =
        taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_MASTER);
    const taiko_section *r =
        taiko_course_get_branch(c, TAIKO_SIDE_RIGHT, TAIKO_BRANCH_MASTER);
    score_branch(l, c, &base, &bonus, &ceil, &combo);
    printf("%s (%s): base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], "P1", base, bonus, ceil, combo);
    score_branch(r, c, &base, &bonus, &ceil, &combo);
    printf("%s (%s): base %d, bonus %d, ceiling %d, max combo %d\n",
           class_names[cls], "P2", base, bonus, ceil, combo);
  }

  return 0;
}

static int score_branch(const taiko_section *b, const taiko_course *c,
                        int *base, int *bonus, int *ceil_score, int *combo) {
  static const double roll_rates[8] = {7.0, 8.5, 11.0, 17.0,
                                       7.0, 8.5, 11.0, 17.0};

  int target = 1000000;
  int units = 0;
  double roll_time = 0;
  int balloon_hits = 0;

  int roll_type = 0;
  int current_balloon_hits = 0;
  double current_roll_start = NAN;

  taiko_section_foreach (i, b) {
    int type = taiko_event_type(i);

    // ignore non-note events
    if (type <= 0)
      continue;

    // check for roll end
    if (!isnan(current_roll_start)) {
      switch (type) {
      case TAIKO_EVENT_ROLL_END: {
        double t = taiko_event_seconds(i, b) - current_roll_start;
        // balloons are treated as normal rolls if too hard
        if (roll_type == TAIKO_EVENT_ROLL ||
            roll_hits(t, 20) < current_balloon_hits)
          roll_time += t;
        else
          balloon_hits += current_balloon_hits;

        current_roll_start = NAN;
        break;
      }
      case TAIKO_EVENT_ROLL:
      case TAIKO_EVENT_BALLOON:
      case TAIKO_EVENT_KUSUDAMA:
      case TAIKO_EVENT_ROLL_CHECKPOINT:
        break;
      default:
        // any roll without a specified end is discarded as invalid
        current_roll_start = NAN;
        break;
      }
    }

    // process stuff outside rolls
    if (isnan(current_roll_start)) {
      switch (type) {
      case TAIKO_EVENT_DON:
      case TAIKO_EVENT_DON_BIG:
      case TAIKO_EVENT_KAT:
      case TAIKO_EVENT_KAT_BIG:
        units += 1;
        break;

      case TAIKO_EVENT_BALLOON:
      case TAIKO_EVENT_KUSUDAMA:
        current_balloon_hits = taiko_event_hits(i);
        /* FALLTHROUGH */
      case TAIKO_EVENT_ROLL_BIG:
      case TAIKO_EVENT_ROLL:
        roll_type = taiko_event_type(i);
        current_roll_start = taiko_event_seconds(i, b);
        break;
      }
    }
  }

  // rolls are worth 100 pts each
  int cls = taiko_course_class(c);
  target -= (roll_hits(roll_time, roll_rates[cls]) + balloon_hits) * 100;

  *base = units ? ceil(((double)target / (double)units) / 10.0) * 10.0 : 0;
  *bonus = 0;
  *ceil_score = *base * units + balloon_hits * 100;
  *combo = units;
  return 0;
}

static int roll_hits(double duration, double roll_rate) {
  return ceil(duration * roll_rate);
}

static void help(const char *arg0) {
  printf("Usage: %s [options] [FILE]\n"
         "Calculate scoring parameters of a TJA file.\n"
         "\n"
         "With no FILE, or when FILE is -, read standard input.\n"
         "\n"
         "Options:\n"
         "  -h, --help    print this help\n"
         "  -v, --version print version info\n",
         arg0);
}

static void version(void) { printf("tacoscore 0.1.0\n"); }
