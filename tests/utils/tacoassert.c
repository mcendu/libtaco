// SPDX-License-Identifier: BSD-2-Clause
#include "tacoassert.h"
#include <check.h>

#include "taco.h"
#include "temp.h"
#include <stdio.h>
#include <stdlib.h>

struct assert_section_state_ {
  FILE *dump;
  FILE *expected;
  char (*block)[4096];
};

assert_section_state *assert_section_setup() {
  assert_section_state *state = malloc(sizeof(assert_section_state));
  state->dump = NULL;
  state->expected = NULL;
  state->block = NULL;
  return state;
}

void assert_section_teardown(assert_section_state *state) {
  if (state->dump)
    fclose(state->dump);
  if (state->expected)
    fclose(state->expected);
  if (state->block)
    free(state->block);
  free(state);
}

static void assert_section_abort_(FILE *dump, const char *expected_path) {
  fprintf(stderr, "assert failed: section does not match %s\n", expected_path);
  fprintf(stderr, "data from dump:\n");

  fseek(dump, 0, SEEK_SET);
  char *block = malloc(4096);

  while (!feof(dump) && !ferror(dump)) {
    size_t len = fread(block, 1, 4096, dump);
    fwrite(block, 1, len, stderr);
  }

  free(block);
  ck_abort_msg("sections does not match");
}

void assert_section_eq(const taco_section *section, const char *path,
                       assert_section_state *state) {
  // dump section to temp file
  state->dump = open_temp();
  ck_assert_ptr_nonnull(state->dump);
  taco_section_print(section, state->dump);

  // prepare for comparison
  state->expected = fopen(path, "r"); // not "rb" due to Windows
  ck_assert_ptr_nonnull(state->expected);
  fseek(state->dump, 0, SEEK_SET);

  state->block = malloc(sizeof(char[2][4096]));
  char *buf_dump = state->block[0];
  char *buf_expected = state->block[1];

  // compare
  mark_point();
  while (!feof(state->expected)) {
    size_t len = fread(buf_dump, 1, 4096, state->dump);
    if (len != fread(buf_expected, 1, 4096, state->expected)) {
      assert_section_abort_(state->dump, path);
    }
    if (memcmp(buf_dump, buf_expected, len) != 0) {
      assert_section_abort_(state->dump, path);
    };
  }

  if (fgetc(state->dump) != EOF) {
    assert_section_abort_(state->dump, path);
  }

  fclose(state->dump);
  fclose(state->expected);
  free(state->block);
  state->dump = NULL;
  state->expected = NULL;
  state->block = NULL;
}
