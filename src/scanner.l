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


<E>{data}     return DATA;
<E>{state}    return STATE;
<E>{act}      return ACT;
<E>{import}   return IMPORT;
<E>{arrow}    return ARROW;
<E>{comma}    return COMMA;
<E>"$("       BEGIN(C); puts("BEG_ARGS"); return BEG_ARGS;
<E>"${"       BEGIN(C); puts("BEG_CODE"); return BEG_CODE;
<E>"$["       return BEG_LIST;
<E>"$]"       return END_LIST;
<E>{digit}    yylval.d = atoi(yytext); return DIGIT;
<E>{id}       yylval.s = yytext; TRY_STRDUP(yylval.s); return ID;

<E>"$"        BEGIN(INITIAL); return END_EXT;

<E>{com}|{spa}+

<E>(.|"\n")   {
  fprintf(stderr, "Error léxico: token \"%s\" no reconocido\n", yytext); 
  exit(EXIT_FAILURE);
}


<C>"$)"       BEGIN(E); return END_ARGS;
<C>"$}"       BEGIN(E); return END_CODE;
<C>"$"([^)}][^$]*)?"$" {
  puts("METHOD_C");
  BEGIN(M);
  yylval.s = yytext+1;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-2] = '\0';
  return CODE;
}

<C>[^$]* {
              yylval.s = yytext; TRY_STRDUP(yylval.s); return CODE;
}

<C>{str}|{com}|{spa}+ {
  yymore();
}


<M>{arrow}{id} {
  yylval.s = yytext+2; TRY_STRDUP(yylval.s); return ID;
}
<M>{dot}{id} {
  BEGIN(C); yylval.s = yytext+1; TRY_STRDUP(yylval.s); return ID;
}
<M>"$("([^$][^)]|"$"[^)]|[^$]")")*(.|"\n")?"$)" {
  BEGIN(C);
  yylval.s = yytext+3;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-5] = '\0';
  return CODE;
}

<M>{str}|{com}|{spa}+ {
  yymore();
}

%%

int yywrap() { return 1; }
