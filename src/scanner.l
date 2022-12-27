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
arc     arc
arrow   "->"
comma   ,

%%

"$"{data}   puts("DATA"); return DATA;
"$"{state}  puts("STATE"); return STATE;
"$"{act}    puts("ACT"); return ACT;
"$"{arc}    puts("ARC"); return ARC;
"$"{import} puts("IMPORT"); return IMPORT;
"$"{arrow}  puts("ARROW"); return ARROW;
"$"{comma}  puts("COMA"); return COMMA;

"$("        puts("L_PAREN"); return L_PAREN;
"$)"        puts("R_PAREN"); return R_PAREN;

"${"        puts("L_BRACE"); return L_BRACE;
"$}"        puts("R_BRACE"); return R_BRACE;

[^$]*       puts("CODE"); yylval.s = yytext; TRY_STRDUP(yylval.s); return CODE;

{str}|{com}|{spa}+ {
  yymore();
}

.|"\n"   {
  fprintf(stderr, "Error léxico: token \"%s\" no reconocido\n", yytext); 
  exit(EXIT_FAILURE);
}

%%

int yywrap() { return 1; }
