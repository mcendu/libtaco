# SPDX-License-Identifier: 0BSD
libtaco_src = files(
  'alloc.c',
  'course.c',
  'courseset.c',
  'io.c',
  'note.c',
  'parser.c',
  'section.c',
)

subdir('tja')

libtaco_deps = []

# link libm for math functions, if separate library
cc = meson.get_compiler('c')
libm_dep = cc.find_library('m', required: false)
if libm_dep.found()
  libtaco_deps += libm_dep
endif

libtaco_lib = both_libraries(
  'taco',
  libtaco_src,
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  version: meson.project_version(),
  install: true,
  pic: true,
  gnu_symbol_visibility: 'hidden',
)

# dep for unit tests
libtaiko_dep = declare_dependency(
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  link_with: libtaco_lib.get_static_lib(),
)
