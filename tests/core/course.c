// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

#include "course.h"
#include "section.h"
#include "taco.h"

START_TEST(test_attach_branch) {
  static const taco_event e[3] = {
      {0, TACO_EVENT_DON, .detail_int = {0}},
      {60, TACO_EVENT_DON, .detail_int = {0}},
      {120, TACO_EVENT_DON, .detail_int = {0}},
  };

  taco_course *c = taco_course_create_();

  ck_assert_ptr_null(
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL));

  {
    taco_section *normal = taco_section_create_();
    taco_section_push_many_(normal, e, 3);
    taco_course_attach_branch_(c, normal, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  }

  const taco_section *b =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_eq(taco_section_size(b), 3);

  taco_course_free_(c);
}
END_TEST

START_TEST(test_setup_branching) {
  static const taco_event e[3] = {
      {0, TACO_EVENT_DON, .detail_int = {0}},
      {60, TACO_EVENT_DON, .detail_int = {0}},
      {120, TACO_EVENT_DON, .detail_int = {0}},
  };

  taco_course *c = taco_course_create_();

  ck_assert_ptr_null(
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL));

  {
    taco_section *normal = taco_section_create_();
    taco_section_push_many_(normal, e, 3);
    taco_course_attach_branch_(c, normal, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  }

  ck_assert_int_eq(taco_course_setup_branching_(c), 0);

  const taco_section *normal =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  const taco_section *advanced =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_ADVANCED);
  const taco_section *master =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_MASTER);

  ck_assert_ptr_nonnull(normal);
  ck_assert_ptr_nonnull(advanced);
  ck_assert_ptr_nonnull(master);
  ck_assert_ptr_ne(normal, advanced);
  ck_assert_ptr_ne(normal, master);
  ck_assert_ptr_ne(advanced, master);
  ck_assert_mem_eq(taco_section_begin(normal), taco_section_begin(advanced),
                   3 * sizeof(taco_event));
  ck_assert_mem_eq(taco_section_begin(normal), taco_section_begin(master),
                   3 * sizeof(taco_event));

  taco_course_free_(c);
}
END_TEST

START_TEST(test_balloons_before) {
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

  const int balloons[3] = {20, 20, 20};

  taco_course *c = taco_course_create_();
  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, input, 4);

  taco_course_set_balloons_(c, balloons, 3, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  taco_course_attach_branch_(c, s, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);

  ck_assert_mem_eq(taco_section_begin(s), expected, 4 * sizeof(taco_event));

  taco_course_free_(c);
}
END_TEST

START_TEST(test_balloons_after) {
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

  taco_course *c = taco_course_create_();
  taco_section *s = taco_section_create_();
  taco_section_push_many_(s, input, 4);

  taco_course_attach_branch_(c, s, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  taco_course_set_balloons_(c, balloons, 3, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);

  ck_assert_mem_eq(taco_section_begin(s), expected, 4 * sizeof(taco_event));

  taco_course_free_(c);
}
END_TEST

START_TEST(test_merge) {
  static const taco_event input_left[] = {
      {0, TACO_EVENT_DON},
  };
  static const taco_event input_right[] = {
      {0, TACO_EVENT_KAT},
  };

  taco_course *course = taco_course_create_();

  {
    taco_course *rc = taco_course_create_();
    taco_section *lb = taco_section_create_();
    taco_section *rb = taco_section_create_();

    taco_section_push_many_(lb, input_left, 1);
    taco_course_attach_branch_(course, lb, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
    taco_course_set_style_(course, TACO_STYLE_DOUBLE);

    taco_section_push_many_(rb, input_right, 1);
    taco_course_attach_branch_(rc, rb, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
    taco_course_set_style_(rc, TACO_STYLE_2P_ONLY);

    taco_course_merge_(course, rc);
  }

  ck_assert_int_eq(taco_course_style(course), TACO_STYLE_DOUBLE);

  {
    const taco_section *lb =
        taco_course_get_branch(course, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
    const taco_section *rb =
        taco_course_get_branch(course, TACO_SIDE_RIGHT, TACO_BRANCH_NORMAL);

    ck_assert_ptr_nonnull(lb);
    ck_assert_ptr_nonnull(rb);
    ck_assert_ptr_ne(lb, rb);
    ck_assert_int_eq(taco_event_type(taco_section_begin(lb)), TACO_EVENT_DON);
    ck_assert_int_eq(taco_event_type(taco_section_begin(rb)), TACO_EVENT_KAT);
  }

  taco_course_free_(course);
}
END_TEST

TCase *case_course(void) {
  TCase *c = tcase_create("course");
  tcase_add_test(c, test_attach_branch);
  tcase_add_test(c, test_setup_branching);
  tcase_add_test(c, test_balloons_before);
  tcase_add_test(c, test_balloons_after);
  tcase_add_test(c, test_merge);
  return c;
}
