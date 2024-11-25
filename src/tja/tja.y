%{
/* SPDX-License-Identifier: BSD-2-Clause */
%}

%code requires {
#include "taiko.h"
#include "tja/branched.h"
#include "tja/coursebody.h"
#include "tja/events.h"
#include "tja/metadata.h"
#include "tja/segment.h"

#include "note.h"
#include <stdbool.h>

typedef struct tja_parser_ tja_parser;
typedef struct branch_info_ branch_info;
typedef void *yyscan_t;

struct branch_info_ {
  int type;
  int advanced;
  int master;
};
}

%{
#include "tja/parser.h"

#include "tja/branchtype.h"
#include "tja/metadata.h"
#include "alloc.h"
#include "course.h"
#include "courseset.h"
#include "io.h"
#include "section.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void *yyscan_t;

struct tja_parser_ {
  taiko_allocator *alloc;
  yyscan_t lexer;
  taiko_file *error_stream;
  taiko_courseset *set;
  tja_metadata *metadata;
};

#define PURPOSE_MEASURE 0
#define PURPOSE_SEGMENT 1
#define PURPOSE_BRANCH_COMMON 2
#define PURPOSE_BRANCH_NORMAL 3
#define PURPOSE_BRANCH_ADVANCED 4
#define PURPOSE_BRANCH_MASTER 5
#define PURPOSE_MEASURESTART_EVENTS 6

#define PURPOSE_BRANCH(n) (3 + n)

static taiko_section *get_section_(tja_parser *parser, int purpose);
static void put_section_(tja_parser *parser, taiko_section *section,
                         int purpose);

static void tja_yyerror(tja_parser *parser, yyscan_t lexer,
                    const char *msg);
%}

%code provides {
extern int tja_yylex(TJA_YYSTYPE *lvalp, yyscan_t lexer);
}

%define api.pure full
%define api.prefix {tja_yy}

%parse-param {tja_parser *parser}
%parse-param {yyscan_t lexer}
%lex-param {yyscan_t lexer}

%union {
  long integer;
  double real;
  char *text;

  taiko_event note;
  branch_info branch;

  taiko_courseset *set;
  taiko_course *course;
  tja_metadata *metadata;
  tja_balloon *balloon;

  tja_metadata_field field;
  tja_coursebody coursebody;
  tja_branched branched;
  tja_segment segment;
  tja_events measure;
}

%type <set> set

%type <metadata> headers

%type <field> header
%type <field> title_header
%type <field> subtitle_header
%type <field> genre_header
%type <field> maker_header
%type <field> audio_header
%type <field> bpm_header
%type <field> offset_header
%type <field> demostart_header

%type <field> side_header
%type <field> course_header
%type <field> level_header
%type <field> style_header
%type <field> balloon_header
%type <field> papamama_command
%type <field> unrecognized_header

%type <integer> balloon_branch
%type <balloon> balloon_data

%type <course> body
%type <coursebody> sections
%type <integer> start_command

%type <branched> branched_section
%type <segment> common_section

%type <segment> measures
%type <measure> note_events
%type <measure> measurestart_events
%type <note> note_command
%type <note> measurestart_command

%type <branch> branchstart_command
%type <integer> branch_command

%type <note> measure_command
%type <note> section_command
%type <note> levelhold_command
%type <note> gogostart_command
%type <note> gogoend_command
%type <note> scroll_command
%type <note> bpmchange_command
%type <note> unrecognized_command

%type <real> real
%type <text> text

%token <integer> INTEGER
%token <real> REAL
%token <text> TEXT
%token <text> IDENTIFIER
%token <text> COMMAND
%token <integer> NOTE

%token TITLE
%token SUBTITLE
%token GENRE
%token MAKER
%token AUDIO
%token BPM
%token OFFSET
%token DEMOSTART

%token SIDE
%token COURSE
%token LEVEL
%token STYLE
%token BALLOON
%token BALLOONNOR
%token BALLOONEXP
%token BALLOONMAS

%token PAPAMAMA_CMD

%token START_CMD
%token END_CMD
%token BRANCHSTART_CMD
%token N_CMD
%token E_CMD
%token M_CMD
%token BRANCHEND_CMD
%token MEASURE_CMD
%token SECTION_CMD
%token LEVELHOLD_CMD
%token GOGOSTART_CMD
%token GOGOEND_CMD
%token SCROLL_CMD
%token BPMCHANGE_CMD

%start set

%%

set:
  %empty {
    parser->set = taiko_courseset_create2_(parser->alloc);
    parser->metadata = tja_metadata_create2_(parser->alloc);
    $$ = parser->set;
  }
  | set headers body {
    tja_metadata_update_(parser->metadata, $2);
    tja_metadata_free_($2);
    tja_course_apply_metadata_($3, parser->metadata);
    tja_courseset_apply_metadata_($1, parser->metadata);

    taiko_courseset_add_course_($1, $3);
    $$ = $1;
  };

headers:
  %empty { $$ = tja_metadata_create2_(parser->alloc); }
  | headers header {
    tja_metadata_add_field_($1, &$2);
    $$ = $1;
  };

header:
  title_header
  | subtitle_header
  | genre_header
  | maker_header
  | audio_header
  | bpm_header
  | offset_header
  | demostart_header
  | side_header
  | course_header
  | level_header
  | style_header
  | balloon_header
  | papamama_command
  | unrecognized_header;

title_header:
  TITLE ':' text '\n' {
    $$.key = TJA_METADATA_TITLE;
    $$.text = $3;
  };

subtitle_header:
  SUBTITLE ':' text '\n' {
    $$.key = TJA_METADATA_SUBTITLE;
    $$.text = $3;
  };

genre_header:
  GENRE ':' text '\n' {
    $$.key = TJA_METADATA_GENRE;
    $$.text = $3;
  };

maker_header:
  MAKER ':' text '\n' {
    $$.key = TJA_METADATA_MAKER;
    $$.text = $3;
  };

audio_header:
  AUDIO ':' text '\n' {
    $$.key = TJA_METADATA_AUDIO;
    $$.text = $3;
  };

bpm_header:
  BPM ':' real '\n' {
    $$.key = TJA_METADATA_BPM;
    $$.real = $3;
  };

offset_header:
  OFFSET ':' real '\n' {
    $$.key = TJA_METADATA_OFFSET;
    $$.real = $3;
  };

demostart_header:
  DEMOSTART ':' real '\n' {
    $$.key = TJA_METADATA_DEMOSTART;
    $$.real = $3;
  };

side_header:
  SIDE ':' text '\n' {
    $$.key = TJA_METADATA_SIDE;
    $$.integer = tja_interpret_side_($3);
    taiko_free_(parser->alloc, $3);
  };

course_header:
  COURSE ':' text '\n' {
    $$.key = TJA_METADATA_COURSE;
    $$.integer = tja_interpret_course_($3);
    taiko_free_(parser->alloc, $3);
  };

level_header:
  LEVEL ':' real '\n' {
    $$.key = TJA_METADATA_LEVEL;
    $$.real = $3;
  };

style_header:
  STYLE ':' text '\n' {
    $$.key = TJA_METADATA_STYLE;
    $$.integer = tja_interpret_style_($3);
    taiko_free_(parser->alloc, $3);
  };

balloon_header:
  balloon_branch ':' balloon_data '\n' {
    $$.key = $1;
    $$.balloon = $3;
  };

balloon_branch:
  BALLOON { $$ = TJA_METADATA_BALLOON; }
  | BALLOONNOR { $$ = TJA_METADATA_BALLOON; }
  | BALLOONEXP { $$ = TJA_METADATA_BALLOONEXP; }
  | BALLOONMAS { $$ = TJA_METADATA_BALLOONMAS; }

balloon_data:
  %empty {
    $$ = tja_balloon_create2_(parser->alloc);
  }
  | INTEGER {
    $$ = tja_balloon_append_(tja_balloon_create2_(parser->alloc), $1);
  }
  | balloon_data ',' INTEGER {
    $$ = tja_balloon_append_($1, $3);
  };

papamama_command:
  PAPAMAMA_CMD '\n' {
    $$.key = TJA_METADATA_PAPAMAMA;
    $$.integer = true;
  }

unrecognized_header:
  IDENTIFIER ':' text '\n' {
    taiko_free_(parser->alloc, $1);
    taiko_free_(parser->alloc, $3);
    $$.key = TJA_METADATA_UNRECOGNIZED;
  }
  | unrecognized_command {
    $$.key = TJA_METADATA_UNRECOGNIZED;
  }

body:
  start_command sections end_command {
    taiko_course_set_style_($2.course, $1);
    $$ = $2.course;
  };

start_command:
  START_CMD text '\n' {
    if ($2 && strcmp($2, "P2") == 0) {
      $$ = TAIKO_STYLE_TJA_R_;
    } else {
      $$ = TAIKO_STYLE_TJA_L_;
    }
    taiko_free_(parser->alloc, $2);
  };

end_command:
  END_CMD '\n';

sections:
  measures {
    tja_coursebody_init_(&$$, parser->alloc);
    tja_coursebody_append_common_(&$$, &$1);
    put_section_(parser, $1.segment, PURPOSE_SEGMENT);
  }
  | sections branched_section {
    tja_coursebody_append_branched_(&$1, &$2);
    $$ = $1;

    put_section_(parser, $2.common, PURPOSE_BRANCH_COMMON);
    for (int i = 0; i < 3; ++i)
      put_section_(parser, $2.branches[i], PURPOSE_BRANCH(i));
  }
  | sections common_section {
    tja_coursebody_append_common_(&$1, &$2);
    $$ = $1;
    put_section_(parser, $2.segment, PURPOSE_SEGMENT);
  };

branched_section:
  branchstart_command {
    memset(&$$, 0, sizeof(tja_branched));

    // record branch info
    $$.branchtype = $1.type;
    $$.threshold_advanced = $1.advanced;
    $$.threshold_master = $1.master;
  }
  | branched_section branch_command measures {
    if (!($1.branches[$2]))
      $1.branches[$2] = get_section_(parser, PURPOSE_BRANCH($2));

    tja_branched_assign_(&$1, &$3, $2);
    $$ = $1;
    put_section_(parser, $3.segment, PURPOSE_SEGMENT);
  };

branchstart_command:
  BRANCHSTART_CMD IDENTIFIER ',' real ',' real '\n' {
    $$.type = tja_branch_type_($2);
    $$.advanced = tja_branch_type_convert_threshold_($2, $4);
    $$.master = tja_branch_type_convert_threshold_($2, $6);
    taiko_free_(parser->alloc, $2);
  }
  | BRANCHSTART_CMD '\n' {
    // Extension: #BRANCHSTART without parameters does not create a
    // branch point. Instead, it is used to allow commands like #MEASURE
    // to be applied at the exact same time of all branches.
    $$.type = TAIKO_BRANCHTYPE_NONE;
  };

branch_command:
  N_CMD '\n' { $$ = TAIKO_BRANCH_NORMAL; }
  | E_CMD '\n' { $$ = TAIKO_BRANCH_ADVANCED; }
  | M_CMD '\n' { $$ = TAIKO_BRANCH_MASTER; };

common_section:
  branchend_command measures {
    $$ = $2;
  };

branchend_command:
  BRANCHEND_CMD '\n';

measures:
  measurestart_events {
    tja_segment_init_(&$$, get_section_(parser, PURPOSE_SEGMENT));
    tja_segment_push_events_(&$$, &$1);
    put_section_(parser, $1.events, PURPOSE_MEASURESTART_EVENTS);
  }
  | measures ',' measurestart_events {
    tja_segment_push_barline_(&$1, 0);
    tja_segment_push_events_(&$1, &$3);
    put_section_(parser, $3.events, PURPOSE_MEASURESTART_EVENTS);
    $$ = $1;
  }
  | measures note_events ',' measurestart_events {
    tja_segment_push_barline_(&$1, $2.units);
    tja_segment_push_events_(&$1, &$2);
    put_section_(parser, $2.events, PURPOSE_MEASURE);
    tja_segment_push_events_(&$1, &$4);
    put_section_(parser, $4.events, PURPOSE_MEASURESTART_EVENTS);
    $$ = $1;
  };

note_events:
  NOTE {
    $$.events = get_section_(parser, PURPOSE_MEASURE);
    $$.units = 0;
    $$.levelhold = false;

    tja_events_push_note_(&$$, $1);
  }
  | note_events NOTE {
    tja_events_push_note_(&$1, $2);
    $$ = $1;
  }
  | note_events note_command {
    tja_events_push_event_(&$1, &$2);
    $$ = $1;
  };

measurestart_events:
  %empty {
    $$.events = get_section_(parser, PURPOSE_MEASURESTART_EVENTS);
    $$.units = 0;
    $$.levelhold = false;
  }
  | measurestart_events measurestart_command {
    tja_events_push_event_(&$1, &$2);
    $$ = $1;
  };

measurestart_command:
  measure_command
  | section_command
  | note_command;

note_command:
  gogostart_command
  | gogoend_command
  | scroll_command
  | bpmchange_command
  | levelhold_command
  | unrecognized_command;

measure_command:
  MEASURE_CMD INTEGER '/' INTEGER '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_TJA_MEASURE_LENGTH;
    $$.tja_measure_length.dividend = $2;
    $$.tja_measure_length.divisor = $4;
  };

section_command:
  SECTION_CMD '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_BRANCH_START;
  };

levelhold_command:
  LEVELHOLD_CMD '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_TJA_LEVELHOLD;
  };

gogostart_command:
  GOGOSTART_CMD '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_GOGOSTART;
  };

gogoend_command:
  GOGOEND_CMD '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_GOGOEND;
  };

scroll_command:
  SCROLL_CMD real '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_SCROLL;
    $$.detail_float.value = $2;
  };

bpmchange_command:
  BPMCHANGE_CMD real '\n' {
    memset(&$$, 0, sizeof(taiko_event));
    $$.type = TAIKO_EVENT_BPM;
    $$.detail_float.value = $2;
  };

unrecognized_command:
  COMMAND text '\n' {
    $$.type = TAIKO_EVENT_NONE;
    taiko_free_(parser->alloc, $1);
    taiko_free_(parser->alloc, $2);
  };

real:
  REAL { $$ = $1;}
  | INTEGER { $$ = (double)$1; };

text:
  %empty {
    $$ = taiko_strndup_(parser->alloc, "", 0);
  }
  | TEXT {
    $$ = $1;
  }

%%

extern int tja_yylex_init(yyscan_t *scanner);
extern int tja_yylex_destroy(yyscan_t scanner);

extern void tja_yyset_extra(void *data, yyscan_t scanner);
extern void tja_yyset_debug(int debug, yyscan_t scanner);

extern void tja_yyrestart(FILE *file, yyscan_t scanner);

tja_parser *tja_parser_create_(void) {
  return tja_parser_create2_(&taiko_default_allocator_);
}

tja_parser *tja_parser_create2_(taiko_allocator *alloc) {
  tja_parser *parser = taiko_malloc_(alloc, sizeof(tja_parser));
  taiko_file *error = taiko_file_open_stdio_(stderr);
  yyscan_t scanner = NULL;
  tja_yylex_init(&scanner);

  if (!parser || !error || !scanner) {
    taiko_free_(alloc, parser);
    taiko_file_close_(error);
    if (scanner) tja_yylex_destroy(scanner);
    return NULL;
  }

  memset(parser, 0, sizeof(tja_parser));
  parser->alloc = alloc;
  parser->lexer = scanner;
  tja_yyset_extra(parser->alloc, scanner);
  parser->error_stream = error;
  return parser;
}

void tja_parser_free_(tja_parser *parser) {
  tja_yylex_destroy(parser->lexer);
  taiko_file_close_(parser->error_stream);
  taiko_free_(parser->alloc, parser);
}

static taiko_courseset *tja_parser_parse_(tja_parser *parser) {
#ifdef YYDEBUG
  tja_yydebug = 1;
  tja_yyset_debug(1, parser->lexer);
#endif
  int errcode = tja_yyparse(parser, parser->lexer);
  tja_metadata_free_(parser->metadata);

  if (errcode) {
    taiko_courseset_free(parser->set);
    parser->set = NULL;
  }

  taiko_courseset *set = parser->set;
  parser->set = NULL;
  return set;
}

taiko_courseset *tja_parser_parse_file_(tja_parser *parser, const char *file) {
  FILE *f = fopen(file, "r");
  taiko_courseset *result = tja_parser_parse_stdio_(parser, f);
  fclose(f);
  return result;
}

taiko_courseset *tja_parser_parse_stdio_(tja_parser *parser, FILE *stream) {
  if (!stream)
    return NULL;
  tja_yyrestart(stream, parser->lexer);

  return tja_parser_parse_(parser);
}

void tja_parser_error_(tja_parser *parser, const char *format, ...) {
  va_list ap;

  // format input
  va_start(ap, format);
  int size = vsnprintf(NULL, 0, format, ap) + 1;
  va_end(ap);
  char *formatted = taiko_malloc_(parser->alloc, size);
  if (formatted) {
    va_start(ap, format);
    vsnprintf(formatted, size, format, ap);
    va_end(ap);
  }

  // forward to error output
  if (formatted)
    taiko_file_printf_(parser->error_stream, "TJA parser error: %s\n", formatted);
  taiko_free_(parser->alloc, formatted);
}

static void tja_yyerror(tja_parser *parser, yyscan_t lexer,
                    const char *msg) {
  tja_parser_error_(parser, "%s", msg);
}

static taiko_section *get_section_(tja_parser *parser, int purpose) {
  return taiko_section_create2_(parser->alloc);
}

static void put_section_(tja_parser *parser, taiko_section *section,
                  int purpose) {
  taiko_section_free_(section);
}
