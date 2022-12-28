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

static _Bool strempty(const char *s)
{
  const char *ecs = " \t\r\n\b";
  _Bool allEmpty = 1;

  for (const char *ps = s; allEmpty && *ps != '\0'; ++ps)
    for (const char *pe = ecs; allEmpty && *pe != *ps; ++pe)
      allEmpty = *pe != '\0';

  return allEmpty;
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

%token DATA 0 STATE 1 ACT 2 ARROW 3 COMMA 4 IMPORT 5
%token BEG_EXT 6 END_EXT 7 BEG_ARGS 8 END_ARGS 9
%token BEG_CODE 10 END_CODE 11 BEG_LIST 12 END_LIST 13
%token<s> CODE 14 ID 15
%token<d> DIGIT 16
%type<pTms> terms
%type<pTm> term
%type<pExtTm> extTerm dataTerm stateTerm actTerm
%type<pExtTm> arcTerm arcInitTerm arcCloseTerm importTerm
%type<pExtTms> extTerms
%type<pIds> ids
%type<s> codeTerm codeTerms
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

codeTerm :
    CODE { $<s>$ = $1; }
  | CODE ID ID {
      size_t c = strlen($1), id0 = strlen($2), id1 = strlen($3);
      char *m = malloc(c+id0+id1+25);

      if (m == NULL)
      {
       perror("No se pudo traducir método");
       exit(EXIT_FAILURE);
      }

      sprintf(m, "((struct %s*) (%s)->pdata)->%s", $2, $1, $3);
      free($1); free($2); free($3);

      $<s>$ = m;

    }
  | CODE ID CODE {
      size_t c0 = strlen($1), id = strlen($2), c1 = strlen($3);
      _Bool emptyArgs = strempty($3);
      char *m = malloc(c0+id+(emptyArgs? 0: c1)+5);

      if (m == NULL)
      {
       perror("No se pudo traducir método");
       exit(EXIT_FAILURE);
      }

      sprintf(
        m,
        "%s(%s%s%s)",
        $2,
        $1,
        emptyArgs? "": ", ",
        emptyArgs? "": $3
      );
      free($1); free($2); free($3);

      $<s>$ = m;
    }
;

codeTerms :
    { 
      $<s>$ = strdup("");

      if ($<s>$ == NULL)
      {
        perror("No se pudo traducir método");
        exit(EXIT_FAILURE);
      }
    }
  |
    codeTerm codeTerms {
      size_t c = strlen($1), cs = strlen($2);
      $1 = realloc($1, c+cs+1);

      if ($1 == NULL)
      {
       perror("No se pudo concatenar código");
       exit(EXIT_FAILURE);
      }

      strcat($1, $2);

      free($2);

      $<s>$ = $1;
    }
;

dataTerm :
    DATA BEG_CODE codeTerms END_CODE {
      TRY_ALLOC(
        struct ExtTm,
        $<pExtTm>$,
        ((struct ExtTm) {{.dataTm={$3}},
        DATA_TM})
      );
    }
;

stateTerm :
    STATE ID BEG_CODE codeTerms END_CODE {
      TRY_ALLOC(
        struct ExtTm,
        $<pExtTm>$,
        ((struct ExtTm) {{.stateTm={$2, $4}},
        STATE_TM})
      );
    }
;

actTerm :
    ACT ID BEG_ARGS codeTerms END_ARGS BEG_CODE codeTerms END_CODE {
      TRY_ALLOC(
        struct ExtTm,
        $<pExtTm>$,
        ((struct ExtTm) {{.actTm={$2, $4, $7}}, ACT_TM})
      );
    }
;

arcTerm :
    ID ARROW ID BEG_LIST ids END_LIST {
      TRY_ALLOC(
        struct ExtTm,
        $<pExtTm>$,
        ((struct ExtTm) {{.arcTm={$1, $3, $5}}, ARC_TM})
      );
    }
;

arcInitTerm :
    ARROW ID BEG_LIST ids END_LIST {
      TRY_ALLOC(
        struct ExtTm,
        $<pExtTm>$,
        ((struct ExtTm) {{.arcTm={NULL, $2, $4}}, ARC_TM})
      );
    }
;

arcCloseTerm :
    ID ARROW BEG_LIST ids END_LIST {
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

