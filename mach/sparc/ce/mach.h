/*
mach.h
*/

#ifndef MACH_H
#define MACH_H

#define alloc_double B_alloc_double
#define alloc_double_var B_allc_double_var
#define alloc_float B_alloc_float
#define alloc_float_var B_allc_float_var
#define alloc_reg B_alloc_reg
#define alloc_reg_var B_allc_reg_var
#define free_reg B_free_reg
#define free_output B_free_output
#define free_double_reg B_free_double_reg
#define forced_alloc_reg B_forced_alloc_reg
#define force_alloc_output B_forcallocoutput
#define flush_cache B_flush_cache
#define dup_tos B_dup_tos
#define con_float B_con_float
#define check_cache B_check_cache
#define change_reg B_change_reg
#define arg_error B_arg_error
#define const13 B_const13
#define type_of_tos B_typeoftos
#define top_const B_top_const
#define soft_alloc_reg B_soft_alloc_reg
#define push_reg B_pushreg
#define push_ext B_pushext
#define push_double_reg B_pushdoublereg
#define push_const B_pushconst
#define pop_reg_reg B_pop_reg_reg
#define pop_reg_c13 B_popregc13
#define pop_reg_as B_popas
#define pop_reg B_popr
#define pop_nop B_popnop
#define pop_float B_popfloat
#define pop_double_reg_as B_popdouble_reg_as
#define pop_double B_pop_double
#define pop_const B_pop_const
#define init_cache B_init_cache
#define inc_tos_reg B_inc_tos_reg
#define inc_tos B_inctos

#define reg_g0 B_regg0
#define reg_g1 B_regg1
#define reg_g2 B_regg2
#define reg_g3 B_regg3
#define reg_g4 B_regg4
#define reg_g5 B_regg5
#define reg_g6 B_regg6
#define reg_g7 B_regg7

#define reg_i0 B_regi0
#define reg_i1 B_regi1
#define reg_i2 B_regi2
#define reg_i3 B_regi3
#define reg_i4 B_regi4
#define reg_i5 B_regi5
#define reg_i6 B_regi6
#define reg_i7 B_regi7

#define reg_o0 B_rego0
#define reg_o1 B_rego1
#define reg_o2 B_rego2
#define reg_o3 B_rego3
#define reg_o4 B_rego4
#define reg_o5 B_rego5
#define reg_o6 B_rego6
#define reg_o7 B_rego7

#define reg_l0 B_regl0
#define reg_l1 B_regl1
#define reg_l2 B_regl2
#define reg_l3 B_regl3
#define reg_l4 B_regl4
#define reg_l5 B_regl5
#define reg_l6 B_regl6
#define reg_l7 B_regl7

#define reg_f0 B_regf0
#define reg_gap B_reggap
#define reg_lb B_reglb
#define reg_sp B_regsp
#define reg_tmp B_regtmp
#define reg_lin B_reglin
#define reg_fil B_regfil

#include "ctype.h"
#include "mach_em.h"
#include "push_pop.h"
#include "ms_reg.h"
#include "misc.h"

#if RESOLV_debug
#define debug 0
#else
extern int debug;
#endif

#endif /* MACH_H */
