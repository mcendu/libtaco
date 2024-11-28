// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>

#include "course.h"
#include "section.h"
#include "taco.h"

START_TEST(test_attach_branch) {
  static const taiko_event e[3] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {60, TAIKO_EVENT_DON, .detail_int = {0}},
      {120, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  taiko_course *c = taiko_course_create_();

  ck_assert_ptr_null(
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL));

  {
    taiko_section *normal = taiko_section_create_();
    taiko_section_push_many_(normal, e, 3);
    taiko_course_attach_branch_(c, normal, TAIKO_SIDE_LEFT,
                                TAIKO_BRANCH_NORMAL);
  }

  const taiko_section *b =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_eq(taiko_section_size(b), 3);

  taiko_course_free_(c);
}
END_TEST

START_TEST(test_setup_branching) {
  static const taiko_event e[3] = {
      {0, TAIKO_EVENT_DON, .detail_int = {0}},
      {60, TAIKO_EVENT_DON, .detail_int = {0}},
      {120, TAIKO_EVENT_DON, .detail_int = {0}},
  };

  taiko_course *c = taiko_course_create_();

  ck_assert_ptr_null(
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL));

  {
    taiko_section *normal = taiko_section_create_();
    taiko_section_push_many_(normal, e, 3);
    taiko_course_attach_branch_(c, normal, TAIKO_SIDE_LEFT,
                                TAIKO_BRANCH_NORMAL);
  }

  ck_assert_int_eq(taiko_course_setup_branching_(c), 0);

  const taiko_section *normal =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  const taiko_section *advanced =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_ADVANCED);
  const taiko_section *master =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_MASTER);

  ck_assert_ptr_nonnull(normal);
  ck_assert_ptr_nonnull(advanced);
  ck_assert_ptr_nonnull(master);
  ck_assert_ptr_ne(normal, advanced);
  ck_assert_ptr_ne(normal, master);
  ck_assert_ptr_ne(advanced, master);
  ck_assert_mem_eq(taiko_section_begin(normal), taiko_section_begin(advanced),
                   3 * sizeof(taiko_event));
  ck_assert_mem_eq(taiko_section_begin(normal), taiko_section_begin(master),
                   3 * sizeof(taiko_event));

  taiko_course_free_(c);
}
END_TEST

START_TEST(test_balloons_before) {
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

  const int balloons[3] = {20, 20, 20};

  taiko_course *c = taiko_course_create_();
  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, input, 4);

  taiko_course_set_balloons_(c, balloons, 3, TAIKO_SIDE_LEFT,
                             TAIKO_BRANCH_NORMAL);
  taiko_course_attach_branch_(c, s, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);

  ck_assert_mem_eq(taiko_section_begin(s), expected, 4 * sizeof(taiko_event));

  taiko_course_free_(c);
}
END_TEST

START_TEST(test_balloons_after) {
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

  taiko_course *c = taiko_course_create_();
  taiko_section *s = taiko_section_create_();
  taiko_section_push_many_(s, input, 4);

  taiko_course_attach_branch_(c, s, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  taiko_course_set_balloons_(c, balloons, 3, TAIKO_SIDE_LEFT,
                             TAIKO_BRANCH_NORMAL);

  ck_assert_mem_eq(taiko_section_begin(s), expected, 4 * sizeof(taiko_event));

  taiko_course_free_(c);
}
END_TEST

START_TEST(test_merge) {
  static const taiko_event input_left[] = {
      {0, TAIKO_EVENT_DON},
  };
  static const taiko_event input_right[] = {
      {0, TAIKO_EVENT_KAT},
  };

  taiko_course *course = taiko_course_create_();

  {
    taiko_course *rc = taiko_course_create_();
    taiko_section *lb = taiko_section_create_();
    taiko_section *rb = taiko_section_create_();

    taiko_section_push_many_(lb, input_left, 1);
    taiko_course_attach_branch_(course, lb, TAIKO_SIDE_LEFT,
                                TAIKO_BRANCH_NORMAL);
    taiko_course_set_style_(course, TAIKO_STYLE_TJA_L_);

    taiko_section_push_many_(rb, input_right, 1);
    taiko_course_attach_branch_(rc, rb, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
    taiko_course_set_style_(rc, TAIKO_STYLE_TJA_R_);

    taiko_course_merge_(course, rc);
  }

  {
    const taiko_section *lb =
        taiko_course_get_branch(course, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
    const taiko_section *rb =
        taiko_course_get_branch(course, TAIKO_SIDE_RIGHT, TAIKO_BRANCH_NORMAL);

    ck_assert_ptr_nonnull(lb);
    ck_assert_ptr_nonnull(rb);
    ck_assert_ptr_ne(lb, rb);
    ck_assert_int_eq(taiko_event_type(taiko_section_begin(lb)),
                     TAIKO_EVENT_DON);
    ck_assert_int_eq(taiko_event_type(taiko_section_begin(rb)),
                     TAIKO_EVENT_KAT);
  }

  taiko_course_free_(course);
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
