# SPDX-License-Identifier: 0BSD
utils_common_src = files(
  'getopt_long.c',
  'getopt.c',
)

utils_common_include = include_directories('.')

utils_common_lib = static_library(
  'utils_common',
  utils_common_src,
  install: false,
  pic: true,
)

utils_common_dep = declare_dependency(
  include_directories: utils_common_include,
  link_with: utils_common_lib,
)
