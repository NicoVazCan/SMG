%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "terms.h"

extern char yylex();
void yyerror (struct Tms **, char const*);
%}

%union{
    char *s;
    struct Tm *pTm;
    struct Tms *pTms;
    struct ExtTm *pExtTm;
    struct ExtTms *pExtTms;
    struct Ids *pIds;
}

%parse-param {struct Tms **pptms}

%token DATA 0 STATE 1 ACT 2 ARROW 3 COMMA 4
%token BEG_EXT 5 END_EXT 6 BEG_ACTS 7 END_ACTS 8
%token<s> CODE 9 ID 10
%type<pTms> terms
%type<pTm> term
%type<pExtTm> extTerm dataTerm stateTerm actTerm arcTerm 
%type<pExtTms> extTerms
%type<pIds> acts
%start S

%%

S : 
    terms { *pptms = $1; }

terms :
    { $<pTms>$ = NULL; }
  | term terms {
    $<pTms>$ = malloc(sizeof(struct Tms));
    *$<pTms>$ = (struct Tms) {$1, $2};
  }
;

term :
    CODE { 
    $<pTm>$ = malloc(sizeof(struct Tm));
    *$<pTm>$ = (struct Tm) {{.code=$1}, CODE_TM};
  }
  | BEG_EXT extTerms END_EXT {
    $<pTm>$ = malloc(sizeof(struct Tm));
    *$<pTm>$ = (struct Tm) {{.pExtTms=$2}, EXT_TM};
  }

extTerms :
    { $<pExtTms>$ = NULL; }
  | extTerm extTerms {
    $<pExtTms>$ = malloc(sizeof(struct ExtTms));
    *$<pExtTms>$ = (struct ExtTms) {$1, $2};
  }

extTerm :
    dataTerm { $<pExtTm>$ = $1; }
  | stateTerm { $<pExtTm>$ = $1; }
  | actTerm { $<pExtTm>$ = $1; }
  | arcTerm { $<pExtTm>$ = $1; }
;

dataTerm :
  DATA ID CODE {
    $<pExtTm>$ = malloc(sizeof(struct ExtTm));
    *$<pExtTm>$ = (struct ExtTm) {{.dataTm={$2, $3}}, DATA_TM};
  }
;

stateTerm :
  STATE ID CODE {
    $<pExtTm>$ = malloc(sizeof(struct ExtTm));
    *$<pExtTm>$ = (struct ExtTm) {{.stateTm={$2, $3}}, STATE_TM};
  }
;

actTerm :
  ACT ID CODE {
    $<pExtTm>$ = malloc(sizeof(struct ExtTm));
    *$<pExtTm>$ = (struct ExtTm) {{.actTm={$2, $3}}, ACT_TM};
  }
;

arcTerm :
  ID ARROW ID BEG_ACTS acts END_ACTS {
    $<pExtTm>$ = malloc(sizeof(struct ExtTm));
    *$<pExtTm>$ = (struct ExtTm) {{.arcTm={$1, $3, $5}}, ARC_TM};
  }
;

acts :
    ID COMMA acts {
    $<pIds>$ = malloc(sizeof(struct Ids));
    *$<pIds>$ = (struct Ids){$1, $3};
    }
  | ID COMMA {
    $<pIds>$ = malloc(sizeof(struct Ids));
    *$<pIds>$ = (struct Ids){$1, NULL};
  }
  | ID {
    $<pIds>$ = malloc(sizeof(struct Ids));
    *$<pIds>$ = (struct Ids){$1, NULL};
  }
;

%%

void yyerror(struct Tms **pptms, char const *msg)
{
  //if(strcmp(msg, "syntax error"))
    fprintf(stderr, "%s\n", msg);
}

