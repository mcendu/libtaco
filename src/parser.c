// SPDX-License-Identifier: BSD-2-Clause
#include "parser.h"
#include "alloc.h"
#include "io.h"
#include "taco.h"

struct taiko_parser_ {
  taiko_allocator *alloc;
  void *parser;
  taiko_parser_vfuncs *vtable;
};

taiko_parser *taiko_parser_wrap_(taiko_allocator *alloc, void *parser,
                                 taiko_parser_vfuncs *vtable) {
  taiko_parser *wrapper = taiko_malloc_(alloc, sizeof(taiko_parser));
  if (!wrapper)
    return NULL;

  wrapper->alloc = alloc;
  wrapper->parser = parser;
  wrapper->vtable = vtable;
  return wrapper;
}

void taiko_parser_free(taiko_parser *parser) {
  if (!parser)
    return;

  parser->vtable->free(parser->parser);
  taiko_free_(parser->alloc, parser);
}

taiko_courseset *taiko_parser_parse_file(taiko_parser *restrict parser,
                                         const char *restrict path) {
  taiko_file *f = taiko_file_open_path_(path, "r");
  taiko_courseset *result = parser->vtable->parse(parser->parser, f);
  taiko_file_close_(f);
  return result;
}

taiko_courseset *taiko_parser_parse_stdio(taiko_parser *restrict parser,
                                          FILE *file) {
  taiko_file *f = taiko_file_open_stdio_(file);
  taiko_courseset *result = parser->vtable->parse(parser->parser, f);
  taiko_file_close_(f);
  return result;
}
