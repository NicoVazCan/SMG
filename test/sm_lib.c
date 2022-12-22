#include "sm_lib.h"

#include "sm0.h"

#include <stdlib.h>
#include <unistd.h>


static int intcmp(int n1, int n2) { return n1 == n2? 0: n1 > n2? 1: -1; }

int sm_next_state
(
  const struct sm_ArcTable *pTb,
  unsigned int state,
  unsigned int act,
  unsigned int *nextState
)
{
  int row_s = pTb->row_i[state];
  int row_e = pTb->row_i[state+1];
  _Bool found = 0;
  int result_index = -1;

  for (int r = row_s, c = pTb->col_i[r];
       r < row_e;
       c = pTb->col_i[++r])
  {
    SGLIB_ARRAY_BINARY_SEARCH(
      int,
      pTb->pArcs[r].vArcs,
      0,
      (pTb->pArcs[r].len-1),
      act,
      intcmp,
      found,
      result_index
    );

    if (found)
    {
      *nextState = c;
      return 0;
    }
  }

  return 1;
}

int sm_create
(
  size_t sm_nInst,
  struct sm_Insts *vpInsts[static sm_nInst],
  unsigned int sm_id,
  struct sm_Inst **ppInst
)
{
  struct sm_Insts *pInsts = malloc(sizeof(struct sm_Insts));

  if (pInsts == NULL) return -1;

  pInsts->next = NULL;
  pInsts->inst.state = 0;
  *ppInst = &pInsts->inst;

  SGLIB_LIST_ADD(struct sm_Insts, vpInsts[sm_id], pInsts, next);

  return 0;
}

int instcmp(struct sm_Insts *pInsts, struct sm_Inst *pInst)
{
  return &pInsts->inst == pInst? 0:
         &pInsts->inst > pInst? 1: -1;
}

int sm_destroy
(
  size_t sm_nInst,
  struct sm_Insts *vpInsts[static sm_nInst],
  unsigned int sm_id,
  struct sm_Inst *pInst
)
{
  if (pInst->state != 0) return -1;

  struct sm_Insts *member = NULL;


  SGLIB_LIST_DELETE_IF_MEMBER(
    struct sm_Insts,
    vpInsts[sm_id],
    pInst,
    instcmp,
    next,
    member
  );

  if (member == NULL) return 1;

  free(member);

  return 0;
}

static void sm_update
(
  size_t sm_nInst,
  sm_State *vvStates[static sm_nInst],
  struct sm_Insts *sm_vpInsts[static sm_nInst],
  struct sm_Pipeline *sm_pl
)
{
  struct sm_Insts *sm_pInsts;
  struct sm_Pipeline *memberPL;
  struct sm_DepGrp *memberDG;

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct sm_Pipeline,
    sm_pl,
    memberPL,
    next,
    {
      SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
        struct sm_DepGrp,
        memberPL->pDepGrps,
        memberDG,
        next,
        {
          SGLIB_LIST_MAP_ON_ELEMENTS(
            struct sm_Insts,
            sm_vpInsts[memberDG->sm_id],
            sm_pInsts,
            next,
            {
              if (sm_pInsts->inst.state)
                vvStates[memberDG->sm_id][sm_pInsts->inst.state-1]
                (
                  &sm_pInsts->inst,
                  sm_nInst,
                  sm_vpInsts
                );
            }
          );
        }
      );
    }
  );
}

void sm_loop
(
  size_t sm_nInst,
  sm_State *vvStates[static sm_nInst],
  struct sm_Insts *sm_vpInsts[static sm_nInst],
  struct sm_Pipeline *sm_pl
)
{
  _Bool insts = 1;

  while (insts)
  {
    for (int i = 0; insts && i < sm_nInst; ++i)
      insts = sm_vpInsts[i] != NULL;

    if (insts)
      sm_update(sm_nInst, vvStates, sm_vpInsts, sm_pl);

    sleep(1);
  }
}

static int depcmp(struct sm_DepGrp *pDG0, struct sm_DepGrp *pDG1)
{
  return pDG0->sm_id == pDG1->sm_id? 0: pDG0->sm_id > pDG1->sm_id? 1: -1;
}

static int plcmp(struct sm_Pipeline *pPL0, struct sm_Pipeline *pPL1)
{
  return pPL0->sm_dep == pPL1->sm_dep? 0: pPL0->sm_dep > pPL1->sm_dep? 1: -1;
}

void sm_addToPipeline
(
  struct sm_Pipeline **sm_ppPl,
  unsigned int sm_id,
  unsigned int sm_dep
)
{
  struct sm_Pipeline elemPL = {sm_dep, NULL, NULL}, *memberPL = NULL;
  struct sm_DepGrp *pDepGrps;

  pDepGrps = malloc(sizeof(struct sm_DepGrp));
  *pDepGrps = (struct sm_DepGrp) {sm_id, NULL};

  SGLIB_SORTED_LIST_FIND_MEMBER(
    struct sm_Pipeline,
    *sm_ppPl,
    &elemPL,
    plcmp,
    next,
    memberPL
  );

  if (memberPL == NULL)
  {
    memberPL = malloc(sizeof(struct sm_Pipeline));
    *memberPL = elemPL;

    SGLIB_SORTED_LIST_ADD(
      struct sm_Pipeline,
      *sm_ppPl,
      memberPL,
      plcmp,
      next
    );
  }

  SGLIB_SORTED_LIST_ADD(
    struct sm_DepGrp,
    memberPL->pDepGrps,
    pDepGrps,
    depcmp,
    next
  );
}

void sm_closePipeline(struct sm_Pipeline **sm_ppPl)
{
  struct sm_Pipeline *pPL;
  struct sm_DepGrp *pDG;

  SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
    struct sm_Pipeline,
    *sm_ppPl,
    pPL,
    next,
    {
      SGLIB_SORTED_LIST_MAP_ON_ELEMENTS(
        struct sm_DepGrp,
        pPL->pDepGrps,
        pDG,
        next,
        free(pDG);
      );
      free(pPL);
    }
  );

  *sm_ppPl = NULL;
}
