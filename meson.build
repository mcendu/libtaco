# Copyright (C) 2024, McEndu.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
project(
  'libtaco',
  'c',
  'cpp',
  version: '0.2.0',
  meson_version: '>=1.0.0',
  default_options: [
    'c_std=c11',
    'cpp_std=c++20',
  ],
)

cmake = import('cmake')

bison_gen = generator(
  find_program('bison'),
  arguments: ['-d', '-o', '@OUTPUT0@', '@INPUT@'],
  output: ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
)

flex_args = ['-o', '@OUTPUT@', '@INPUT@']
if get_option('buildtype') == 'debug'
  flex_args = ['-d'] + flex_args
endif

flex_gen = generator(
  find_program('flex', 'lex'),
  arguments: flex_args,
  output: ['@BASENAME@.yy.c'],
)

simdutf_dep = get_option('taco_force_bundled_simdutf') ? disabler() : dependency('simdutf', required: get_option('taco_use_simdutf'))

if not simdutf_dep.found() and get_option('taco_use_simdutf').allowed()
  simdutf_options = cmake.subproject_options()
  simdutf_options.add_cmake_defines(
    {
      'CMAKE_BUILD_TYPE': 'Release',
      'SIMDUTF_TESTS': 'OFF',
      'SIMDUTF_TOOLS': 'OFF',
    },
  )
  simdutf_options.set_install(false)
  bundled_simdutf = cmake.subproject(
    'simdutf',
    required: false,
    options: simdutf_options,
  )
  if bundled_simdutf.found()
    simdutf_dep = bundled_simdutf.dependency('simdutf')
  endif
endif

subdir('include')
subdir('src')
subdir('utils')
subdir('tests')
