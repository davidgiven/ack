/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S E M A N T I C   A N A L Y S I S -- C H A P T E R  7 RM	*/

#include	"lint.h"
#include	"nofloat.h"
#include	"debug.h"
#include	"nobitfield.h"
#include	"idf.h"
#include	"arith.h"
#include	"type.h"
#include	"struct.h"
#include	"label.h"
#include	"expr.h"
#include	"def.h"
#include	"Lpars.h"
#include	"assert.h"

extern char options[];
extern char *symbol2str();

/*	Most expression-handling routines have a pointer to a
	(struct type *) as first parameter. The object under the pointer
	gets updated in the process.
*/

ch7sel(expp, oper, idf)
	struct expr **expp;
	struct idf *idf;
{
	/*	The selector idf is applied to *expp; oper may be '.' or
		ARROW.
	*/
	register struct expr *exp;
	register struct type *tp;
	register struct sdef *sd;

	any2opnd(expp, oper);
	exp = *expp;
	tp = exp->ex_type;
	if (oper == ARROW)	{
		if (tp->tp_fund == POINTER &&
		    ( tp->tp_up->tp_fund == STRUCT ||
		      tp->tp_up->tp_fund == UNION))	/* normal case */
			tp = tp->tp_up;
		else {	/* constructions like "12->selector" and
				"char c; c->selector"
			*/
			switch (tp->tp_fund)	{
			case INT:
			case LONG:
				/* Allowed by RM 14.1 */
				ch7cast(expp, CAST, pa_type);
				sd = idf2sdef(idf, tp);
				tp = sd->sd_stype;
				break;
			case POINTER:
				break;
			default:
				expr_error(exp, "-> applied to %s",
					symbol2str(tp->tp_fund));
			case ERRONEOUS:
				exp->ex_type = error_type;
				return;
			}
		}
	} /* oper == ARROW */
	else { /* oper == '.' */
		/* filter out illegal expressions "non_lvalue.sel" */
		if (!exp->ex_lvalue) {
			expr_error(exp, "dot requires lvalue");
			return;
		}
	}
	exp = *expp;
	switch (tp->tp_fund)	{
	case POINTER:	/* for int *p;	p->next = ...	*/
	case STRUCT:
	case UNION:
		break;
	case INT:
	case LONG:
		/* warning will be given by idf2sdef() */
		break;
	default:
		if (!is_anon_idf(idf))
			expr_error(exp, "selector %s applied to %s",
				idf->id_text, symbol2str(tp->tp_fund));
	case ERRONEOUS:
		exp->ex_type = error_type;
		return;
	}
	sd = idf2sdef(idf, tp);
	if (oper == '.')	{
		/*	there are 3 cases in which the selection can be
			performed compile-time: 
			I:	n.sel (n either an identifier or a constant)
			II:	(e.s1).s2 (transformed into (e.(s1+s2)))
			III:	(e->s1).s2 (transformed into (e->(s1+s2)))
				The code performing these conversions is
				extremely obscure.
		*/
		if (exp->ex_class == Value)	{
			/*	It is an object we know the address of; so
				we can calculate the address of the
				selected member 
			*/
			exp->VL_VALUE += sd->sd_offset;
			exp->ex_type = sd->sd_type;
			if (exp->ex_type == error_type)
				exp->ex_flags |= EX_ERROR;
		}
		else
		if (exp->ex_class == Oper)	{
			struct oper *op = &(exp->ex_object.ex_oper);
			
			if (op->op_oper == '.' || op->op_oper == ARROW)	{
				ASSERT(is_cp_cst(op->op_right));
				op->op_right->VL_VALUE += sd->sd_offset;
				exp->ex_type = sd->sd_type;
				if (exp->ex_type == error_type)
					exp->ex_flags |= EX_ERROR;
			}
			else
				exp = new_oper(sd->sd_type, exp, '.',
						intexpr(sd->sd_offset, INT));
		}
	}
	else /* oper == ARROW */
		exp = new_oper(sd->sd_type,
			exp, oper, intexpr(sd->sd_offset, INT));
	exp->ex_lvalue = (sd->sd_type->tp_fund != ARRAY);
	*expp = exp;
}

ch7incr(expp, oper)
	struct expr **expp;
{
	/*	The monadic prefix/postfix incr/decr operator oper is
		applied to *expp.
	*/
	ch7asgn(expp, oper, intexpr((arith)1, INT));
}

ch7cast(expp, oper, tp)
	register struct expr **expp;
	register struct type *tp;
{
	/*	The expression *expp is cast to type tp; the cast is
		caused by the operator oper.  If the cast has
		to be passed on to run time, its left operand will be an
		expression of class Type.
	*/
	register struct type *oldtp;

	if ((*expp)->ex_type->tp_fund == FUNCTION)
		function2pointer(*expp);
	if ((*expp)->ex_type->tp_fund == ARRAY)
		array2pointer(*expp);
	if ((*expp)->ex_class == String)
		string2pointer(*expp);
	oldtp = (*expp)->ex_type;

#ifndef NOBITFIELD
	if (oldtp->tp_fund == FIELD)	{
		field2arith(expp);
		ch7cast(expp, oper, tp);
	}
	else
	if (tp->tp_fund == FIELD) {
		ch7cast(expp, oper, tp->tp_up);
	}
	else
#endif /* NOBITFIELD */
	if (oldtp == tp) {
		/* life is easy */
	}
	else
	if (tp->tp_fund == VOID) {
		/* Easy again */
		(*expp)->ex_type = void_type;
	}
	else
	if (is_arith_type(oldtp) && is_arith_type(tp))	{
		int oldi = is_integral_type(oldtp);
		int i = is_integral_type(tp);

		if (oldi && i)	{
			if (	oper != CAST
			&&	(	tp->tp_fund == ENUM
				||	oldtp->tp_fund == ENUM
				)
			) {
				expr_warning(*expp,
					"dubious %s on enum",
					symbol2str(oper));
			}
#ifdef	LINT
			if (oper == CAST)
				(*expp)->ex_type = tp;
			else
				int2int(expp, tp);
#else	/* LINT */
			int2int(expp, tp);
#endif	/* LINT */
		}
#ifndef NOFLOAT
		else
		if (oldi && !i)	{
			if (oldtp->tp_fund == ENUM && oper != CAST)
				expr_warning(*expp,
					"conversion of enum to %s\n",
					symbol2str(tp->tp_fund));
#ifdef	LINT
			if (oper == CAST)
				(*expp)->ex_type = tp;
			else
				int2float(expp, tp);
#else	/* LINT */
			int2float(expp, tp);
#endif	/* LINT */
		}
		else
		if (!oldi && i) {
#ifdef	LINT
			if (oper == CAST)
				(*expp)->ex_type = tp;
			else
				float2int(expp, tp);
#else	/* LINT */
			float2int(expp, tp);
#endif	/* LINT */
		}
		else {
			/* !oldi && !i */
#ifdef	LINT
			if (oper == CAST)
				(*expp)->ex_type = tp;
			else
				float2float(expp, tp);
#else	/* LINT */
			float2float(expp, tp);
#endif	/* LINT */
		}
#else /* NOFLOAT */
		else {
			crash("(ch7cast) floats not implemented\n");
			/*NOTREACHED*/
		}
#endif /* NOFLOAT */
	}
	else
	if (oldtp->tp_fund == POINTER && tp->tp_fund == POINTER)	{
		if (oper != CAST)
			expr_warning(*expp, "incompatible pointers in %s",
							symbol2str(oper));
#ifdef	LINT
		if (oper != CAST)
			lint_ptr_conv(oldtp->tp_up->tp_fund, tp->tp_up->tp_fund);
#endif	/* LINT */
		(*expp)->ex_type = tp;	/* free conversion */
	}
	else
	if (oldtp->tp_fund == POINTER && is_integral_type(tp))	{
		/* from pointer to integral */
		if (oper != CAST)
			expr_warning(*expp,
				"illegal conversion of pointer to %s",
				symbol2str(tp->tp_fund));
		if (oldtp->tp_size > tp->tp_size)
			expr_warning(*expp,
				"conversion of pointer to %s loses accuracy",
				symbol2str(tp->tp_fund));
		if (oldtp->tp_size != tp->tp_size)
			int2int(expp, tp);
		else
			(*expp)->ex_type = tp;
	}
	else
	if (tp->tp_fund == POINTER && is_integral_type(oldtp))	{
		/* from integral to pointer */
		switch (oper)	{
		case CAST:
			break;
		case EQUAL:
		case NOTEQUAL:
		case ':':
		case '=':
		case RETURN:
			if (is_cp_cst(*expp) && (*expp)->VL_VALUE == (arith)0)
				break;
		default:
			expr_warning(*expp,
				"dubious conversion of %s to pointer",
				symbol2str(oldtp->tp_fund));
			break;
		}
		if (oldtp->tp_size > tp->tp_size)
			expr_warning(*expp,
				"conversion of %s to pointer loses accuracy",
				symbol2str(oldtp->tp_fund));
		if (oldtp->tp_size != tp->tp_size)
			int2int(expp, tp);
		else
			(*expp)->ex_type = tp;
	}
	else
	if (oldtp->tp_fund == ERRONEOUS) {
		/* we just won't look */
		(*expp)->ex_type = tp;	/* brute force */
	}
	else
	if (oldtp->tp_size == tp->tp_size && oper == CAST)	{
		expr_warning(*expp, "dubious conversion based on equal size");
		(*expp)->ex_type = tp;		/* brute force */
	}
	else	{
		if (oldtp->tp_fund != ERRONEOUS && tp->tp_fund != ERRONEOUS)
			expr_error(*expp, "cannot convert %s to %s",
				symbol2str(oldtp->tp_fund),
				symbol2str(tp->tp_fund)
			);
		(*expp)->ex_type = tp;		/* brute force */
	}
}

ch7asgn(expp, oper, expr)
	struct expr **expp;
	struct expr *expr;
{
	/*	The assignment operators.
		"f op= e" should be interpreted as
		"f = (typeof f)((typeof (f op e))f op (typeof (f op e))e)"
		and not as "f = f op (typeof f)e".
		Consider, for example, (i == 10) i *= 0.9; (i == 9), where
		typeof i == int.
		The resulting expression tree becomes:
				op=
				/ \
			       /   \
			      f     (typeof (f op e))e
		EVAL should however take care of evaluating (typeof (f op e))f
	*/
	register struct expr *exp = *expp;
	int fund = exp->ex_type->tp_fund;
	struct type *tp;

	/* We expect an lvalue */
	if (!exp->ex_lvalue)	{
		expr_error(exp, "no lvalue in lhs of %s", symbol2str(oper));
		exp->ex_depth = 99;	/* no direct store/load at EVAL() */
			/* what is 99 ??? DG */
	}
	if (oper == '=') {
		ch7cast(&expr, oper, exp->ex_type);
		tp = expr->ex_type;
	}
	else {	/* turn e into e' where typeof(e') = typeof (f op e) */
		struct expr *extmp = intexpr((arith)0, INT);

		/* this is really $#@&*%$# ! */
		/* if you correct this, please correct lint_new_oper() too */
		extmp->ex_lvalue = 1;
		extmp->ex_type = exp->ex_type;
		ch7bin(&extmp, oper, expr);
		/* Note that ch7bin creates a tree of the expression
			((typeof (f op e))f op (typeof (f op e))e),
		   where f ~ extmp and e ~ expr.
		   We want to use (typeof (f op e))e.
		   Ch7bin does not create a tree if both operands
		   were illegal or constants!
		*/
		tp = extmp->ex_type;	/* perform the arithmetic in type tp */
		if (extmp->ex_class == Oper) {
			expr = extmp->OP_RIGHT;
			extmp->OP_RIGHT = NILEXPR;
			free_expression(extmp);
		}
		else
			expr = extmp;
	}
#ifndef NOBITFIELD
	if (fund == FIELD)
		exp = new_oper(exp->ex_type->tp_up, exp, oper, expr);
	else
		exp = new_oper(exp->ex_type, exp, oper, expr);
#else /* NOBITFIELD */
	exp = new_oper(exp->ex_type, exp, oper, expr);
#endif /* NOBITFIELD */
	exp->OP_TYPE = tp;	/* for EVAL() */
	exp->ex_flags |= EX_SIDEEFFECTS;
	*expp = exp;
}

/*	Some interesting (?) questions answered.
*/
int
is_integral_type(tp)
	register struct type *tp;
{
	switch (tp->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
		return 1;
#ifndef NOBITFIELD
	case FIELD:
		return is_integral_type(tp->tp_up);
#endif /* NOBITFIELD */
	default:
		return 0;
	}
}

int
is_arith_type(tp)
	register struct type *tp;
{
	switch (tp->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
#endif /* NOFLOAT */
		return 1;
#ifndef NOBITFIELD
	case FIELD:
		return is_arith_type(tp->tp_up);
#endif /* NOBITFIELD */
	default:
		return 0;
	}
}
