#ifndef SM_LIB
#define SM_LIB

#include <stddef.h>

#include "sglib.h"

struct sm_Inst
{
  unsigned int state;
  void *pdata;
};

struct sm_VArcs
{
  size_t len;
  unsigned int *vArcs;
};

struct sm_ArcTable
{
  struct sm_VArcs *pArcs;
  unsigned int *col_i;
  unsigned int *row_i;
};

struct sm_Insts
{
  struct sm_Inst inst;
  struct sm_Insts *next;
};

int sm_next_state
(
  const struct sm_ArcTable *pTb,
  unsigned int state,
  unsigned int act,
  unsigned int *nextState
);

typedef void (*sm_State)
(
  struct sm_Inst *sm_pInst,
  size_t sm_nInst,
  struct sm_Insts *vpInsts[static sm_nInst]
);

struct sm_VStates
{
  size_t len;
  sm_State *vStates;
};

struct sm_DepGrp
{
  unsigned int sm_id;
  struct sm_DepGrp *next;
};

struct sm_Pipeline
{
  unsigned int sm_dep;
  struct sm_DepGrp *pDepGrps;
  struct sm_Pipeline *next;
};

int sm_create
(
  size_t sm_nInst,
  struct sm_Insts *vpInsts[static sm_nInst],
  unsigned int sm_id,
  struct sm_Inst **ppInst
);

int sm_destroy
(
  size_t sm_nInst,
  struct sm_Insts *vpInsts[static sm_nInst],
  unsigned int sm_id,
  struct sm_Inst *pInst
);

#endif