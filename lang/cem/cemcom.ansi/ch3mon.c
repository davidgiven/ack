/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* SEMANTIC ANALYSIS (CHAPTER 3.3) -- MONADIC OPERATORS */

#include	"botch_free.h"
#include	"debug.h"
#include	<alloc.h>
#include	"nobitfield.h"
#include	"Lpars.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"idf.h"
#include	"def.h"
#include	"sizes.h"

extern char options[];
extern arith full_mask[/*MAXSIZE*/];	/* cstoper.c */
char *symbol2str();

ch3mon(oper, expp)
	register struct expr **expp;
{
	/*	The monadic prefix operator oper is applied to *expp.
	*/
	register struct expr *expr;

	if (oper != PLUSPLUS && oper != MINMIN)
		any2opnd(expp, oper);

	switch (oper)	{
	case '*':			/* 3.3.3.2 */
		/* no FIELD type allowed	*/
		expr = *expp;
		if (expr->ex_type->tp_fund != POINTER) {
			if (expr->ex_type != error_type) {
			    expr_error(expr,
				    "* applied to non-pointer (%s)",
				    symbol2str(expr->ex_type->tp_fund));
			}
		} else {
			if (is_ld_cst(expr))
				/* dereference in administration only */
				expr->ex_type = expr->ex_type->tp_up;
			else	/* runtime code */
				*expp = new_oper(expr->ex_type->tp_up, NILEXPR,
							'*', expr);
			expr = *expp;
			expr->ex_lvalue = (
				expr->ex_type->tp_fund != ARRAY &&
				expr->ex_type->tp_fund != FUNCTION
				);
			if (expr->ex_lvalue && expr->ex_type->tp_size <= 0) {
				expr_error(expr, "incomplete type in expression");
			}
			if (expr->ex_type->tp_typequal & TQ_CONST)
				expr->ex_flags |= EX_READONLY;
			if (expr->ex_type->tp_typequal & TQ_VOLATILE)
				expr->ex_flags |= EX_VOLATILE;
			expr->ex_flags &= ~EX_ILVALUE;
		}
		break;
	case ADDRESSOF:
		if ((*expp)->ex_type->tp_fund == ARRAY) {
			(*expp)->ex_type = pointer_to((*expp)->ex_type, 0);
		}
		else
		if ((*expp)->ex_type->tp_fund == FUNCTION) {
			(*expp)->ex_type = pointer_to((*expp)->ex_type, 0);
		}
		else
#ifndef NOBITFIELD
		if ((*expp)->ex_type->tp_fund == FIELD)
			expr_error(*expp, "& applied to field variable");
		else
#endif /* NOBITFIELD */
		if (!(*expp)->ex_lvalue)
			expr_error(*expp, "& applied to non-lvalue");
		else if ((*expp)->ex_flags & EX_ILVALUE)
			expr_error(*expp, "& applied to illegal lvalue");
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
					break;	/* break case ADDRESSOF */
				}
			}
			(*expp)->ex_type = pointer_to((*expp)->ex_type,
						(*expp)->ex_type->tp_typequal);
			(*expp)->ex_lvalue = 0;
			(*expp)->ex_flags &= ~(EX_READONLY | EX_VOLATILE);
		}
		break;
	case '~':
	{
		int fund = (*expp)->ex_type->tp_fund;

		if (fund == FLOAT || fund == DOUBLE || fund == LNGDBL)	{
			expr_error( *expp,
				    "~ not allowed on %s operands",
				    symbol2str(fund));
			erroneous2int(expp);
			break;
		}
		/* FALLTHROUGH */
	}
	case '-':
		any2arith(expp, oper);
		if (is_cp_cst(*expp))	{
			arith o1 = (*expp)->VL_VALUE;

			(*expp)->VL_VALUE = (oper == '-') ? -o1 :
			  ((*expp)->ex_type->tp_unsigned ?
				(~o1) & full_mask[(int)(*expp)->ex_type->tp_size] :
				~o1
			  );
		}
		else
		if (is_fp_cst(*expp))
			switch_sign_fp(*expp);
		else
			*expp = new_oper((*expp)->ex_type,
					NILEXPR, oper, *expp);
		break;
	case '!':
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
		ch3incr(expp, oper);
		break;
	case SIZEOF:
		if (ISNAME(*expp) && (*expp)->VL_IDF->id_def->df_formal_array)
			expr_warning(*expp, "sizeof formal array %s is sizeof pointer!",
				(*expp)->VL_IDF->id_text);
		expr = intexpr((*expp)->ex_class == String ?
				   (arith)((*expp)->SG_LEN) :
				   size_of_type((*expp)->ex_type,
				       symbol2str((*expp)->ex_type->tp_fund))
			       , UNSIGNED);
		expr->ex_flags |= EX_SIZEOF;
		free_expression(*expp);
		*expp = expr;
		break;
	}
}
