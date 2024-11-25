# SPDX-License-Identifier: 0BSD
libtaiko_src += files(
  'balloon.c',
  'branched.c',
  'branchtype.c',
  'coursebody.c',
  'enums.c',
  'events.c',
  'metadata.c',
  'pass_cleanup.c',
  'pass_convert_time.c',
  'segment.c',
  'timestamp.c',
)

libtaiko_src += bison_gen.process('tja.y')
libtaiko_src += flex_gen.process('tja.l')
