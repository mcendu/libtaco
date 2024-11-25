# SPDX-License-Identifier: 0BSD
libtaiko_src = files(
  'alloc.c',
  'course.c',
  'courseset.c',
  'io.c',
  'note.c',
  'section.c',
)

subdir('tja')

libtaiko_lib = both_libraries(
  'taiko',
  libtaiko_src,
  include_directories: [libtaiko_include, libtaiko_include_private],
  version: meson.project_version(),
  install: true,
  pic: true,
  gnu_symbol_visibility: 'hidden',
)
