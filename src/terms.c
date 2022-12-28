#include "terms.h"
#include "sglib.h"

#include <stdlib.h>
#include <string.h>

#define TRY_ALLOC(type, p, v) \
{ \
  if ((p = malloc(sizeof(type))) != NULL) *p = v; \
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

struct Deps
{
  char *name;
  struct Deps *next;
};

static int depcmp(struct Deps *deps0, struct Deps *deps1)
{
  return strcmp(deps0->name, deps1->name);
}

struct States
{
  struct StateTm tm;
  unsigned int id;
  _Bool def;
  struct States *next;
};

static int statecmp(struct States *state0, struct States *state1)
{
  return strcmp(state0->tm.id, state1->tm.id);
}

static int statecmp0(struct States *state0, struct States *state1)
{
  return state0->id == state1->id? 0: state0->id > state1->id? 1: -1;
}

enum Efect
{
  INIT, NORM, DEST
};

struct Acts
{
  struct ActTm tm;
  unsigned int id;
  _Bool def, used;
  enum Efect efect;
  struct Acts *next;
};

static int actcmp(struct Acts *act0, struct Acts *act1)
{
  return strcmp(act0->tm.id, act1->tm.id);
}

struct Arcs
{
  unsigned int id0, id1;
  struct Arcs *next;
};

static int arccmp(struct Arcs *arc0, struct Arcs *arc1)
{
  return arc0->id0 == arc1->id0?
         (arc0->id1 == arc1->id1? 0: arc0->id1 > arc1->id1? 1: -1):
         arc0->id0 > arc1->id0? 1: -1;
}

int terms_eval(struct Tms *pTms, unsigned int sm_id, char *sm_name, FILE *sm_h, FILE *sm_c)
{
  struct Deps *deps = NULL, *elemDeps = NULL, *memberDeps = NULL, valueDeps;
  unsigned int stateId = 1;
  struct States *states = NULL, *elemStates = NULL, *memberStates = NULL, *memberStates0 = NULL, valueStates = {.def=0, .next=NULL};
  unsigned int actId = 0;
  struct Acts *acts = NULL, *elemActs = NULL, *memberActs = NULL, valueActs = {.def=0, .used=0, .efect=NORM, .next=NULL};
  struct Arcs *arcs = NULL, *elemArcs = NULL, *memberArcs = NULL, valueArcs;
  enum Efect efect;
  unsigned int nActs, prevRow, colPos;
  _Bool found = 0, emptyArgs;

  fprintf(
    sm_h,
    "#ifndef SM_ID_%s\n"
    "#define SM_ID_%s %d\n"
    "\n"
    "#include <sm_lib.h>\n"
    "#include <stdlib.h>\n"
    "#include <stdio.h>\n"
    "\n"
    "extern const sm_State sm_vStates_%s[];\n"
    "\n",
    sm_name,
    sm_name,
    sm_id,
    sm_name
  );

  fprintf(
    sm_c,
    "#include \"%s.h\"\n"
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
                "{%s};\n"
                "\n",
                sm_name,
                memberExtTms->pExtTm->extTm.dataTm.code
              );
              break;

            case STATE_TM:
              valueStates.id = stateId++;
              valueStates.tm = memberExtTms->pExtTm->extTm.stateTm;
              valueStates.def = 1;

              SGLIB_SORTED_LIST_FIND_MEMBER(
                struct States,
                states,
                &valueStates,
                statecmp,
                next,
                memberStates
              );

              if (memberStates != NULL)
              {
                if (memberStates->def)
                {
                  printf(
                    "Aviso: Sobrescrito el estado '%s'\n",
                    memberExtTms->pExtTm->extTm.stateTm.id
                  );
                  valueStates.id = memberStates->id;
                }
                else stateId--;

                *memberStates = valueStates;
                valueStates.def = 0;
              }
              else
              {
                TRY_ALLOC(struct States, elemStates, valueStates);
                SGLIB_SORTED_LIST_ADD(
                  struct States,
                  states,
                  elemStates,
                  statecmp,
                  next
                );
              }
              break;

            case ACT_TM:
              valueActs.id = actId++;
              valueActs.tm = memberExtTms->pExtTm->extTm.actTm;
              valueActs.def = 1;

              SGLIB_SORTED_LIST_FIND_MEMBER(
                struct Acts,
                acts,
                &valueActs,
                actcmp,
                next,
                memberActs
              );

              if (memberActs != NULL)
              {
                if (memberActs->def)
                {
                  printf(
                    "Aviso: Sobrescrita la acción '%s'\n",
                    memberExtTms->pExtTm->extTm.actTm.id
                  );
                  valueActs.id = memberActs->id;
                }
                else actId--;

                *memberActs = valueActs;
                valueActs.def = 0;
              }
              else
              {
                TRY_ALLOC(struct Acts, elemActs, valueActs);
                SGLIB_SORTED_LIST_ADD(struct Acts, acts, elemActs, actcmp, next);
              }
              break;

            case ARC_TM:
              efect = memberExtTms->pExtTm->extTm.arcTm.id0 == NULL? INIT:
                      memberExtTms->pExtTm->extTm.arcTm.id1 == NULL? DEST:
                      NORM;

              if (memberExtTms->pExtTm->extTm.arcTm.id0 != NULL)
              {
                valueStates.id = stateId++;
                valueStates.tm.id = memberExtTms->pExtTm->extTm.arcTm.id0;

                SGLIB_SORTED_LIST_FIND_MEMBER(
                  struct States,
                  states,
                  &valueStates,
                  statecmp,
                  next,
                  memberStates
                );

                if (memberStates != NULL)
                  stateId--;
                else
                {
                  TRY_ALLOC(struct States, memberStates, valueStates);
                  SGLIB_SORTED_LIST_ADD(
                    struct States,
                    states,
                    memberStates,
                    statecmp,
                    next
                  );
                }
              }

              if (memberExtTms->pExtTm->extTm.arcTm.id1 != NULL)
              {
                valueStates.id = stateId++;
                valueStates.tm.id = memberExtTms->pExtTm->extTm.arcTm.id1;

                SGLIB_SORTED_LIST_FIND_MEMBER(
                  struct States,
                  states,
                  &valueStates,
                  statecmp,
                  next,
                  memberStates0
                );

                if (memberStates0 != NULL)
                  stateId--;
                else
                {
                  TRY_ALLOC(struct States, memberStates0, valueStates);
                  SGLIB_SORTED_LIST_ADD(
                    struct States,
                    states,
                    memberStates0,
                    statecmp,
                    next
                  );
                }
              }

              switch (efect)
              {
              case INIT:
                valueArcs = ((struct Arcs) {0, memberStates0->id, NULL});
                break;
              case NORM:
                valueArcs = ((struct Arcs) {memberStates->id, memberStates0->id, NULL});
                break;
              case DEST:
                valueArcs = ((struct Arcs) {memberStates->id, 0, NULL});
                break;
              }

              SGLIB_SORTED_LIST_IS_MEMBER(
                struct Arcs,
                arcs,
                &valueArcs,
                arccmp,
                next,
                found
              );

              if (found)
              {
                printf(
                  "Aviso: Ignorada sobrescritura del arco '%s->%s'\n",
                  memberExtTms->pExtTm->extTm.arcTm.id0 != NULL?
                    memberExtTms->pExtTm->extTm.arcTm.id0: "",
                  memberExtTms->pExtTm->extTm.arcTm.id1 != NULL?
                    memberExtTms->pExtTm->extTm.arcTm.id1: ""
                );
              }
              else
              {
                TRY_ALLOC(struct Arcs, memberArcs, valueArcs);
                SGLIB_SORTED_LIST_ADD(struct Arcs, arcs, memberArcs, arccmp, next)

                fprintf(
                  sm_c,
                  "static const unsigned int sm_acts_%d_%d[] = {\n",
                  valueArcs.id0,
                  valueArcs.id1
                );

                memberStates = memberStates0 = NULL;
                nActs = 0;

                SGLIB_LIST_MAP_ON_ELEMENTS(
                  struct Ids,
                  memberExtTms->pExtTm->extTm.arcTm.pActs,
                  memberIds,
                  next,
                  {
                    valueActs.id = actId++;
                    valueActs.tm.id = memberIds->id;

                    SGLIB_SORTED_LIST_FIND_MEMBER(
                      struct Acts,
                      acts,
                      &valueActs,
                      actcmp,
                      next,
                      memberActs
                    );

                    if (memberActs != NULL)
                    {
                      actId--;
                    }
                    else
                    {
                      TRY_ALLOC(struct Acts, memberActs, valueActs);
                      SGLIB_SORTED_LIST_ADD(
                        struct Acts,
                        acts,
                        memberActs,
                        actcmp,
                        next
                      );
                    }
                    memberActs->used = 1;
                    memberActs->efect = efect;

                    fprintf(
                      sm_c,
                      "  %d,\n",
                      memberActs->id
                    );

                    nActs++;
                  }
                );

                fprintf(
                  sm_c,
                  "};\n"
                  "static const struct sm_VActs sm_vActs_%d_%d = {\n"
                  "  %d,\n"
                  "  sm_acts_%d_%d\n"
                  "};\n"
                  "\n",
                  valueArcs.id0,
                  valueArcs.id1,
                  nActs,
                  valueArcs.id0,
                  valueArcs.id1
                );

              }
              break;

            case IMPORT_TM:
              fprintf(
                sm_h,
                "\n"
                "#include \"%s.h\""
                "\n", 
                memberExtTms->pExtTm->extTm.importTm.id
              );

              valueDeps =
                ((struct Deps) {memberExtTms->pExtTm->extTm.importTm.id, NULL});

              SGLIB_SORTED_LIST_FIND_MEMBER(
                struct Deps,
                deps,
                &valueDeps,
                depcmp,
                next,
                memberDeps
              );

              if (memberDeps == NULL)
              {
                TRY_ALLOC(struct Deps, elemDeps, valueDeps);

                SGLIB_SORTED_LIST_ADD(struct Deps, deps, elemDeps, depcmp, next);
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
        fprintf(
          stderr, "Id de término '%d' no reconocido.\n",
          memberTms->pTm->id
        );
        abort();
      }
    }
  );

  fprintf(sm_c,
    "static const struct sm_VActs sm_vArcs[] = {\n"
  );
  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct Arcs,
    arcs,
    memberArcs,
    next,
    {
      fprintf(sm_c,
        "  sm_vActs_%d_%d,\n",
        memberArcs->id0,
        memberArcs->id1
      );
    }
  );
  fprintf(sm_c,
    "};\n"
  );

  fprintf(sm_c,
    "static const unsigned int sm_col_i[] = {\n"
  );
  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct Arcs,
    arcs,
    memberArcs,
    next,
    {
      fprintf(sm_c,
        "  %d,\n",
        memberArcs->id1
      );
    }
  );
  fprintf(sm_c,
    "}, sm_row_i[] = {\n"
  );


  if (arcs != NULL)
  {
    prevRow = arcs->id0;
    colPos = 0;

    fprintf(sm_c,
      "  %d,\n",
      colPos
    );
    SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
      struct Arcs,
      arcs->next,
      memberArcs,
      next,
      {
        colPos++;
        for (; prevRow < memberArcs->id0; ++prevRow)          
            fprintf(sm_c, "  %d,\n", colPos);
      }
    );
    fprintf(sm_c,
      "  %d\n",
      colPos+1
    );
  }
  fprintf(sm_c,
    "};\n"
    "static const struct sm_ArcTable sm_tb = {sm_vArcs, sm_col_i, sm_row_i};\n"
    "\n"
    "\n"
  );


  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct Acts,
    acts,
    memberActs,
    next,
    {
      if (memberActs->def)
      {
        if (!memberActs->used)
          fprintf(
            stderr,
            "Aviso: acción '%s' definida pero no usada\n",
            memberActs->tm.id
          );

        emptyArgs = strempty(memberActs->tm.args);

        fprintf(
          sm_h,
          "void %s(struct sm_Inst *sm_this%s%s);\n"
          "\n",
          memberActs->tm.id,
          emptyArgs? "": ", ",
          emptyArgs? "": memberActs->tm.args
        );

        fprintf(
          sm_c,
          "void %s(struct sm_Inst *sm_this%s%s)\n"
          "{\n"
          "  if (sm_this == NULL)\n"
          "  {\n"
          "    fprintf(stderr, \"Aviso: instancia sin crear\\n\"); return;\n"
          "  }\n"
          "\n"
          "  unsigned int sm_next;\n"
          "\n"
          "  if (!sm_next_state(&sm_tb, sm_this->state, %d, &sm_next))\n"
          "    sm_this->state = sm_next;\n"
          "  else\n"
          "  {\n"
          "    fprintf(\n"
          "      stderr,\n"
          "      \"Aviso: acción %d no procedente en estado %%d actual en %s\\n\",\n"
          "      sm_this->state\n"
          "    );\n"
          "    return;\n"
          "  }\n"
          "\n",
          memberActs->tm.id,
          emptyArgs? "": ", ",
          emptyArgs? "": memberActs->tm.args,
          memberActs->id,
          memberActs->id,
          sm_name
        );

        if (memberActs->efect == INIT)
          fprintf(
            sm_c,
            "  if (sm_this->pdata != NULL)\n"
            "  {\n"
            "    fprintf(stderr, \"Aviso: acción inicial ya realizada\\n\"); return;\n"
            "  }\n"
            "  else if ((sm_this->pdata = malloc(sizeof(struct %s))) == NULL)\n"
            "  {\n"
            "    perror(\"Aviso: No se pudo realizar acción inicial\"); return;\n"
            "  }\n"
            "\n",
            sm_name
          );

        fprintf(
          sm_c,
          "  if (sm_this->pdata == NULL)\n"
          "  {\n"
          "    fprintf(stderr, \"Aviso: realize la acćión inicial antes\\n\");\n"
          "    return;\n"
          "  }\n"
          "\n"
          "%s",
          memberActs->tm.code
        );

        if (memberActs->efect == DEST)
          fprintf(
            sm_c,
            "\n"
            "  free(sm_this->pdata);\n"
            "  sm_this->pdata = NULL;\n"
          );

        fprintf(
          sm_c,
          "}\n"
          "\n"
        );
      }
      else
        fprintf(
          stderr,
          "Aviso: acción '%s' declarada pero no definida\n",
          memberActs->tm.id
        );
    }
  );

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct States,
    states,
    memberStates,
    next,
    {
      if(memberStates->def)
      {
        fprintf(
          sm_c,
          "static void sm_%s\n"
          "(\n"
          "  struct sm_Inst *sm_this,\n"
          "  size_t sm_nInst,\n"
          "  struct sm_Insts *sm_vpInsts[static sm_nInst]\n"
          ")\n"
          "{\n"
          "  if (sm_this == NULL)\n"
          "  {\n"
          "    fprintf(stderr, \"Aviso: instancia sin crear\\n\"); return;\n"
          "  }\n"
          "\n"
          "%s"
          "}\n"
          "\n",
          memberStates->tm.id,
          memberStates->tm.code
        );

        fprintf(
          sm_h,
          "#define SM_STATE_%s %d\n"
          "\n",
          memberStates->tm.id,
          memberStates->id
        );
      }
      else
        fprintf(
          stderr,
          "Aviso: estado '%s' declarado pero no definido\n",
          memberStates->tm.id
        );
    }
  );

  fprintf(
    sm_c,
    "const sm_State sm_vStates_%s[] = {\n",
    sm_name
  );

  SGLIB_LIST_SORT(struct States, states, statecmp0, next);

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct States,
    states,
    memberStates,
    next,
    {
      fprintf(
        sm_c,
        "  sm_%s,\n",
        memberStates->tm.id
      );
    }
  );

  fprintf(
    sm_c,
    "};\n"
    "\n"
  );

  fprintf(sm_h, "\n");
  if (deps == NULL)
    fprintf(sm_h, "#define SM_DEP_%s 0\n", sm_name);
  else
  {
    fprintf(sm_h, "#define SM_DEP_%s 1 \\\n", sm_name);

    SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
      struct Deps,
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

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct States,
    states,
    memberStates,
    next,
    free(memberStates)
  );

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct Acts,
    acts,
    memberActs,
    next,
    free(memberActs)
  );

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct Arcs,
    arcs,
    memberArcs,
    next,
    free(memberArcs)
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
