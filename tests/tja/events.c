// SPDX-License-Identifier: BSD-2-Clause
#include "tja/events.h"
#include <check.h>

#include "note.h"
#include "section.h"
#include "taco.h"
#include "tja/parser.h"
#include <stdbool.h>

static tja_events events;

static void setup(void) {
  events.units = 0;
  events.levelhold = false;
  events.events = taiko_section_create_();
  ck_assert_ptr_nonnull(events.events);
}

static void teardown(void) { taiko_section_free_(events.events); }

START_TEST(test_push_note) {
  tja_events_push_note_(&events, '0', 2);
  ck_assert_int_eq(events.units, 1);
  ck_assert_int_eq(taiko_section_size(events.events), 0);

  tja_events_push_note_(&events, '1', 2);
  ck_assert_int_eq(events.units, 2);
  ck_assert_int_eq(taiko_section_size(events.events), 1);
  ck_assert_int_eq(taiko_event_type(taiko_section_begin(events.events)),
                   TAIKO_EVENT_DON);
}
END_TEST

START_TEST(test_push_event) {
  taiko_event e = {
      .time = -1,
      .type = TAIKO_EVENT_SCROLL,
      .detail_float = {4},
  };

  tja_events_push_event_(&events, &e);
  ck_assert_int_eq(events.units, 0);
  ck_assert_int_eq(taiko_section_size(events.events), 1);
  ck_assert_int_eq(taiko_event_time(taiko_section_begin(events.events)), 0);
  ck_assert_int_eq(taiko_event_type(taiko_section_begin(events.events)),
                   TAIKO_EVENT_SCROLL);

  tja_events_push_note_(&events, '3', 2);
  ck_assert_int_eq(events.units, 1);
  ck_assert_int_eq(taiko_section_size(events.events), 2);
}
END_TEST

START_TEST(test_levelhold) {
  taiko_event e = {
      .time = 0,
      .type = TAIKO_EVENT_TJA_LEVELHOLD,
  };

  tja_events_push_event_(&events, &e);
  ck_assert_int_eq(taiko_section_size(events.events), 0);
  ck_assert_int_eq(events.levelhold, true);
}

TCase *case_events(void) {
  TCase *c = tcase_create("events");
  tcase_add_checked_fixture(c, setup, teardown);
  tcase_add_test(c, test_push_note);
  tcase_add_test(c, test_push_event);
  tcase_add_test(c, test_levelhold);
  return c;
}
