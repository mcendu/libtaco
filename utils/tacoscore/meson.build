# SPDX-License-Identifier: 0BSD
tacoscore_src = files(
  'main.c',
)

executable(
  'tacoscore',
  tacoscore_src,
  dependencies: [libc_deps, libtaco_dep, utils_common_dep],
  install: true,
)