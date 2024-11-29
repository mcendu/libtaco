# SPDX-License-Identifier: 0BSD
tests_core_src = files(
  'core.c',
  'course.c',
  'section.c',
)

tests_core_bin = executable(
  'tests_core',
  tests_core_src,
  link_with: [tests_main],
  dependencies: [libtaco_tests_dep, check_dep],
  install: false,
)

test(
  'libtaiko',
  tests_core_bin,
  protocol: 'tap',
  workdir: meson.current_source_dir(),
)
