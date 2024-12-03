// SPDX-License-Identifier: BSD-2-Clause
#include "tja/parser.h"
#include <check.h>

#include "taco.h"

static taco_parser *parser;

static void setup(void) { parser = taco_parser_tja_create(); }

static void teardown(void) { taco_parser_free(parser); }

START_TEST(test_basic) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/basic.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "Example");

  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taco_course_bpm(c), 130);
  ck_assert_int_eq(taco_course_class(c), TACO_CLASS_ONI);
  ck_assert_double_eq(taco_course_level(c), 1);

  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taco_section_size(s), 2);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 0)),
                   TACO_EVENT_MEASURE);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 1)), TACO_EVENT_DON);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_empty) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/empty.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_ptr_null(taco_courseset_title(set));
  ck_assert_ptr_null(taco_courseset_get_course(set, TACO_CLASS_ONI));
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_bom) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/bom.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "Example");

  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taco_course_bpm(c), 130);
  ck_assert_int_eq(taco_course_class(c), TACO_CLASS_ONI);
  ck_assert_double_eq(taco_course_level(c), 1);

  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taco_section_size(s), 2);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 0)),
                   TACO_EVENT_MEASURE);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 1)), TACO_EVENT_DON);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_crlf) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/crlf.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "Example");

  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taco_course_bpm(c), 130);
  ck_assert_int_eq(taco_course_class(c), TACO_CLASS_ONI);
  ck_assert_double_eq(taco_course_level(c), 1);

  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taco_section_size(s), 2);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 0)),
                   TACO_EVENT_MEASURE);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 1)), TACO_EVENT_DON);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_eof) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/eof.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "Example");

  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_double_eq(taco_course_bpm(c), 130);
  ck_assert_int_eq(taco_course_class(c), TACO_CLASS_ONI);
  ck_assert_double_eq(taco_course_level(c), 1);

  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  ck_assert_int_eq(taco_section_size(s), 2);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 0)),
                   TACO_EVENT_MEASURE);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 1)), TACO_EVENT_DON);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_whitespace) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/whitespace.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "I don't have whitespaces");

  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  ck_assert_int_eq(taco_course_class(c), TACO_CLASS_ONI);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_commands) {
  static const int types[] = {
      TACO_EVENT_SCROLL,  TACO_EVENT_MEASURE, TACO_EVENT_DON,
      TACO_EVENT_KAT,     TACO_EVENT_DON_BIG, TACO_EVENT_SCROLL,
      TACO_EVENT_KAT_BIG,
  };

  taco_courseset *set = taco_parser_parse_file(parser, "assets/commands.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  for (int i = 0; i < 7; ++i) {
    ck_assert_int_eq(taco_event_type(taco_section_locate(s, i)), types[i]);
  }
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_balloon) {
  static const int count[] = {10, 20};

  taco_courseset *set = taco_parser_parse_file(parser, "assets/balloon.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);

  int j = 0;
  taco_section_foreach(i, s) {
    if (taco_event_type(i) == TACO_EVENT_BALLOON) {
      ck_assert_int_eq(taco_event_hits(i), count[j]);
      j += 1;
    }
  }
  ck_assert_int_eq(j, 2);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_division) {
  static const int time[] = {0, 48};
  static const int type[] = {TACO_EVENT_MEASURE, TACO_EVENT_DON};

  taco_courseset *set = taco_parser_parse_file(parser, "assets/division.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);

  ck_assert_int_eq(taco_section_size(s), 2);
  for (int i = 0; i < taco_section_size(s); ++i) {
    const taco_event *e = taco_section_locate(s, i);
    ck_assert_int_eq(taco_event_time(e), time[i]);
    ck_assert_int_eq(taco_event_type(e), type[i]);
  }

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_emptymeasures) {
  static const int time[] = {0, 96};
  taco_courseset *set =
      taco_parser_parse_file(parser, "assets/emptymeasures.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s = taco_course_get_branch(c, 0, 0);
  for (int i = 0; i < taco_section_size(s); ++i) {
    const taco_event *e = taco_section_locate(s, i);
    ck_assert_int_eq(taco_event_time(e), time[i]);
  }
  taco_courseset_free(set);
}

START_TEST(test_measures) {
  static const int time[] = {0, 0, 96, 96};
  static const int type[] = {TACO_EVENT_MEASURE, TACO_EVENT_DON,
                             TACO_EVENT_MEASURE, TACO_EVENT_KAT};

  taco_courseset *set = taco_parser_parse_file(parser, "assets/measures.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);

  ck_assert_int_eq(taco_section_size(s), 4);
  for (int i = 0; i < taco_section_size(s); ++i) {
    const taco_event *e = taco_section_locate(s, i);
    ck_assert_int_eq(taco_event_time(e), time[i]);
    ck_assert_int_eq(taco_event_type(e), type[i]);
  }

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_branch) {
  static const int lengths[] = {11, 12, 13};

  static const int types[] = {
      TACO_EVENT_BRANCH_START, TACO_EVENT_MEASURE,      TACO_EVENT_DON,
      TACO_EVENT_KAT,          TACO_EVENT_BRANCH_CHECK, TACO_EVENT_MEASURE,
      TACO_EVENT_BRANCH_JUMP,  TACO_EVENT_MEASURE,
  };

  taco_courseset *set = taco_parser_parse_file(parser, "assets/branch.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);

  for (int i = 0; i < 3; ++i) {
    const taco_section *s = taco_course_get_branch(c, TACO_SIDE_LEFT, i);
    ck_assert_ptr_nonnull(s);
    ck_assert_int_eq(taco_section_size(s), lengths[i]);

    for (int i = 0; i < 8; ++i) {
      ck_assert_int_eq(taco_event_type(taco_section_locate(s, i)), types[i]);
    }
  }

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_double) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/double.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_int_eq(taco_course_style(c), TACO_STYLE_COUPLE);

  const taco_section *l =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(l);
  ck_assert_int_eq(taco_event_type(taco_section_locate(l, 1)), TACO_EVENT_DON);

  const taco_section *r =
      taco_course_get_branch(c, TACO_SIDE_RIGHT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(r);
  ck_assert_int_eq(taco_event_type(taco_section_locate(r, 1)), TACO_EVENT_KAT);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_badmeasure) {
  ck_assert_ptr_null(taco_parser_parse_file(parser, "assets/badmeasure.tja"));
}

START_TEST(test_checkpoint) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/checkpoint.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s = taco_course_get_branch(c, 0, 0);

  ck_assert_int_eq(taco_section_size(s), 8);

  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 1)),
                   TACO_EVENT_BALLOON);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 2)),
                   TACO_EVENT_ROLL_END);

  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 4)),
                   TACO_EVENT_KUSUDAMA);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 5)),
                   TACO_EVENT_ROLL_CHECKPOINT);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 6)),
                   TACO_EVENT_ROLL_CHECKPOINT);
  ck_assert_int_eq(taco_event_type(taco_section_locate(s, 7)),
                   TACO_EVENT_ROLL_END);

  taco_courseset_free(set);
}

TCase *case_parser(void) {
  TCase *c = tcase_create("parser");
  tcase_add_checked_fixture(c, setup, teardown);
  tcase_add_test(c, test_badmeasure);
  tcase_add_test(c, test_balloon);
  tcase_add_test(c, test_basic);
  tcase_add_test(c, test_bom);
  tcase_add_test(c, test_branch);
  tcase_add_test(c, test_commands);
  tcase_add_test(c, test_checkpoint);
  tcase_add_test(c, test_crlf);
  tcase_add_test(c, test_division);
  tcase_add_test(c, test_double);
  tcase_add_test(c, test_empty);
  tcase_add_test(c, test_emptymeasures);
  tcase_add_test(c, test_eof);
  tcase_add_test(c, test_measures);
  tcase_add_test(c, test_whitespace);
  return c;
}
