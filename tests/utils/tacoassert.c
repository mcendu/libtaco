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
  while (!feof(state->expected)) {
    size_t len = fread(buf_dump, 1, 4096, state->dump);
    ck_assert_int_eq(len, fread(buf_expected, 1, 4096, state->expected));
    ck_assert_mem_eq(buf_dump, buf_expected, len);
  }
  ck_assert_int_eq(fgetc(state->dump), EOF);

  fclose(state->dump);
  fclose(state->expected);
  free(state->block);
  state->dump = NULL;
  state->expected = NULL;
  state->block = NULL;
}
