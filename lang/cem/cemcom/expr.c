/* $Header$ */
/* EXPRESSION TREE HANDLING */

#include	"botch_free.h"	/* UF */
#include	"alloc.h"
#include	"idf.h"
#include	"arith.h"
#include	"def.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"decspecs.h"
#include	"declar.h"
#include	"storage.h"
#include	"sizes.h"

extern char *symbol2str();
extern char options[];

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

int
rank_of_expression(expr)
	struct expr *expr;
{
	/*	Returns the rank of the top node in the expression.
	*/
	if (!expr || (expr->ex_flags & EX_PARENS) || expr->ex_class != Oper)
		return 0;
	return rank_of(expr->OP_OPER);
}

check_conditional(expr, oper, pos_descr)
	struct expr *expr;
	char *pos_descr;
{
	/*	Warn if restricted C is in effect and the expression expr,
		which occurs at the position pos_descr, is not lighter than
		the operator oper.
	*/
	if (options['R'] && rank_of_expression(expr) >= rank_of(oper))
		warning("%s %s is ungrammatical",
			symbol2str(expr->OP_OPER), pos_descr);
}

dot2expr(expp)
	struct expr **expp;
{
	/*	The token in dot is converted into an expression, a
		pointer to which is stored in *expp.
	*/
	*expp = new_expr();
	clear((char *)*expp, sizeof(struct expr));
	(*expp)->ex_file = dot.tk_file;
	(*expp)->ex_line = dot.tk_line;
	switch (DOT)	{
	case IDENTIFIER:
		idf2expr(*expp);
		break;
	case STRING:
		string2expr(*expp);
		break;
	case INTEGER:
		*expp = intexpr(dot.tk_ival, dot.tk_fund);
		break;
	case FLOATING:
		float2expr(*expp);
		break;
	default:
		crash("bad conversion to expression");
		break;
	}
}

idf2expr(expr)
	struct expr *expr;
{
	/*	Dot contains an identifier which is turned into an
		expression.
		Note that this constitutes an applied occurrence of
		the identifier.
	*/
	register struct idf *idf = dot.tk_idf;	/* != 0*/
	register struct def *def = idf->id_def;
	
	if (def == 0)	{
		if (AHEAD == '(')	{
			/* Function call, so declare the name IMPLICITly. */
			/* See RM 13. */
			add_def(idf, IMPLICIT, funint_type, level);
		}
		else	{
			if (!is_anon_idf(idf))
				error("%s undefined", idf->id_text);
			/* Declare the idf anyway */
			add_def(idf, 0, error_type, level);
		}
		def = idf->id_def;
	}
	/* now def != 0 */
	if (def->df_type->tp_fund == LABEL) {
		error("illegal use of label %s", idf->id_text);
		expr->ex_type = error_type;
	}
	else {
		def->df_used = 1;
		expr->ex_type = def->df_type;
	}
	expr->ex_lvalue =
		(	def->df_type->tp_fund == FUNCTION ||
			def->df_type->tp_fund == ARRAY ||
			def->df_sc == ENUM
		) ? 0 : 1;
	expr->ex_class = Value;
	if (def->df_sc == ENUM)	{
		expr->VL_IDF = 0;
		expr->VL_VALUE = def->df_address;
	}
	else	{
		expr->VL_IDF = idf;
		expr->VL_VALUE = (arith)0;
	}
}

string2expr(expr)
	struct expr *expr;
{
	/*	Dot contains a string which is turned into an expression.
	*/
	expr->ex_type = string_type;
	expr->ex_lvalue = 0;
	expr->ex_class = String;
	expr->SG_VALUE = dot.tk_str;
	expr->SG_DATLAB = 0;
}

struct expr*
intexpr(ivalue, fund)
	arith ivalue;
{
	/*	The value ivalue is turned into an integer expression of
		the size indicated by fund.
	*/
	struct expr *expr = new_expr();

	clear((char *)expr, sizeof(struct expr));
	expr->ex_file = dot.tk_file;
	expr->ex_line = dot.tk_line;

	switch (fund) {

	case INT:
		expr->ex_type = int_type;
		break;

	case LONG:
		expr->ex_type = long_type;
		break;

	case UNSIGNED:
		/*	We cannot make a test like "ivalue <= max_unsigned"
			because, if sizeof(long) == int_size holds, max_unsigned
			may be a negative long in which case the comparison
			results in an unexpected answer.  We assume that
			the type "unsigned long" is not part of portable C !
		*/
		expr->ex_type = 
			(ivalue & ~max_unsigned) ? long_type : uint_type;
		break;

	case INTEGER:
		expr->ex_type = (ivalue <= max_int) ? int_type : long_type;
		break;

	default:
		crash("(intexpr) bad fund %s\n", symbol2str(fund));
	}
	expr->ex_class = Value;
	expr->VL_VALUE = ivalue;

	cut_size(expr);
	return expr;
}

float2expr(expr)
	struct expr *expr;
{
	/*	Dot contains a floating point constant which is turned
		into an expression.
	*/
	expr->ex_type = double_type;
	expr->ex_class = Float;
	expr->FL_VALUE = dot.tk_fval;
	expr->FL_DATLAB = 0;
}

struct expr *
new_oper(tp, e1, oper, e2)
	struct type *tp;
	struct expr *e1, *e2;
{
	/*	A new expression is constructed which consists of the
		operator oper which has e1 and e2 as operands; for a
		monadic operator e1 == NILEXPR.
		During the construction of the right recursive initialisation
		tree it is possible for e2 to be NILEXPR.
	*/
	struct expr *expr = new_expr();
	struct oper *op;

	clear((char *)expr, sizeof(struct expr));
	if (!e1 || !e2)	{
		expr->ex_file = dot.tk_file;
		expr->ex_line = dot.tk_line;
	}
	else	{
		expr->ex_file = e2->ex_file;
		expr->ex_line = e2->ex_line;
	}
	expr->ex_type = tp;
	expr->ex_class = Oper;
	/* combine depths and flags of both expressions */
	if (e2)	{
		int e1_depth = e1 ? e1->ex_depth : 0;
		int e1_flags = e1 ? e1->ex_flags : 0;
		
		expr->ex_depth =
			(e1_depth > e2->ex_depth ? e1_depth : e2->ex_depth)
				+ 1;
		expr->ex_flags = (e1_flags | e2->ex_flags) & ~EX_PARENS;
	}
	op = &expr->ex_object.ex_oper;
	op->op_type = tp;
	op->op_oper = oper;
	op->op_left = e1;
	op->op_right = e2;

	return expr;
}

chk_cst_expr(expp)
	register struct expr **expp;
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
		
		This routine will give a warning for those operators
		not allowed by K&R, under the R-option only.  The anomalies
		are cast, logical operators and the expression comma.
		Special problems (of which there is only one, sizeof in
		Preprocessor #if) have to be dealt with locally

		Note that according to K&R the negation ! is illegal in
		constant expressions and is indeed rejected by the
		Ritchie compiler.
	*/
	register struct expr *expr = *expp;
	register int fund = expr->ex_type->tp_fund;
	register int flags = expr->ex_flags;
	register int err = 0;
	
#ifdef	DEBUG
	print_expr("constant_expression", expr);
#endif	DEBUG
	if (	fund != CHAR && fund != SHORT && fund != INT &&
		fund != ENUM && fund != LONG
	)	{
		expr_error(expr, "non-numerical constant expression"), err++;
	}
	else
	if (!is_ld_cst(expr))
		expr_error(expr, "expression is not constant"), err++;
	
	if (options['R'])	{
		if (flags & EX_CAST)
			expr_warning(expr,
				"cast in constant expression");
		if (flags & EX_LOGICAL)
			expr_warning(expr,
				"logical operator in constant expression");
		if (flags & EX_COMMA)
			expr_warning(expr,
				"expression comma in constant expression");
	}
	
	if (err) {
		free_expression(expr);
		*expp = intexpr((arith)1, INT);
		(*expp)->ex_type = error_type;
	}
}

init_expression(eppp, expr)
	struct expr ***eppp, *expr;
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

free_expression(expr)
	struct expr *expr;
{
	/*	The expression expr is freed recursively.
	*/
	if (!expr)
		return;
	if (expr->ex_class == Oper)	{
		free_expression(expr->OP_LEFT);
		free_expression(expr->OP_RIGHT);
	}
	free_expr(expr);
}
