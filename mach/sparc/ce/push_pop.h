/*
push_pop.h
*/

#ifndef PUSH_POP_H
#define PUSH_POP_H

#include "back.h"

#define NR_REGS 64

typedef char *reg_t;

typedef struct cache_elt {
	reg_t	reg, reg2;
	char	*ext;
	arith	cst;
} cache_elt;

#define T_cst	1
#define T_ext	2
#define T_reg	4
#define T_float	8
#define T_reg2	16
#define T_float2 32

typedef char const_str_t[NR_REGS];

extern reg_t reg_g0, reg_g1, reg_g2, reg_g3, reg_g4, reg_g5, reg_g6, reg_g7;
extern reg_t reg_i0, reg_i1, reg_i2, reg_i3, reg_i4, reg_i5, reg_i6, reg_i7;
extern reg_t reg_l0, reg_l1, reg_l2, reg_l3, reg_l4, reg_l5, reg_l6, reg_l7;
extern reg_t reg_o0, reg_o1, reg_o2, reg_o3, reg_o4, reg_o5, reg_o6, reg_o7;
extern reg_t reg_f0;
extern reg_t reg_sp, reg_lb, reg_gap;
extern reg_t reg_tmp;

#ifdef __STDC__
#define _PROTOTYPE(x,y) x y
#else
#define _PROTOTYPE(x,y) x()
#endif

_PROTOTYPE(int const13, (int));
_PROTOTYPE(void init_cache, (void));
_PROTOTYPE(void free_reg, (reg_t));
_PROTOTYPE(void free_double, (reg_t));	/* ..._reg */
_PROTOTYPE(void forced_alloc_reg, (reg_t));
_PROTOTYPE(void soft_alloc_reg, (reg_t));
_PROTOTYPE(void change_reg, (reg_t));
_PROTOTYPE(int type_of_tos, (void));
_PROTOTYPE(void inc_tos, (arith));
_PROTOTYPE(void inc_tos_reg, (reg_t));
_PROTOTYPE(void push_const, (arith));
_PROTOTYPE(void push_reg, (reg_t));
_PROTOTYPE(void push_ext, (char *));
_PROTOTYPE(void flush_cache, (void));
static _PROTOTYPE(void flush_part_cache, (int c, int r, int f, int d));
static _PROTOTYPE(void subst_reg, (reg_t, reg_t));
_PROTOTYPE(void cache_need, (int));
static _PROTOTYPE(int cache_read, (int n, int i));
static _PROTOTYPE(void dump_cache, (File *stream));
_PROTOTYPE(void pop_nop, (int));
static _PROTOTYPE(void panic, (char*));

_PROTOTYPE(reg_t alloc_reg, (void));
_PROTOTYPE(reg_t alloc_reg_var, (void));
_PROTOTYPE(reg_t alloc_float, (void));
_PROTOTYPE(reg_t alloc_float_var, (void));
_PROTOTYPE(reg_t alloc_double, (reg_t *sub_reg));
_PROTOTYPE(reg_t alloc_double_var, (reg_t *sub_reg));
_PROTOTYPE(reg_t pop_reg, (void));
_PROTOTYPE(reg_t pop_reg_c13, (char*));
_PROTOTYPE(reg_t pop_reg_reg, (reg_t*));
_PROTOTYPE(reg_t pop_float, (void));
_PROTOTYPE(reg_t pop_double, (reg_t *sub_reg));
_PROTOTYPE(void pop_reg_as, (reg_t r));
static _PROTOTYPE(reg_t top_reg, (void));
static _PROTOTYPE(reg_t top_reg_c13, (char*));

_PROTOTYPE(arith pop_const, (char *n_str));
_PROTOTYPE(arith top_const, (void));
_PROTOTYPE(void dup_tos, (int n));
_PROTOTYPE(void exg_top, (int n));

#define code_combiner(x) x

#endif /* PUSH_POP_H */
