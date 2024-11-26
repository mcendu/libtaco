/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_PARSER_H_
#define TAIKO_PARSER_H_

#include "taiko.h"
typedef const struct taiko_parser_vfuncs_ taiko_parser_vfuncs;

typedef void (*taiko_parser_free_fn)(void *parser);
typedef taiko_courseset *(*taiko_parser_parse_file_fn)(
    void *restrict parser, const char *restrict path);
typedef taiko_courseset *(*taiko_parser_parser_stdio_fn)(void *restrict parser,
                                                         FILE *restrict file);

struct taiko_parser_vfuncs_ {
  taiko_parser_free_fn free;
  taiko_parser_parse_file_fn parse_file;
  taiko_parser_parser_stdio_fn parse_stdio;
};

extern taiko_parser *taiko_parser_wrap_(taiko_allocator *alloc, void *parser,
                                        taiko_parser_vfuncs *vtable);

#endif /* TAIKO_PARSER_H_ */
