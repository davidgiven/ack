/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	A R I T H M E T I C   C O N V E R S I O N S	 */

/*	This file contains the routines for the various conversions that
	may befall operands in C. It is structurally a mess, but I haven't
	decided yet whether I can't find the right structure or the
	semantics of C is a mess.
*/

#include	<alloc.h>
#include	"lint.h"
#include	"nofloat.h"
#include	"nobitfield.h"
#include	"idf.h"
#include	"arith.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"Lpars.h"
#include	"field.h"
#include	"mes.h"
#include	"noRoption.h"

extern char *symbol2str();
extern char options[];

arithbalance(e1p, oper, e2p)	/* RM 6.6 */
	register struct expr **e1p, **e2p;
	int oper;
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the arithmetic operator oper.
	*/
	register int t1, t2, u1, u2;

	t1 = any2arith(e1p, oper);
	t2 = any2arith(e2p, oper);

	/* Now t1 and t2 are either INT or LONG or DOUBLE */
#ifndef NOFLOAT
	if (t1 == DOUBLE) {
		if (t2 != DOUBLE)
			int2float(e2p, double_type);
		return;
	}
	if (t2 == DOUBLE) {
		if (t1 != DOUBLE)
			int2float(e1p, double_type);
		return;
	}
#endif /* NOFLOAT */

	/* Now they are INT or LONG */
	u1 = (*e1p)->ex_type->tp_unsigned;
	u2 = (*e2p)->ex_type->tp_unsigned;

	/* if either is long, the other will be	*/
	if (t1 == LONG && t2 != LONG)
		t2 = int2int(e2p, u2 ? ulong_type : long_type);
	else
	if (t2 == LONG && t1 != LONG)
		t1 = int2int(e1p, u1 ? ulong_type : long_type);

	/* if either is unsigned, the other will be	*/
	if (u1 && !u2)
		t2 = int2int(e2p, (t1 == LONG) ? ulong_type : uint_type);
	else
	if (!u1 && u2)
		t1 = int2int(e1p, (t2 == LONG) ? ulong_type : uint_type);
}

relbalance(e1p, oper, e2p)
	register struct expr **e1p, **e2p;
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the relational operator oper.
	*/
	if ((*e1p)->ex_type->tp_fund == FUNCTION)
		function2pointer(*e1p);
	if ((*e2p)->ex_type->tp_fund == FUNCTION)
		function2pointer(*e2p);
	if ((*e1p)->ex_type->tp_fund == POINTER)
		ch76pointer(e2p, oper, (*e1p)->ex_type);
	else
	if ((*e2p)->ex_type->tp_fund == POINTER)
		ch76pointer(e1p, oper, (*e2p)->ex_type);
	else
	if (	(*e1p)->ex_type == (*e2p)->ex_type &&
		(*e1p)->ex_type->tp_fund == ENUM
	)
		{}
	else
		arithbalance(e1p, oper, e2p);
}

ch76pointer(expp, oper, tp)
	struct expr **expp;
	register struct type *tp;
{
	/*	Checks whether *expp may be compared to tp using oper,
		as described in chapter 7.6 and 7.7.
		tp is known to be a pointer.
	*/
	register struct expr *exp = *expp;

	if (exp->ex_type->tp_fund == POINTER)	{
		if (exp->ex_type != tp)
			ch7cast(expp, oper, tp);
	}
	else
	if (is_integral_type(exp->ex_type)) {
		if (	(oper != EQUAL && oper != NOTEQUAL && oper != ':') ||
			(!is_cp_cst(exp) || exp->VL_VALUE != 0)
		) {	/* ch 7.6, ch 7.7 */
			expr_warning(exp, "%s on %s and pointer",
					  symbol2str(oper),
					  symbol2str(exp->ex_type->tp_fund)
				);
		}
		ch7cast(expp, CAST, tp);
	}
	else	{
		expr_error(exp, "%s on %s and pointer",
				symbol2str(oper),
				symbol2str(exp->ex_type->tp_fund)
			);
		ch7cast(expp, oper, tp);
	}
}

int
any2arith(expp, oper)
	register struct expr **expp;
	register int oper;
{
	/*	Turns any expression into int_type, long_type or
		double_type.
	*/
	int fund;

	switch (fund = (*expp)->ex_type->tp_fund)	{
	case CHAR:
	case SHORT:
		int2int(expp,
			(*expp)->ex_type->tp_unsigned ? uint_type : int_type);
		break;
	case INT:
	case LONG:
		break;
	case ENUM:
		/* test the admissibility of the operator */
		if (	is_test_op(oper) || oper == '=' || oper == PARCOMMA ||
			oper == ',' || oper == ':'
		)	{
			/* allowed by K & R */
		}
		else
#ifndef NOROPTION
		if (!options['R'])	{
			/* allowed by us */
		}
		else
			expr_warning(*expp, "%s on enum", symbol2str(oper));
#endif /* NOROPTION */
#ifndef	LINT
		int2int(expp, int_type);
#endif	/* LINT */
		break;
#ifndef	NOFLOAT
	case FLOAT:
		float2float(expp, double_type);
		break;
	case DOUBLE:
		break;
#endif	/* NOFLOAT */
#ifndef NOBITFIELD
	case FIELD:
		field2arith(expp);
		break;
#endif /* NOBITFIELD */
	default:
		expr_error(*expp, "operator %s on non-numerical operand (%s)",
			symbol2str(oper), symbol2str(fund));
	case ERRONEOUS:
		erroneous2int(expp);
		break;
	}

	return (*expp)->ex_type->tp_fund;
}

erroneous2int(expp)
	struct expr **expp;
{
	/*	the (erroneous) expression *expp is replaced by an
		int expression
	*/
	register struct expr *exp = *expp;
	int flags = exp->ex_flags;
	
	free_expression(exp);
	exp = intexpr((arith)0, INT);
	exp->ex_flags = (flags | EX_ERROR);
	*expp = exp;
}

struct expr *
arith2arith(tp, oper, expr)
	struct type *tp;
	int oper;
	register struct expr *expr;
{
	/*	arith2arith constructs a new expression containing a
		run-time conversion between some arithmetic types.
	*/
	register struct expr *new = new_expr();
	
	new->ex_file = expr->ex_file;
	new->ex_line = expr->ex_line;
	new->ex_type = tp;
	new->ex_class = Type;
	return new_oper(tp, new, oper, expr);
}

int
int2int(expp, tp)
	struct expr **expp;
	register struct type *tp;
{
	/*	The expression *expp, which is of some integral type, is
		converted to the integral type tp.
	*/
	register struct expr *exp = *expp;
	
	if (is_cp_cst(exp))	{
		register struct type *tp1 = exp->ex_type;

		exp->ex_type = tp;
		if (! tp1->tp_unsigned && tp->tp_unsigned) {
			/*	Avoid "unreal" overflow warnings, such as
				caused by f.i.:
					unsigned int x = ~0;
					unsigned int y = -1;
			*/
			extern long full_mask[];
			long remainder = exp->VL_VALUE &
						~full_mask[(int)(tp->tp_size)];

			if (remainder == 0 ||
			    remainder == ~full_mask[(int)(tp->tp_size)]) {
				exp->VL_VALUE &= ~remainder;
			}
		}
		cut_size(exp);
	}
	else	{
		exp = arith2arith(tp, INT2INT, exp);
	}
	*expp = exp;
	return exp->ex_type->tp_fund;
}

#ifndef NOFLOAT
int2float(expp, tp)
	register struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some integral type, is
		converted to the floating type tp.
	*/
	register struct expr *exp = *expp;
	char buf[32];
	
	fp_used = 1;
	if (is_cp_cst(exp)) {
		*expp = new_expr();
		**expp = *exp;
		sprint(buf+1, "%ld", (long)(exp->VL_VALUE));
		buf[0] = '-';
		exp = *expp;
		exp->ex_type = tp;
		exp->ex_class = Float;
		exp->FL_VALUE = Salloc(buf, (unsigned)strlen(buf)+2) + 1;
		exp->FL_DATLAB = 0;
	}
	else	*expp = arith2arith(tp, INT2FLOAT, *expp);
}

float2int(expp, tp)
	struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some floating type, is
		converted to the integral type tp.
	*/
	
	fp_used = 1;
	*expp = arith2arith(tp, FLOAT2INT, *expp);
}

float2float(expp, tp)
	register struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some floating type, is
		converted to the floating type tp.
		There is no need for an explicit conversion operator
		if the expression is a constant.
	*/
	
	fp_used = 1;
	if (is_fp_cst(*expp))
		(*expp)->ex_type = tp;
	else
		*expp = arith2arith(tp, FLOAT2FLOAT, *expp);
}
#endif /* NOFLOAT */

array2pointer(exp)
	register struct expr *exp;
{
	/*	The expression, which must be an array, is converted
		to a pointer.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type->tp_up, (arith)0);
}

function2pointer(exp)
	register struct expr *exp;
{
	/*	The expression, which must be a function, is converted
		to a pointer to the function.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type, (arith)0);
}

string2pointer(ex)
	register struct expr *ex;
{
	/*	The expression, which must be a string constant, is converted
		to a pointer to the string-containing area.
	*/
	label lbl = data_label();

	code_string(ex->SG_VALUE, ex->SG_LEN, lbl);
	ex->ex_class = Value;
	ex->VL_CLASS = Label;
	ex->VL_LBL = lbl;
	ex->VL_VALUE = (arith)0;
}

opnd2integral(expp, oper)
	register struct expr **expp;
	int oper;
{
	register int fund = (*expp)->ex_type->tp_fund;

	if (fund != INT && fund != LONG)	{
		expr_error(*expp, "%s operand to %s",
				symbol2str(fund), symbol2str(oper));
		erroneous2int(expp);
		/* fund = INT; */
	}
}

opnd2logical(expp, oper)
	register struct expr **expp;
	int oper;
{
	int fund = (*expp)->ex_type->tp_fund;

	if (fund == FUNCTION || fund == ARRAY) {
		expr_warning(*expp, "%s operand to %s",
			symbol2str(fund),
			symbol2str(oper));
		if (fund == FUNCTION) {
			function2pointer(*expp);
		}
		else	array2pointer(*expp);
	}
#ifndef NOBITFIELD
	else
	if (fund == FIELD)
		field2arith(expp);
#endif /* NOBITFIELD */
	switch (fund = (*expp)->ex_type->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
	case POINTER:
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
#endif /* NOFLOAT */
		break;
	default:
		expr_error(*expp, "%s operand to %s",
			symbol2str(fund), symbol2str(oper));
	case ERRONEOUS:
		erroneous2int(expp);
		break;
	}
}

opnd2test(expp, oper)
	register struct expr **expp;
{
	opnd2logical(expp, oper);
	if ((*expp)->ex_class == Oper) {
		if (is_test_op((*expp)->OP_OPER)) {
			/* It is already a test */
			return;
		}
		if ((*expp)->OP_OPER == ',') {
			opnd2test(&((*expp)->OP_RIGHT), oper);
			return;
		}
	}
	ch7bin(expp, NOTEQUAL, intexpr((arith)0, INT));
}

int
is_test_op(oper)
{
	switch (oper)	{
	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
	case EQUAL:
	case NOTEQUAL:
	case '!':
	case AND:
	case OR:	/* && and || also impose a test	*/
		return 1;
	default:
		return 0;
	}
	/*NOTREACHED*/
}

any2opnd(expp, oper)
	register struct expr **expp;
{
	if (!*expp)
		return;
	switch ((*expp)->ex_type->tp_fund)	{	/* RM 7.1 */
	case CHAR:
	case SHORT:
	case ENUM:
#ifndef NOFLOAT
	case FLOAT:
#endif /* NOFLOAT */
		any2arith(expp, oper);
		break;
	case ARRAY:
		array2pointer(*expp);
		break;
	case POINTER:
		if ((*expp)->ex_class == String)
			string2pointer(*expp);
		break;
#ifndef NOBITFIELD
	case FIELD:
		field2arith(expp);
		break;
#endif /* NOBITFIELD */
	}
}

#ifndef NOBITFIELD
field2arith(expp)
	register struct expr **expp;
{
	/*	The expression to extract the bitfield value from the
		memory word is put in the tree.
	*/
	register struct type *tp = (*expp)->ex_type->tp_up;
	register struct field *fd = (*expp)->ex_type->tp_field;
	register struct type *atype = tp->tp_unsigned ? uword_type : word_type;

	(*expp)->ex_type = atype;

	if (atype->tp_unsigned)	{	/* don't worry about the sign bit */
		ch7bin(expp, RIGHT, intexpr((arith)fd->fd_shift, INT));
		ch7bin(expp, '&', intexpr(fd->fd_mask, INT));
	}
	else	{	/* take care of the sign bit: sign extend if needed */
		arith bits_in_type = atype->tp_size * 8;

		ch7bin(expp, LEFT,
			intexpr(bits_in_type - fd->fd_width - fd->fd_shift,
						INT)
		);
		ch7bin(expp, RIGHT, intexpr(bits_in_type - fd->fd_width, INT));
	}
	ch7cast(expp, CAST, tp);	/* restore its original type */
}
#endif /* NOBITFIELD */

#ifndef NOFLOAT
/*	switch_sign_fp() negates the given floating constant expression
	The lexical analyser has reserved an extra byte of space in front
	of the string containing the representation of the floating
	constant.  This byte contains the '-' character and we have to
	take care of the first byte the fl_value pointer points to.
*/
switch_sign_fp(expr)
	register struct expr *expr;
{
	if (*(expr->FL_VALUE) == '-')
		++(expr->FL_VALUE);
	else
		--(expr->FL_VALUE);
}
#endif /* NOFLOAT */
