# SPDX-License-Identifier: 0BSD
tests_tja_src = files(
  'events.c',
  'parser.c',
  'segment.c',
  'tja.c',
)

tests_tja_bin = executable(
  'tests_tja',
  tests_tja_src,
  link_with: [tests_main],
  dependencies: [libtaiko_dep, check_dep],
  install: false,
)

test(
  'libtaiko_tja',
  tests_tja_bin,
  protocol: 'tap',
  workdir: meson.current_source_dir(),
)
