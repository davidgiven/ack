/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* SEMANTIC ANALYSIS (CHAPTER 7RM) -- MONADIC OPERATORS */

#include	"botch_free.h"
#include	<alloc.h>
#include	"nofloat.h"
#include	"nobitfield.h"
#include	"Lpars.h"
#include	"arith.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"idf.h"
#include	"def.h"

extern char options[];
extern long full_mask[/*MAXSIZE*/];	/* cstoper.c */
char *symbol2str();

ch7mon(oper, expp)
	register struct expr **expp;
{
	/*	The monadic prefix operator oper is applied to *expp.
	*/
	register struct expr *expr;

	switch (oper)	{
	case '*':			/* RM 7.2 */
		/* no FIELD type allowed	*/
		if ((*expp)->ex_type->tp_fund == ARRAY)
			array2pointer(*expp);
		if ((*expp)->ex_type->tp_fund != POINTER)	{
			expr_error(*expp,
				"* applied to non-pointer (%s)",
				symbol2str((*expp)->ex_type->tp_fund));
		}
		else {
			expr = *expp;
			if (expr->ex_lvalue == 0 && expr->ex_class != String)
				/* dereference in administration only */
				expr->ex_type = expr->ex_type->tp_up;
			else	/* runtime code */
				*expp = new_oper(expr->ex_type->tp_up, NILEXPR,
							'*', expr);
			(*expp)->ex_lvalue = (
				(*expp)->ex_type->tp_fund != ARRAY &&
				(*expp)->ex_type->tp_fund != FUNCTION);
		}
		break;
	case '&':
		if ((*expp)->ex_type->tp_fund == ARRAY) {
			expr_warning(*expp, "& before array ignored");
			array2pointer(*expp);
		}
		else
		if ((*expp)->ex_type->tp_fund == FUNCTION) {
			expr_warning(*expp, "& before function ignored");
			function2pointer(*expp);
		}
		else
#ifndef NOBITFIELD
		if ((*expp)->ex_type->tp_fund == FIELD)
			expr_error(*expp, "& applied to field variable");
		else
#endif /* NOBITFIELD */
		if (!(*expp)->ex_lvalue)
			expr_error(*expp, "& applied to non-lvalue");
		else {
			/* assume that enums are already filtered out	*/
			if (ISNAME(*expp)) {
				register struct def *def =
					(*expp)->VL_IDF->id_def;

				/*	&<var> indicates that <var>
					cannot be used as register
					anymore
				*/
				if (def->df_sc == REGISTER) {
					expr_error(*expp,
					"& on register variable not allowed");
					break;	/* break case '&' */
				}
			}
			(*expp)->ex_type = pointer_to((*expp)->ex_type);
			(*expp)->ex_lvalue = 0;
		}
		break;
	case '~':
#ifndef NOFLOAT
	{
		int fund = (*expp)->ex_type->tp_fund;

		if (fund == FLOAT || fund == DOUBLE)	{
			expr_error(
				*expp,
				"~ not allowed on %s operands",
				symbol2str(fund)
			);
			erroneous2int(expp);
			break;
		}
		/* FALLTHROUGH */
	}
#endif /* NOFLOAT */
	case '-':
		any2arith(expp, oper);
		if (is_cp_cst(*expp))	{
			arith o1 = (*expp)->VL_VALUE;

			o1 = (oper == '-') ? -o1 : ~o1;
			(*expp)->VL_VALUE =
			  ((*expp)->ex_type->tp_unsigned ?
				o1 & full_mask[(*expp)->ex_type->tp_size] :
				o1
			  );
		}
		else
#ifndef NOFLOAT
		if (is_fp_cst(*expp))
			switch_sign_fp(*expp);
		else
#endif /* NOFLOAT */
			*expp = new_oper((*expp)->ex_type,
					NILEXPR, oper, *expp);
		break;
	case '!':
		if ((*expp)->ex_type->tp_fund == FUNCTION)
			function2pointer(*expp);
		if ((*expp)->ex_type->tp_fund != POINTER)
			any2arith(expp, oper);
		opnd2test(expp, '!');
		if (is_cp_cst(*expp))	{
			(*expp)->VL_VALUE = !((*expp)->VL_VALUE);
			(*expp)->ex_type = int_type;	/* a cast ???(EB) */
		}
		else
			*expp = new_oper(int_type, NILEXPR, oper, *expp);
		(*expp)->ex_flags |= EX_LOGICAL;
		break;
	case PLUSPLUS:
	case MINMIN:
		ch7incr(expp, oper);
		break;
	case SIZEOF:
		if (ISNAME(*expp) && (*expp)->VL_IDF->id_def->df_formal_array)
			expr_warning(*expp, "sizeof formal array %s is sizeof pointer!",
				(*expp)->VL_IDF->id_text);
		expr = intexpr((*expp)->ex_class == String ?
				   (arith)((*expp)->SG_LEN) :
				   size_of_type((*expp)->ex_type, "object"),
				INT);
		expr->ex_flags |= EX_SIZEOF;
		free_expression(*expp);
		*expp = expr;
		break;
	}
}
