/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TACO_PARSER_H_
#define TACO_PARSER_H_

#include "io.h"
#include "taco.h"
typedef const struct taco_parser_vfuncs_ taco_parser_vfuncs;

typedef void (*taco_parser_free_fn)(void *parser);
typedef taco_courseset *(*taco_parser_parse_fn)(void *restrict parser,
                                                  taco_file *restrict file);
typedef int (*taco_parser_seterror_fn)(void *restrict parser,
                                        taco_file *restrict file);

struct taco_parser_vfuncs_ {
  taco_parser_free_fn free;
  taco_parser_parse_fn parse;
  taco_parser_seterror_fn set_error;
};

extern taco_parser *taco_parser_wrap_(taco_allocator *alloc, void *parser,
                                        taco_parser_vfuncs *vtable);

#endif /* TACO_PARSER_H_ */
