#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "terms.h"

#define OPT_H 1
#define OPT_O 2


int main(int argc, char const *argv[])
{
  extern FILE *yyin;
  FILE *out;
  char const *fin = NULL, *fout = "sm.c";
  struct Tms *pTms = NULL;
  
  {
    int len;
    char opt = 0x00;
    const char *usag = "Modo de empleo: smg [OPCIÓN]... [FICHERO]",
               *help = "Pruebe 'smg --help' para más información.";

    for (int i = 1; i < argc; ++i)
    {
      len = strlen(argv[i]);

      if (argv[i][0] == '-' && len > 1)
      {
        if (argv[i][1] == '-' && len > 2)
        {
          if (!strcmp(argv[i]+2, "help"))
            opt |= OPT_H;
          else if (!strcmp(argv[i]+2, "output"))
            opt |= OPT_O;
        }
        else
          for (int j = 1; j < len; ++j)
            switch (argv[i][j])
            {
            case 'h':
              opt |= OPT_H; break;
            case 'o':
              opt |= OPT_O; break;
            default:
              printf("opción incorrecta «%c»\n", argv[i][j]);
              printf("%s\n", help);
              return 0;
            }

        for (int j = 0; j < 2; ++j)
          switch (opt & (1 << j)) {
          case OPT_H:
            printf("%s\n", usag);
            return 0;
          case OPT_O:
            fout = argv[++i];
          }
      }
      else fin = argv[i];
    }
  }

  yyin = fin == NULL? stdin: fopen(fin, "r");

  if (yyin == NULL)
  {
    perror("Fallo al abrir el fichero de entrada");
    return 0;
  }

  out = fopen(fout, "w");

  if (out == NULL)
  {
    perror("Fallo al crear el fichero de salida");
    return 0;
  }


  yyparse(&pTms);
  terms_eval(pTms, out);

  fclose(yyin);
  fclose(out);

  return 0;
}