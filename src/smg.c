#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "y.tab.h"
#include "terms.h"
#include <sglib.h>

#define OPT_H 1

struct Files
{
  char *name;
  FILE *sm;
  FILE *h;
  FILE *c;
  struct Files *next;
};

static int filescmp(struct Files *files0, struct Files *files1)
{
  return strcmp(files0->name, files1->name);
}

int main(int argc, char const *argv[])
{
  extern FILE *yyin;
  char *name, *aux;
  _Bool existFile = 0;
  FILE *sm_loop = NULL, *sm = NULL, *h = NULL, *c = NULL;
  struct Files *files = NULL, *elem = NULL, value;
  struct Tms *pTms = NULL;
  unsigned int sm_id = 0;
  
  {
    int len;
    char opt = 0x00;
    const char *usag = "Modo de empleo: smg [OPCIÓN]... [FICHERO]...",
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
        }
        else
          for (int j = 1; j < len; ++j)
            switch (argv[i][j])
            {
            case 'h':
              opt |= OPT_H; break;
            default:
              printf("opción incorrecta «%c»\n", argv[i][j]);
              printf("%s\n", help);
              return 0;
            }

          for (int j = 0; j < 2; ++j)
            switch (opt & (1 << j))
            {
            case OPT_H:
              printf("%s\n", usag);
              return 0;
            }
      }
      else
      {
        value.name = (char*) argv[i];
        SGLIB_SORTED_LIST_IS_MEMBER(
          struct Files,
          files,
          &value,
          filescmp,
          next,
          existFile
        );

        if (!existFile)
        {
          elem = malloc(sizeof(struct Files));

          if (elem == NULL)
          {
            perror("No se pudo almacenar nombre de fichero");
            return 1;
          }
          name = strdup(argv[i]);

          if (name == NULL)
          {
            free(elem);
            perror("No se pudo almacenar nombre de fichero");
            return 1;
          }
          sm = fopen(argv[i], "r");

          if (sm == NULL)
          {
            free(name);
            fprintf(stderr, "No se pudo abrir el fichero '%s'\n", argv[i]);
            perror(NULL);
            return 1;
          }

          aux = strrchr(name, '.');

          if (aux == NULL)
          {
            int size = sizeof(name);
            name = realloc(name, size);

            if (name == NULL)
            {
              perror("No se pudo almacenar la extension");
              return 1;
            }
            name[size-1] = '.';
            aux = name + size - 1;
          }
          aux[1] = 'h';
          aux[2] = '\0';

          h = fopen(name, "w");

          if (h == NULL)
          {
            fclose(sm);
            fprintf(stderr, "No se pudo crear el fichero '%s'\n", name);
            free(name);
            perror(NULL);
            return 1;
          }

          aux[1] = 'c';

          c = fopen(name, "w");

          if (c == NULL)
          {
            fclose(sm);
            fclose(h);
            fprintf(stderr, "No se pudo crear el fichero '%s'\n", name);
            free(name);
            perror(NULL);
            return 1;
          }

          aux[0] = '\0';

          *elem = (struct Files) {name, sm, h, c, NULL};
          SGLIB_SORTED_LIST_ADD(struct Files, files, elem, filescmp, next);
        }
      }
    }
  }

  if (files == NULL)
  {
    char *aux;
    elem = malloc(sizeof(struct Files));

    if (elem == NULL)
    {
      perror("No se pudo almacenar nombre de fichero");
      return 1;
    }
    name = strdup("sm.h");

    if (name == NULL)
    {
      free(elem);
      perror("No se pudo almacenar nombre de fichero");
      return 1;
    }
    sm = stdin;

    aux = name + 2;

    h = fopen(name, "w");

    if (h == NULL)
    {
      fprintf(stderr, "No se pudo abrir el fichero '%s'\n", name);
      free(name);
      perror(NULL);
      return 1;
    }

    aux[1] = 'c';

    c = fopen(name, "w");

    if (c == NULL)
    {
      fclose(sm);
      fclose(h);
      fprintf(stderr, "No se pudo abrir el fichero '%s'\n", name);
      free(name);
      perror(NULL);
      return 1;
    }

    aux[0] = '\0';

    *elem = (struct Files) {name, sm, h, c, NULL};
    SGLIB_SORTED_LIST_ADD(struct Files, files, elem, filescmp, next);
  }

  elem = NULL;


  sm_loop = fopen("sm_loop.c", "w");

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(struct Files, files, elem, next,
    {
      yyin = elem->sm;
      yyparse(&pTms);
      terms_eval(pTms, sm_id, elem->name, elem->h, elem->c);
      terms_free(&pTms);
      sm_id++;
    }
  );

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(struct Files, files, elem, next,
    {
      free(elem->name);
      if (elem->sm != stdin) fclose(elem->sm);
      fclose(elem->h);
      fclose(elem->c);
      free(elem);
    }
  );

  fclose(sm_loop);

  return 0;
}
