#include <stddef.h>

#include "sm_lib.h"

#include "sm0.h"

int sm_loop(size_t sm_n, struct sm_Insts *vpInsts[static sm_n])
{
  struct sm_Pipeline *pPL = NULL;
  sm_State *vvStates[sm_n];
  
  
  vvStates[SM_ID_sm0] = sm_vStates_sm0;
  sm_addToPipeline(&pPL, SM_ID_sm0, SM_DEP_sm0);


  sm_innerLoop(sm_n, vvStates, vpInsts, pPL);
  sm_closePipeline(&pPL);

  return 0;
}