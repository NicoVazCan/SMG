#ifndef sm_id_sm0
#define sm_id_sm0 0

#include "test.h"

struct sm0
{
  int i;
};

extern unsigned int sm_dep_sm0;

void sm0(struct sm_VStates *sm_pvStates);

void init_sm0(struct sm_Inst *sm_pInst);

void act0_sm0(struct sm_Inst *sm_pInst);

void close_sm0(struct sm_Inst *sm_pInst);

#endif
