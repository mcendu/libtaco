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

subdir('debug')
subdir('tja')

libc_deps = []

# link libm for math functions, if separate library
cc = meson.get_compiler('c')
libm_dep = cc.find_library('m', required: false)
if libm_dep.found()
  libc_deps += libm_dep
endif

libtaco_deps = [libc_deps]

libtaco_lib = library(
  'taco',
  libtaco_src,
  c_args: ['-DTACO_BUILDING_'],
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  version: meson.project_version(),
  install: true,
  pic: true,
  gnu_symbol_visibility: 'hidden',
)

pkgconf = import('pkgconfig')
pkgconf.generate(
  libtaco_lib,
  description: 'Library for processing levels for a certain drum game'
)

libtaco_dep = declare_dependency(
  include_directories: [libtaco_include],
  link_with: libtaco_lib,
)
