# SPDX-License-Identifier: 0BSD
tests_tja_src = files(
  'parser.c',
  'tja.c',
)

tests_tja_bin = executable(
  'tests_tja',
  tests_tja_src,
  link_with: [tests_main],
  include_directories: [testutils_include],
  dependencies: [libtaco_tests_dep, check_dep],
  install: false,
)

test(
  'libtaco_tja',
  tests_tja_bin,
  protocol: 'tap',
  workdir: meson.current_source_dir(),
)
