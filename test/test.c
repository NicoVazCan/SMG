#include "test.h"

#include "sm0.h"

#include <stdio.h>
#include <stdlib.h>

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
  return &pInsts->inst == pInst? 0: &pInsts->inst > pInst? 1: -1;
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
  struct sm_VStates vStates[static sm_nInst],
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  struct sm_Insts *sm_pInsts;

  for (int i = 0; i < sm_nInst; ++i)
  {
    SGLIB_LIST_MAP_ON_ELEMENTS(
      struct sm_Insts,
      sm_vpInsts[i],
      sm_pInsts,
      next,
      {
        if (sm_pInsts->inst.state)
          vStates[i].vStates[sm_pInsts->inst.state-1]
          (
            &sm_pInsts->inst,
            sm_nInst,
            sm_vpInsts
          );
      }
    );
  }
  
}

int main(int argc, char const *argv[])
{
  struct sm_Insts *vpInsts[1] = {NULL};
  struct sm_Inst *psm0 = NULL;

  struct sm_VStates vStates[1];
  sm0(vStates);

  sm_create(1, vpInsts, SM0, &psm0);
  init_sm0(psm0);
  sm_update(1, vStates, vpInsts);
  act0_sm0(psm0);
  sm_update(1, vStates, vpInsts);
  close_sm0(psm0);
  sm_destroy(1, vpInsts, SM0, psm0);

  return 0;
}