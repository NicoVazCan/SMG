#include "sm1.h"

#include <stdio.h>

static const unsigned int sm_acts_0_1[] = {
  0,
};
static const struct sm_VActs sm_vActs_0_1 = {
  1,
  sm_acts_0_1
};

static const unsigned int sm_acts_1_2[] = {
  1,
};
static const struct sm_VActs sm_vActs_1_2 = {
  1,
  sm_acts_1_2
};

static const unsigned int sm_acts_1_3[] = {
  2,
};
static const struct sm_VActs sm_vActs_1_3 = {
  1,
  sm_acts_1_3
};

static const unsigned int sm_acts_2_4[] = {
  3,
};
static const struct sm_VActs sm_vActs_2_4 = {
  1,
  sm_acts_2_4
};

static const unsigned int sm_acts_3_4[] = {
  3,
};
static const struct sm_VActs sm_vActs_3_4 = {
  1,
  sm_acts_3_4
};

static const unsigned int sm_acts_4_0[] = {
  4,
};
static const struct sm_VActs sm_vActs_4_0 = {
  1,
  sm_acts_4_0
};


static const struct sm_VActs sm_vArcs[] = {
  sm_vActs_0_1,
  sm_vActs_1_2,
  sm_vActs_1_3,
  sm_vActs_2_4,
  sm_vActs_3_4,
  sm_vActs_4_0,
};
static const unsigned int sm_col_i[] = {
  1,
  2,
  3,
  4,
  4,
  0,
}, sm_row_i[] = {
  0,
  1,
  2,
  3,
  4,
  5
};
static const struct sm_ArcTable sm_tb = {sm_vArcs, sm_col_i, sm_row_i};


void close(struct sm_Inst *sm_this)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }

  unsigned int sm_next;

  if (!sm_next_state(&sm_tb, sm_this->state, 4, &sm_next))
    sm_this->state = sm_next;
  else
  {
    fprintf(stderr, "Aviso: acción no procedente en estado actual");    return;
  }

  if (sm_this->pdata == NULL)
  {
    fprintf(stderr, "Aviso: realize la acćión inicial antes");
    return;
  }


  printf("%s\n", __func__);

  free(sm_this->pdata);
  sm_this->pdata = NULL;
}

void done(struct sm_Inst *sm_this)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }

  unsigned int sm_next;

  if (!sm_next_state(&sm_tb, sm_this->state, 3, &sm_next))
    sm_this->state = sm_next;
  else
  {
    fprintf(stderr, "Aviso: acción no procedente en estado actual");    return;
  }

  if (sm_this->pdata == NULL)
  {
    fprintf(stderr, "Aviso: realize la acćión inicial antes");
    return;
  }


  printf("%s\n", __func__);
}

void dos(struct sm_Inst *sm_this)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }

  unsigned int sm_next;

  if (!sm_next_state(&sm_tb, sm_this->state, 2, &sm_next))
    sm_this->state = sm_next;
  else
  {
    fprintf(stderr, "Aviso: acción no procedente en estado actual");    return;
  }

  if (sm_this->pdata == NULL)
  {
    fprintf(stderr, "Aviso: realize la acćión inicial antes");
    return;
  }


  printf("%s\n", __func__);
}

void init_sm1(struct sm_Inst *sm_this)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }

  unsigned int sm_next;

  if (!sm_next_state(&sm_tb, sm_this->state, 0, &sm_next))
    sm_this->state = sm_next;
  else
  {
    fprintf(stderr, "Aviso: acción no procedente en estado actual");    return;
  }

  if (sm_this->pdata != NULL)
  {
    fprintf(stderr, "Aviso: acción inicial ya realizada"); return;
  }
  else if ((sm_this->pdata = malloc(sizeof(struct sm1))) == NULL)
  {
    perror("Aviso: No se pudo realizar acción inicial"); return;
  }

  if (sm_this->pdata == NULL)
  {
    fprintf(stderr, "Aviso: realize la acćión inicial antes");
    return;
  }


  printf("%s\n", __func__);
}

void uno(struct sm_Inst *sm_this)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }

  unsigned int sm_next;

  if (!sm_next_state(&sm_tb, sm_this->state, 1, &sm_next))
    sm_this->state = sm_next;
  else
  {
    fprintf(stderr, "Aviso: acción no procedente en estado actual");    return;
  }

  if (sm_this->pdata == NULL)
  {
    fprintf(stderr, "Aviso: realize la acćión inicial antes");
    return;
  }


  printf("%s\n", __func__);
}

static void sm_done
(
  struct sm_Inst *sm_this,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }


  close(sm_this);
  sm_destroy(sm_nInst, sm_vpInsts, SM_ID_sm1, sm_this);

  close(sm_this);
}

static void sm_dos
(
  struct sm_Inst *sm_this,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }


  printf("Especificar dos números\n");
  scanf("%d", &((struct sm1*) (sm_this)->pdata)->maxi0);
  scanf("%d", &((struct sm1*) (sm_this)->pdata)->maxi1);

  done(sm_this);
}

static void sm_uno
(
  struct sm_Inst *sm_this,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }


  printf("Especificar un número\n");
  scanf("%d", &((struct sm1*) (sm_this)->pdata)->maxi0);
  ((struct sm1*) (sm_this)->pdata)->maxi1 = ((struct sm1*) (sm_this)->pdata)->maxi0;

  done(sm_this);
}

static void sm_unoOdos
(
  struct sm_Inst *sm_this,
  size_t sm_nInst,
  struct sm_Insts *sm_vpInsts[static sm_nInst]
)
{
  if (sm_this == NULL)
  {
    fprintf(stderr, "Aviso: instancia sin crear"); return;
  }


  int d;
  printf("Especificar uno o dos números?\n");

  scanf("%d", &d);

  if(d)
    dos(sm_this);
  else
    uno(sm_this);

}

const sm_State sm_vStates_sm0[] = {
  sm_done,
  sm_dos,
  sm_uno,
  sm_unoOdos,
};

