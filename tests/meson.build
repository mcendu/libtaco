# SPDX-License-Identifier: 0BSD
check_dep = dependency('check', required: false, modules: ['Check::checkShared'])

if not check_dep.found()
  subdir_done()
endif

libtaco_tests_lib = static_library(
  'taco_tests',
  libtaco_src,
  c_args: ['-DTACO_BUILDING_', '-DTACO_TESTS_'],
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  install: false,
  pic: true,
)

libtaco_tests_dep = declare_dependency(
  compile_args: ['-DTACO_TESTS_'],
  include_directories: [libtaco_include, libtaco_include_private],
  dependencies: libtaco_deps,
  link_with: libtaco_tests_lib,
)

subdir('utils')
subdir('core')
subdir('tja')
