#include "terms.h"
#include "sglib.h"

#include <stdlib.h>
#include <string.h>

#define TRY_ALLOC(type, p, v) \
{ \
  if ((p = malloc(sizeof(type))) != NULL) *p = v; \
  else { perror("No se pudo alojar " #v); exit(EXIT_FAILURE); } \
}

struct sm_Deps
{
  char *name;
  struct sm_Deps *next;
};

static int depcmp(struct sm_Deps *deps0, struct sm_Deps *deps1)
{
  return strcmp(deps0->name, deps1->name);
}

int terms_eval(struct Tms *pTms, unsigned int sm_id, char *sm_name, FILE *sm_h, FILE *sm_c)
{
  struct sm_Deps *deps = NULL, *elemDeps = NULL, *memberDeps = NULL, valueDeps;

  fprintf(
    sm_h,
    "#ifndef SM_ID_%s\n"
    "#define SM_ID_%s %d\n"
    "\n"
    "#include \"sm_lib.h\"\n"
    "\n"
    "extern sm_State sm_vStates_%s[];\n"
    "\n",
    sm_name,
    sm_name,
    sm_id,
    sm_name
  );

  fprintf(
    sm_c,
    "#include <%s.h>\n"
    "\n",
    sm_name
  );

  SGLIB_LIST_MAP_ON_ELEMENTS(
    struct Tms,
    pTms,
    memberTms,
    next,
    {
      switch (memberTms->pTm->id)
      {
      case CODE_TM:
        fprintf(sm_c, "%s", memberTms->pTm->tm.code);
        break;

      case EXT_TM:
        SGLIB_LIST_MAP_ON_ELEMENTS(
          struct ExtTms,
          memberTms->pTm->tm.pExtTms,
          memberExtTms,
          next,
          {
            switch (memberExtTms->pExtTm->id)
            {
            case DATA_TM:
              fprintf(
                sm_h,
                "struct %s\n"
                "{%s}\n"
                "\n",
                sm_name,
                memberExtTms->pExtTm->extTm.dataTm.code
              );
              break;

            case STATE_TM:
              fprintf(
                sm_c,
                "static void %s\n"
                "(\n"
                "  struct sm_Inst *sm_pInst,\n"
                "  size_t sm_nInst,\n"
                "  struct sm_Insts *sm_vpInsts[static sm_nInst]\n"
                ")\n"
                "{%s}\n"
                "\n",
                memberExtTms->pExtTm->extTm.stateTm.id,
                memberExtTms->pExtTm->extTm.stateTm.code
              );
              break;

            case ACT_TM:
              fprintf(
                sm_h,
                "void %s(struct sm_Inst *sm_pInst, %s);\n"
                "\n",
                memberExtTms->pExtTm->extTm.actTm.id,
                memberExtTms->pExtTm->extTm.actTm.args
              );
              fprintf(
                sm_c,
                "void %s(struct sm_Inst *sm_pInst, %s)\n"
                "{\n"
                "  struct %s *sm_this = sm_pInst->pdata;\n"
                "  unsigned int next;\n"
                "\n"
                "  if (!sm_next_state(&tb, sm_pInst->state, 0, &next))\n"
                "    sm_pInst->state = next;\n"
                "  else return;\n"
                "\n"
                "%s"
                "\n"
                "  sm_pInst->pdata = sm_this;\n"
                "}\n"
                "\n",
                memberExtTms->pExtTm->extTm.actTm.id,
                memberExtTms->pExtTm->extTm.actTm.args,
                sm_name,
                memberExtTms->pExtTm->extTm.actTm.code
              );
              break;

            case ARC_TM:
              /*
              if (memberExtTms->pExtTm->extTm.arcTm.id0 != NULL)
                free(memberExtTms->pExtTm->extTm.arcTm.id0);

              if (memberExtTms->pExtTm->extTm.arcTm.id1 != NULL)
                free(memberExtTms->pExtTm->extTm.arcTm.id1);

              SGLIB_LIST_MAP_ON_ELEMENTS(
                struct Ids,
                memberExtTms->pExtTm->extTm.arcTm.pActs,
                memberIds,
                next,
                {
                  free(memberIds->id);
                  memberIds->id = NULL;
                  free(memberIds);
                }
              );*/
              break;

            case IMPORT_TM:
              fprintf(
                sm_h,
                "\n"
                "#include <%s.h>"
                "\n", 
                memberExtTms->pExtTm->extTm.importTm.id
              );

              valueDeps =
                ((struct sm_Deps) {memberExtTms->pExtTm->extTm.importTm.id, NULL});

              SGLIB_SORTED_LIST_FIND_MEMBER(
                struct sm_Deps,
                deps,
                &valueDeps,
                depcmp,
                next,
                memberDeps
              );

              if (memberDeps == NULL)
              {
                TRY_ALLOC(struct sm_Deps, elemDeps, valueDeps);

                SGLIB_SORTED_LIST_ADD(struct sm_Deps, deps, elemDeps, depcmp, next);
              }
              break;

            default:
              fprintf(
                stderr, "Id de término '%d' no reconocido.\n",
                memberExtTms->pExtTm->id
              );
              abort();
            }
          }
        );
        break;

      default:
        fprintf(stderr,
          "Id de término '%d' no reconocido.\n",
          memberTms->pTm->id
        );
        abort();
      }
    }
  );

  fprintf(sm_h, "\n");
  if (deps == NULL)
    fprintf(sm_h, "#define SM_DEP_%s 0\n", sm_name);
  else
  {
    fprintf(sm_h, "#define SM_DEP_%s 1 \\\n", sm_name);

    SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
      struct sm_Deps,
      deps,
      elemDeps,
      next,
      {
        fprintf(sm_h, "  + SM_DEP_%s \\\n", elemDeps->name);
        free(elemDeps);
      }
    );
  }
  
  fprintf(
    sm_h,
    "\n"
    "#endif\n"
    "\n"
  );

  return 0;
}

void terms_free(struct Tms **ppTms)
{
  SGLIB_LIST_MAP_ON_ELEMENTS(
    struct Tms,
    *ppTms,
    memberTms,
    next,
    {
      switch (memberTms->pTm->id)
      {
      case CODE_TM:
        free(memberTms->pTm->tm.code); 
        break;

      case EXT_TM:
        SGLIB_LIST_MAP_ON_ELEMENTS(
          struct ExtTms,
          memberTms->pTm->tm.pExtTms,
          memberExtTms,
          next,
          {
            switch (memberExtTms->pExtTm->id)
            {
            case DATA_TM:
              free(memberExtTms->pExtTm->extTm.dataTm.code);
              break;

            case STATE_TM:
              free(memberExtTms->pExtTm->extTm.stateTm.id);
              free(memberExtTms->pExtTm->extTm.stateTm.code);
              break;

            case ACT_TM:
              free(memberExtTms->pExtTm->extTm.actTm.id);
              free(memberExtTms->pExtTm->extTm.actTm.args);
              free(memberExtTms->pExtTm->extTm.actTm.code);
              break;

            case ARC_TM:
              if (memberExtTms->pExtTm->extTm.arcTm.id0 != NULL)
                free(memberExtTms->pExtTm->extTm.arcTm.id0);

              if (memberExtTms->pExtTm->extTm.arcTm.id1 != NULL)
                free(memberExtTms->pExtTm->extTm.arcTm.id1);

              SGLIB_LIST_MAP_ON_ELEMENTS(
                struct Ids,
                memberExtTms->pExtTm->extTm.arcTm.pActs,
                memberIds,
                next,
                {
                  free(memberIds->id);
                  memberIds->id = NULL;
                  free(memberIds);
                }
              );
              break;

            case IMPORT_TM:
              free(memberExtTms->pExtTm->extTm.importTm.id);
              break;

            default:
              fprintf(
                stderr, "Id de término '%d' no reconocido.\n",
                memberExtTms->pExtTm->id
              );
              abort();
            }

            free(memberExtTms->pExtTm);
            memberExtTms->pExtTm = NULL;
            free(memberExtTms);
          }
        );
        break;

      default:
        fprintf(stderr,
          "Id de término '%d' no reconocido.\n",
          memberTms->pTm->id
        );
        abort();
      }

      free(memberTms->pTm); 
      memberTms->pTm = NULL;

      free(memberTms);
    }
  );

  *ppTms = NULL;
}
