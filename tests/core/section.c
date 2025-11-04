// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

#include "note.h" // IWYU pragma: keep; for definition of taco_event
#include "section.h"
#include "taco.h"

START_TEST(test_create) {
  taco_section *s = taco_section_create_();
  ck_assert_int_eq(taco_section_size(s), 0);
  ck_assert_ptr_eq(taco_section_begin(s), taco_section_end(s));
  taco_section_free_(s);
}
END_TEST

START_TEST(test_push) {
  static const taco_event e = {
      .time = 0,
      .type = TACO_EVENT_DON,
      .detail_int = {0},
  };

  taco_section *s = taco_section_create_();
  taco_section_push_(s, &e);
  ck_assert_int_eq(taco_section_size(s), 1);
  ck_assert_mem_eq(taco_section_begin(s), &e, sizeof(taco_event));
  taco_section_free_(s);
}
END_TEST

START_TEST(test_push_many) {
  static const taco_event e[3] = {
      {0, TACO_EVENT_DON, .detail_int = {0}},
      {60, TACO_EVENT_DON, .detail_int = {0}},
      {120, TACO_EVENT_DON, .detail_int = {0}},
  };

  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, e, 3);
  ck_assert_int_eq(taco_section_size(s), 3);

  const taco_event *i = taco_section_begin(s);
  ck_assert_mem_eq(i, &e[0], sizeof(taco_event));
  i = taco_event_next(i);
  ck_assert_mem_eq(i, &e[1], sizeof(taco_event));
  i = taco_event_next(i);
  ck_assert_mem_eq(i, &e[2], sizeof(taco_event));

  taco_section_free_(s);
}
END_TEST

START_TEST(test_clone) {
  static const taco_event e[3] = {
      {0, TACO_EVENT_DON, .detail_int = {0}},
      {60, TACO_EVENT_DON, .detail_int = {0}},
      {120, TACO_EVENT_DON, .detail_int = {0}},
  };

  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, e, 3);

  taco_section *other = taco_section_clone_(s);
  ck_assert_ptr_nonnull(other);
  ck_assert_ptr_ne(s, other);
  ck_assert_ptr_ne(taco_section_begin(s), taco_section_begin(other));
  ck_assert_mem_eq(taco_section_begin(s), taco_section_begin(other),
                   3 * sizeof(taco_event));

  taco_section_free_(other);
  taco_section_free_(s);
}

START_TEST(test_reserve) {
  taco_event e = {0, TACO_EVENT_DON, .detail_int = {0}};

  taco_section *s = taco_section_create_();

  for (int i = 0; i < 1024; ++i) {
    taco_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taco_section_size(s), 1024);
  taco_section_free_(s);
}
END_TEST

START_TEST(test_clear) {
  taco_event e = {0, TACO_EVENT_DON, .detail_int = {0}};

  taco_section *s = taco_section_create_();

  for (int i = 0; i < 1024; ++i) {
    taco_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taco_section_size(s), 1024);

  taco_section_clear_(s);
  ck_assert_int_eq(taco_section_size(s), 0);

  taco_section_free_(s);
}
END_TEST

START_TEST(test_trim) {
  taco_event e = {0, TACO_EVENT_DON, .detail_int = {0}};

  taco_section *s = taco_section_create_();

  for (int i = 0; i < 750; ++i) {
    taco_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taco_section_size(s), 750);

  taco_section_trim_(s);
  ck_assert_int_eq(taco_section_size(s), 750);

  taco_section_free_(s);
}
END_TEST

START_TEST(test_balloons) {
  static const taco_event input[] = {
      {0, TACO_EVENT_BALLOON, .detail_int = {0}},
      {480, TACO_EVENT_BALLOON, .detail_int = {0}},
      {960, TACO_EVENT_DON, .detail_int = {0}},
      {1440, TACO_EVENT_KUSUDAMA, .detail_int = {0}},
  };

  static const taco_event expected[] = {
      {0, TACO_EVENT_BALLOON, .detail_int = {20}},
      {480, TACO_EVENT_BALLOON, .detail_int = {20}},
      {960, TACO_EVENT_DON, .detail_int = {0}},
      {1440, TACO_EVENT_KUSUDAMA, .detail_int = {20}},
  };

  const int balloons[] = {20, 20, 20};

  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, input, 4);
  taco_section_set_balloons_(s, balloons, 3);
  ck_assert_mem_eq(taco_section_begin(s), expected, 4 * sizeof(taco_event));
  taco_section_free_(s);
}
END_TEST

START_TEST(test_time) {
  static const taco_event events[] = {
      {0, TACO_EVENT_BPM, .detail_float = {120.0}},
      {0, TACO_EVENT_MEASURE},
      {96, TACO_EVENT_MEASURE},
      {192, TACO_EVENT_BPM, .detail_float = {180.0}},
      {192, TACO_EVENT_MEASURE},
      {288, TACO_EVENT_MEASURE},
      {384, TACO_EVENT_MEASURE},
      {480, TACO_EVENT_MEASURE},
  };

  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, events, 8);
  taco_section_cache_seconds_(s);

  const taco_event *i = taco_section_locate(s, 1);
  ck_assert_double_eq(taco_event_seconds(i, s), 0.0);
  i = taco_section_locate(s, 2);
  ck_assert_double_eq(taco_event_seconds(i, s), 2.0);
  i = taco_section_locate(s, 4);
  ck_assert_double_eq(taco_event_seconds(i, s), 4.0);
  i = taco_section_locate(s, 7);
  ck_assert_double_eq(taco_event_seconds(i, s), 8.0);

  taco_section_free_(s);
}
END_TEST

START_TEST(test_delay) {
  static const taco_event events[] = {
      {0, TACO_EVENT_BPM, .detail_float = {120.0}},
      {0, TACO_EVENT_MEASURE},
      {96, TACO_EVENT_MEASURE},
      {192, TACO_EVENT_DELAY, .detail_float = {1.0}},
      {192, TACO_EVENT_BPM, .detail_float = {180.0}},
      {192, TACO_EVENT_MEASURE},
      {288, TACO_EVENT_MEASURE},
      {384, TACO_EVENT_MEASURE},
      {480, TACO_EVENT_MEASURE},
  };

  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, events, 9);
  taco_section_cache_seconds_(s);

  const taco_event *i;
  i = taco_section_locate(s, 1);
  ck_assert_double_eq(taco_event_seconds(i, s), 0.0);
  i = taco_section_locate(s, 2);
  ck_assert_double_eq(taco_event_seconds(i, s), 2.0);
  i = taco_section_locate(s, 5);
  ck_assert_double_eq(taco_event_seconds(i, s), 5.0);
  i = taco_section_locate(s, 8);
  ck_assert_double_eq(taco_event_seconds(i, s), 9.0);

  taco_section_free_(s);
}
END_TEST

TCase *case_section(void) {
  TCase *c = tcase_create("section");
  tcase_add_test(c, test_create);
  tcase_add_test(c, test_clone);
  tcase_add_test(c, test_push);
  tcase_add_test(c, test_push_many);
  tcase_add_test(c, test_reserve);
  tcase_add_test(c, test_clear);
  tcase_add_test(c, test_trim);
  tcase_add_test(c, test_balloons);
  tcase_add_test(c, test_time);
  tcase_add_test(c, test_delay);
  return c;
}
