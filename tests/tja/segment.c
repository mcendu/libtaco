// SPDX-License-Identifier: BSD-2-Clause
#include "tja/segment.h"
#include <check.h>

#include "section.h"
#include "taco.h"
#include "tja/events.h"
#include "tja/timestamp.h"

static tja_segment segment;

static void setup(void) {
  taco_section *s = taco_section_create_();
  tja_segment_init_(&segment, s);
}

static void teardown(void) { taco_section_free_(segment.segment); }

START_TEST(test_segment) {
  taco_section *section = taco_section_create_();
  tja_events measure = {
      .units = 0,
      .levelhold = false,
      .events = section,
  };
  tja_events_push_note_(&measure, '1', 2);

  tja_segment_push_barline_(&segment, 1);
  tja_segment_push_events_(&segment, &measure);
  ck_assert_int_eq(taco_section_size(segment.segment), 2);

  ck_assert_int_eq(segment.measures, 0);
  tja_segment_finish_measure_(&segment);
  ck_assert_int_eq(segment.measures, 1);

  taco_section_free_(section);
}
END_TEST

START_TEST(test_segment_multi) {
  taco_section *section = taco_section_create_();
  tja_events measure = {
      .units = 0,
      .levelhold = false,
      .events = section,
  };
  tja_events_push_note_(&measure, '1', 2);

  tja_segment_push_barline_(&segment, 1);
  tja_segment_push_events_(&segment, &measure);
  tja_segment_finish_measure_(&segment);
  ck_assert_int_eq(segment.measures, 1);

  tja_segment_push_barline_(&segment, 1);
  tja_segment_push_events_(&segment, &measure);
  tja_segment_finish_measure_(&segment);
  ck_assert_int_eq(taco_section_size(segment.segment), 4);
  ck_assert_int_eq(tja_event_measure_(taco_section_locate(segment.segment, 0)),
                   0);
  ck_assert_int_eq(tja_event_measure_(taco_section_locate(segment.segment, 2)),
                   1);
  taco_section_free_(section);
}
END_TEST

START_TEST(test_levelhold) {
  taco_section *section = taco_section_create_();
  tja_events measure = {
      .units = 0,
      .levelhold = true,
      .events = section,
  };

  tja_segment_push_barline_(&segment, 1);
  tja_segment_push_events_(&segment, &measure);
  tja_segment_finish_measure_(&segment);
  ck_assert_int_eq(segment.levelhold, true);

  taco_section_free_(section);
}
END_TEST

TCase *case_segment(void) {
  TCase *c = tcase_create("segment");
  tcase_add_checked_fixture(c, setup, teardown);
  tcase_add_test(c, test_segment);
  tcase_add_test(c, test_segment_multi);
  tcase_add_test(c, test_levelhold);
  return c;
}
