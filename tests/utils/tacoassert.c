// SPDX-License-Identifier: BSD-2-Clause
#include "tacoassert.h"
#include <check.h>

#include "taco.h"
#include "temp.h"
#include <stdio.h>
#include <stdlib.h>

void assert_section_matches(const taco_section *section, const char *path) {
  // dump section to temp file
  FILE *dump = open_temp();
  taco_section_print(section, dump);

  // prepare for comparison
  FILE *expected = fopen(path, "r"); // not "rb" due to Windows
  fseek(dump, 0, SEEK_SET);

  char (*block)[4096] = malloc(sizeof(char[2][4096]));
  char *buf_dump = block[0];
  char *buf_expected = block[1];

  // compare
  while (!feof(expected)) {
    size_t len = fread(buf_dump, 1, 4096, dump);
    ck_assert_int_eq(len, fread(buf_expected, 1, 4096, expected));
    ck_assert_mem_eq(buf_dump, buf_expected, len);
  }
  ck_assert_int_eq(fgetc(dump), EOF);

  fclose(dump);
  fclose(expected);
  free(block);
}
