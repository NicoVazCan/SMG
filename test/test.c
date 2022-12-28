#include <sm_lib.h>

#include "sm0.h"
#include "sm1.h"

int main(int argc, char const *argv[])
{
  struct sm_Insts *vpInsts[2] = {NULL, NULL};
  struct sm_Inst *psm0 = NULL, *psm1 = NULL;

  sm_create(2, vpInsts, SM_ID_sm1, &psm1);
  init_sm1(psm1);
  sm_create(2, vpInsts, SM_ID_sm0, &psm0);
  init_sm0(psm0);

  sm_loop(2, vpInsts);

  return 0;
}