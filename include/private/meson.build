# SPDX-License-Identifier: 0BSD
cc = meson.get_compiler('c')
cfg = configuration_data()

# link libm for math functions, if separate library
libc_deps = []
libm_dep = cc.find_library('m', required: false)
if libm_dep.found()
  libc_deps += libm_dep
endif

cfg.set(
  'TACO_HAS_FSEEKO_',
  cc.has_header_symbol('stdio.h', 'fseeko', args: ['-D_POSIX_C_SOURCE=200112L']),
)

iconv_dep = []
cfg.set('TACO_HAS_ICONV_', cc.has_function('iconv'))
if not cfg.get('TACO_HAS_ICONV_')
  iconv_dep = dependency('iconv', required: false)
  cfg.set('TACO_HAS_ICONV_', iconv_dep.found())
endif

libtaco_deps = [libc_deps, iconv_dep]

if simdutf_dep.found()
  libtaco_deps += simdutf_dep
endif

configure_file(configuration: cfg, input: 'config.h.in', output: 'config.h')
