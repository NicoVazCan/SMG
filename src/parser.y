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
void yyerror (struct Tm **, char const*);
%}

%union{
    char *s;
    struct Tm *pTm;
    struct Tms *pTms;
    struct Ids *pIds;
}

%parse-param {struct Tm **pptm}

%token DATA 0 STATE 1 ACT 2 ARC 3 ARROW 4 COMMA 5 IMPORT 6
%token L_PAREN 7 R_PAREN 8 L_BRACE 9 R_BRACE 10
%token<s> CODE 11
%type<pTm> term codeTerm dataTerm stateTerm actTerm
%type<pTm> arcTerm arcInitTerm arcCloseTerm importTerm
%type<pIds> ids
%type<s> code
%start S

%%

S : 
  term { *pptm = $1; }
;

term :
    codeTerm {      $<pTm>$ = $1; }
  | dataTerm {      $<pTm>$ = $1; }
  | stateTerm {     $<pTm>$ = $1; }
  | actTerm {       $<pTm>$ = $1; }
  | arcTerm {       $<pTm>$ = $1; }
  | arcInitTerm {   $<pTm>$ = $1; }
  | arcCloseTerm {  $<pTm>$ = $1; }
  | importTerm {    $<pTm>$ = $1; }
;

code:
    CODE { 
      $<s>$ = $1;
    }
  | CODE code {
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
  | L_PAREN code ARROW CODE R_PAREN {
      size_t c = strlen($2), id = strlen($4);
      char *m = malloc(c+id+23);

      if (m == NULL)
      {
       perror("No se pudo traducir método");
       exit(EXIT_FAILURE);
      }

      sprintf(m, "((struct %s*) (%s)->pdata)", $4, $2);
      free($2); free($4);

      $<s>$ = m;

    }
  | L_PAREN code ARROW CODE L_PAREN code R_PAREN R_PAREN {
      size_t c0 = strlen($2), id = strlen($4), c1 = strlen($6);
      _Bool emptyArgs = strempty($4);
      char *m = malloc(c0+id+(emptyArgs? 0: c1)+5);

      if (m == NULL)
      {
       perror("No se pudo traducir método");
       exit(EXIT_FAILURE);
      }

      sprintf(
        m,
        "%s(%s%s%s)",
        $4,
        $2,
        emptyArgs? "": ", ",
        emptyArgs? "": $6
      );
      free($2); free($4); free($6);

      $<s>$ = m;
    }
;

codeTerm :
    CODE {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.codeTm={$1}},
        CODE_TM})
      );
    }
;

dataTerm :
    DATA L_BRACE CODE R_BRACE {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.dataTm={$3}},
        DATA_TM})
      );
    }
;

stateTerm :
    STATE CODE L_BRACE code R_BRACE {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.stateTm={$2, $4}},
        STATE_TM})
      );
    }
;

actTerm :
    ACT CODE L_PAREN CODE R_PAREN L_BRACE code R_BRACE {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.actTm={$2, $4, $7}}, ACT_TM})
      );
    }
;

arcTerm :
    ARC CODE ARROW CODE L_PAREN ids R_PAREN {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.arcTm={$2, $4, $6}}, ARC_TM})
      );
    }
;

arcInitTerm :
    ARC ARROW CODE L_PAREN ids R_PAREN {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.arcTm={NULL, $3, $5}}, ARC_TM})
      );
    }
;

arcCloseTerm :
    ARC CODE ARROW L_PAREN ids R_PAREN {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.arcTm={$2, NULL, $5}}, ARC_TM})
      );
    }
;

importTerm :
    IMPORT L_PAREN CODE R_PAREN {
      TRY_ALLOC(
        struct Tm,
        $<pTm>$,
        ((struct Tm) {{.importTm={$3}}, IMPORT_TM})
      );
    }
;

ids :
    CODE COMMA ids {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, $3}));
    }
  | CODE COMMA {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, NULL}));
    }
  | CODE {
      TRY_ALLOC(struct Ids, $<pIds>$, ((struct Ids){$1, NULL}));
    }
;

%%

void yyerror(struct Tm **pptm, char const *msg)
{
  //if(strcmp(msg, "syntax error"))
    fprintf(stderr, "%s\n", msg);
}

