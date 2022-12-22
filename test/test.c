#include "sm_lib.h"

#include "sm0.h"

int main(int argc, char const *argv[])
{
  struct sm_Insts *vpInsts[1] = {NULL};
  struct sm_Inst *psm00 = NULL, *psm01 = NULL;

  sm_create(1, vpInsts, SM_ID_sm0, &psm00);
  init_sm0(psm00);
  sm_create(1, vpInsts, SM_ID_sm0, &psm01);
  init_sm0(psm01);

  sm_loop(1, vpInsts);

  return 0;
}