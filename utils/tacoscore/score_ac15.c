// SPDX-License-Identifier: BSD-2-Clause
#include "calc.h"

#include <math.h>
#include <taco.h>

struct result {
  int *base;
  int *bonus;
  int *ceiling;
  int *combo;
};

struct roll_info {
  double small;
  double big;
  double balloon;
  int balloon_hits;
  int breaks;
  int late_breaks;
};

static const int score_targets[10][4] = {
    {300000, 400000, 550000, 700000},  // Lv.1
    {320000, 450000, 600000, 750000},  // Lv.2
    {340000, 500000, 650000, 800000},  // Lv.3
    {360000, 550000, 700000, 850000},  // Lv.4
    {380000, 600000, 750000, 900000},  // Lv.5
    {380000, 650000, 800000, 950000},  // Lv.6
    {380000, 700000, 850000, 1000000}, // Lv.7
    {380000, 700000, 900000, 1050000}, // Lv.8
    {380000, 700000, 900000, 1100000}, // Lv.9
    {380000, 700000, 900000, 1200000}, // Lv.10+
};

static int note_units(int combo, int gogotime) {
  static const int table[2][11] = {
      {20, 25, 25, 30, 30, 40, 40, 40, 40, 40, 60},
      {24, 30, 30, 36, 36, 48, 48, 48, 48, 48, 72},
  };

  if (gogotime)
    gogotime = 1;
  if (combo < 0)
    return table[gogotime][0];
  if (combo >= 100)
    return table[gogotime][10];
  return table[gogotime][combo / 10];
}

static int note_bonus(int combo) {
  static const int table[11] = {0, 1, 1, 2, 2, 4, 4, 4, 4, 4, 8};
  if (combo < 0)
    return table[0];
  if (combo >= 100)
    return table[10];
  return table[combo / 10];
}

static void process_roll(struct roll_info *info, int type, double length,
                         int hits, double checkpoint) {
  if (type == TACO_EVENT_KUSUDAMA) {
    if (isnan(checkpoint) || checkpoint > length)
      checkpoint = length;

    if (roll_hits(checkpoint, 20) >= hits) {
      info->balloon_hits += hits - 1;
      info->breaks += 1;
    } else if (roll_hits(length, 20) >= hits) {
      info->balloon_hits += hits - 1;
      info->late_breaks += 1;
    } else {
      info->balloon += length;
    }
  } else if (type == TACO_EVENT_BALLOON) {
    if (roll_hits(length, 20) >= hits) {
      info->balloon_hits += hits - 1;
      info->breaks += 1;
    } else {
      info->balloon += length;
    }
  } else if (type == TACO_EVENT_ROLL_BIG) {
    info->big += length;
  } else if (type == TACO_EVENT_ROLL) {
    info->small += length;
  }
}

static void score_targetted(const taco_section *b, struct result *result,
                            int target) {
  int units = 0;
  int base_count[2] = {0, 0};
  int bonus_count[2] = {0, 0};
  struct roll_info roll_info = {0, 0, 0, 0, 0, 0};

  int combo = 0;
  int roll_type = 0;
  int balloon_hits = 0;
  int gogotime = 0;
  double roll_start = NAN;
  double roll_checkpoint = NAN;

  taco_section_foreach(i, b) {
    int type = taco_event_type(i);

    if (type == TACO_EVENT_GOGOSTART)
      gogotime = 1;
    else if (type == TACO_EVENT_GOGOEND)
      gogotime = 0;

    // process normal notes
    if (type <= 0)
      continue;

    if (!isnan(roll_start)) {
      // process rolls
      switch (type) {
      case TACO_EVENT_ROLL_END:
        process_roll(&roll_info, roll_type,
                     taco_event_seconds(i, b) - roll_start, balloon_hits,
                     taco_event_seconds(i, b) - roll_checkpoint);
        roll_start = NAN;
        roll_checkpoint = NAN;
        break;
      case TACO_EVENT_ROLL_CHECKPOINT:
        roll_checkpoint = taco_event_seconds(i, b);
      default:
        // any roll without a specified end is discarded as invalid
        roll_start = NAN;
        break;
      }
    } else {
      // process other stuff
      switch (type) {
      case TACO_EVENT_DON:
      case TACO_EVENT_KAT:
        combo += 1;
        base_count[gogotime] += 1;
        bonus_count[gogotime] += note_bonus(combo);
        units += note_units(combo, gogotime);
        break;
      case TACO_EVENT_DON_BIG:
      case TACO_EVENT_KAT_BIG:
        combo += 1;
        base_count[gogotime] += 2;
        bonus_count[gogotime] += note_bonus(combo) * 2;
        units += note_units(combo, gogotime) * 2;
        break;

      case TACO_EVENT_BALLOON:
      case TACO_EVENT_KUSUDAMA:
        balloon_hits = taco_event_hits(i);
        /* FALLTHROUGH */
      case TACO_EVENT_ROLL_BIG:
      case TACO_EVENT_ROLL:
        roll_type = taco_event_type(i);
        roll_start = taco_event_seconds(i, b);
        break;
      }
    }
  }

  // determine contribution of rolls to target
  target -= roll_hits(roll_info.small, 15) * 100;
  target -= roll_hits(roll_info.big, 15) * 200;
  target -= (roll_info.balloon_hits + roll_hits(roll_info.balloon, 15)) * 300;
  target -= roll_info.breaks * 5000;
  target -= roll_info.late_breaks * 1000;

  // 10000 points are given every 100 combo
  target -= (combo / 100) * 10000;

  // calculate score attributes
  double unit = units ? target / (double)units : 0;
  int base = ceil((unit * 20) / 10) * 10;
  int bonus = ceil(unit * 5);
  int gogobase = floor(base * 1.2);
  int gogobonus = floor(bonus * 1.2);

  // calculate ceiling
  int ceiling = base * base_count[0] + bonus * bonus_count[0] +
                gogobase * base_count[1] + gogobonus * bonus_count[1] +
                roll_info.balloon_hits * 300 + roll_info.breaks * 5000 +
                roll_info.late_breaks * 1000;

  *(result->base) = base;
  *(result->bonus) = bonus;
  *(result->ceiling) = ceiling;
  *(result->combo) = combo;
}

int score_branch_ac15(const taco_section *branch, const taco_course *meta,
                      int *base, int *bonus, int *ceil, int *combo) {
  int level = taco_course_level(meta) - 1;
  if (level < 0)
    level = 0;
  if (level > 9)
    level = 9;

  int target = score_targets[level][taco_course_class(meta) % 4];
  struct result result = {base, bonus, ceil, combo};
  score_targetted(branch, &result, target);
  return 0;
}

int score_branch_ac15s(const taco_section *branch, const taco_course *meta,
                       int *base, int *bonus, int *ceiling, int *combo) {
  int units = 0;
  int target = 1000000;
  struct roll_info roll_info = {0, 0, 0, 0, 0, 0};

  int notes = 0;
  int roll_type = 0;
  int balloon_hits = 0;
  double roll_start = NAN;
  double roll_checkpoint = NAN;

  taco_section_foreach(i, branch) {
    int type = taco_event_type(i);

    // process normal notes
    if (type <= 0)
      continue;

    if (!isnan(roll_start)) {
      // process rolls
      switch (type) {
      case TACO_EVENT_ROLL_END:
        process_roll(&roll_info, roll_type,
                     taco_event_seconds(i, branch) - roll_start, balloon_hits,
                     taco_event_seconds(i, branch) - roll_checkpoint);
        roll_start = NAN;
        roll_checkpoint = NAN;
        break;
      case TACO_EVENT_ROLL_CHECKPOINT:
        roll_checkpoint = taco_event_seconds(i, branch);
      default:
        // any roll without a specified end is discarded as invalid
        roll_start = NAN;
        break;
      }
    } else {
      // process other stuff
      switch (type) {
      case TACO_EVENT_DON:
      case TACO_EVENT_KAT:
        notes += 1;
        units += 1;
        break;
      case TACO_EVENT_DON_BIG:
      case TACO_EVENT_KAT_BIG:
        notes += 1;
        units += 2;
        break;

      case TACO_EVENT_BALLOON:
      case TACO_EVENT_KUSUDAMA:
        balloon_hits = taco_event_hits(i);
        /* FALLTHROUGH */
      case TACO_EVENT_ROLL_BIG:
      case TACO_EVENT_ROLL:
        roll_type = taco_event_type(i);
        roll_start = taco_event_seconds(i, branch);
        break;
      }
    }
  }

  // determine contribution of rolls to target
  target -= roll_hits(roll_info.small, 15) * 100;
  target -= roll_hits(roll_info.big, 15) * 200;
  target -= (roll_info.balloon_hits + roll_hits(roll_info.balloon, 15)) * 300;
  target -= roll_info.breaks * 5000;
  target -= roll_info.late_breaks * 1000;

  *base = ceil(((double)target / units) / 10.0) * 10.0;
  *bonus = 0;
  *combo = notes;
  *ceiling = *base * units + 300 * roll_info.balloon_hits +
             5000 * roll_info.breaks + 1000 * roll_info.late_breaks;
  return 0;
}
