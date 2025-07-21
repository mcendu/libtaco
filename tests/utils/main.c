// SPDX-License-Identifier: BSD-2-Clause
#include <check.h>
#include <stdlib.h>

typedef TCase *(*testcase)();

extern const char suite_name[];
extern const testcase cases[];

int main(int argc, char **argv) {
  Suite *suite = suite_create("libtaiko");

  for (TCase *(*const *c)(void) = cases; *c != NULL; ++c)
    suite_add_tcase(suite, (*c)());

  SRunner *srunner = srunner_create(suite);
  srunner_set_tap(srunner, "-");
  srunner_run_all(srunner, CK_SILENT);
  srunner_free(srunner);
  return EXIT_SUCCESS;
}
