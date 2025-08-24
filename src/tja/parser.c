// SPDX-License-Identifier: BSD-2-Clause
#include "parser.h"

#include "alloc.h"
#include "config.h"
#include "io.h"
#include "section.h"
#include "taco.h"
#include "tja.tab.h"
#include "tja/parser.h"
#include <string.h>

#ifdef TACO_HAS_ICONV_
#include "tja/shiftjis.h"
#endif

extern int tja_yylex_init(yyscan_t *scanner);
extern int tja_yylex_destroy(yyscan_t scanner);

extern void tja_yyset_extra(void *data, yyscan_t scanner);
extern void tja_yyset_debug(int debug, yyscan_t scanner);

extern void tja_yyrestart(FILE *file, yyscan_t scanner);

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

tja_parser *tja_parser_create_(void) {
  return tja_parser_create2_(&taco_default_allocator_);
}

tja_parser *tja_parser_create2_(taco_allocator *alloc) {
  tja_parser *parser = taco_malloc_(alloc, sizeof(tja_parser));
  taco_file *error = taco_file_open_stdio_(stderr);
  yyscan_t scanner = NULL;
  tja_yylex_init(&scanner);

  if (!parser || !error || !scanner) {
    taco_free_(alloc, parser);
    taco_file_close_(error);
    if (scanner)
      tja_yylex_destroy(scanner);
    return NULL;
  }

  memset(parser, 0, sizeof(tja_parser));
  parser->alloc = alloc;
  parser->lexer = scanner;
  tja_yyset_extra(parser, scanner);
  parser->error_stream = error;

  for (int i = 0; i < PURPOSE_MAX; ++i)
    parser->tmpsections[i] = taco_section_create2_(parser->alloc);

  return parser;
}

void tja_parser_free_(tja_parser *parser) {
  if (!parser)
    return;

  tja_yylex_destroy(parser->lexer);
  taco_file_close_(parser->error_stream);
  for (int i = 0; i < PURPOSE_MAX; ++i)
    taco_section_free_(parser->tmpsections[i]);
  taco_free_(parser->alloc, parser);
}

taco_courseset *tja_parser_parse_(tja_parser *parser, taco_file *file) {
  if (!file)
    return NULL;

#ifdef TACO_HAS_ICONV_
  // libtaco is expected to store all strings as UTF-8. The TJA frontend,
  // when compiled with character set conversion support, supports UTF-8
  // (with or without BOM) and Shift JIS as input encodings.
  //
  // The input detection process is as follows:
  //
  //   - The input file is passed through a UTF-8 validator. If it is
  //     valid, the file is passed to the parser as-is.
  //   - If the input fails UTF-8 validation, the input is treated as
  //     Shift JIS (most non UTF-8 TJA files are Shift JIS). A filter
  //     is created to convert the input to UTF-8. The frontend does
  //     not respect byte order marks (U+FEFF), and malformed UTF-8
  //     files will appear as mojibake.
  taco_file *filter = NULL;

  if (tja_is_file_utf8_(file, parser->alloc)) {
    parser->input = file;
  } else {
    filter = tja_iconv_open_(parser->alloc, file, "Shift_JIS");
    if (!filter) {
      tja_parser_diagnose_(
          parser, 0, TJA_DIAG_FATAL,
          "failed to create character set conversion filter");
      return NULL;
    }
    parser->input = filter;
  }
#else
  // Without support for character set conversion, the input is not
  // processed. The user is warned of mojibake in this case.
  tja_parser_diagnose_(
      parser, 0, TJA_DIAG_NOTE,
      "character set conversion not available; output may be unreadable");
  parser->input = file;
#endif

#ifdef YYDEBUG
  if (tja_yydebug)
    tja_yyset_debug(1, parser->lexer);
#endif

  // parse
  int errcode = tja_yyparse(parser, parser->lexer);
  tja_metadata_free_(parser->metadata);

  if (errcode) {
    taco_courseset_free(parser->set);
    parser->set = NULL;
  }

  taco_courseset *set = parser->set;
  parser->set = NULL;
  parser->input = NULL;

  if (filter) {
    taco_file_close_(filter);
  }

  return set;
}

taco_allocator *tja_parser_allocator_(tja_parser *parser) {
  return parser->alloc;
}

int tja_parser_set_error_(tja_parser *parser, taco_file *file) {
  taco_file_close_(parser->error_stream);
  parser->error_stream = file;
  return 0;
}

void tja_parser_diagnose_(tja_parser *parser, int line, int level,
                          const char *format, ...) {
  const char *const templates[] = {
      "%s:%d: fatal error: %s\n",
      "%s:%d: error: %s\n",
      "%s:%d: warning: %s\n",
      "%s:%d: info: %s\n",
  };
  va_list ap;

  // format input
  va_start(ap, format);
  int size = vsnprintf(NULL, 0, format, ap) + 1;
  va_end(ap);
  char *formatted = taco_malloc_(parser->alloc, size);
  if (formatted) {
    va_start(ap, format);
    vsnprintf(formatted, size, format, ap);
    va_end(ap);
  }

  // forward to error output
  if (formatted)
    taco_file_printf_(parser->error_stream, templates[level],
                      taco_file_name_(parser->input), line, formatted);
  taco_free_(parser->alloc, formatted);
}
