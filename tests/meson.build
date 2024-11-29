# SPDX-License-Identifier: 0BSD
check_dep = dependency('check', required: false)

libtaco_tests_dep = declare_dependency(
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  link_with: libtaco_lib.get_static_lib(),
)

tests_main = static_library('tests_main', 'main.c')

subdir('core')
subdir('tja')
