// SPDX-License-Identifier: BSD-2-Clause
#include "parser.h"
#include "alloc.h"
#include "taco.h"
#include "tja/parser.h"

static taco_parser_vfuncs vfuncs = {
    .free = (taco_parser_free_fn)tja_parser_free_,
    .parse = (taco_parser_parse_fn)tja_parser_parse_,
    .set_error = (taco_parser_seterror_fn)tja_parser_set_error_,
};

taco_parser *taco_parser_tja_create() {
  return taco_parser_tja_create2(&taco_default_allocator_);
}

taco_parser *taco_parser_tja_create2(taco_allocator *alloc) {
  tja_parser *parser = tja_parser_create2_(alloc);
  taco_parser *wrapper = taco_parser_wrap_(alloc, parser, &vfuncs);

  if (!parser) {
    taco_parser_free(wrapper);
  } else if (!wrapper) {
    tja_parser_free_(parser);
  }

  return wrapper;
}
