#include "calc.h"

#include <math.h>
#include <taco.h>

int score_branch_ac16(const taiko_section *b, const taiko_course *c, int *base,
                 int *bonus, int *ceil_score, int *combo) {
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