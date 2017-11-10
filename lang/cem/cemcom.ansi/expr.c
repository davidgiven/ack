/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* EXPRESSION TREE HANDLING */

#include	<assert.h>
#include    <stdlib.h>
#include	"parameters.h"
#include	<alloc.h>
#include	<flt_arith.h>
#include    "idf.h"
#include	"arith.h"
#include	"def.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"decspecs.h"
#include	"declar.h"
#include	"sizes.h"
#include	"level.h"

extern char *symbol2str();
extern char options[];
extern int InSizeof;

int
rank_of(oper)
	int oper;
{
	/*	The rank of the operator oper is returned.
	*/
	switch (oper)	{
	default:
		return 0;			/* INT2INT etc. */
	case '[':
	case '(':
	case '.':
	case ARROW:
	case PARCOMMA:
		return 1;
	case '!':
	case PLUSPLUS:
	case MINMIN:
	case CAST:
	case SIZEOF:
	case ADDRESSOF:
		return 2;			/* monadic */
	case '*':
	case '/':
	case '%':
		return 3;
	case '+':
	case '-':
		return 4;
	case LEFT:
	case RIGHT:
		return 5;
	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
		return 6;
	case EQUAL:
	case NOTEQUAL:
		return 7;
	case '&':
		return 8;
	case '^':
		return 9;
	case '|':
		return 10;
	case AND:
		return 11;
	case OR:
		return 12;
	case '?':
	case ':':
		return 13;
	case '=':
	case PLUSAB:
	case MINAB:
	case TIMESAB:
	case DIVAB:
	case MODAB:
	case RIGHTAB:
	case LEFTAB:
	case ANDAB:
	case XORAB:
	case ORAB:
		return 14;
	case ',':
		return 15;
	}
	/*NOTREACHED*/
}

dot2expr(expp)
	struct expr **expp;
{
	/*	The token in dot is converted into an expression, a
		pointer to which is stored in *expp.
	*/
	register struct expr *ex = new_expr();

	*expp = ex;
	ex->ex_file = dot.tk_file;
	ex->ex_line = dot.tk_line;
	switch (DOT)	{
	case IDENTIFIER:
		idf2expr(ex);
		break;
	case INTEGER:
		int2expr(ex);
		break;
	case FLOATING:
		float2expr(ex);
		break;
	default:
		crash("bad conversion to expression");
		/*NOTREACHED*/
	}
}

idf2expr(expr)
	register struct expr *expr;
{
	/*	Dot contains an identifier which is turned into an
		expression.
		Note that this constitutes an applied occurrence of
		the identifier.
	*/
	register struct idf *idf = dot.tk_idf;	/* != 0*/
	register struct def *def = idf->id_def;
	
	if (def == 0)	{
		if (AHEAD == '(') {
			/* function call, declare name implicitly (3.3.2.2) */
			if (!options['o'])
				warning("implicit declaration of function %s"
					, idf->id_text);
			add_def(idf, EXTERN, funint_type, level);
		} else	{
			if (!is_anon_idf(idf))
				error("%s undefined", idf->id_text);
			/* declare idf anyway */
			add_def(idf, 0, error_type, level);
		}
		def = idf->id_def;
	}
	/* now def != 0 */
#ifndef	LINT
	if (!InSizeof) {
		if (! def->df_used) {
#ifndef PREPEND_SCOPES
			code_scope(idf->id_text, def);
#endif /* PREPEND_SCOPES */
			def->df_used = 1;
		}
	}
#endif	/* LINT */
	expr->ex_type = def->df_type;
	if (expr->ex_type == error_type) {
		expr->ex_flags |= EX_ERROR;
	}
	expr->ex_lvalue =
		(	def->df_type->tp_fund == FUNCTION ||
			def->df_type->tp_fund == ARRAY ||
			def->df_sc == ENUM
		) ? 0 : 1;
	if (def->df_type->tp_typequal & TQ_CONST)
		expr->ex_flags |= EX_READONLY;
	if (def->df_type->tp_typequal & TQ_VOLATILE)
		expr->ex_flags |= EX_VOLATILE;
	expr->ex_class = Value;
	if (def->df_sc == ENUM)	{
		expr->VL_CLASS = Const;
		expr->VL_VALUE = def->df_address;
	}
#ifndef	LINT
	else
	if (def->df_sc == STATIC && def->df_level >= L_LOCAL) {
		expr->VL_CLASS = Label;
		expr->VL_LBL = def->df_address;
		expr->VL_VALUE = (arith)0;
	}
#endif	/* LINT */
	else {
		expr->VL_CLASS = Name;
		expr->VL_IDF = idf;
		expr->VL_VALUE = (arith)0;
	}
}

string2expr(expp, str, len)
	register struct expr **expp;
	int len;
	char *str;
{
	/*	The string in the argument is converted into an expression,
		a pointer to which is stored in *expp.
	*/
	register struct expr *ex = new_expr();

	*expp = ex;
	ex->ex_file = dot.tk_file;
	ex->ex_line = dot.tk_line;
	ex->ex_type = string_type;
	/* ex->ex_type = qualifier_type(ex->ex_type, TQ_CONST); */
	ex->ex_flags |= EX_READONLY;
	/* ex->ex_lvalue = 0; */
	ex->ex_class = String;
	ex->SG_VALUE = str;
	ex->SG_LEN = len;
}

int2expr(expr)
	struct expr *expr;
{
	/*	Dot contains an integer constant which is turned
		into an expression.
	*/
	fill_int_expr(expr, dot.tk_ival, dot.tk_fund);
}

float2expr(expr)
	register struct expr *expr;
{
	/*	Dot contains a floating point constant which is turned
		into an expression.
	*/
	register int fund;

	fund = dot.tk_fund;
	switch (fund) {
	case FLOAT:
		expr->ex_type = float_type;
		break;
	case DOUBLE:
		expr->ex_type = double_type;
		break;
	case LNGDBL:
		expr->ex_type = lngdbl_type;
		break;
	default:
		crash("(float2expr) bad fund %s\n", symbol2str(fund));
	}
	expr->ex_class = Float;
	flt_str2flt(dot.tk_fval, &(expr->FL_ARITH));
	free(dot.tk_fval);
	assert(flt_status != FLT_NOFLT);
	if (flt_status == FLT_OVFL)
		expr_warning(expr,"internal floating point overflow");
}

struct expr*
intexpr(ivalue, fund)
	arith ivalue;
	int fund;
{
	/*	The value ivalue is turned into an integer expression of
		the size indicated by fund.
	*/
	register struct expr *expr = new_expr();

	expr->ex_file = dot.tk_file;
	expr->ex_line = dot.tk_line;
	fill_int_expr(expr, ivalue, fund);
	return expr;
}

fill_int_expr(ex, ivalue, fund)
	register struct expr *ex;
	arith ivalue;
	int fund;
{
	/*	Details derived from ivalue and fund are put into the
		constant integer expression ex.
	*/
	switch (fund) {
	case INT:
		ex->ex_type = int_type;
		break;
	case UNSIGNED:
		ex->ex_type = uint_type;
		break;
	case LONG:
		ex->ex_type =  long_type;
		break;
	case ULONG:
		ex->ex_type = ulong_type;
		break;
	default:
		crash("(fill_int_expr) bad fund %s\n", symbol2str(fund));
		/*NOTREACHED*/
	}
	ex->ex_class = Value;
	ex->VL_CLASS = Const;
	ex->VL_VALUE = ivalue;
	cut_size(ex);
}

struct expr *
new_oper(tp, e1, oper, e2)
	struct type *tp;
	register struct expr *e1, *e2;
{
	/*	A new expression is constructed which consists of the
		operator oper which has e1 and e2 as operands; for a
		monadic operator e1 == NILEXPR.
		During the construction of the right recursive initialisation
		tree it is possible for e2 to be NILEXPR.
	*/
	register struct expr *expr = new_expr();
	register struct oper *op;

	if (e2)	{
		register struct expr *e = e2;
		
		while (e->ex_class == Oper && e->OP_LEFT)
			e = e->OP_LEFT;
		expr->ex_file = e->ex_file;
		expr->ex_line = e->ex_line;
	}
	else
	if (e1)	{
		register struct expr *e = e1;
		
		while (e->ex_class == Oper && e->OP_RIGHT)
			e = e->OP_RIGHT;
		expr->ex_file = e->ex_file;
		expr->ex_line = e->ex_line;
	}
	else	{
		expr->ex_file = dot.tk_file;
		expr->ex_line = dot.tk_line;
	}

	expr->ex_type = tp;
	expr->ex_class = Oper;
	/* combine depths and flags of both expressions */
	if (e2)	{
		int e1_depth = e1 ? e1->ex_depth : 0;
		int e1_flags = e1 ? e1->ex_flags : 0;
		
		expr->ex_depth =
			(e1_depth > e2->ex_depth ? e1_depth : e2->ex_depth) + 1;
		expr->ex_flags = (e1_flags | e2->ex_flags)
			& ~(EX_PARENS | EX_READONLY | EX_VOLATILE );
	}
	/*
	 * A function call should be evaluated first when possible.  Just say
	 * that the expression tree is very deep.
	 */
	if (oper == '(') {
		expr->ex_depth = 50;
	}
	op = &expr->ex_object.ex_oper;
	op->op_type = tp;
	op->op_oper = oper;
	op->op_left = e1;
	op->op_right = e2;
#ifdef	LINT
	lint_new_oper(expr);
#endif	/* LINT */
	return expr;
}

void
chk_cst_expr(expp)
	struct expr **expp;
{
	/*	The expression expr is checked for constancy.
	
		There are 6 places where constant expressions occur in C:
		1.	after #if
		2.	in a global initialization
		3.	as size in an array declaration
		4.	as value in an enum declaration
		5.	as width in a bit field
		6.	as case value in a switch
		
		The constant expression in a global initialization is
		handled separately (by IVAL()).
		
		There are various disparate restrictions on each of
		the others in the various C compilers.  I have tried some
		hypotheses to unify them, but all have failed.
		
		Special problems (of which there is only one, sizeof in
		Preprocessor #if) have to be dealt with locally
	*/
	register struct expr *expr = *expp;
	
#ifdef	DEBUG
	print_expr("constant_expression", expr);
#endif	/* DEBUG */
	switch(expr->ex_type->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case ENUM:
	case LONG:
		if (is_ld_cst(expr)) {
			return;
		}
		expr_error(expr, "expression is not constant");
		break;
	default:
		expr_error(expr, "non-numerical constant expression");
		break;
	}
	erroneous2int(expp);
}

init_expression(eppp, expr)
	register struct expr ***eppp;
	struct expr *expr;
{
	/*	The expression expr is added to the tree designated
		indirectly by **eppp.
		The natural form of a tree representing an
		initial_value_list is right-recursive, ie. with the
		left-most comma as main operator. The iterative grammar in
		expression.g, however, tends to produce a left-recursive
		tree, ie. one with the right-most comma as its main
		operator.
		To produce a right-recursive tree from the iterative
		grammar, we keep track of the address of the pointer where
		the next expression must be hooked in.
	*/
	**eppp = new_oper(void_type, expr, INITCOMMA, NILEXPR);
	*eppp = &(**eppp)->OP_RIGHT;
}

int
is_ld_cst(expr)
	register struct expr *expr;
{
	/*	An expression is a `load-time constant' if it is of the form
		<idf> +/- <integral> or <integral>.
	*/
#ifdef	LINT
	if (expr->ex_class == String)
		return 1;
#endif	/* LINT */
	return expr->ex_lvalue == 0 && expr->ex_class == Value;
}

int
is_cp_cst(expr)
	struct expr *expr;
{
	/*	An expression is a `compile-time constant' if it is a
		load-time constant, and the idf is not there.
	*/
	return is_ld_cst(expr) && expr->VL_CLASS == Const;
}

int
is_fp_cst(expr)
	struct expr *expr;
{
	/*	An expression is a `floating-point constant' if it consists
		of the float only.
	*/
	return expr->ex_class == Float;
}

int
is_zero_cst(expr)
	register struct expr *expr;
{
	flt_arith var;

	switch(expr->ex_class) {
	case Value:
		return expr->VL_VALUE == 0;
	case Float:
		flt_arith2flt((arith) 0, &var, 0);
		return flt_cmp(&var, &(expr->FL_ARITH)) == 0;
	}
	/*NOTREACHED*/
}

free_expression(expr)
	register struct expr *expr;
{
	/*	The expression expr is freed recursively.
	*/
	if (expr) {
		if (expr->ex_class == Oper)	{
			free_expression(expr->OP_LEFT);
			free_expression(expr->OP_RIGHT);
		}
		free_expr(expr);
	}
}
