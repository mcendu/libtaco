// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

#include <math.h>
#include <stddef.h>

#include "note.h"
#include "taco.h"

START_TEST(test_basic) {
  static const taco_event e = {96, TACO_EVENT_DON, .detail_int = {0}};
  ck_assert_int_eq(taco_event_time(&e), e.time);
  ck_assert_int_eq(taco_event_type(&e), e.type);

  ck_assert_int_eq(taco_event_detail_int(&e), e.detail_int.value);
  ck_assert_float_nan(taco_event_detail_float(&e));
}
END_TEST

START_TEST(test_float) {
  static const taco_event bpm = {0, TACO_EVENT_BPM, .detail_float = {222.22}};
  static const taco_event scroll = {0, TACO_EVENT_SCROLL,
                                    .detail_float = {0.5}};

  ck_assert_int_eq(taco_event_detail_int(&bpm), -1);
  ck_assert_int_eq(taco_event_detail_int(&scroll), -1);

  ck_assert_float_eq(taco_event_detail_float(&bpm), 222.22);
  ck_assert_float_eq(taco_event_detail_float(&scroll), 0.5);
}
END_TEST

START_TEST(test_branchstart) {
  static const taco_event branchstart = {
      .time = 0,
      .type = TACO_EVENT_BRANCH_START,
      .branch_start =
          {
              .good = 30,
              .good_big = 40,
              .ok = 10,
              .ok_big = 20,
              .roll = 1,
              .bad = 0,
          },
  };

  taco_branch_scoring scoring;

  ck_assert_int_eq(taco_event_detail_int(&branchstart), -1);
  ck_assert_float_nan(taco_event_detail_float(&branchstart));

  ck_assert_int_eq(taco_event_branch_scoring(&branchstart, &scoring), 0);
  ck_assert_int_eq(scoring.good, branchstart.branch_start.good);
  ck_assert_int_eq(scoring.ok, branchstart.branch_start.ok);
  ck_assert_int_eq(scoring.good_big, branchstart.branch_start.good_big);
  ck_assert_int_eq(scoring.ok_big, branchstart.branch_start.ok_big);
  ck_assert_int_eq(scoring.roll, branchstart.branch_start.roll);
  ck_assert_int_eq(scoring.bad, branchstart.branch_start.bad);
}
END_TEST

START_TEST(test_branchcond) {
  static const taco_event cond = {
      .time = 0,
      .type = TACO_EVENT_BRANCH_CHECK,
      .branch_cond =
          {
              .advanced = 1200,
              .master = 1600,
          },
  };

  int advanced, master;

  ck_assert_int_eq(taco_event_detail_int(&cond), -1);
  ck_assert_float_nan(taco_event_detail_float(&cond));

  ck_assert_int_eq(taco_event_branch_thresholds(&cond, &advanced, &master), 0);
  ck_assert_int_eq(advanced, cond.branch_cond.advanced);
  ck_assert_int_eq(master, cond.branch_cond.master);
}
END_TEST

START_TEST(test_null) {
  static const taco_event branchstart = {
      .time = 0,
      .type = TACO_EVENT_BRANCH_START,
      .branch_start =
          {
              .good = 30,
              .good_big = 40,
              .ok = 10,
              .ok_big = 20,
              .roll = 1,
              .bad = 0,
          },
  };
  static const taco_event cond = {
      .time = 0,
      .type = TACO_EVENT_BRANCH_CHECK,
      .branch_cond =
          {
              .advanced = 1200,
              .master = 1600,
          },
  };

  ck_assert_int_eq(taco_event_branch_scoring(&branchstart, NULL), 0);
  ck_assert_int_eq(taco_event_branch_thresholds(&cond, NULL, NULL), 0);
}
END_TEST

TCase *case_note() {
  TCase *c = tcase_create("section");
  tcase_add_test(c, test_basic);
  tcase_add_test(c, test_float);
  tcase_add_test(c, test_branchstart);
  tcase_add_test(c, test_branchcond);
  tcase_add_test(c, test_null);
  return c;
}
