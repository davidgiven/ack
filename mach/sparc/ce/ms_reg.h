/* ms_reg.h */

#ifndef MS_REG_H
#define MS_REG_H

#include "push_pop.h"

#define init_reg_man B_init_reg_man
#define free_all_reg_vars B_free_all_reg_vars
#define alloc_all_reg_vars B_alloc_all_reg_vars
#define load_float_regs B_load_float_regs
#define find_local B_find_local

_PROTOTYPE(reg_t find_local, (int, reg_t*));

#endif
