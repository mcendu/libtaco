// SPDX-License-Identifier: BSD-2-Clause
#include "getopt.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taco.h>

#include "calc.h"

static const struct option options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"rules", required_argument, NULL, 'r'},
    {NULL, 0, NULL, 0},
};

static const struct scoring_rule {
  const char *name;
  score_branch_fn score_fn;
} scoring_rules[] = {
    {"ac15", score_branch_ac15},      {"ac15s", score_branch_ac15s},
    {"ac16", score_branch_ac16},      {"arcade", score_branch_ac16},
    {"console", score_branch_ac15},   {"default", score_branch_ac15s},
    {"shinuchi", score_branch_ac15s},
};

int cmp_scoring_rules(const void *k, const void *e) {
  const struct scoring_rule *entry = e;
  return strcmp(k, entry->name);
}

static void help(const char *arg0);
static void version();

int main(int argc, char **argv) {
  int opt;
  int options_valid = 1;
  FILE *input = stdin;
  const char *filename = "<stdin>";
  score_branch_fn score_fn = score_branch_ac16;

  while ((opt = getopt_long(argc, argv, "hvr:", options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      help(argv[0]);
      exit(0);
    case 'v':
      version();
      exit(0);
    case 'r': {
      const struct scoring_rule *entry =
          bsearch(optarg, scoring_rules,
                  sizeof(scoring_rules) / sizeof(struct scoring_rule),
                  sizeof(struct scoring_rule), cmp_scoring_rules);
      if (entry) {
        score_fn = entry->score_fn;
      } else {
        fprintf(stderr, "%s: unrecognized scoring rule: %s\n", argv[0], optarg);
        options_valid = 0;
      }
      break;
    }
    case '?':
    default:
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

  taco_parser *parser = taco_parser_tja_create();
  int result = score_set(parser, input, filename, score_fn);
  taco_parser_free(parser);

  exit(result);
}

static void help(const char *arg0) {
  printf("Usage: %s [options] [FILE]\n"
         "Calculate scoring parameters of a TJA file.\n"
         "\n"
         "With no FILE, or when FILE is -, read standard input.\n"
         "\n"
         "Options:\n"
         "  -r, --rules=RULES  use specified scoring rules\n"
         "  -h, --help         print this help\n"
         "  -v, --version      print version info\n"
         "\n"
         "Available rules:\n"
         "  - ac15 (console)\n"
         "  - ac15s (default, shinuchi)\n"
         "  - ac16 (arcade)\n",
         arg0);
}

static void version(void) { printf("tacoscore 0.2.0\n"); }
