// SPDX-License-Identifier: BSD-2-Clause
#include "tja/parser.h"
#include <check.h>

#include "taco.h"
#include "tacoassert.h"

static taco_parser *parser;
static assert_section_state *assert_section;

static void setup(void) {
  parser = taco_parser_tja_create();
  assert_section = assert_section_setup();
}

static void teardown(void) {
  taco_parser_free(parser);
  assert_section_teardown(assert_section);
}

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
  assert_section_eq(s, "assets/basic.txt", assert_section);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_empty) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/empty.tja");
  ck_assert_ptr_nonnull(set);
  ck_assert_str_eq(taco_courseset_title(set), "Untitled");
  ck_assert_ptr_null(taco_courseset_get_course(set, TACO_CLASS_ONI));
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_bom) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/bom.tja");
  ck_assert_ptr_nonnull(set);
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  assert_section_eq(s, "assets/basic.txt", assert_section);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_crlf) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/crlf.tja");
  ck_assert_ptr_nonnull(set);
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_ptr_nonnull(c);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  assert_section_eq(s, "assets/basic.txt", assert_section);

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
  assert_section_eq(s, "assets/basic.txt", assert_section);

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

  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  ck_assert_ptr_nonnull(s);
  assert_section_eq(s, "assets/whitespace.txt", assert_section);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_commands) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/commands.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  assert_section_eq(s, "assets/commands.txt", assert_section);
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_balloon) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/balloon.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  assert_section_eq(s, "assets/balloon.txt", assert_section);
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_division) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/division.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  assert_section_eq(s, "assets/division.txt", assert_section);
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_emptymeasures) {
  taco_courseset *set =
      taco_parser_parse_file(parser, "assets/emptymeasures.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s = taco_course_get_branch(c, 0, 0);
  assert_section_eq(s, "assets/emptymeasures.txt", assert_section);
  taco_courseset_free(set);
}

START_TEST(test_measures) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/measures.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s =
      taco_course_get_branch(c, TACO_SIDE_LEFT, TACO_BRANCH_NORMAL);
  assert_section_eq(s, "assets/measures.txt", assert_section);
  taco_courseset_free(set);
}
END_TEST

START_TEST(test_branch) {
  static const char *expected[] = {
      "assets/branch_n.txt",
      "assets/branch_a.txt",
      "assets/branch_m.txt",
  };

  taco_courseset *set = taco_parser_parse_file(parser, "assets/branch.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);

  const taco_section *s = taco_course_get_branch(c, TACO_SIDE_LEFT, _i);
  assert_section_eq(s, expected[_i], assert_section);

  taco_courseset_free(set);
}
END_TEST

START_TEST(test_double) {
  static const char *expected[] = {
      "assets/double_l.txt",
      "assets/double_r.txt",
  };

  taco_courseset *set = taco_parser_parse_file(parser, "assets/double.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  ck_assert_int_eq(taco_course_style(c), TACO_STYLE_COUPLE);

  const taco_section *s = taco_course_get_branch(c, _i, TACO_BRANCH_NORMAL);
  assert_section_eq(s, expected[_i], assert_section);
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
  assert_section_eq(s, "assets/checkpoint.txt", assert_section);
  taco_courseset_free(set);
}

START_TEST(test_label) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/label.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s = taco_course_get_branch(c, 0, 0);
  assert_section_eq(s, "assets/label.txt", assert_section);
  taco_courseset_free(set);
}

START_TEST(test_hand) {
  taco_courseset *set = taco_parser_parse_file(parser, "assets/hand.tja");
  const taco_course *c = taco_courseset_get_course(set, TACO_CLASS_ONI);
  const taco_section *s = taco_course_get_branch(c, 0, 0);
  assert_section_eq(s, "assets/hand.txt", assert_section);
  taco_courseset_free(set);
}

TCase *case_parser(void) {
  TCase *c = tcase_create("parser");
  tcase_add_checked_fixture(c, setup, teardown);
  tcase_add_test(c, test_badmeasure);
  tcase_add_test(c, test_balloon);
  tcase_add_test(c, test_basic);
  tcase_add_test(c, test_bom);
  tcase_add_loop_test(c, test_branch, 0, 3);
  tcase_add_test(c, test_commands);
  tcase_add_test(c, test_checkpoint);
  tcase_add_test(c, test_crlf);
  tcase_add_test(c, test_division);
  tcase_add_loop_test(c, test_double, 0, 2);
  tcase_add_test(c, test_empty);
  tcase_add_test(c, test_emptymeasures);
  tcase_add_test(c, test_eof);
  tcase_add_test(c, test_hand);
  tcase_add_test(c, test_label);
  tcase_add_test(c, test_measures);
  tcase_add_test(c, test_whitespace);
  return c;
}
