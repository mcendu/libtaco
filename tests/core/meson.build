# SPDX-License-Identifier: 0BSD
tests_core_src = files(
  'core.c',
  'course.c',
  'section.c',
)

tests_core_bin = executable(
  'tests_core',
  tests_core_src,
  include_directories: [libtaiko_include, libtaiko_include_private],
  link_with: [tests_main, libtaiko_lib.get_static_lib()],
  dependencies: check_dep,
  install: false,
)

test(
  'libtaiko',
  tests_core_bin,
  protocol: 'tap',
  workdir: meson.current_source_dir(),
)
