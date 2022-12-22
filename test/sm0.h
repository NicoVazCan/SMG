#ifndef SM_ID_sm0
#define SM_ID_sm0 0
#define SM_DEP_sm0 0

#include "sm_lib.h"

struct sm0
{
  int i;
};

extern sm_State sm_vStates_sm0[];

void init_sm0(struct sm_Inst *sm_pInst);

void act0_sm0(struct sm_Inst *sm_pInst);

void close_sm0(struct sm_Inst *sm_pInst);

#endif
