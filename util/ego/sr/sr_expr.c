/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ E X P R . C
 *
 */

#include <stdio.h>
#include <em_mnem.h>
#include "ego/share/types.h"
#include "sr.h"
#include "ego/share/debug.h"
#include "ego/share/global.h"
#include "ego/share/utils.h"
#include "sr_aux.h"
#include "ego/share/lset.h"
#include "sr_iv.h"

#define ME_NONE 0
#define ME_UNAIR 1
#define ME_BINAIR 2
#define ME_LOOPCONST 3
#define ME_IV 4

static iv_p last_iv;
static int iv_sign;
static lset ivars, loopvars;

static bool is_loadiv(line_p lnp)
{
	/* See if lnp is a LOL iv instruction, where iv is an
	 * induction variable of the set ivars. If so, set the
	 * the global variable last_iv to its descriptor.
	 */

	Lindex i;
	iv_p iv;
	offset off;

	if (INSTR(lnp) == op_lol)
	{
		off = off_set(lnp);
		for (i = Lfirst(ivars); i != (Lindex)0; i = Lnext(i, ivars))
		{
			iv = (iv_p)Lelem(i);
			if (iv->iv_off == off)
			{
				last_iv = iv;
				return true;
			}
		}
	}
	return false;
}

#define size_ok(l) (TYPE(l) == OPSHORT && SHORT(l) == ws)

static int me_kind(line_p l, int sign_in, int* sign_out)
{
	if (l != (line_p)0)
	{
		switch (INSTR(l))
		{
			case op_adi:
			case op_adu:
				if (size_ok(l))
				{
					*sign_out = sign_in;
					return ME_BINAIR;
				}
				break;
			case op_sbi:
			case op_sbu:
				if (size_ok(l))
				{
					*sign_out = -sign_in;
					return ME_BINAIR;
				}
				break;
			case op_ngi:
				if (size_ok(l))
				{
					*sign_out = -sign_in;
					return ME_UNAIR;
				}
				break;
			case op_inc:
			case op_dec:
				*sign_out = sign_in;
				return ME_UNAIR;
			case op_loc:
				return ME_LOOPCONST;
			case op_lol:
				if (is_loadiv(l))
				{
					iv_sign = sign_in;
					return ME_IV;
				}
				if (is_loopconst(l, loopvars))
					return ME_LOOPCONST;
		}
	}
	return ME_NONE;
}

static bool match_expr(line_p l, bool iv_allowed, line_p* lbegin, bool* iv_seen, int sign)
{
	/* This routine is a top down parser for simple
	 * EM expressions. It recognizes expressions that
	 * have as operators + and - (unary - is also allowed)
	 * and that have as operands a number of loop constants
	 * (either a constant or a variable that is not
	 * changed within the loop) and at most one induction
	 * variable.
	 * The parameter iv_allowed is propagated downwards
	 * in the expression tree, indicating whether the
	 * subexpression may use an induction variable as
	 * operand. The parameter iv_seen is propagated
	 * upwards, indicating if the subexpression has used
	 * an induction variable. The parameter sign is
	 * propagated downwards; it indicates the sign of
	 * the subexpression. lbegin will point to the
	 * beginning of the recognized subexpression
	 * (it is an out parameter). Note that we scan the
	 * EM text from right to left (i.e. top down).
	 */

	line_p l1;
	bool iv_insubexpr;
	int sign2;

	switch (me_kind(l, sign, &sign2))
	{
		case ME_UNAIR:
			/* unairy operator, match one subexpression */
			if (match_expr(PREV(l), iv_allowed, &l1, &iv_insubexpr, sign2))
			{
				*lbegin = l1;
				*iv_seen = iv_insubexpr;
				return true;
			}
			return false;
		case ME_BINAIR:
			/* binairy operator, match two subexpressions */
			if (match_expr(PREV(l), iv_allowed, &l1, &iv_insubexpr, sign2))
			{
				l = PREV(l1);
				iv_allowed = iv_allowed && !iv_insubexpr;
				if (match_expr(l, iv_allowed, &l1, &iv_insubexpr, sign))
				{
					*lbegin = l1;
					*iv_seen = !iv_allowed || iv_insubexpr;
					return true;
				}
			}
			return false; /* subexpression not recognized */
		case ME_LOOPCONST:
			*lbegin = l; /* expression is a loop constant */
			*iv_seen = false;
			return true;
		case ME_IV:
			if (iv_allowed)
			{
				*iv_seen = true;
				*lbegin = l;
				return true;
			}
			/* fall through ... */
		default:
			return false;
	}
}

bool is_ivexpr(line_p l, lset ivs, lset vars, line_p* lbegin_out, iv_p* iv_out, int* sign_out)
{
	line_p l2;
	bool iv_seen;

	loopvars = vars;
	ivars = ivs;
	if (match_expr(l, true, &l2, &iv_seen, 1))
	{
		if (iv_seen)
		{
			/* recognized a correct expression */
			*lbegin_out = l2;
			*iv_out = last_iv;
			*sign_out = iv_sign;
			return true;
		}
	}
	return false;
}
