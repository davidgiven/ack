/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W  */

/* Macro's for extended data structures: */

#define B_SEMI		b_extend->bx_cf.bx_semi
#define B_PARENT	b_extend->bx_cf.bx_parent
#define B_BUCKET	b_extend->bx_cf.bx_bucket
#define B_ANCESTOR	b_extend->bx_cf.bx_ancestor
#define B_LABEL		b_extend->bx_cf.bx_label

#define LP_BLOCKS	lp_extend->lpx_cf.lpx_blocks
#define LP_COUNT	lp_extend->lpx_cf.lpx_count
#define LP_MESSY	lp_extend->lpx_cf.lpx_messy

#define newcflpx()	(lpext_p) newstruct(lpext_cf)
#define oldcflpx(x)	oldstruct(lpext_cf,x)
