/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  L I V E   V A R I A B L E S   A N A L Y S I S
 *
 *  L V . H
 */


#define USE(b)		(b)->b_extend->bx_lv.bx_use
#define DEF(b)		(b)->b_extend->bx_lv.bx_def
#define L_IN(b)		(b)->b_extend->bx_lv.bx_lin
#define L_OUT(b)	(b)->b_extend->bx_lv.bx_lout

extern short nrglobals;  /* number of global variables for which
			  * ud-info is maintained.
			  */
extern short nrvars;	 /* total number of variables (global + local)
			  * for which ud-info is maintained.
			  */

/* Every global variable for which ud-info is maintained has
 * a 'global variable number' (o_globnr). Every useful local
 * has a 'local variable number', which is its index in the
 * 'locals' table. All these variables also have a
 * 'variable number'. Conversions exist between these numbers.
 */

#define TO_GLOBAL(v)	(v)
#define TO_LOCAL(v)	(v - nrglobals)
#define GLOB_TO_VARNR(v) (v)
#define LOC_TO_VARNR(v) (v + nrglobals)
#define IS_GLOBAL(v)	(v <= nrglobals)
#define IS_LOCAL(v)	(v > nrglobals)

#define REGVAR(lc)	lc->lc_flags |= LCF_REG
#define IS_REGVAR(lc)	(lc->lc_flags & LCF_REG)
#define BADLC(lc)	lc->lc_flags |= LCF_BAD
#define IS_BADLC(lc)	(lc->lc_flags & LCF_BAD)
#define LIVE(lc)	lc->lc_flags |= LCF_LIVE
#define DEAD(lc)	lc->lc_flags &= ~LCF_LIVE
#define IS_LIVE(lc)	(lc->lc_flags & LCF_LIVE)
#define IS_DEAD(lc)	(!(lc->lc_flags & LCF_LIVE))


