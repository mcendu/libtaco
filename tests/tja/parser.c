// SPDX-License-Identifier: BSD-2-Clause
#include "tja/parser.h"
#include <check.h>

#include "taiko.h"

static tja_parser *parser;

static void setup(void) { parser = tja_parser_create_(); }

static void teardown(void) { tja_parser_free_(parser); }

START_TEST(test_basic) {
  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/basic.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taiko_courseset_title(set), "Example");

  const taiko_course *c = taiko_courseset_get_course(set, TAIKO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taiko_course_bpm(c), 130);
  ck_assert_int_eq(taiko_course_class(c), TAIKO_CLASS_ONI);
  ck_assert_double_eq(taiko_course_level(c), 1);

  const taiko_section *s =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taiko_section_size(s), 2);
  ck_assert_int_eq(taiko_event_type(taiko_section_locate(s, 0)),
                   TAIKO_EVENT_MEASURE);
  ck_assert_int_eq(taiko_event_type(taiko_section_locate(s, 1)),
                   TAIKO_EVENT_DON);

  taiko_courseset_free(set);
}
END_TEST

START_TEST(test_empty) {
  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/empty.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_ptr_null(taiko_courseset_title(set));
  ck_assert_ptr_null(taiko_courseset_get_course(set, TAIKO_CLASS_ONI));
  taiko_courseset_free(set);
}
END_TEST

START_TEST(test_crlf) {
  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/crlf.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taiko_courseset_title(set), "Example");

  const taiko_course *c = taiko_courseset_get_course(set, TAIKO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taiko_course_bpm(c), 130);
  ck_assert_int_eq(taiko_course_class(c), TAIKO_CLASS_ONI);
  ck_assert_double_eq(taiko_course_level(c), 1);

  const taiko_section *s =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taiko_section_size(s), 2);
  ck_assert_int_eq(taiko_event_type(taiko_section_locate(s, 0)),
                   TAIKO_EVENT_MEASURE);
  ck_assert_int_eq(taiko_event_type(taiko_section_locate(s, 1)),
                   TAIKO_EVENT_DON);

  taiko_courseset_free(set);
}
END_TEST

START_TEST(test_commands) {
  static const int types[] = {
      TAIKO_EVENT_SCROLL,  TAIKO_EVENT_MEASURE, TAIKO_EVENT_DON,
      TAIKO_EVENT_KAT,     TAIKO_EVENT_DON_BIG, TAIKO_EVENT_SCROLL,
      TAIKO_EVENT_KAT_BIG,
  };

  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/commands.tja");
  const taiko_course *c = taiko_courseset_get_course(set, TAIKO_CLASS_ONI);
  const taiko_section *s =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);
  for (int i = 0; i < 7; ++i) {
    ck_assert_int_eq(taiko_event_type(taiko_section_locate(s, i)), types[i]);
  }
  taiko_courseset_free(set);
}
END_TEST

START_TEST(test_balloon) {
  static const int count[] = {10, 20};

  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/balloon.tja");
  const taiko_course *c = taiko_courseset_get_course(set, TAIKO_CLASS_ONI);
  const taiko_section *s =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);

  int j = 0;
  taiko_section_foreach (i, s) {
    if (taiko_event_type(i) == TAIKO_EVENT_BALLOON) {
      ck_assert_int_eq(taiko_event_hits(i), count[j]);
      j += 1;
    }
  }
  ck_assert_int_eq(j, 2);

  taiko_courseset_free(set);
}

START_TEST(test_division) {
  static const int time[] = {0, 48};
  static const int type[] = {TAIKO_EVENT_MEASURE, TAIKO_EVENT_DON};

  taiko_courseset *set = tja_parser_parse_file_(parser, "assets/division.tja");
  const taiko_course *c = taiko_courseset_get_course(set, TAIKO_CLASS_ONI);
  const taiko_section *s =
      taiko_course_get_branch(c, TAIKO_SIDE_LEFT, TAIKO_BRANCH_NORMAL);

  for (int i = 0; i < 2; ++i) {
    const taiko_event *e = taiko_section_locate(s, i);
    ck_assert_int_eq(taiko_event_time(e), time[i]);
    ck_assert_int_eq(taiko_event_type(e), type[i]);
  }

  taiko_courseset_free(set);
}
END_TEST

TCase *case_parser(void) {
  TCase *c = tcase_create("parser");
  tcase_add_checked_fixture(c, setup, teardown);
  tcase_add_test(c, test_balloon);
  tcase_add_test(c, test_basic);
  tcase_add_test(c, test_commands);
  tcase_add_test(c, test_crlf);
  tcase_add_test(c, test_division);
  tcase_add_test(c, test_empty);
  return c;
}
