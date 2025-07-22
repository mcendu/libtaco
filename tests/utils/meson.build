testutils_src = files(
  'main.c',
  'tacoassert.c',
)

if host_machine.system() == 'windows'
  subdir('windows')
else
  # Assume Unix for the rest, as non-Windows operating systems non-
  # compatible with Unix are almost all obsolete. (Modern macOS is
  # Unix as far as C is concerned.)
  #
  # Hacks for specific flavors of Unix (like Linux) also go here.
  subdir('unix')
endif

testutils_include = include_directories('.')

tests_main = static_library(
  'testutils',
  testutils_src,
  c_args: ['-DTACO_TESTS_'],
  include_directories: [testutils_include, libtaco_include],
  dependencies: libtaco_deps,
  link_with: libtaco_tests_lib,
)
