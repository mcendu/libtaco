# SPDX-License-Identifier: 0BSD
cc = meson.get_compiler('c')
cfg = configuration_data()

cfg.set(
  'TACO_HAS_FSEEKO_',
  cc.has_header_symbol('stdio.h', 'fseeko', args: ['-D_POSIX_C_SOURCE=200112L']),
)

configure_file(configuration: cfg, input: 'config.h.in', output: 'config.h')
