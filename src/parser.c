// SPDX-License-Identifier: BSD-2-Clause
#include "parser.h"
#include "alloc.h"
#include "courseset.h"
#include "io.h"
#include "taco.h"

struct taco_parser_ {
  taco_allocator *alloc;
  void *parser;
  taco_parser_vfuncs *vtable;
};

taco_parser *taco_parser_wrap_(taco_allocator *alloc, void *parser,
                               taco_parser_vfuncs *vtable) {
  taco_parser *wrapper = taco_malloc_(alloc, sizeof(taco_parser));
  if (!wrapper)
    return NULL;

  wrapper->alloc = alloc;
  wrapper->parser = parser;
  wrapper->vtable = vtable;
  return wrapper;
}

void taco_parser_free(taco_parser *parser) {
  if (!parser)
    return;

  parser->vtable->free(parser->parser);
  taco_free_(parser->alloc, parser);
}

static void post_parse_cleanup_(taco_courseset *restrict c,
                                taco_file *restrict f) {
  if (c) {
    taco_courseset_set_filename_(c, taco_file_name_(f));
  }

  taco_file_close_(f);
}

taco_courseset *taco_parser_parse_file(taco_parser *restrict parser,
                                       const char *restrict path) {
  taco_file *f = taco_file_open_path_(path, "r");
  if (!f)
    return NULL;

  taco_courseset *result = parser->vtable->parse(parser->parser, f);
  post_parse_cleanup_(result, f);
  return result;
}

taco_courseset *taco_parser_parse_stdio(taco_parser *restrict parser,
                                        FILE *file) {
  taco_file *f = taco_file_open_stdio_(file);
  taco_courseset *result = parser->vtable->parse(parser->parser, f);
  post_parse_cleanup_(result, f);
  return result;
}

int taco_parser_set_error_stdio(taco_parser *restrict parser, FILE *file) {
  taco_file *f;
  if (file) {
    f = taco_file_open_stdio_(file);
  } else {
    f = taco_file_open_null_(parser->alloc);
  }

  return parser->vtable->set_error(parser->parser, f);
}
