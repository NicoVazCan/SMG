#ifndef SM_ID_sm1
#define SM_ID_sm1 1

#include <sm_lib.h>
#include <stdlib.h>
#include <stdio.h>

extern const sm_State sm_vStates_sm1[];

struct sm1
{
  int maxi0, maxi1;
};

void close(struct sm_Inst *sm_this);

void done(struct sm_Inst *sm_this);

void dos(struct sm_Inst *sm_this);

void init_sm1(struct sm_Inst *sm_this);

void uno(struct sm_Inst *sm_this);

#define SM_STATE_done 4

#define SM_STATE_dos 3

#define SM_STATE_uno 2

#define SM_STATE_unoOdos 1


#define SM_DEP_sm1 0

#endif

