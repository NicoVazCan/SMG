#ifndef SM_LIB
#define SM_LIB

#include <stddef.h>

#include "sglib.h"



// TIPOS DE DATOS


// Tabla de arcos entre estados (USO INTERNO)

struct sm_VArcs
{
  size_t len;
  unsigned int *vArcs;
};

struct sm_ArcTable
{
  struct sm_VArcs *pArcs;
  unsigned int *col_i;
  unsigned int *row_i;
};


// Coleción de instancias de una misma clase de SM

struct sm_Inst
{
  unsigned int state;
  void *pdata;
};

struct sm_Insts
{
  struct sm_Inst inst;
  struct sm_Insts *next;
};


// Prototipo de función a ejecutarse en cada estado (USO INTERNO)

typedef void (*sm_State)
(
  struct sm_Inst *sm_pInst,
  size_t sm_n,
  struct sm_Insts *vpInsts[static sm_n]
);


// Serie de grupos independientes de SM para procesarse en orden (USO INTERNO)

struct sm_DepGrp
{
  unsigned int sm_id;
  struct sm_DepGrp *next;
};

struct sm_Pipeline
{
  unsigned int sm_dep;
  struct sm_DepGrp *pDepGrps;
  struct sm_Pipeline *next;
};



// FUNCIONES


// Siguiente estado (USO INTERNO)

int sm_next_state
(
  const struct sm_ArcTable *pTb,
  unsigned int state,
  unsigned int act,
  unsigned int *nextState
);


// Crea y anhade al grupo de colecciones de instancias una nueva

int sm_create
(
  size_t sm_n,
  struct sm_Insts *vpInsts[static sm_n],
  unsigned int sm_id,
  struct sm_Inst **ppInst
);


// Destruye y elimina del grupo de colecciones de instancias una

int sm_destroy
(
  size_t sm_n,
  struct sm_Insts *vpInsts[static sm_n],
  unsigned int sm_id,
  struct sm_Inst *pInst
);


// Añade al pipeline una nueva SM segun sus dependencias con otras (USO INTERNO)

void sm_addToPipeline
(
  struct sm_Pipeline **sm_ppPl,
  unsigned int sm_id,
  unsigned int sm_dep
);


// Finaliza el pipeline (USO INTERNO)

void sm_closePipeline(struct sm_Pipeline **sm_ppPl);


// Bucle para ejecutar el pipeline de SM hasta que no quede ninguna (USO INTERNO)

void sm_innerLoop
(
  size_t sm_n,
  sm_State *vvStates[static sm_n],
  struct sm_Insts *sm_vpInsts[static sm_n],
  struct sm_Pipeline *sm_pl
);


// Bucle de SM

int sm_loop(size_t sm_n, struct sm_Insts *vpInsts[static sm_n]);

#endif