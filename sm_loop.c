#include <sm_lib.h>
#include <stddef.h>

#include "sm.h"

int sm_loop(size_t sm_n, struct sm_Insts *vpInsts[static sm_n])
{
  struct sm_Pipeline *pPL = NULL;
  sm_State const *vvStates[sm_n];

  vvStates[SM_ID_sm] = sm_vStates_sm;
  sm_addToPipeline(&pPL, SM_ID_sm, SM_DEP_sm);
  

  sm_innerLoop(sm_n, vvStates, vpInsts, pPL);
  sm_closePipeline(&pPL);

  return 0;
}
