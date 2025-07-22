// SPDX-License-Identifier: BSD-2-Clause
#include "taco.h"

#include "note.h"
#include <stdio.h>

static const char *annotations[16] = {
    "do", "ko", "don", "don", "ka", "ka", "kat", "kat",
    "do", "ko", "don", "don", "ka", "ka", "kat", "kat",
};

#define FLAG(str, flag) ((flag) ? " " str : "")

static void print_event_(const taco_event *restrict event,
                         FILE *restrict file) {
  switch (event->type) {
  case TACO_EVENT_NONE:
    fprintf(file, "%d none\n", event->time);
    break;
  case TACO_EVENT_DON:
    fprintf(file, "%d don annotation=%s\n", event->time,
            annotations[TACO_TEXT(event->detail_int.value)]);
    break;
  case TACO_EVENT_KAT:
    fprintf(file, "%d kat annotation=%s\n", event->time,
            annotations[TACO_TEXT(event->detail_int.value)]);
    break;
  case TACO_EVENT_DON_BIG:
    fprintf(file, "%d don_big%s\n", event->time,
            FLAG("hand", event->detail_int.value & TACO_DETAIL_HAND));
    break;
  case TACO_EVENT_KAT_BIG:
    fprintf(file, "%d kat_big%s\n", event->time,
            FLAG("hand", event->detail_int.value & TACO_DETAIL_HAND));
    break;
  case TACO_EVENT_ROLL:
    fprintf(file, "%d roll\n", event->time);
    break;
  case TACO_EVENT_ROLL_BIG:
    fprintf(file, "%d roll_big\n", event->time);
    break;
  case TACO_EVENT_BALLOON:
    fprintf(file, "%d balloon hits=%d\n", event->time, event->detail_int.value);
    break;
  case TACO_EVENT_ROLL_END:
    fprintf(file, "%d roll_end\n", event->time);
    break;
  case TACO_EVENT_KUSUDAMA:
    fprintf(file, "%d kusudama hits=%d\n", event->time,
            event->detail_int.value);
    break;
  case TACO_EVENT_ROLL_CHECKPOINT:
    fprintf(file, "%d roll_checkpoint\n", event->time);
    break;

  case TACO_EVENT_LANDMINE:
    fprintf(file, "%d landmine annotation=%s\n", event->time,
            annotations[TACO_TEXT(event->detail_int.value)]);
    break;

  case TACO_EVENT_MEASURE:
    fprintf(file, "%d measure%s%s\n", event->time,
            FLAG("fake", !event->measure.real),
            FLAG("hidden", event->measure.hidden));
    break;
  case TACO_EVENT_GOGOSTART:
    fprintf(file, "%d gogostart\n", event->time);
    break;
  case TACO_EVENT_GOGOEND:
    fprintf(file, "%d gogoend\n", event->time);
    break;
  case TACO_EVENT_SCROLL:
    fprintf(file, "%d scroll mult=%lf\n", event->time,
            event->detail_float.value);
    break;
  case TACO_EVENT_BPM:
    fprintf(file, "%d bpm tempo=%lf\n", event->time, event->detail_float.value);
    break;
  case TACO_EVENT_BRANCH_START:
    fprintf(
        file,
        "%d branch_start good=%d good_big=%d ok=%d ok_big=%d roll=%d bad=%d\n",
        event->time, event->branch_start.good, event->branch_start.good_big,
        event->branch_start.ok, event->branch_start.ok_big,
        event->branch_start.roll, event->branch_start.bad);
    break;
  case TACO_EVENT_BRANCH_JUMP:
    fprintf(file, "%d branch_jump\n", event->time);
    break;
  case TACO_EVENT_BRANCH_CHECK:
    fprintf(file, "%d branch_check advanced=%d master=%d\n", event->time,
            event->branch_cond.advanced, event->branch_cond.master);
    break;
  }
}

void taco_section_print(const taco_section *restrict section,
                                    FILE *restrict file) {
  taco_section_foreach(i, section) { print_event_(i, file); }
}
