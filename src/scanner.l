%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y.tab.h"

#define TRY_STRDUP(s) \
{ \
  if (((s) = strdup(s)) == NULL) \
    { perror("No se pudo duplicar el string " #s); exit(EXIT_FAILURE); } \
}
#define UPDT_LOC \
{ \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    yylloc.last_line += strnchr(yytext, '\n'); \
    yylloc.last_column = yylloc.first_line == yylloc.last_line? \
      yylloc.last_column+yyleng: 0; \
}

static size_t strnchr(const char *s, char f)
{
  size_t n = 0;

  for (const char *pc = s; *pc != '\0'; ++pc)
    if (*pc == f) ++n;

  return n;
}

%}

spa     [ \t\r\n\b]
com     ("/*""/"*([^*/]|[^*]"/"|"*"[^/])*"*"*"*/")|("//".*)
str     "\""[^"]*"\""

import  import
data    data
state   state
act     act
arrow   "->"
comma   ,
dot     .
lBrack "["
rBrack "]"

letter  [a-zA-Z_]
digit   [0-9]
letdig  {letter}|{digit}

id      {letter}{letdig}*

%x E C M

%%

"$"           BEGIN(E); return BEG_EXT;


[^$]*         yylval.s = yytext; TRY_STRDUP(yylval.s); return CODE;

{str}|{com}|{spa}+ {
  yymore();
}


<E>{data}     UPDT_LOC; return DATA;
<E>{state}    UPDT_LOC; return STATE;
<E>{act}      UPDT_LOC; return ACT;
<E>{import}   UPDT_LOC; return IMPORT;
<E>{arrow}    UPDT_LOC; return ARROW;
<E>{comma}    UPDT_LOC; return COMMA;
<E>"$("       UPDT_LOC; BEGIN(C); return BEG_ARGS;
<E>"${"       UPDT_LOC; BEGIN(C); return BEG_CODE;
<E>"$["       UPDT_LOC; return BEG_LIST;
<E>"$]"       UPDT_LOC;return END_LIST;
<E>{digit}    UPDT_LOC; yylval.d = atoi(yytext); return DIGIT;
<E>{id}       UPDT_LOC; yylval.s = yytext; TRY_STRDUP(yylval.s); return ID;

<E>"$"        UPDT_LOC; BEGIN(INITIAL); return END_EXT;

<E>{com}|{spa}+ {
  UPDT_LOC;
}

<E>(.|"\n")   UPDT_LOC; yylval.s = yytext; TRY_STRDUP(yylval.s); return ERROR;


<C>"$)"       UPDT_LOC; BEGIN(E); return END_ARGS;
<C>"$}"       UPDT_LOC; BEGIN(E); return END_CODE;
<C>"$"([^)}][^$]*)?"$" {
  UPDT_LOC;
  BEGIN(M);
  yylval.s = yytext+1;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-2] = '\0';
  return CODE;
}

<C>[^$]*      UPDT_LOC; yylval.s = yytext; TRY_STRDUP(yylval.s); return CODE;

<C>{str}|{com}|{spa}+ {
  UPDT_LOC;
  yymore();
}

<C>(.|"\n")   UPDT_LOC; yylval.s = yytext; TRY_STRDUP(yylval.s); return ERROR;


<M>{arrow}{id} {
  UPDT_LOC;
  yylval.s = yytext+2; TRY_STRDUP(yylval.s); return ID;
}
<M>{dot}{id} {
  UPDT_LOC;
  BEGIN(C); yylval.s = yytext+1; TRY_STRDUP(yylval.s); return ID;
}
<M>"$("([^$][^)]|"$"[^)]|[^$]")")*(.|"\n")?"$)" {
  UPDT_LOC;
  BEGIN(C);
  yylval.s = yytext+2;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-4] = '\0';
  return CODE;
}

<M>(.|"\n")   UPDT_LOC; yylval.s = yytext; TRY_STRDUP(yylval.s); return ERROR;

%%

int yywrap() { return 1; }
