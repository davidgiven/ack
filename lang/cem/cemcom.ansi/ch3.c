/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S E M A N T I C   A N A L Y S I S -- C H A P T E R  3.3		*/

#include	<assert.h>
#include	"parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include    "ch3.h"
#include    "idf.h"
#include	"proto.h"
#include	"type.h"
#include	"struct.h"
#include	"label.h"
#include	"expr.h"
#include	"def.h"
#include	"Lpars.h"
#include    "error.h"
#include    "ch3bin.h"
#include	"file_info.h"

extern char options[];
extern char *symbol2str();
extern struct type *qualifier_type();



/*	Most expression-handling routines have a pointer to a
	(struct type *) as first parameter. The object under the pointer
	gets updated in the process.
*/

void ch3sel(struct expr **expp, int oper, struct idf *idf)
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
			case POINTER:
				break;
			case INT:
			case LONG:
			case LNGLNG:
				/* An error is given in idf2sdef() */
				ch3cast(expp, CAST, pa_type);
				sd = idf2sdef(idf, tp);
				tp = sd->sd_stype;
				break;
			default:
				expr_error(exp, "-> applied to %s",
					symbol2str(tp->tp_fund));
			case ERRONEOUS:
				exp->ex_type = error_type;
				return;
			}
		}
	} else {		/* oper == '.' */
		/* nothing */
	}
	exp = *expp;
	switch (tp->tp_fund)	{
	case POINTER:	/* for int *p;	p->next = ...	*/
	case STRUCT:
	case UNION:
		break;
	case INT:
	case LONG:
	case LNGLNG:
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
			exp->ex_lvalue = exp->ex_type->tp_fund != ARRAY;
			if (exp->ex_type == error_type) {
				exp->ex_flags |= EX_ERROR;
			}
		}
		else
		if (exp->ex_class == Oper)	{
			struct oper *op = &(exp->ex_object.ex_oper);
			
			if (op->op_oper == '.' || op->op_oper == ARROW)	{
				assert(is_cp_cst(op->op_right));
				op->op_right->VL_VALUE += sd->sd_offset;
				exp->ex_type = sd->sd_type;
				exp->ex_lvalue = exp->ex_type->tp_fund != ARRAY;
				if (exp->ex_type == error_type) {
					exp->ex_flags |= EX_ERROR;
				}
			}
			else {
				exp = new_oper(sd->sd_type, exp, '.',
						intexpr(sd->sd_offset, INT));
				exp->ex_lvalue = sd->sd_type->tp_fund != ARRAY;
				if (!exp->OP_LEFT->ex_lvalue)
					exp->ex_flags |= EX_ILVALUE;
			}
		}
	}
	else { /* oper == ARROW */
		if (is_ld_cst(exp)) {
			exp->VL_VALUE += sd->sd_offset;
			exp->ex_type = sd->sd_type;
		}
		else  {
			exp = new_oper(sd->sd_type,
				exp, oper, intexpr(sd->sd_offset, INT));
		}
		exp->ex_lvalue = (sd->sd_type->tp_fund != ARRAY);
		exp->ex_flags &= ~EX_ILVALUE;
	}
	if ((sd->sd_type->tp_typequal & TQ_CONST)
	    || (tp->tp_typequal & TQ_CONST))
		exp->ex_flags |= EX_READONLY;
	if ((sd->sd_type->tp_typequal & TQ_VOLATILE)
	    || (tp->tp_typequal & TQ_VOLATILE))
		exp->ex_flags |= EX_VOLATILE;
	if (oper == '.' && exp->ex_flags & EX_READONLY)  {
		exp->ex_type = qualifier_type(exp->ex_type, TQ_CONST);
	}
	if (exp->ex_flags & EX_VOLATILE)  {
		exp->ex_type = qualifier_type(exp->ex_type, TQ_VOLATILE);
	}
	*expp = exp;
}

void ch3incr(struct expr **expp, int oper)
{
	/*	The monadic prefix/postfix incr/decr operator oper is
		applied to *expp.
	*/
	ch3asgn(expp, oper, intexpr((arith)1, INT));
}

void ch3cast(register struct expr **expp, int oper, register struct type *tp)
{
	/*	The expression *expp is cast to type tp; the cast is
		caused by the operator oper.  If the cast has
		to be passed on to run time, its left operand will be an
		expression of class Type.
	*/
	register struct type *oldtp;
	register struct expr *exp = *expp;
	int qual_lev, ascompat = 0;

	if (oper == RETURN && tp->tp_fund == VOID) {
		expr_strict(exp, "return <expression> in function returning void");
		exp->ex_type = void_type;
		return;
	}
	if (exp->ex_type->tp_fund == FUNCTION) {
		function2pointer(exp);
	}
	if (exp->ex_type->tp_fund == ARRAY)
		array2pointer(exp);
	if (exp->ex_class == String)
		string2pointer(exp);
	oldtp = exp->ex_type;

	if (oldtp->tp_size <= 0 && oldtp->tp_fund != VOID) {
		expr_error(exp,"incomplete type in expression");
	}

#ifndef NOBITFIELD
	if (oldtp->tp_fund == FIELD)	{
		field2arith(expp);
		ch3cast(expp, oper, tp);
		return;
	}
	if (tp->tp_fund == FIELD) {
		ch3cast(expp, oper, tp->tp_up);
		return;
	}
#endif /* NOBITFIELD */
	switch (oper) {
	default:	qual_lev = -1; break;
	case CAST:	qual_lev = -999; break;		/* ??? hack */
	case CASTAB:
	case '=':
	case RETURN:	ascompat = 1;		/* assignment compatibility */
		/* fallthrough */
	case '-':
	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
	case EQUAL:
	case NOTEQUAL:
	case ':':
			qual_lev = -2;
			break;
	}

	if (equal_type(tp, oldtp, qual_lev, 0)) {
		/* life is easy */
		if (ascompat && tp->tp_fund == POINTER) {
			if ((tp->tp_up->tp_typequal & oldtp->tp_up->tp_typequal)
			    != oldtp->tp_up->tp_typequal) {
				expr_strict( exp, "qualifier error");
			}
		}
		exp->ex_type = tp;	/* so qualifiers are allright */
	}
	else
	if (tp->tp_fund == VOID) {
		/* easy again */
		exp->ex_type = void_type;
	}
	else
	if (is_arith_type(oldtp) && is_arith_type(tp))	{
		int oldi = is_integral_type(oldtp);
		int i = is_integral_type(tp);

		if (oldi && i)	{
#ifdef	LINT
			if (oper == CAST)
				exp->ex_type = tp;
			else {
				int2int(expp, tp);
			}
#else	/* LINT */
			int2int(expp, tp);
#endif	/* LINT */
		}
		else
		if (oldi && !i)	{
#ifdef	LINT
			if (oper == CAST)
				exp->ex_type = tp;
			else {
				int2float(expp, tp);
			}
#else	/* LINT */
			int2float(expp, tp);
#endif	/* LINT */
		}
		else
		if (!oldi && i) {
#ifdef	LINT
			if (oper == CAST)
				exp->ex_type = tp;
			else {
				float2int(expp, tp);
			}
#else	/* LINT */
			float2int(expp, tp);
#endif	/* LINT */
		}
		else {
			/* !oldi && !i */
#ifdef	LINT
			if (oper == CAST)
				exp->ex_type = tp;
			else {
				float2float(expp, tp);
			}
#else	/* LINT */
			float2float(expp, tp);
#endif	/* LINT */
		}
	}
	else
	if (oldtp->tp_fund == POINTER && tp->tp_fund == POINTER)	{
		switch (oper) {
		case EQUAL:
		case NOTEQUAL:
		case '=':
		case CASTAB:
		case RETURN:
		case ':':
		    if (tp->tp_up && oldtp->tp_up) {
			    if (tp->tp_up->tp_fund == VOID
				&& oldtp->tp_up->tp_fund != FUNCTION) {
				break;	/* switch */
			    }
			    if (oldtp->tp_up->tp_fund == VOID
				&& tp->tp_up->tp_fund != FUNCTION) {
				break;	/* switch */
			    }
			    if (oldtp->tp_up->tp_fund == VOID
				&& is_cp_cst(exp)
				&& exp->VL_VALUE == (arith)0)
				break;	/* switch */
		    }
		    /* falltrough */
		default:
		    if (oper == CASTAB)
			    expr_strict(exp, "incompatible pointers in call");
		    else
			    expr_strict(exp, "incompatible pointers in %s",
							symbol2str(oper));
		    break;
		case CAST: break;
		}
#ifdef	LINT
		if (oper != CAST)
			lint_ptr_conv(oldtp->tp_up->tp_fund, tp->tp_up->tp_fund);
#endif	/* LINT */
		exp->ex_type = tp;	/* free conversion */
	}
	else
	if (oldtp->tp_fund == POINTER && is_integral_type(tp))	{
		/* from pointer to integral */
		if (oper != CAST)
			expr_strict(exp,
				"illegal conversion of pointer to %s",
				symbol2str(tp->tp_fund));
		if (oldtp->tp_size > tp->tp_size)
			expr_warning(exp,
				"conversion of pointer to %s loses accuracy",
				symbol2str(tp->tp_fund));
		if (oldtp->tp_size != tp->tp_size) {
			int2int(expp, tp);
		} else
			exp->ex_type = tp;
	}
	else
	if (tp->tp_fund == POINTER && is_integral_type(oldtp))	{
		/* from integral to pointer */
		switch (oper)	{
		case CAST:
			break;
		case CASTAB:
		case EQUAL:
		case NOTEQUAL:
		case '=':
		case RETURN:
			if (is_cp_cst(exp) && exp->VL_VALUE == (arith)0)
				break;
		default:
			expr_strict(exp,
				"illegal conversion of %s to pointer",
				symbol2str(oldtp->tp_fund));
			break;
		}
		if (oldtp->tp_size > tp->tp_size)
			expr_warning(exp,
				"conversion of %s to pointer loses accuracy",
				symbol2str(oldtp->tp_fund));
		if (oldtp->tp_size != tp->tp_size) {
			int2int(expp, tp);
		} else
			exp->ex_type = tp;
	}
	else
	if (oldtp->tp_fund == ERRONEOUS) {
		/* we just won't look */
		exp->ex_type = tp;	/* brute force */
	}
	else
	if (oldtp->tp_size == tp->tp_size && oper == CAST)	{
		expr_strict(exp, "dubious conversion based on equal size");
		exp->ex_type = tp;		/* brute force */
	}
	else	{
		if (oldtp->tp_fund != ERRONEOUS && tp->tp_fund != ERRONEOUS)
			expr_error(exp, "cannot convert %s to %s",
					symbol2str(oldtp->tp_fund),
					symbol2str(tp->tp_fund)
				    );
		exp->ex_type = tp;		/* brute force */
	}
	/* re-initialize exp, since *expp may have changed */
	exp = *expp;
	if (oper == CAST) {
		exp->ex_flags |= EX_ILVALUE;
	}
}

/*	Determine whether two types are equal.
*/
int equal_type(register struct type *tp,register struct type *otp, int qual_lev, int diag)
{
	 if (tp == otp)
		return 1;
	if (!tp
	    || !otp
	    || (tp->tp_fund != otp->tp_fund)
	    || (tp->tp_unsigned != otp->tp_unsigned)
	    || (tp->tp_align != otp->tp_align))
		return 0;
	if (tp->tp_size != otp->tp_size) {
		if (tp->tp_fund != ARRAY
		    || (tp->tp_size != -1 && otp->tp_size != -1))
			return 0;
	}

	if (qual_lev >= 0 && tp->tp_typequal != otp->tp_typequal) {
		strict("missing or illegal qualifiers");
	}

	switch (tp->tp_fund) {

	case FUNCTION:
		/*	If both types have parameter type lists, the type of
			each parameter in the composite parameter type list
			is the composite type of the corresponding paramaters.
		*/
		if (tp->tp_proto && otp->tp_proto) {
			if (!equal_proto(tp->tp_proto, otp->tp_proto, diag))
				return 0;
		} else if (tp->tp_proto || otp->tp_proto) {
			if (!legal_mixture(tp, otp, diag))
				return 0;
		}
		return equal_type(tp->tp_up, otp->tp_up, qual_lev + 1, diag);

	case ARRAY:
		/*	If one type is an array of known size, the composite
			type is an array of that size
		*/
		if (tp->tp_size != otp->tp_size &&
		     (tp->tp_size != -1 && otp->tp_size != -1))
			return 0;
		return equal_type(tp->tp_up, otp->tp_up, qual_lev/* ??? +1 */, diag);

	case POINTER:
		return equal_type(tp->tp_up, otp->tp_up, qual_lev + 1, diag);

	case FIELD:
		return equal_type(tp->tp_up, otp->tp_up, qual_lev/* ??? +1 */, diag);

	case STRUCT:
	case UNION:
	case ENUM:
		return tp->tp_idf == otp->tp_idf && tp->tp_sdef == otp->tp_sdef;

	default:
		return 1;
	}
}

int check_pseudoproto(register struct proto *pl,register struct proto *opl, int diag)
{
	int retval = 1;

	if (pl->pl_flag & PL_ELLIPSIS) {
		if (diag) {	
			error("illegal ellipsis terminator");
			pl->pl_flag |= PL_ERRGIVEN;
			opl->pl_flag |= PL_ERRGIVEN;
		}
		return 0;
	}
	if (opl->pl_flag & PL_VOID) {
		if (!(pl->pl_flag & PL_VOID)) {
			if (diag) {
				strict("function is defined without parameters");
			}
			return 0;
		}
		return 1;
	}
	while (pl && opl) {
	    if (!equal_type(pl->pl_type, opl->pl_type, -1, diag)) {
		if (diag) {
			if (!(pl->pl_flag & PL_ERRGIVEN)
			    && !(opl->pl_flag & PL_ERRGIVEN))
				error("incorrect type for parameter %s of definition",
					opl->pl_idf->id_text);
			pl->pl_flag |= PL_ERRGIVEN;
			opl->pl_flag |= PL_ERRGIVEN;
		}
		retval = 0;
	    }
	    pl = pl->next;
	    opl = opl->next;
	}
	if (pl || opl) {
		if (diag) error("incorrect number of parameters");
		retval = 0;
	}
	return retval;
}

int legal_mixture(struct type *tp, struct type *otp, int diag)
{
	struct proto *pl = tp->tp_proto, *opl = otp->tp_proto;
	int retval = 1;
	register struct proto *prot;
	int fund;

	assert( (pl != 0) ^ (opl != 0));
	if (pl)  {
		prot = pl;
	} else  {
		prot = opl;
	}
	if (!opl && otp->tp_pseudoproto) {
		return check_pseudoproto(tp->tp_proto, otp->tp_pseudoproto, diag);
	}

	if (prot->pl_flag & PL_ELLIPSIS) {
		if (prot->pl_flag & PL_ERRGIVEN) {
			if (pl)
				error("illegal ellipsis terminator");
			else	error("ellipsis terminator in previous (prototype) declaration");
			prot->pl_flag |= PL_ERRGIVEN;
		}
		return 0;
	}
	while (prot) {
				/* if (!(prot->pl_flag & PL_ELLIPSIS)) {} */
		fund = prot->pl_type->tp_fund;
		if (fund == CHAR || fund == SHORT || fund == FLOAT) {
			if (diag && !(prot->pl_flag & PL_ERRGIVEN))
			    error("illegal %s parameter in %sdeclaration",
				symbol2str(fund), (opl ? "previous (prototype) " : "" ));
			prot->pl_flag |= PL_ERRGIVEN;
			retval = 0;
		}
		prot = prot->next;
	}
	return retval;
}

int equal_proto(register struct proto *pl, register struct proto *opl, int diag)
{
	if (pl == opl)
		return 1;

	/*	If only one type is a function type with a parameter type list
		(a function prototype), the composite type is a function
		prototype with parameter type list.
	*/
	while ( pl && opl) {

	    if ((pl->pl_flag & ~PL_ERRGIVEN) != (opl->pl_flag & ~PL_ERRGIVEN) ||
	        !equal_type(pl->pl_type, opl->pl_type, -1, diag))
		return 0;

	    pl = pl->next;
	    opl = opl->next;
	}
	return !(pl || opl);
}

/* check if a type has a consqualified member */
int recurqual(struct type *tp, int qual)
{
	register struct sdef *sdf;

	assert(tp);

	if (tp->tp_typequal & qual) return 1;
	switch(tp->tp_fund) {
	case UNION:
	case STRUCT:
	case ENUM:
		sdf = tp->tp_sdef;
		while (sdf) {
			if (recurqual(sdf->sd_type, qual))
				return 1;
			sdf = sdf->sd_sdef;
		}
		break;
	}
	return 0;
}

void ch3asgn(struct expr **expp, int oper, struct expr *expr)
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
	char *oper_string = symbol2str(oper);

	/* We expect an lvalue */
	if (fund == ARRAY || fund == FUNCTION) exp->ex_lvalue = 0;
	if (!exp->ex_lvalue) {
		expr_error(exp, "no lvalue in operand of %s", oper_string);
	} else if (exp->ex_flags & EX_ILVALUE)	{
		expr_strict(exp, "incorrect lvalue in operand of %s", oper_string);
	} else if (exp->ex_flags & EX_READONLY) {
		expr_error(exp, "operand of %s is read-only", oper_string);
	} else if (fund == STRUCT || fund == UNION) {
		if (recurqual(exp->ex_type, TQ_CONST))
			expr_error(exp,"operand of %s contains a const-qualified member",
					    oper_string);
	}

	if (oper == '=') {
		ch3cast(&expr, oper, exp->ex_type);
		tp = expr->ex_type;
	}
	else {	/* turn e into e' where typeof(e') = typeof (f op e) */
		struct expr *extmp = intexpr((arith)0, INT);

		/* this is really $#@&*%$# ! */
		/* if you correct this, please correct lint_new_oper() too */
		extmp->ex_lvalue = 1;
		extmp->ex_type = exp->ex_type;
		ch3bin(&extmp, oper, expr);
		/* Note that ch3bin creates a tree of the expression
			((typeof (f op e))f op (typeof (f op e))e),
		   where f ~ extmp and e ~ expr.
		   We want to use (typeof (f op e))e.
		   Ch3bin does not create a tree if both operands
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
	exp = new_oper(fund == FIELD ? exp->ex_type->tp_up : exp->ex_type,
		exp, oper, expr);
#else /* NOBITFIELD */
	exp = new_oper(exp->ex_type, exp, oper, expr);
#endif /* NOBITFIELD */
	exp->OP_TYPE = tp;	/* for EVAL() */
	exp->ex_flags |= EX_SIDEEFFECTS;
	*expp = exp;
}

/*	Some interesting (?) questions answered.
*/
int is_integral_type(register struct type *tp)
{
	switch (tp->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case LNGLNG:
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

int is_arith_type(register struct type *tp)
{
	switch (tp->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case LNGLNG:
	case ENUM:
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		return 1;
#ifndef NOBITFIELD
	case FIELD:
		return is_arith_type(tp->tp_up);
#endif /* NOBITFIELD */
	default:
		return 0;
	}
}
