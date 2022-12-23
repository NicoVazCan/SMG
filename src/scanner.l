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

spa     ([ \t\r\n\b]|{com})+
com     ("/*""/"*([^*/]|[^*]"/"|"*"[^/])*"*"*"*/")|("//".*)
str     "\""[^"]*"\""

import  import
data    data
state   state
act     act
arrow   "->"
comma   ,
lBrack "["
rBrack "]"
code   "{"[^}]*"}"
args   "("[^)]*")" 

letter  [a-zA-Z_]
digit   [0-9]
letdig  {letter}|{digit}

id      {letter}{letdig}*

%x E

%%

"$"           BEGIN(E); return BEG_EXT;


[^$]*         yylval.s = yytext; TRY_STRDUP(yylval.s); return CODE;

{str}|{com}   yymore();



<E>{spa}

<E>{data}     return DATA;
<E>{state}    return STATE;
<E>{act}      return ACT;
<E>{import}   return IMPORT;
<E>{arrow}    return ARROW;
<E>{comma}    return COMMA;
<E>{lBrack}   return L_BRACK;
<E>{rBrack}   return R_BRACK;
<E>{code}     {
  yylval.s = yytext+1;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-2] = '\0'; 
  return CODE;
}
<E>{args}     {
  yylval.s = yytext+1;
  TRY_STRDUP(yylval.s);
  yylval.s[yyleng-2] = '\0'; 
  return ARGS;
}
<E>{digit}    yylval.d = atoi(yytext); return DIGIT;
<E>{id}       yylval.s = yytext; TRY_STRDUP(yylval.s); return ID;


<E>"$"        BEGIN(INITIAL); return END_EXT;

<E>(.|"\n")   {
  fprintf(stderr, "Error léxico: token \"%s\" no reconocido\n", yytext); 
  exit(EXIT_FAILURE);
}

%%

int yywrap() { return 1; }
