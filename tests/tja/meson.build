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
  include_directories: [libtaiko_include, libtaiko_include_private],
  link_with: [tests_main, libtaiko_lib.get_static_lib()],
  dependencies: check_dep,
  install: false,
)

test(
  'libtaiko_tja',
  tests_tja_bin,
  protocol: 'tap',
  workdir: meson.current_source_dir(),
)
