/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_PARSER_H_
#define TJA_PARSER_H_

#include "taiko.h"
#include <stdio.h>

typedef struct tja_parser_ tja_parser;

extern tja_parser *tja_parser_create_();
extern tja_parser *tja_parser_create2_(taiko_allocator *alloc);
extern void tja_parser_free_(tja_parser *parser);

extern taiko_courseset *tja_parser_parse_file_(tja_parser *parser,
                                               const char *file);
extern taiko_courseset *tja_parser_parse_stdio_(tja_parser *parser,
                                                FILE *stream);
extern taiko_courseset *tja_parser_parse_memory_(tja_parser *parser,
                                                 const void *data, size_t size);

extern int tja_parser_set_error_(tja_parser *parser, FILE *stream);
extern void tja_parser_error_(tja_parser *parser, const char *format, ...);

#endif /* !TJA_PARSER_H_ */
