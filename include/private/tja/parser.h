/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_PARSER_H_
#define TJA_PARSER_H_

#include "io.h"
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

extern int tja_parser_set_error_(tja_parser *parser, taiko_file *file);
extern void tja_parser_error_(tja_parser *parser, const char *format, ...);

extern taiko_section *tja_pass_convert_time_(taiko_section *section);
extern taiko_section *tja_pass_cleanup_(taiko_section *section);

#endif /* !TJA_PARSER_H_ */
