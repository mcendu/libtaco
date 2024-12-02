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
  version: '0.1.0',
  meson_version: '>=0.61.0',
  default_options: [
    'c_std=c11',
  ],
)

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

subdir('include')
subdir('src')
subdir('utils')
subdir('tests')
