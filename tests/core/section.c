// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

#include "note.h" // IWYU pragma: keep; for definition of taiko_event
#include "section.h"
#include "taco.h"

START_TEST(test_create) {
  taiko_section *s = taiko_section_create_();
  ck_assert_int_eq(taiko_section_size(s), 0);
  ck_assert_ptr_eq(taiko_section_begin(s), taiko_section_end(s));
  taiko_section_free_(s);
}
END_TEST

START_TEST(test_push) {
  static const taiko_event e = {
      .time = 0,
      .type = TAIKO_EVENT_DON,
      .detail_int = {0},
  };

  taiko_section *s = taiko_section_create_();
  taiko_section_push_(s, &e);
  ck_assert_int_eq(taiko_section_size(s), 1);
  ck_assert_mem_eq(taiko_section_begin(s), &e, sizeof(taiko_event));
  taiko_section_free_(s);
}
END_TEST

START_TEST(test_push_many) {
  static const taiko_event e[3] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {60, TAIKO_EVENT_DON, .detail_int = {0}},
      {120, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, e, 3);
  ck_assert_int_eq(taiko_section_size(s), 3);

  const taiko_event *i = taiko_section_begin(s);
  ck_assert_mem_eq(i, &e[0], sizeof(taiko_event));
  i = taiko_event_next(i);
  ck_assert_mem_eq(i, &e[1], sizeof(taiko_event));
  i = taiko_event_next(i);
  ck_assert_mem_eq(i, &e[2], sizeof(taiko_event));

  taiko_section_free_(s);
}
END_TEST

START_TEST(test_clone) {
  static const taiko_event e[3] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {60, TAIKO_EVENT_DON, .detail_int = {0}},
      {120, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, e, 3);

  taiko_section *other = taiko_section_clone_(s);
  ck_assert_ptr_nonnull(other);
  ck_assert_ptr_ne(s, other);
  ck_assert_ptr_ne(taiko_section_begin(s), taiko_section_begin(other));
  ck_assert_mem_eq(taiko_section_begin(s), taiko_section_begin(other),
                   3 * sizeof(taiko_event));

  taiko_section_free_(other);
  taiko_section_free_(s);
}

START_TEST(test_reserve) {
  taiko_event e = {0, TAIKO_EVENT_DON, .detail_int = {0}};

  taiko_section *s = taiko_section_create_();

  for (int i = 0; i < 1024; ++i) {
    taiko_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taiko_section_size(s), 1024);
  taiko_section_free_(s);
}
END_TEST

START_TEST(test_clear) {
  taiko_event e = {0, TAIKO_EVENT_DON, .detail_int = {0}};

  taiko_section *s = taiko_section_create_();

  for (int i = 0; i < 1024; ++i) {
    taiko_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taiko_section_size(s), 1024);

  taiko_section_clear_(s);
  ck_assert_int_eq(taiko_section_size(s), 0);

  taiko_section_free_(s);
}
END_TEST

START_TEST(test_trim) {
  taiko_event e = {0, TAIKO_EVENT_DON, .detail_int = {0}};

  taiko_section *s = taiko_section_create_();

  for (int i = 0; i < 750; ++i) {
    taiko_section_push_(s, &e);
    e.time += 30;
  }

  ck_assert_int_eq(taiko_section_size(s), 750);

  taiko_section_trim_(s);
  ck_assert_int_eq(taiko_section_size(s), 750);

  taiko_section_free_(s);
}
END_TEST

START_TEST(test_balloons) {
  static const taiko_event input[] = {
      {0, TAIKO_EVENT_BALLOON, .detail_int = {0}},
      {480, TAIKO_EVENT_BALLOON, .detail_int = {0}},
      {960, TAIKO_EVENT_DON, .detail_int = {0}},
      {1440, TAIKO_EVENT_KUSUDAMA, .detail_int = {0}},
  };

  static const taiko_event expected[] = {
      {0, TAIKO_EVENT_BALLOON, .detail_int = {20}},
      {480, TAIKO_EVENT_BALLOON, .detail_int = {20}},
      {960, TAIKO_EVENT_DON, .detail_int = {0}},
      {1440, TAIKO_EVENT_KUSUDAMA, .detail_int = {20}},
  };

  const int balloons[] = {20, 20, 20};

  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, input, 4);
  taiko_section_set_balloons_(s, balloons, 3);
  ck_assert_mem_eq(taiko_section_begin(s), expected, 4 * sizeof(taiko_event));
  taiko_section_free_(s);
}
END_TEST

START_TEST(test_delete) {
  static const taiko_event input[] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {480, TAIKO_EVENT_DON, .detail_int = {0}},
      {960, TAIKO_EVENT_KAT, .detail_int = {0}},
      {1440, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  static const taiko_event expected[] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {480, TAIKO_EVENT_DON, .detail_int = {0}},
      {1440, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, input, 4);

  taiko_event *i = taiko_section_locate_mut_(s, 2);
  ck_assert_mem_eq(i, &input[2], sizeof(taiko_event));
  taiko_section_delete_(s, i);
  ck_assert_int_eq(taiko_section_size(s), 3);
  ck_assert_mem_eq(i, &input[3], sizeof(taiko_event));
  ck_assert_mem_eq(taiko_section_begin(s), expected, 3 * sizeof(taiko_event));
  taiko_section_free_(s);
}
END_TEST

START_TEST(test_time) {
  static const taiko_event events[] = {
      {0, TAIKO_EVENT_BPM, .detail_float = {120.0}},
      {0, TAIKO_EVENT_MEASURE},
      {96, TAIKO_EVENT_MEASURE},
      {192, TAIKO_EVENT_BPM, .detail_float = {180.0}},
      {192, TAIKO_EVENT_MEASURE},
      {288, TAIKO_EVENT_MEASURE},
      {384, TAIKO_EVENT_MEASURE},
      {480, TAIKO_EVENT_MEASURE},
  };

  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, events, 8);
  taiko_section_set_bpm_(s, 120.0);

  const taiko_event *i = taiko_section_locate(s, 1);
  ck_assert_double_eq(taiko_event_seconds(i, s), 0.0);
  i = taiko_section_locate(s, 2);
  ck_assert_double_eq(taiko_event_seconds(i, s), 2.0);
  i = taiko_section_locate(s, 4);
  ck_assert_double_eq(taiko_event_seconds(i, s), 4.0);
  i = taiko_section_locate(s, 7);
  ck_assert_double_eq(taiko_event_seconds(i, s), 8.0);
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
  tcase_add_test(c, test_delete);
  tcase_add_test(c, test_time);
  return c;
}
