%{
/* SPDX-License-Identifier: BSD-2-Clause */
#include "tja.tab.h"

#include "alloc.h"
#include <stdlib.h>

#define YYSTYPE TJA_YYSTYPE
%}

%option bison-bridge
%option reentrant
%option prefix="tja_yy"

INTEGER [0-9]+
EXPONENT [eE][+-]?{INTEGER}
REAL {INTEGER}\.{INTEGER}{EXPONENT}?
SPACE [\s\t]
NEWLINE [\r\n]

COLON \:{SPACE}*

IDSTART [a-zA-Z_]
IDCONT [a-zA-Z0-9_]
IDENTIFIER {IDSTART}{IDCONT}*

%x header_pretext
%x header_text
%x header_numeric
%x header_command
%x body
%x body_command
%x body_pretext
%x body_text

%%

<INITIAL>^TITLE {
  BEGIN(header_pretext);
  return TITLE;
}
<INITIAL>^SUBTITLE {
  BEGIN(header_pretext);
  return SUBTITLE;
}
<INITIAL>^GENRE {
  BEGIN(header_pretext);
  return GENRE;
}
<INITIAL>^MAKER {
  BEGIN(header_pretext);
  return MAKER;
}
<INITIAL>^AUDIO {
  BEGIN(header_pretext);
  return AUDIO;
}
<INITIAL>^BPM {
  return BPM;
}
<INITIAL>^OFFSET {
  return OFFSET;
}
<INITIAL>^DEMOSTART {
  return DEMOSTART;
}
<INITIAL>^SIDE {
  BEGIN(header_pretext);
  return SIDE;
}
<INITIAL>^COURSE {
  BEGIN(header_pretext);
  return COURSE;
}
<INITIAL>^LEVEL {
  return LEVEL;
}
<INITIAL>^STYLE {
  BEGIN(header_pretext);
  return STYLE;
}
<INITIAL>^BALLOON {
  return BALLOON;
}
<INITIAL>^BALLOONNOR {
  return BALLOONNOR;
}
<INITIAL>^BALLOONEXP {
  return BALLOONEXP;
}
<INITIAL>^BALLOONMAS {
  return BALLOONMAS;
}

<INITIAL>#PAPAMAMA {
  BEGIN(header_command);
  return PAPAMAMA_CMD;
}

<INITIAL>{COLON} {
  BEGIN(header_numeric);
  return ':';
}

<header_numeric>, {
  return ',';
}

<INITIAL,header_numeric,header_pretext>{SPACE}
<INITIAL>{NEWLINE}

<header_pretext>{COLON} {
  BEGIN(header_text);
  return ':';
}

<header_text>[^\r\n]* {
  yylval->text = taiko_strndup_(yyextra, yytext, yyleng);
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

<header_numeric,header_pretext,header_command>{NEWLINE} {
  BEGIN(INITIAL);
  return '\n';
}

<header_numeric,header_pretext,header_command><<EOF>> {
  return '\n';
}

<INITIAL>#START{SPACE}* {
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

<body_pretext>{SPACE}+ {
  BEGIN(body_text);
}

<body_command>{SPACE}
<body_command>\/ { return '/'; }

<body_command>{INTEGER} {
  yylval->integer = strtol(yytext, NULL, 10);
  return INTEGER;
}

<body_command>{REAL} {
  yylval->real = strtod(yytext, NULL);
  return REAL;
}

<body_text>[^\r\n]* {
  yylval->text = taiko_strndup_(yyextra, yytext, yyleng);
  BEGIN(body_command);
  return TEXT;
}

<body_pretext,body_command>{NEWLINE} {
  BEGIN(body);
  return '\n';
}

<body_pretext,body_command><<EOF>> {
  return '\n';
}

<INITIAL>^{IDENTIFIER} {
  yylval->text = taiko_strndup_(yyextra, yytext, yyleng);
  BEGIN(header_pretext);
  return IDENTIFIER;
}

<INITIAL>#{IDENTIFIER} {
  yylval->text = taiko_strndup_(yyextra, yytext + 1, yyleng - 1);
  BEGIN(header_pretext);
  return COMMAND;
}

<body>#{IDENTIFIER} {
  yylval->text = taiko_strndup_(yyextra, yytext + 1, yyleng - 1);
  BEGIN(body_pretext);
  return COMMAND;
}
%%

int yywrap(yyscan_t scanner) {
  return 1;
}
