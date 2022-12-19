#ifndef TERMS
#define TERMS

#include <stdio.h>

#define DATA_TM   0
#define STATE_TM  1
#define ACT_TM    2
#define ARC_TM    3
#define CODE_TM   4
#define EXT_TM    5

struct Ids
{
  char *id;
  struct Ids *next;
};

struct ExtTm
{
  union
  {
    struct DataTm
    {
      char *id;
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
      char *code;
    } actTm;

    struct ArcTm
    {
      char *id0;
      char *id1;
      struct Ids *pActs;
    } arcTm;
  } extTm;

  int nTm;
};

struct ExtTms
{
  struct ExtTm *pExtTm;
  struct ExtTms *next;
};

struct Tm
{
  union
  {
    char* code;
    struct ExtTms *pExtTms;
  } tm;

  int nTm;
};

struct Tms
{
  struct Tm *pTm;
  struct Tms *next;
};

int terms_eval(struct Tms *pTms, FILE *out);

#endif