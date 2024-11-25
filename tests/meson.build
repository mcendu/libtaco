# SPDX-License-Identifier: 0BSD
check_dep = dependency('check', required: false)

if not check_dep.found()
  subdir_done()
endif

tests_main = static_library('tests_main', 'main.c')

subdir('core')
subdir('tja')