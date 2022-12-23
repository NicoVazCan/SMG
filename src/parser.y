%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "terms.h"

#define TRY_ALLOC(type, p, v) \
{ \
  if (((p) = malloc(sizeof(type))) != NULL) *p = v; \
  else { perror("No se pudo alojar " #v); exit(EXIT_FAILURE); } \
}

extern char yylex();
void yyerror (struct Tms **, char const*);
%}

%union{
    char *s;
    int d;
    struct Tm *pTm;
    struct Tms *pTms;
    struct ExtTm *pExtTm;
    struct ExtTms *pExtTms;
    struct Ids *pIds;
}

%parse-param {struct Tms **pptms}

%token DATA 0 STATE 1 ACT 2 ARROW 3 COMMA 4
%token BEG_EXT 5 END_EXT 6 L_BRACK 7 R_BRACK 8 IMPORT 9
%token<s> CODE 10 ID 11 ARGS 12
%token<d> DIGIT 13
%type<pTms> terms
%type<pTm> term
%type<pExtTm> extTerm dataTerm stateTerm actTerm
%type<pExtTm> arcTerm arcInitTerm arcCloseTerm importTerm
%type<pExtTms> extTerms
%type<pIds> ids
%start S

%%

S : 
    terms { *pptms = $1; }

terms :
    { $<pTms>$ = NULL; }
  | term terms {
    TRY_ALLOC(struct Tms, $<pTms>$, ((struct Tms) {$1, $2}));
  }
;

term :
    CODE {
      TRY_ALLOC(struct Tm, $<pTm>$, ((struct Tm) {{.code=$1}, CODE_TM}));
    }
  | BEG_EXT extTerms END_EXT {
      TRY_ALLOC(struct Tm, $<pTm>$, ((struct Tm) {{.pExtTms=$2}, EXT_TM}));
    }

extTerms :
    { $<pExtTms>$ = NULL; }
  | extTerm extTerms {
    TRY_ALLOC(struct ExtTms, $<pExtTms>$, ((struct ExtTms) {$1, $2}));
  }
;

extTerm :
    dataTerm { $<pExtTm>$ = $1; }
  | stateTerm { $<pExtTm>$ = $1; }
  | actTerm { $<pExtTm>$ = $1; }
  | arcTerm { $<pExtTm>$ = $1; }
  | arcInitTerm { $<pExtTm>$ = $1; }
  | arcCloseTerm { $<pExtTm>$ =  $1; }
  | importTerm { $<pExtTm>$ = $1; }
;

dataTerm :
  DATA CODE {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.dataTm={$2}},
      DATA_TM})
    );
  }
;

stateTerm :
  STATE ID CODE {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.stateTm={$2, $3}},
      STATE_TM})
    );
  }
;

actTerm :
  ACT ID ARGS CODE {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.actTm={$2, $3, $4}}, ACT_TM})
    );
  }
;

arcTerm :
  ID ARROW ID L_BRACK ids R_BRACK {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.arcTm={$1, $3, $5}}, ARC_TM})
    );
  }
;

arcInitTerm :
  ARROW ID L_BRACK ids R_BRACK {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.arcTm={NULL, $2, $4}}, ARC_TM})
    );
  }
;

arcCloseTerm :
  ID ARROW L_BRACK ids R_BRACK {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.arcTm={$1, NULL, $4}}, ARC_TM})
    );
  }
;

importTerm :
  IMPORT ID {
    TRY_ALLOC(
      struct ExtTm,
      $<pExtTm>$,
      ((struct ExtTm) {{.importTm={$2}}, IMPORT_TM})
    );
  }
;

ids :
    ID COMMA ids {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, $3}));
    }
  | ID COMMA {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, NULL}));
    }
  | ID {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, NULL}));
    }
;

%%

void yyerror(struct Tms **pptms, char const *msg)
{
  //if(strcmp(msg, "syntax error"))
    fprintf(stderr, "%s\n", msg);
}

