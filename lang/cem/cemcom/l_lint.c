/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	Lint main routines	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
#include	"arith.h"	/* definition arith */
#include	"label.h"	/* definition label */
#include	"expr.h"
#include	"idf.h"
#include	"def.h"
#include	"code.h"	/* RVAL etc */
#include	"LLlex.h"
#include	"Lpars.h"
#include	"stack.h"
#include	"type.h"
#include	"level.h"
#include	"nofloat.h"
#include	"l_lint.h"
#include	"l_state.h"
#include	"l_outdef.h"

extern char options[128];
extern char *symbol2str();

static struct expr_state *lint_expr();
static struct expr_state *lint_value();
static struct expr_state *lint_oper();

lint_init()
{
/* Allocate some memory for the global stack_bottom
 * and some other initializations
 */

	extern struct lint_stack_entry stack_bottom;

	stack_bottom.ls_current = new_state();
}

pre_lint_expr(expr, val, used)
	struct expr *expr;
	int val;			/* LVAL or RVAL */
	int used;
{
/* Introduced to dispose the returned expression states */

	register struct expr_state *esp;

	esp = lint_expr(expr, val, used);
	free_expr_states(esp);
}

free_expr_states(esp)
	register struct expr_state *esp;
{
	register struct expr_state *esp2;

	while (esp) {
		esp2 = esp;
		esp = esp->next;
		free_expr_state(esp2);
	}
}

static struct expr_state *
lint_expr(expr, val, used)
	register struct expr *expr;
	int val;
	int used;
{
/* Main function to process an expression tree.
 * It returns a structure containing information about which variables
 * are set and which are used in the expression.
 * In addition it sets 'used' and 'set' fields of the corresponding
 * variables in the current state.
 * If the value of an operation without side-effects is not used,
 * a warning is given.
 */
	if (used == IGNORED) {
		expr_ignored(expr);
	}

	switch (expr->ex_class) {
	case Value:
		return lint_value(expr, val);

	case Oper:
		return lint_oper(expr, val, used);

	default:			/* String Float Type */
		return 0;
	}
}

static struct expr_state *
lint_value(expr, val)
	register struct expr *expr;
{
	switch (expr->VL_CLASS) {
	case Const:
	case Label:
		return 0;

	case Name:
	{
		register struct idf *idf = expr->VL_IDF;
		struct expr_state *esp1 = 0;

		if (!idf || !idf->id_def)
			return 0;

		if (	val == LVAL
		||	(	val == RVAL
			&&	expr->ex_type->tp_fund == POINTER
			&&	!expr->ex_lvalue
			)
		) {
			change_state(idf, SET);
			idf->id_def->df_file =
				Salloc(dot.tk_file,
					strlen(dot.tk_file) + 1);
			idf->id_def->df_line = dot.tk_line;
		}
		if (val == RVAL) {
			change_state(idf, USED);
			add_expr_state(expr->EX_VALUE, USED, &esp1);
		}
		return esp1;
	}

	default:
		crash("(lint_expr) bad value class\n");
		/* NOTREACHED */
	}
}

static struct expr_state *
lint_oper(expr, val, used)
	struct expr *expr;
	int val;
	int used;
{
	register int oper = expr->OP_OPER;
	register struct expr *left = expr->OP_LEFT;
	register struct expr *right = expr->OP_RIGHT;
	struct expr_state *esp1 = 0;
	struct expr_state *esp2 = 0;

	switch (oper) {
	case '=':
	case PLUSAB:
	case MINAB:
	case TIMESAB:
	case DIVAB:
	case MODAB:
	case LEFTAB:
	case RIGHTAB:
	case ANDAB:
	case XORAB:
	case ORAB:
		lint_conversion(oper, right->ex_type, left->ex_type);
		/* for cases like i += l; */
		esp1 = lint_expr(right, RVAL, USED);
		if (oper != '=') {
			/* i += 1; is interpreted as i = i + 1; */
			esp2 = lint_expr(left, RVAL, USED);
			check_and_merge(&esp1, esp2, oper);
		}
		esp2 = lint_expr(left, LVAL, USED);
		/* for cases like i = i + 1; and i not set, this
		** order is essential
		*/
		check_and_merge(&esp1, esp2, oper);
		if (	left->ex_class == Value
		&&	left->VL_CLASS == Name
		) {
			add_expr_state(left->EX_VALUE, SET, &esp1);
		}
		return esp1;

	case POSTINCR:
	case POSTDECR:
	case PLUSPLUS:
	case MINMIN:
		/* i++; is parsed as i = i + 1;
		 * This isn't quite correct :
		 * The first statement doesn't USE i,
		 * the second does.
		 */
		esp1 = lint_expr(left, RVAL, USED);
		esp2 = lint_expr(left, LVAL, USED);
		check_and_merge(&esp1, esp2, oper);
		if (	left->ex_class == Value
		&&	left->VL_CLASS == Name
		) {
			add_expr_state(left->EX_VALUE, SET, &esp1);
			add_expr_state(left->EX_VALUE, USED, &esp1);
		}
		return esp1;

	case '-':
	case '*':
		if (left == 0)	/* unary */
			return lint_expr(right, RVAL, USED);
		esp1 = lint_expr(left, RVAL, USED);
		esp2 = lint_expr(right, RVAL, USED);
		check_and_merge(&esp1, esp2, oper);
		return esp1;

	case '(':
		if (right != 0) {
			/* function call with parameters */
			register struct expr *ex = right;

			while (	ex->ex_class == Oper
			&&	ex->OP_OPER == PARCOMMA
			) {
				esp2 = lint_expr(ex->OP_RIGHT, RVAL,
						 USED);
				check_and_merge(&esp1, esp2, oper);
				ex = ex->OP_LEFT;
			}
			esp2 = lint_expr(ex, RVAL, USED);
			check_and_merge(&esp1, esp2, oper);
		}
		if (	left->ex_class == Value
		&&	left->VL_CLASS == Name
		) {
			fill_outcall(expr,
				expr->ex_type->tp_fund == VOID ?
				VOIDED : used
			);
			outcall();
			left->VL_IDF->id_def->df_used = 1;
		}
		else {
			esp2 = lint_expr(left, val, USED);
			check_and_merge(&esp1, esp2, oper);
		}
		return esp1;

	case '.':
		return lint_expr(left, val, USED);

	case ARROW:
		return lint_expr(left, RVAL, USED);

	case '~':
	case '!':
		return lint_expr(right, RVAL, USED);

	case '?':
		esp1 = lint_expr(left, RVAL, USED);
		esp2 = lint_expr(right->OP_LEFT, RVAL, USED);
		check_and_merge(&esp1, esp2, 0);
		esp2 = lint_expr(right->OP_RIGHT, RVAL, USED);
		check_and_merge(&esp1, esp2, 0);
		return esp1;

	case INT2INT:
	case INT2FLOAT:
	case FLOAT2INT:
	case FLOAT2FLOAT:
		lint_conversion(oper, right->ex_type, left->ex_type);
		return lint_expr(right, RVAL, USED);

	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
	case EQUAL:
	case NOTEQUAL:
		lint_relop(left, right, oper);
		lint_relop(right, left, 
			oper == '<' ? '>' :
			oper == '>' ? '<' :
			oper == LESSEQ ? GREATEREQ :
			oper == GREATEREQ ? LESSEQ :
			oper
		);
		/*FALLTHROUGH*/
	case '+':
	case '/':
	case '%':
	case ',':
	case LEFT:
	case RIGHT:
	case '&':
	case '|':
	case '^':
	case OR:
	case AND:
		esp1 = lint_expr(left, RVAL,
					oper == ',' ? IGNORED : USED);
		esp2 = lint_expr(right, RVAL,
					oper == ',' ? used : USED);
		if (oper == OR || oper == AND || oper == ',')
			check_and_merge(&esp1, esp2, 0);
		else
			check_and_merge(&esp1, esp2, oper);
		return esp1;

	default:
		return 0;	/* for initcomma */
	}
}

expr_ignored(expr)
	struct expr *expr;
{
	switch (expr->ex_class) {
	case Oper:
		switch (expr->OP_OPER) {
		case '=':
		case TIMESAB:
		case DIVAB:
		case MODAB:
		case LEFTAB:
		case RIGHTAB:
		case ANDAB:
		case XORAB:
		case ORAB:
		case AND:			/* doubtful but useful */
		case OR:			/* doubtful but useful */
		case '(':
		case '?':
		case ',':
			break;

		case PLUSAB:
		case MINAB:
		case POSTINCR:
		case POSTDECR:
		case PLUSPLUS:
		case MINMIN:
			/* may hide the operator * */
			if (	/* operation on a pointer */
				expr->OP_TYPE->tp_fund == POINTER
			&&	/* the result is dereferenced, e.g. *p++; */
				expr->ex_type == expr->OP_TYPE->tp_up
			) {
				hwarning("result of * ignored");
			}
			break;

		default:
			hwarning("result of %s ignored",
						symbol2str(expr->OP_OPER));
			break;
		}
		break;

	case Value:
		hwarning("identifier as statement");
		break;

	default:			/* String Float */
		hwarning("constant as statement");
		break;
	}
}

#endif	LINT
