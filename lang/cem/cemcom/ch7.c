/* $Header$ */
/*	S E M A N T I C   A N A L Y S I S -- C H A P T E R  7 RM	*/

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
	register struct expr **expp;
	struct idf *idf;
{
	/*	The selector idf is applied to *expp; oper may be '.' or
		ARROW.
	*/
	register struct type *tp = (*expp)->ex_type;
	register struct sdef *sd;

	if (oper == ARROW)	{
		if (tp->tp_fund == POINTER)	/* normal case */
			tp = tp->tp_up;
		else {	/* constructions like "12->selector" and
				"char c; c->selector"
			*/
			switch (tp->tp_fund)	{
			case CHAR:
			case SHORT:
			case INT:
			case LONG:
			case ENUM:
				/* Allowed by RM 14.1 */
				ch7cast(expp, CAST, pa_type);
				sd = idf2sdef(idf, tp);
				tp = sd->sd_stype;
				break;
			default:
				expr_error(*expp, "-> applied to %s",
					symbol2str(tp->tp_fund));
			case ERRONEOUS:
				(*expp)->ex_type = error_type;
				return;
			}
		} /* tp->tp_fund != POINTER */
	} /* oper == ARROW */
	else { /* oper == '.' */
		/* filter out illegal expressions "non_lvalue.sel" */
		if (!(*expp)->ex_lvalue) {
			expr_error(*expp, "dot requires lvalue");
			return;
		}
	}
	switch (tp->tp_fund)	{
	case POINTER:	/* for int *p;	p->next = ...	*/
	case STRUCT:
	case UNION:
		break;
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
		/* warning will be given by idf2sdef() */
		break;
	default:
		if (!is_anon_idf(idf))
			expr_error(*expp, "selector %s applied to %s",
				idf->id_text, symbol2str(tp->tp_fund));
	case ERRONEOUS:
		(*expp)->ex_type = error_type;
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
		if ((*expp)->ex_class == Value)	{
			/*	It is an object we know the address of; so
				we can calculate the address of the
				selected member 
			*/
			(*expp)->VL_VALUE += sd->sd_offset;
			(*expp)->ex_type = sd->sd_type;
			if ((*expp)->ex_type == error_type)
				(*expp)->ex_flags |= EX_ERROR;
		}
		else
		if ((*expp)->ex_class == Oper)	{
			struct oper *op = &((*expp)->ex_object.ex_oper);
			
			if (op->op_oper == '.' || op->op_oper == ARROW)	{
				ASSERT(is_cp_cst(op->op_right));
				op->op_right->VL_VALUE += sd->sd_offset;
				(*expp)->ex_type = sd->sd_type;
				if ((*expp)->ex_type == error_type)
					(*expp)->ex_flags |= EX_ERROR;
			}
			else
				*expp = new_oper(sd->sd_type, *expp, '.',
						intexpr(sd->sd_offset, INT));
		}
	}
	else	{
		/* oper == ARROW */
		*expp = new_oper(sd->sd_type,
			*expp, oper, intexpr(sd->sd_offset, INT));
	}
	(*expp)->ex_lvalue = (sd->sd_type->tp_fund != ARRAY);
}

ch7incr(expp, oper)
	register struct expr **expp;
{
	/*	The monadic prefix/postfix incr/decr operator oper is
		applied to *expp.
	*/
	arith addend;
	struct expr *expr;
	register int fund = (*expp)->ex_type->tp_fund;

	if (!(*expp)->ex_lvalue)	{
		expr_error(*expp, "no lvalue with %s", symbol2str(oper));
		return;
	}
	if (fund == ENUM)	{
		expr_warning(*expp, "%s on enum", symbol2str(oper));
		addend = (arith)1;
	}
	else
	if (is_arith_type((*expp)->ex_type))
		addend = (arith)1;
	else
	if (fund == POINTER)
		addend = size_of_type((*expp)->ex_type->tp_up, "object");
#ifndef NOBITFIELD
	else
	if (fund == FIELD)
		addend = (arith)1;
#endif NOBITFIELD
	else	{
		expr_error(*expp, "%s on %s",
				symbol2str(oper),
				symbol2str((*expp)->ex_type->tp_fund)
			);
		return;
	}
	expr = intexpr(addend, INT);
	ch7cast(&expr, CAST, (*expp)->ex_type);
#ifndef NOBITFIELD
	if (fund == FIELD)
		*expp = new_oper((*expp)->ex_type->tp_up, *expp, oper, expr);
	else
#endif NOBITFIELD
		*expp = new_oper((*expp)->ex_type, *expp, oper, expr);
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
		function2pointer(expp);
	if ((*expp)->ex_type->tp_fund == ARRAY)
		array2pointer(expp);
	if ((*expp)->ex_class == String)
		string2pointer(expp);
	oldtp = (*expp)->ex_type;
#ifndef NOBITFIELD
	if (oldtp->tp_fund == FIELD)	{
		field2arith(expp);
		ch7cast(expp, oper, tp);
	}
	else
	if (tp->tp_fund == FIELD)
		ch7cast(expp, oper, tp->tp_up);
	else
#endif NOBITFIELD
	if (oldtp == tp)
		{}			/* life is easy */
	else
	if (tp->tp_fund == VOID)	/* Easy again */
		(*expp)->ex_type = void_type;
	else
	if (is_arith_type(oldtp) && is_arith_type(tp))	{
		int oldi = is_integral_type(oldtp);
		int i = is_integral_type(tp);

		if (oldi && i)	{
			if (	oldtp->tp_fund == ENUM &&
				tp->tp_fund == ENUM &&
				oper != CAST
			)
				expr_warning(*expp,
					"%s on enums of different types",
					symbol2str(oper));
			int2int(expp, tp);
		}
#ifndef NOFLOAT
		else
		if (oldi && !i)	{
			if (oldtp->tp_fund == ENUM && oper != CAST)
				expr_warning(*expp,
					"conversion of enum to %s\n",
					symbol2str(tp->tp_fund));
			int2float(expp, tp);
		}
		else
		if (!oldi && i)
			float2int(expp, tp);
		else		/* !oldi && !i */
			float2float(expp, tp);
#else NOFLOAT
		else
			crash("(ch7cast) floats not implemented\n");
#endif NOFLOAT
	}
	else
	if (oldtp->tp_fund == POINTER && tp->tp_fund == POINTER)	{
		if (oper != CAST)
			expr_warning(*expp, "incompatible pointers in %s",
							symbol2str(oper));
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
		case '=':
		case RETURN:
			if (is_cp_cst(*expp) && (*expp)->VL_VALUE == (arith)0)
				break;
		default:
			expr_warning(*expp,
				"illegal conversion of %s to pointer",
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
	if (oldtp->tp_fund == ERRONEOUS)	{
		/* we just won't look */
		(*expp)->ex_type = tp;		/* brute force */
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
	register struct expr **expp;
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
	int fund = (*expp)->ex_type->tp_fund;
	struct type *tp;

	/* We expect an lvalue */
	if (!(*expp)->ex_lvalue)	{
		expr_error(*expp, "no lvalue in lhs of %s", symbol2str(oper));
		(*expp)->ex_depth = 99;	/* no direct store/load at EVAL() */
			/* what is 99 ??? DG */
	}
	if (oper == '=') {
		ch7cast(&expr, oper, (*expp)->ex_type);
		tp = expr->ex_type;
	}
	else {	/* turn e into e' where typeof(e') = typeof (f op e) */
		struct expr *extmp = intexpr((arith)0, INT);

		/* this is really $#@&*%$# ! */
		extmp->ex_lvalue = 1;
		extmp->ex_type = (*expp)->ex_type;
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
		*expp = new_oper((*expp)->ex_type->tp_up, *expp, oper, expr);
	else
		*expp = new_oper((*expp)->ex_type, *expp, oper, expr);
#else NOBITFIELD
	*expp = new_oper((*expp)->ex_type, *expp, oper, expr);
#endif NOBITFIELD

	(*expp)->OP_TYPE = tp;	/* for EVAL() */
}

/*	Some interesting (?) questions answered.
*/
int
is_integral_type(tp)
	struct type *tp;
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
#endif NOBITFIELD
	default:
		return 0;
	}
}

int
is_arith_type(tp)
	struct type *tp;
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
#endif NOFLOAT
		return 1;
#ifndef NOBITFIELD
	case FIELD:
		return is_arith_type(tp->tp_up);
#endif NOBITFIELD
	default:
		return 0;
	}
}
