# SPDX-License-Identifier: 0BSD
tacoscore_src = files(
  'calc.c',
  'main.c',
  'score_ac15.c',
  'score_ac16.c',
)

executable(
  'tacoscore',
  tacoscore_src,
  dependencies: [libc_deps, libtaco_dep, utils_common_dep],
  install: true,
)
