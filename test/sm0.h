#ifndef SM_ID_sm0
#define SM_ID_sm0 0

#include <sm_lib.h>
#include <stdlib.h>
#include <stdio.h>

extern const sm_State sm_vStates_sm0[];


#include "sm1.h"
struct sm0
{
  int i;
  int maxi0, maxi1;
};

void act0_sm0(struct sm_Inst *sm_this);

void act_sm0(struct sm_Inst *sm_this, int maxi0, int maxi1);

void close_sm0(struct sm_Inst *sm_this);

void init_sm0(struct sm_Inst *sm_this);

#define SM_STATE_state0 2

#define SM_STATE_state1 3

#define SM_STATE_waitSm1 1


#define SM_DEP_sm0 1 \
  + SM_DEP_sm1 \

#endif

