%{
/* SPDX-License-Identifier: BSD-2-Clause */
#include "tja/parser.h"
#include "tja.tab.h"

#include "alloc.h"
#include "io.h"
#include <stdlib.h>

#define YYSTYPE TJA_YYSTYPE
#define YYLTYPE TJA_YYLTYPE

#define YY_INPUT(buf, result, max_size) \
  (result = taco_file_read_(yyextra->input, buf, max_size))

#define YY_USER_ACTION do { \
  yylloc->first_line = yylineno; \
  yylloc->first_column = 1; \
  yylloc->last_line = yylineno; \
  yylloc->last_column = 1; \
} while (0);

#define YY_NO_INPUT
#define YY_NO_UNPUT
%}

%option batch
%option never-interactive
%option reentrant
%option bison-bridge
%option bison-locations
%option yylineno
%option nounistd
%option prefix="tja_yy"
%option extra-type="tja_parser *"

NATURAL [0-9]+
INTEGER [+-]?{NATURAL}
EXPONENT [eE]{INTEGER}
REAL [+-]?{NATURAL}\.{NATURAL}{EXPONENT}?
SPACE [ \t]
NEWLINE [\r\n]

COLON \:{SPACE}*

IDSTART [a-zA-Z_]
IDCONT [a-zA-Z0-9_]
IDENTIFIER {IDSTART}{IDCONT}*

%x header_text
%x header_numeric
%x header_command
%x body
%x body_command
%x body_pretext
%x body_text

%%

<INITIAL>TITLE{COLON} {
  BEGIN(header_text);
  return TITLE;
}
<INITIAL>SUBTITLE{COLON} {
  BEGIN(header_text);
  return SUBTITLE;
}
<INITIAL>GENRE{COLON} {
  BEGIN(header_text);
  return GENRE;
}
<INITIAL>MAKER{COLON} {
  BEGIN(header_text);
  return MAKER;
}
<INITIAL>WAVE{COLON} {
  BEGIN(header_text);
  return AUDIO;
}
<INITIAL>BPM{COLON} {
  BEGIN(header_numeric);
  return BPM;
}
<INITIAL>OFFSET{COLON} {
  BEGIN(header_numeric);
  return OFFSET;
}
<INITIAL>DEMOSTART{COLON} {
  BEGIN(header_numeric);
  return DEMOSTART;
}
<INITIAL>SIDE{COLON} {
  BEGIN(header_text);
  return SIDE;
}
<INITIAL>COURSE{COLON} {
  BEGIN(header_text);
  return COURSE;
}
<INITIAL>LEVEL{COLON} {
  BEGIN(header_numeric);
  return LEVEL;
}
<INITIAL>STYLE{COLON} {
  BEGIN(header_text);
  return STYLE;
}
<INITIAL>SCOREINIT{COLON} {
  BEGIN(header_numeric);
  return SCOREINIT;
}
<INITIAL>SCOREDIFF{COLON} {
  BEGIN(header_numeric);
  return SCOREDIFF;
}
<INITIAL>BALLOON{COLON} {
  BEGIN(header_numeric);
  return BALLOON;
}
<INITIAL>BALLOONNOR{COLON} {
  BEGIN(header_numeric);
  return BALLOONNOR;
}
<INITIAL>BALLOONEXP{COLON} {
  BEGIN(header_numeric);
  return BALLOONEXP;
}
<INITIAL>BALLOONMAS{COLON} {
  BEGIN(header_numeric);
  return BALLOONMAS;
}
<INITIAL>NOTESDESIGNER{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER;
}
<INITIAL>NOTESDESIGNER0{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER0;
}
<INITIAL>NOTESDESIGNER1{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER1;
}
<INITIAL>NOTESDESIGNER2{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER2;
}
<INITIAL>NOTESDESIGNER3{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER3;
}
<INITIAL>NOTESDESIGNER4{COLON} {
  BEGIN(header_text);
  return NOTESDESIGNER4;
}

<INITIAL>#PAPAMAMA {
  BEGIN(header_command);
  return PAPAMAMA_CMD;
}

<header_numeric>, {
  return ',';
}

<INITIAL,header_numeric>{SPACE}+
<INITIAL>{NEWLINE}

<header_text>[^\r\n]* {
  yylval->text = taco_strndup_(yyextra->alloc, yytext, yyleng);
  BEGIN(header_numeric);
  return TEXT;
}

<header_numeric,header_command>{INTEGER} {
  yylval->integer = strtol(yytext, NULL, 10);
  return INTEGER;
}

<header_numeric,header_command>{REAL} {
  yylval->real = strtod(yytext, NULL);
  return REAL;
}

<header_numeric,header_command>{NEWLINE} {
  BEGIN(INITIAL);
  return '\n';
}

<header_numeric,header_command><<EOF>> {
  BEGIN(INITIAL);
  return '\n';
}

<INITIAL>#START {
  BEGIN(body_pretext);
  return START_CMD;
}

<body>#END {
  BEGIN(header_command);
  return END_CMD;
}

<body>[0-9A-Z] {
  yylval->integer = *yytext;
  return NOTE;
}

<body>{NEWLINE}
<body,body_command>, { return ','; }

<body>#BRANCHSTART {
  BEGIN(body_command);
  return BRANCHSTART_CMD;
}
<body>#N {
  BEGIN(body_command);
  return N_CMD;
}
<body>#E {
  BEGIN(body_command);
  return E_CMD;
}
<body>#M {
  BEGIN(body_command);
  return M_CMD;
}
<body>#BRANCHEND {
  BEGIN(body_command);
  return BRANCHEND_CMD;
}

<body>#MEASURE {
  BEGIN(body_command);
  return MEASURE_CMD;
}
<body>#SECTION {
  BEGIN(body_command);
  return SECTION_CMD;
}
<body>#LEVELHOLD {
  BEGIN(body_command);
  return SECTION_CMD;
}
<body>#GOGOSTART {
  BEGIN(body_command);
  return GOGOSTART_CMD;
}
<body>#GOGOEND {
  BEGIN(body_command);
  return GOGOEND_CMD;
}
<body>#SCROLL {
  BEGIN(body_command);
  return SCROLL_CMD;
}
<body>#BPMCHANGE {
  BEGIN(body_command);
  return BPMCHANGE_CMD;
}
<body>#DELAY {
  BEGIN(body_command);
  return DELAY_CMD;
}
<body>#BARLINEON {
  BEGIN(body_command);
  return BARLINEON_CMD;
}
<body>#BARLINEOFF {
  BEGIN(body_command);
  return BARLINEOFF_CMD;
}
<body>#BARLINE {
  BEGIN(body_command);
  return BARLINE_CMD;
}

<body_pretext>{SPACE}+ {
  BEGIN(body_text);
}

<body,body_command>{SPACE}+
<body_command>\/ { return '/'; }

<body_command>{INTEGER} {
  yylval->integer = strtol(yytext, NULL, 10);
  return INTEGER;
}

<body_command>{REAL} {
  yylval->real = strtod(yytext, NULL);
  return REAL;
}

<body_command>{IDENTIFIER} {
  yylval->text = taco_strndup_(yyextra->alloc, yytext, yyleng);
  return IDENTIFIER;
}

<body_text>[^\r\n]* {
  yylval->text = taco_strndup_(yyextra->alloc, yytext, yyleng);
  BEGIN(body_command);
  return TEXT;
}

<body_pretext,body_command>{NEWLINE} {
  BEGIN(body);
  return '\n';
}

<body_pretext,body_command><<EOF>> {
  BEGIN(body);
  return '\n';
}

<INITIAL>^{IDENTIFIER}{COLON} {
  yylval->text = taco_strndup_(yyextra->alloc, yytext, yyleng);
  BEGIN(header_text);
  return HEADER;
}

<INITIAL>#{IDENTIFIER} {
  yylval->text = taco_strndup_(yyextra->alloc, yytext + 1, yyleng - 1);
  BEGIN(header_text);
  return COMMAND;
}

<body>#{IDENTIFIER} {
  yylval->text = taco_strndup_(yyextra->alloc, yytext + 1, yyleng - 1);
  BEGIN(body_pretext);
  return COMMAND;
}

\/\/[^\r\n]* { /* discard comments */ }
\xef\xbb\xbf { /* discard U+FEFF */ }

<*>. {
  // fallback; feed into token stream, potentially causing parser to yell
  return *yytext;
}

%%

int yywrap(yyscan_t scanner) {
  return 1;
}
