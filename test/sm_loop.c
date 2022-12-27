#include <sm_lib.h>
#include <stddef.h>

#include "sm0.h"
#include "sm1.h"

int sm_loop(size_t sm_n, struct sm_Insts *vpInsts[static sm_n])
{
  struct sm_Pipeline *pPL = NULL;
  sm_State const *vvStates[sm_n];

  vvStates[SM_ID_sm0] = sm_vStates_sm0;
  sm_addToPipeline(&pPL, SM_ID_sm0, SM_DEP_sm0);
  
  vvStates[SM_ID_sm1] = sm_vStates_sm1;
  sm_addToPipeline(&pPL, SM_ID_sm1, SM_DEP_sm1);
  

  sm_innerLoop(sm_n, vvStates, vpInsts, pPL);
  sm_closePipeline(&pPL);

  return 0;
}