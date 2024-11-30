/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_PARSER_H_
#define TJA_PARSER_H_

#include "io.h"
#include "taco.h"

typedef struct tja_parser_ tja_parser;

#define TJA_DIAG_FATAL 0
#define TJA_DIAG_ERROR 1
#define TJA_DIAG_WARN 2

extern tja_parser *tja_parser_create_();
extern tja_parser *tja_parser_create2_(taiko_allocator *alloc);
extern void tja_parser_free_(tja_parser *parser);

extern taiko_courseset *tja_parser_parse_(tja_parser *parser, taiko_file *file);

extern int tja_parser_set_error_(tja_parser *parser, taiko_file *file);
extern void tja_parser_diagnose_(tja_parser *parser, int line, int level,
                                 const char *format, ...);

#endif /* !TJA_PARSER_H_ */
