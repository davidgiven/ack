/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	D E C L A R A T O R   M A N I P U L A T I O N		*/

#include	"botch_free.h"
#include	<alloc.h>
#include	"arith.h"
#include	"type.h"
#include	"Lpars.h"
#include	"declar.h"
#include	"idf.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"

struct declarator null_declarator;

struct type *
declare_type(tp, dc)
	struct type *tp;
	struct declarator *dc;
{
	/*	Applies the decl_unary list starting at dc->dc_decl_unary
		to the type tp and returns the result.
	*/
	register struct decl_unary *du = dc->dc_decl_unary;

	while (du)	{
		tp = construct_type(du->du_fund, tp, du->du_count);
		du = du->next;
	}
	return tp;
}

add_decl_unary(dc, fund, count, fm)
	register struct declarator *dc;
	arith count;
	struct formal *fm;
{
	/*	A decl_unary describing a constructor with fundamental
		type fund and with size count is inserted in front of the
		declarator dc.
	*/
	register struct decl_unary *new = new_decl_unary();

	new->next = dc->dc_decl_unary;
	new->du_fund = fund;
	new->du_count = count;
	if (fm)	{
		if (dc->dc_decl_unary)	{
			/* paramlist only allowed at first decl_unary	*/
			error("formal parameter list discarded");
		}
		else	{
			/* register the parameters	*/
			dc->dc_formal = fm;
		}
	}
	dc->dc_decl_unary = new;
}

remove_declarator(dc)
	struct declarator *dc;
{
	/*	The decl_unary list starting at dc->dc_decl_unary is
		removed.
	*/
	register struct decl_unary *du = dc->dc_decl_unary;

	while (du)	{
		struct decl_unary *old_du = du;

		du = du->next;
		free_decl_unary(old_du);
	}
}

reject_params(dc)
	register struct declarator *dc;
{
	/*	The declarator is checked to have no parameters, if it
		is a function.
	*/
	if (dc->dc_formal)	{
		error("non_empty formal parameter pack");
		free_formals(dc->dc_formal);
		dc->dc_formal = 0;
	}
}

check_array_subscript(expr)
	register struct expr *expr;
{
	arith size = expr->VL_VALUE;

	if (size < 0)	{
		error("array size is negative");
		expr->VL_VALUE = (arith)1;
	}
	else
	if (size == 0) {
		warning("array size is 0");
	}
	else
	if (size & ~max_unsigned) {	/* absolutely ridiculous */
		expr_error(expr, "overflow in array size");
		expr->VL_VALUE = (arith)1;
	}
}
