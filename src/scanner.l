%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}

spa     ([ \t\r\n\b]|{com})+
com     ("/*""/"*([^*/]|[^*]"/"|"*"[^/])*"*"*"*/")|("//".*)
str     "\""[^"]*"\""

data    data
state   state
act     action
arrow   "->"
comma   ,
begActs "["
endActs "]"
code    "{"[^}]*"}"

letter  [a-zA-Z_]
digit   [0-9]
letdig  {letter}|{digit}

id      {letter}{letdig}*

%x E

%%

"$"           BEGIN(E); return BEG_EXT;


[^$]*         yylval.s = strdup(yytext); return CODE;

{str}|{com}   yymore();



<E>{spa}

<E>{data}     return DATA;
<E>{state}    return STATE;
<E>{act}      return ACT;
<E>{arrow}    return ARROW;
<E>{comma}    return COMMA;
<E>{begActs}  return BEG_ACTS;
<E>{endActs}  return END_ACTS;
<E>{code}     yylval.s = strdup(yytext); return CODE;

<E>"$"        BEGIN(INITIAL); return END_EXT;

<E>{id}       yylval.s = strdup(yytext); return ID;


<E>(.|"\n")   {
  fprintf(stderr, "Error léxico: token \"%s\" no reconocido\n", yytext); 
  exit(EXIT_FAILURE);
}

%%

int yywrap() { return 1; }
