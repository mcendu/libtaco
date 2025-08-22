# SPDX-License-Identifier: 0BSD
libtaco_src += files(
  'balloon.c',
  'branched.c',
  'branchtype.c',
  'coursebody.c',
  'enums.c',
  'events.c',
  'metadata.c',
  'parser.c',
  'pass_annotate.c',
  'pass_barlines.c',
  'pass_checkpoint_rolls.c',
  'pass_cleanup.c',
  'pass_compile_branches.c',
  'pass_convert_time.c',
  'pass_prepend_bpm.c',
  'segment.c',
  'timestamp.c',
)

libtaco_src += bison_gen.process('tja.y')
libtaco_src += flex_gen.process('tja.l')
