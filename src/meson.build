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

libtaiko_deps = []

# link libm for math functions, if separate library
cc = meson.get_compiler('c')
libm_dep = cc.find_library('m', required: false)
if libm_dep.found()
  libtaiko_deps += libm_dep
endif

libtaiko_lib = both_libraries(
  'taiko',
  libtaiko_src,
  include_directories: [libtaiko_include, libtaiko_include_private],
  dependencies: libtaiko_deps,
  version: meson.project_version(),
  install: true,
  pic: true,
  gnu_symbol_visibility: 'hidden',
)

# dep for unit tests
libtaiko_dep = declare_dependency(
  include_directories: [libtaiko_include, libtaiko_include_private],
  dependencies: libtaiko_deps,
  link_with: libtaiko_lib.get_static_lib(),
)
