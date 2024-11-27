/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TAIKO_PARSER_H_
#define TAIKO_PARSER_H_

#include "io.h"
#include "taco.h"
typedef const struct taiko_parser_vfuncs_ taiko_parser_vfuncs;

typedef void (*taiko_parser_free_fn)(void *parser);
typedef taiko_courseset *(*taiko_parser_parse_fn)(void *restrict parser,
                                                  taiko_file *restrict file);
typedef int (*taiko_parser_seterror_fn)(void *restrict parser,
                                        taiko_file *restrict file);

struct taiko_parser_vfuncs_ {
  taiko_parser_free_fn free;
  taiko_parser_parse_fn parse;
  taiko_parser_seterror_fn set_error;
};

extern taiko_parser *taiko_parser_wrap_(taiko_allocator *alloc, void *parser,
                                        taiko_parser_vfuncs *vtable);

#endif /* TAIKO_PARSER_H_ */
