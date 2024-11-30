# SPDX-License-Identifier: 0BSD
check_dep = dependency('check', required: false)

libtaco_tests_lib = static_library(
  'taco_tests',
  libtaco_src,
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  install: false,
  pic: true,
)

libtaco_tests_dep = declare_dependency(
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  link_with: libtaco_tests_lib,
)

tests_main = static_library('tests_main', 'main.c')

subdir('core')
subdir('tja')
