#ifndef TERMS
#define TERMS

#include <stdio.h>

enum IdTm
{
  DATA_TM,
  STATE_TM,
  ACT_TM,
  ARC_TM,
  IMPORT_TM,
  CODE_TM,
};

struct Ids
{
  char *id;
  struct Ids *next;
};

struct Tm
{
  union
  {
    struct CodeTm
    {
      char *code;
    } codeTm;

    struct DataTm
    {
      char *code;
    } dataTm;

    struct StateTm
    {
      char *id;
      char *code;
    } stateTm;

    struct ActTm
    {
      char *id;
      char *args;
      char *code;
    } actTm;

    struct ArcTm
    {
      char *id0;
      char *id1;
      struct Ids *pActs;
    } arcTm;

    struct ImportTm
    {
      char *id;
    } importTm;
  } tm;

  enum IdTm id;
};

struct Tms
{
  struct Tm *pTm;
  struct Tms *next;
};

int terms_eval(struct Tms *pTms, unsigned int sm_id, char *sm_name, FILE *sm_h, FILE *sm_c);

void terms_free(struct Tms **ppTms);

#endif