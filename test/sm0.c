#include "sm0.h"

#include <stdio.h>
#include <stdlib.h>


static void state0
(
  struct sm_Inst *sm_pInst,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  struct sm0 *sm_this = sm_pInst->pdata;

  printf("%s: %d\n", __func__, sm_this->i);

  act0_sm0(sm_pInst);
}

static void state1
(
  struct sm_Inst *sm_pInst,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  struct sm0 *sm_this = sm_pInst->pdata;

  printf("%s: %d\n", __func__, sm_this->i);

  close_sm0(sm_pInst);
  sm_destroy(sm_nInst, sm_vpInsts, SM_ID_sm0, sm_pInst);
}

sm_State sm_vStates_sm0[2] = {state0, state1};


static unsigned int acts0[1] = {0}, acts1[1] = {1}, acts2[1] = {2};
static struct sm_VActs vArcs[3] = {{1, acts0}, {1, acts1}, {1, acts2}};
static unsigned int col_i[3] = {1, 2, 0}, row_i[4] = {0, 1, 2, 3};
static struct sm_ArcTable tb = {vArcs, col_i, row_i};


void init_sm0(struct sm_Inst *sm_pInst)
{
  struct sm0 *sm_this = sm_pInst->pdata;
  unsigned int next;

  if (!sm_next_state(&tb, sm_pInst->state, 0, &next))
    sm_pInst->state = next;
  else return;

  sm_this = malloc(sizeof(struct sm0));

  sm_this->i = 0;

  printf("%s\n", __func__);

  sm_pInst->pdata = sm_this;
}

void act0_sm0(struct sm_Inst *sm_pInst)
{
  struct sm0 *sm_this = sm_pInst->pdata;
  unsigned int next;

  if (!sm_next_state(&tb, sm_pInst->state, 1, &next))
    sm_pInst->state = next;
  else return;

  sm_this->i = 1;

  printf("%s\n", __func__);

  sm_pInst->pdata = sm_this;
}

void close_sm0(struct sm_Inst *sm_pInst)
{
  struct sm0 *sm_this = sm_pInst->pdata;
  unsigned int next;

  if (!sm_next_state(&tb, sm_pInst->state, 2, &next))
    sm_pInst->state = next;
  else return;

  free(sm_this);
  sm_this = NULL;

  printf("%s\n", __func__);

  sm_pInst->pdata = sm_this;
}
