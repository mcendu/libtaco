/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef TJA_PARSER_H_
#define TJA_PARSER_H_

#include "io.h"
#include "taco.h"

typedef struct tja_parser_ tja_parser;

#define TJA_DIAG_FATAL 0
#define TJA_DIAG_ERROR 1
#define TJA_DIAG_WARN 2
#define TJA_DIAG_NOTE 3

extern tja_parser *tja_parser_create_();
extern tja_parser *tja_parser_create2_(taco_allocator *alloc);
extern void tja_parser_free_(tja_parser *parser);

extern taco_courseset *tja_parser_parse_(tja_parser *parser, taco_file *file);

extern int tja_parser_set_error_(tja_parser *parser, taco_file *file);
extern void tja_parser_diagnose_(tja_parser *parser, int line, int level,
                                 const char *format, ...);

extern taco_allocator *tja_parser_allocator_(tja_parser *parser);

#define TACO_EVENT_TJA_BARLINEON (-0x4000)
#define TACO_EVENT_TJA_BARLINEOFF (-0x4001)
#define TACO_EVENT_TJA_MEASURE_LENGTH (-0x4002)
#define TACO_EVENT_TJA_BRANCH_THRESHOLD (-0x4003)
#define TACO_EVENT_TJA_BRANCH_TYPE (-0x4004)
#define TACO_EVENT_TJA_LEVELHOLD (-0x4005)

#endif /* !TJA_PARSER_H_ */
