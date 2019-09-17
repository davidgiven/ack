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

#include	<assert.h>
#include	<stddef.h>
#include    "parameters.h"
#include	<alloc.h>
#include	<flt_arith.h>
#include	"arith.h"
#include	"sizes.h"
#include	"type.h"
#include	"proto.h"
#include	"label.h"
#include	"expr.h"
#include	"Lpars.h"
#include	"field.h"
#include	"mes.h"
#include    "cstoper.h"
#include    "ch3bin.h"
#include    "ch3.h"
#include    "error.h"


extern char *symbol2str();
extern char options[];
extern arith flt_flt2arith();
extern label code_string();

/* 3.1.2.5 */
void arithbalance(register struct expr **e1p, int oper, register struct expr **e2p)
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the arithmetic operator oper.
		We check here if the EX_PTRDIFF flag should be retained.
		They are set to zero in because one of the opreands might
		have a floating type, in which case the flags shouldn't
		travel upward in the expression tree.
	*/
	struct type *convert1, *convert2;
	int t1, t2, u1, u2;
	int shifting = (oper == LEFT || oper == RIGHT
			|| oper == LEFTAB || oper == RIGHTAB);
	int ptrdiff = 0;

	t1 = any2arith(e1p, oper);
	t2 = any2arith(e2p, oper);

	if (int_size != pointer_size) {
		if (ptrdiff = ((*e1p)->ex_flags & EX_PTRDIFF)
			    || ((*e2p)->ex_flags & EX_PTRDIFF)) {
			if (!((*e1p)->ex_flags & EX_PTRDIFF)
			    && (t1 == LONG || t1 == LNGLNG))
				ptrdiff = 0;
			if (!((*e2p)->ex_flags & EX_PTRDIFF)
			    && (t2 == LONG || t2 == LNGLNG)
			    && !shifting)
				ptrdiff = 0;
		}
		/* Now turn off ptrdiff flags */
		(*e1p)->ex_flags &= ~EX_PTRDIFF;
		(*e2p)->ex_flags &= ~EX_PTRDIFF;
	}

	/*	Now t1 and t2 are either INT, LONG, LNGLNG,
		FLOAT, DOUBLE, or LNGDBL
	*/

	/*	If any operand has the type long double, the other operand
		is converted to long double.
	*/
	/* ??? t1 == LNGDBL, t2 == DOUBLE */
	if (t1 == LNGDBL) {
		if (t2 != LNGDBL) {
		    if (t2 == DOUBLE || t2 == FLOAT)
			float2float(e2p, lngdbl_type);
		    else
			int2float(e2p, lngdbl_type);
		}
		return;
	} else if (t2 == LNGDBL) {
		if (t1 != LNGDBL) {
		    if (t1 == DOUBLE || t1 == FLOAT)
			float2float(e1p, lngdbl_type);
		    else
			int2float(e1p, lngdbl_type);
		}
		return;
	}

	/*	If any operand has the type double, the other operand
		is converted to double.
	*/
	if (t1 == DOUBLE) {
		if (t2 == FLOAT)
			float2float(e2p, double_type);
		else if (t2 != DOUBLE)
			int2float(e2p, double_type);
		return;
	} else if (t2 == DOUBLE) {
		if (t1 == FLOAT)
			float2float(e1p, double_type);
		else if (t1 != DOUBLE)
			int2float(e1p, double_type);
		return;
	}

	/*	If any operand has the type float, the other operand
		is converted to float.
	*/
	if (t1 == FLOAT) {
		if (t2 != FLOAT)
			int2float(e2p, float_type);
		return;
	} else if (t2 == FLOAT) {
		if (t1 != FLOAT)
			int2float(e1p, float_type);
		return;
	}

	/* Now they are INT, LONG or LNGLNG */
	u1 = (*e1p)->ex_type->tp_unsigned;
	u2 = (*e2p)->ex_type->tp_unsigned;
	convert1 = NULL;
	convert2 = NULL;

	/*	If either operand is a long long, the other operand
		is converted to long long; else if either operand is
		a long, the other operand is converted to a long.

		If one operand is signed and the other operand is
		unsigned, if the signed type can represent all values
		of the unsigned type, the unsigned operand is
		converted to the signed type, else both operands are
		converted to an unsigned type.
	*/
	if (shifting) {
		/*	In shifts like o1 << o2, never convert o1,
			and let ch3bin() convert o2 to int.
		*/
	} else if (t1 == LNGLNG && u1 && (t2 != LNGLNG || !u2))
		convert2 = ulnglng_type;
	else if (t2 == LNGLNG && u2 && (t1 != LNGLNG || !u1))
		convert1 = ulnglng_type;
	else if (t1 == LNGLNG && t2 != LNGLNG && u2) {
		if ((t2 == LONG ? long_size : int_size) < lnglng_size)
			convert2 = lnglng_type;
		else
			convert1 = convert2 = ulnglng_type;
	} else if (t2 == LNGLNG && t1 != LNGLNG && u1) {
		if ((t1 == LONG ? long_size : int_size) < lnglng_size)
			convert1 = lnglng_type;
		else
			convert1 = convert2 = ulnglng_type;
	} else if (t1 == LNGLNG && t2 != LNGLNG)
		convert2 = lnglng_type;
	else if (t2 == LNGLNG && t1 != LNGLNG)
		convert1 = lnglng_type;
	else if (t1 == LONG && u1 && (t2 != LONG || !u2))
		convert2 = ulong_type;
	else if (t2 == LONG && u2 && (t1 != LONG || !u1))
		convert1 = ulong_type;
	else if (t1 == LONG && t2 == INT && u2) {
		if (int_size < long_size)
			convert2 = long_type;
		else
			convert1 = convert2 = ulong_type;
	} else if (t2 == LONG && t1 == INT && u1) {
		if (int_size < long_size)
			convert1 = long_type;
		else
			convert1 = convert2 = ulong_type;
	} else if (t1 == LONG && t2 != LONG)
		convert2 = long_type;
	else if (t2 == LONG && t1 != LONG)
		convert1 = long_type;

	if (convert1)
		t1 = int2int(e1p, convert1);
	if (convert2)
		t2 = int2int(e2p, convert2);

	u1 = (*e1p)->ex_type->tp_unsigned;
	u2 = (*e2p)->ex_type->tp_unsigned;

	/*	If either operand has type unsigned int, the other operand
		is converted to unsigned int.
		Otherwise, both operands have type int.
	*/
	if (u1 && !u2 && !shifting)
		t2 = int2int(e2p, uint_type);
	else
	if (!u1 && u2 && !shifting)
		t1 = int2int(e1p, uint_type);

	if (int_size != pointer_size) {
		if (ptrdiff) {
			(*e1p)->ex_flags |= EX_PTRDIFF;
			(*e2p)->ex_flags |= EX_PTRDIFF;
		}
	}
}

void relbalance(register struct expr **e1p, int oper, register struct expr **e2p)
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the relational operator oper, or the ':'.
		Care is taken to switch the operands in case of a
		null-pointer constant. This is done so that ch3cast()
		allows assignments of a null-pointer to a function
		pointer.
	*/
	register struct expr *e1 = *e1p, *e2 = *e2p;
	struct expr *tmpexpr;

	if (e1->ex_type->tp_fund == POINTER
	    && is_cp_cst(e1)
	    && e1->VL_VALUE == 0) {
		tmpexpr = e1;
		e1 = e2;
		e2 = tmpexpr;
	}
	if (e1->ex_type->tp_fund == POINTER)
		ch3pointer(e2p, oper, e1->ex_type);
	else if (e2->ex_type->tp_fund == POINTER)
		ch3pointer(e1p, oper, e2->ex_type);
	else if (e1->ex_type == e2->ex_type
		&& e1->ex_type->tp_fund == ENUM) {}
	else if (oper == ':'
		    && e1->ex_type->tp_fund == VOID
		    && e2->ex_type->tp_fund == VOID) {}
	else
		arithbalance(e1p, oper, e2p);
}

void ch3pointer(struct expr **expp, int oper, register struct type *tp)
{
	/*	Checks whether *expp may be compared to tp using oper,
		as described in chapter 3.3.8 and 3.3.9.
		tp is known to be a pointer.
	*/
	register struct expr *exp = *expp;

	if (exp->ex_type->tp_fund == POINTER)	{
		if (exp->ex_type != tp)
			ch3cast(expp, oper, tp);
	}
	else
	if (is_integral_type(exp->ex_type)) {
		if ((oper != EQUAL && oper != NOTEQUAL && oper != ':')
		    || !(is_cp_cst(exp) && exp->VL_VALUE == 0)) {
			expr_error(exp,"%s on %s and pointer",
					symbol2str(oper),
					symbol2str(exp->ex_type->tp_fund));
		}
		ch3cast(expp, CAST, tp);
	}
	else	{
		expr_error(exp, "%s on %s and pointer",
				symbol2str(oper),
				symbol2str(exp->ex_type->tp_fund)
			);
		ch3cast(expp, oper, tp);
	}
}

int
any2arith(register struct expr **expp, register int oper)
{
	/*	Turns any expression into int_type, long_type,
		float_type, double_type or lngdbl_type.
	*/
	int fund;

	switch (fund = (*expp)->ex_type->tp_fund)	{
	case CHAR:
	case SHORT:
		assert((*expp)->ex_type->tp_size <= int_type->tp_size);

		if ((*expp)->ex_type->tp_unsigned
		    && (*expp)->ex_type->tp_size == int_type->tp_size) {
			int2int(expp, uint_type);
		} else {
			int2int(expp, int_type);
		}
		break;
	case INT:
	case LONG:
	case LNGLNG:
		break;
	case ENUM:
#ifndef	LINT
		/*	we do not want this conversion for lint, since we
			want to keep enums and ints separate
		*/
		int2int(expp, int_type);
#endif	/* LINT */
		break;
	case FLOAT:
/*	only when it is a parameter and the default promotion should
	occur. Hence this code is moved to any2parameter().
		float2float(expp, double_type);
		break;
*/
	case DOUBLE:
	case LNGDBL:
		break;
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

void erroneous2int(struct expr **expp)
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

struct expr *arith2arith(struct type *tp, int oper, register struct expr *expr)
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

int int2int(struct expr **expp, register struct type *tp)
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
			writh remainder = exp->VL_VALUE &
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

static int fit4(writh val, int uns)
{
	/* Does this value fit in 4 bytes? */
	unsigned writh u = (unsigned writh)val;

	if (!uns)
		u += 0x80000000UL;
	return (u & full_mask[4]) == u;
}

/* With compile-time constants, we don't set fp_used, since this is done
 * only when necessary in eval.c.
 */
void int2float(register struct expr **expp, struct type *tp)
{
	/*	The expression *expp, which is of some integral type, is
		converted to the floating type tp.
	*/
	register struct expr *exp = *expp;
	int uns = exp->ex_type->tp_unsigned;
	
	if (is_cp_cst(exp) && fit4(exp->VL_VALUE, uns)) {
		exp->ex_type = tp;
		exp->ex_class = Float;
		flt_arith2flt((arith)exp->VL_VALUE, &(exp->FL_ARITH), uns);
	}
	else	{
		fp_used = 1;
		*expp = arith2arith(tp, INT2FLOAT, *expp);
	}
}

void float2int(struct expr **expp, struct type *tp)
{
	/*	The expression *expp, which is of some floating type, is
		converted to the integral type tp.
	*/
	register struct expr *ex = *expp;

	if (is_fp_cst(ex)) {
		arith ar = flt_flt2arith(&ex->FL_ARITH, tp->tp_unsigned);
#ifdef NOTDEF
		/*	Historically, we always did the conversion at
		    compile time.   This is now wrong if type arith is
		    too narrow for an 8-byte integer.
		*/
		if (flt_status == FLT_OVFL)
			expr_warning(ex,"overflow in float to int conversion");
		else if (flt_status == FLT_UNFL)
			expr_warning(ex,"underflow in float to unsigned conversion");
#endif /* NOTDEF */
		/*	Now, we defer the conversion until run time
		    unless it fits in 4 bytes.
		*/
		if (flt_status != FLT_OVFL && flt_status != FLT_UNFL &&
		    fit4((writh)ar, tp->tp_unsigned)) {
			ex->ex_type = tp;
			/* The following lines are copied from fill_int_expr */
			ex->ex_class = Value;
			ex->VL_CLASS = Const;
			ex->VL_VALUE = (writh)ar;
			cut_size(ex);
			return;
		}
	}
	fp_used = 1;
	*expp = arith2arith(tp, FLOAT2INT, ex);
}

void float2float(register struct expr **expp, struct type *tp)
{
	/*	The expression *expp, which is of some floating type, is
		converted to the floating type tp.
		There is no need for an explicit conversion operator
		if the expression is a constant.
	*/
	
	if (is_fp_cst(*expp))
		(*expp)->ex_type = tp;
	else {
		fp_used = 1;
		*expp = arith2arith(tp, FLOAT2FLOAT, *expp);
	}
}

void array2pointer(register struct expr *exp)
{
	/*	The expression, which must be an array, is converted
		to a pointer.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type->tp_up
				    , /* exp->ex_type->tp_typequal */ 0
				    , (arith)0, NO_PROTO);
}

void function2pointer(register struct expr *exp)
{
	/*	The expression, which must be a function, is converted
		to a pointer to the function.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type, 0,
				     (arith)0, NO_PROTO);
}

void string2pointer(register struct expr *ex)
{
	/*	The expression, which must be a string constant, is converted
		to a pointer to the string-containing area.
	*/
	label lbl;

	lbl = code_string(ex->SG_VALUE, ex->SG_LEN);
	ex->ex_class = Value;
	ex->VL_CLASS = Label;
	ex->VL_LBL = lbl;
	ex->VL_VALUE = (arith)0;
}

void opnd2integral(register struct expr **expp, int oper)
{
	register int fund = (*expp)->ex_type->tp_fund;

	if (fund != INT && fund != LONG && fund != LNGLNG) {
		expr_error(*expp, "%s operand to %s",
				symbol2str(fund), symbol2str(oper));
		erroneous2int(expp);
		/* fund = INT; */
	}
}

void opnd2logical(register struct expr **expp, int oper)
{
	int fund = (*expp)->ex_type->tp_fund;

	if (fund == FUNCTION || fund == ARRAY) {
		expr_warning(*expp, "%s operand to %s",
			symbol2str(fund),
			symbol2str(oper));
		if (fund == FUNCTION) function2pointer(*expp);
		else array2pointer(*expp);
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
	case LNGLNG:
	case ENUM:
	case POINTER:
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		break;
	default:
		expr_error(*expp, "%s operand to %s",
			symbol2str(fund), symbol2str(oper));
	case ERRONEOUS:
		erroneous2int(expp);
		break;
	}
}

void
opnd2test(register struct expr **expp, int oper)
{
	opnd2logical(expp, oper);
	if ((*expp)->ex_class == Oper) {
		switch((*expp)->OP_OPER) {
		case '<':
		case '>':
		case LESSEQ:
		case GREATEREQ:
		case EQUAL:
		case NOTEQUAL:
		case '!':
		case AND:
		case OR:	/* && and || also impose a test	*/
			/* It is already a test */
			return;
		case ',':
			opnd2test(&((*expp)->OP_RIGHT), oper);
			return;
		}
	}
	ch3bin(expp, NOTEQUAL, intexpr((arith)0, INT));
}

void any2opnd(register struct expr **expp, int oper)
{
	if (!*expp)
		return;

	if (oper == SIZEOF || oper == ADDRESSOF) return;

	switch ((*expp)->ex_type->tp_fund)	{
	case CHAR:
	case SHORT:
	case ENUM:
	/* case FLOAT:	*//* not necessary anymore */
		any2arith(expp, oper);
		break;
	case ARRAY:
		array2pointer(*expp);
		break;
	case POINTER:
		if ((*expp)->ex_class == String)
			string2pointer(*expp);
		break;
	case FUNCTION:
		function2pointer(*expp);
		break;
#ifndef NOBITFIELD
	case FIELD:
		field2arith(expp);
		break;
#endif /* NOBITFIELD */
	}
}

void any2parameter(register struct expr **expp)
{
	/*	To handle default argument promotions
	*/
	any2opnd(expp, '(');
	if (int_size != pointer_size)
	    if ((*expp)->ex_flags & EX_PTRDIFF)
		expr_warning(*expp, "pointer difference caused long expression");
	if ((*expp)->ex_type->tp_fund == FLOAT)
		float2float(expp, double_type);
}

#ifndef NOBITFIELD
void field2arith(register struct expr **expp)
{
	/*	The expression to extract the bitfield value from the
		memory word is put in the tree.
	*/
	register struct type *tp = (*expp)->ex_type->tp_up;
	register struct field *fd = (*expp)->ex_type->tp_field;

	(*expp)->ex_type = word_type;

	if (tp->tp_unsigned)	{	/* don't worry about the sign bit */
		if (fd->fd_width >= 8 * (int)word_size)
			(*expp)->ex_type = uword_type;
		ch3bin(expp, RIGHT, intexpr((arith)fd->fd_shift, INT));
		ch3bin(expp, '&', intexpr(fd->fd_mask, INT));
	}
	else	{	/* take care of the sign bit: sign extend if needed */
		arith other_bits = (int)word_size * 8 - fd->fd_width;

		ch3bin(expp, LEFT,
			intexpr(other_bits - fd->fd_shift,
						INT)
		);
		ch3bin(expp, RIGHT, intexpr(other_bits, INT));
	}
}
#endif /* NOBITFIELD */

/*	switch_sign_fp() negates the given floating constant expression,
 *	and frees the string representing the old value.
 */
void switch_sign_fp(register struct expr *expr)
{
	flt_umin(&(expr->FL_ARITH));
}

char *writh2str(writh val, int uns)
{
	/*	Converts val to a decimal string, like
		long2str(val, 10), but allows wider values.
	*/
	static char buf[NUMSIZE + 1];
	char *cp = &buf[NUMSIZE + 1];
	int negative = (!uns && val < 0);
	unsigned writh u = (unsigned writh)val;

	if (negative)
		u = -u;
	*--cp = '\0';
	do {
		*--cp = '0' + (u % 10);
		u /= 10;
	} while (u != 0);
	if (negative)
		*--cp = '-';
	return cp;
}
