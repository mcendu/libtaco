// SPDX-License-Identifier: BSD-2-Clause
#include "tja/postproc.h"

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include <assert.h>
#include <string.h>

const struct branch_start_event_ branch_scores[] = {
    [TACO_BRANCHTYPE_NONE] = {0, 0, 0, 0, 0, 0, 0, 0},
    [TACO_BRANCHTYPE_ACCURACY] = {20, 20, 10, 10, 1, 0, 0, 0},
    [TACO_BRANCHTYPE_ACCURACY_BIG] = {0, 20, 0, 10, 1, 0, 0, 0},
    [TACO_BRANCHTYPE_ROLL] = {0, 0, 0, 0, 1, 0, 0, 0},
};

typedef struct threshold_stats_ threshold_stats;

struct threshold_stats_ {
  int branch_type;
  int small_notes;
  int big_notes;
  int thres_a_src;
  int thres_m_src;
  int check_time;
  int jump_time;
  const taco_event *start;
};

typedef void branch_threshold_calc(const threshold_stats *stats, int *thres_a,
                                   int *thres_m);

static void roll_threshold(const threshold_stats *stats, int *thres_a,
                           int *thres_m) {
  *thres_a = stats->thres_a_src;
  *thres_m = stats->thres_m_src;
}

static void acc_threshold(const threshold_stats *stats, int *thres_a,
                          int *thres_m) {
  int total = branch_scores[stats->branch_type].good * stats->small_notes +
              branch_scores[stats->branch_type].good_big * stats->big_notes;
  double percentage_a = (double)stats->thres_a_src / (double)0x1000000;
  double percentage_m = (double)stats->thres_m_src / (double)0x1000000;
  *thres_a = percentage_a * total;
  *thres_m = percentage_m * total;
}

static void none_threshold(const threshold_stats *stats, int *thres_a,
                           int *thres_m) {
  *thres_a = 1;
  *thres_m = 1;
}

branch_threshold_calc *const threshold_calc[] = {
    [TACO_BRANCHTYPE_NONE] = none_threshold,
    [TACO_BRANCHTYPE_ACCURACY] = acc_threshold,
    [TACO_BRANCHTYPE_ACCURACY_BIG] = acc_threshold,
    [TACO_BRANCHTYPE_ROLL] = roll_threshold,
};

static void calculate_stats(threshold_stats *stats,
                            const taco_section *branch) {
  assert((stats->start >= taco_section_begin(branch) &&
          stats->start < taco_section_end(branch)));

  stats->small_notes = 0;
  stats->big_notes = 0;

  for (const taco_event *i = stats->start; i->time < stats->check_time; ++i) {
    switch (i->type) {
    case TACO_EVENT_DON:
    case TACO_EVENT_KAT:
      stats->small_notes += 1;
      break;
    case TACO_EVENT_DON_BIG:
    case TACO_EVENT_KAT_BIG:
      stats->big_notes += 1;
      break;
    }
  }
}

static void compile_branch(taco_section *new_events,
                           const threshold_stats *stats, int line) {
  int threshold_a = 0;
  int threshold_m = 0;
  threshold_calc[stats->branch_type](stats, &threshold_a, &threshold_m);

  taco_event branch_events[3] = {
      {
          .time = stats->start->time,
          .type = TACO_EVENT_BRANCH_START,
          .line = line,
          .branch_start = branch_scores[stats->branch_type],
      },
      {
          .time = stats->check_time,
          .type = TACO_EVENT_BRANCH_CHECK,
          .line = line,
          .branch_cond = {.advanced = threshold_a, .master = threshold_m},
      },
      {
          .time = stats->jump_time,
          .type = TACO_EVENT_BRANCH_JUMP,
          .line = line,
          .raw_params = {},
      },
  };
  taco_section_push_many_(new_events, branch_events, 3);
}

int tja_pass_compile_branches_(tja_parser *parser, taco_section *branch) {
  threshold_stats stats;
  memset(&stats, 0, sizeof(stats));
  stats.start = taco_section_begin(branch);

  taco_section *new_events =
      taco_section_create2_(tja_parser_allocator_(parser));

  taco_section_foreach_mut_(i, branch) {
    switch (taco_event_type(i)) {
    case TACO_EVENT_BRANCH_START:
      i->type = TACO_EVENT_NONE;
      memset(&stats, 0, sizeof(stats));
      stats.start = i;
      break;

    case TACO_EVENT_TJA_BRANCH_TYPE:
      stats.branch_type = taco_event_detail_int(i);
      if (stats.branch_type < 0 || stats.branch_type >= TACO_BRANCHTYPE_MAX)
        stats.branch_type = TACO_BRANCHTYPE_NONE;
      i->type = TACO_EVENT_NONE;
      break;

    case TACO_EVENT_TJA_BRANCH_THRESHOLD: {
      // get threshold expectations
      stats.thres_a_src = i->branch_cond.advanced;
      stats.thres_m_src = i->branch_cond.master;

      // get jump and check times; check normally happens one whole note
      // before the jump time
      stats.jump_time = taco_event_time(i);
      stats.check_time = stats.jump_time - taco_section_tickrate(branch);

      if (stats.check_time <= stats.start->time) {
        // not enough time for a proper check time. diagnosing.
        tja_parser_diagnose_(parser, i->line, TJA_DIAG_WARN,
                             "not enough time before #BRANCHSTART");
        // fallback to a sane time; end time is later because the internal
        // representation sorts branch checks before branch starts
        stats.check_time = stats.start->time + 1;
      }

      // calculate number of notes from start time to check time
      calculate_stats(&stats, branch);
      // compile branch
      compile_branch(new_events, &stats, i->line);

      i->type = TACO_EVENT_NONE;
      memset(&stats, 0, sizeof(stats));
      stats.start = i;
      break;
    }
    }
  }

  taco_section_concat_(branch, new_events);
  taco_section_free_(new_events);

  return 0;
}
