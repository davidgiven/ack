/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint main routines	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
#include	"debug.h"
#include	"interface.h"
#include	"assert.h"
#ifdef ANSI
#include	<flt_arith.h>
#endif /* ANSI */
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
#include	"l_lint.h"
#include	"l_state.h"
#include	"l_outdef.h"

extern char options[128];
extern char *symbol2str();

PRIVATE struct expr_state *expr2state();
PRIVATE struct expr_state *value2state();
PRIVATE struct expr_state *oper2state();
PRIVATE expr_ignored();
PRIVATE add_expr_state();
PRIVATE referred_esp();
PRIVATE free_expr_states();

lint_init()
{
	lint_init_comment();
	lint_init_stack();
}

lint_expr(expr, used)
	struct expr *expr;
	int used;			/* USED or IGNORED */
{
	register struct expr_state *esp;

	esp = expr2state(expr, RVAL, used);
	referred_esp(esp);
	free_expr_states(esp);
}

PRIVATE struct expr_state *
expr2state(expr, val, used)
	register struct expr *expr;
	int val;			/* RVAL or LVAL */
	int used;			/* USED or IGNORED */
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
		return value2state(expr, val);

	case Oper:
		return oper2state(expr, val, used);

	default:			/* String, Float, Type */
		return 0;
	}
}

PRIVATE struct expr_state *
value2state(expr, val)
	struct expr *expr;
	int val;			/* RVAL or LVAL */
{
	switch (expr->VL_CLASS) {
	case Const:
	case Label:
		return 0;

	case Name:
	{
		register struct idf *idf = expr->VL_IDF;
		struct expr_state *esp = 0;

		if (!idf || !idf->id_def)
			return 0;

		if (val == RVAL && expr->ex_lvalue == 1) {
			/* value of identifier used */
			change_state(idf, USED);
			add_expr_state(expr->EX_VALUE, USED, &esp);
		}
		if (val == RVAL && expr->ex_lvalue == 0) {
			/* address of identifier used */
			add_expr_state(expr->EX_VALUE, REFERRED, &esp);
		}
		return esp;
	}

	default:
		NOTREACHED();
		/* NOTREACHED */
	}
}

/*	Let's get this straight.
	An assignment is performed by elaborating the LHS and the RHS
	collaterally, to use the A68 terminology, and then serially do the
	actual assignment. This means:
	1.	evaluate the LHS as an LVAL,
	2.	evaluate the RHS as an RVAL,
	3.	merge them checking for interference,
	4.	set the result of the LHS to SET, if it is a named variable
*/

PRIVATE struct expr_state *
oper2state(expr, val, used)
	struct expr *expr;
	int val;			/* RVAL or LVAL */
	int used;			/* USED or IGNORED */
{
	register int oper = expr->OP_OPER;
	register struct expr *left = expr->OP_LEFT;
	register struct expr *right = expr->OP_RIGHT;
	struct expr_state *esp_l = 0;
	struct expr_state *esp_r = 0;

	switch (oper) {

	/* assignments */
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
		/* evaluate the LHS, only once; see RM 7.14 */
		esp_l = expr2state(left, (oper == '=' ? LVAL : RVAL), USED);

		/* evaluate the RHS as an RVAL and merge */
		esp_r = expr2state(right, RVAL, USED);
		check_and_merge(expr, &esp_l, esp_r);

		/* set resulting variable, if any */
		if (ISNAME(left)) {
			change_state(left->VL_IDF, SET);
			add_expr_state(left->EX_VALUE, SET, &esp_l);
		}

		return esp_l;

	case POSTINCR:
	case POSTDECR:
	case PLUSPLUS:
	case MINMIN:
		esp_l = expr2state(left, RVAL, USED);

		/* set resulting variable, if any */
		if (ISNAME(left)) {
			change_state(left->VL_IDF, SET);
			add_expr_state(left->EX_VALUE, SET, &esp_l);
		}

		return esp_l;

	case '?':
		esp_l = expr2state(left, RVAL, USED);
		esp_r = expr2state(right->OP_LEFT, RVAL, USED);
		check_and_merge(expr, &esp_l, esp_r);
		esp_r = expr2state(right->OP_RIGHT, RVAL, USED);
		check_and_merge(expr, &esp_l, esp_r);
		return esp_l;

	case '(':
		if (right != 0) {
			/* function call with parameters */
			register struct expr *ex = right;

			while (	ex->ex_class == Oper
			&&	ex->OP_OPER == PARCOMMA
			) {
				esp_r = expr2state(ex->OP_RIGHT, RVAL, USED);
				check_and_merge(expr, &esp_l, esp_r);
				ex = ex->OP_LEFT;
			}
			esp_r = expr2state(ex, RVAL, USED);
			check_and_merge(expr, &esp_l, esp_r);
		}

		if (ISNAME(left)) {
			fill_outcall(expr,
				expr->ex_type->tp_fund == VOID ?
				VOIDED : used
			);
			outcall();
			left->VL_IDF->id_def->df_used = 1;
		}
		else {
			esp_r = expr2state(left, RVAL, USED);
			check_and_merge(expr, &esp_l, esp_r);
		}
		referred_esp(esp_l);
		return esp_l;

	case '.':
		return expr2state(left, val, USED);

	case ARROW:
		return expr2state(left, RVAL, USED);

	case INT2INT:
	case INT2FLOAT:
	case FLOAT2INT:
	case FLOAT2FLOAT:
		return expr2state(right, RVAL, USED);

	/* monadic operators */
	case '-':
	case '*':
		if (left)
			goto dyadic;
	case '~':
	case '!':
		return expr2state(right, RVAL, USED);

	/* relational operators */
	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
	case EQUAL:
	case NOTEQUAL:
		goto dyadic;

	/* dyadic operators */
	dyadic:
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
		esp_l = expr2state(left, RVAL,
					oper == ',' ? IGNORED : USED);
		esp_r = expr2state(right, RVAL,
					oper == ',' ? used : USED);
		check_and_merge(expr, &esp_l, esp_r);

		return esp_l;

	default:
		return 0;	/* for initcomma */
	}
}

PRIVATE
expr_ignored(expr)
	struct expr *expr;
{
	switch (expr->ex_class) {
		int oper;
	case Oper:
		oper = expr->OP_OPER;
		switch (oper) {
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
			oper = 0;		/* ignore the ignoring */
			break;

		case PLUSAB:
		case MINAB:
		case POSTINCR:
		case POSTDECR:
		case PLUSPLUS:
		case MINMIN:
			oper = 0;		/* ignore in priciple */
			/* may, however, hide the operator '*' */
			if (	/* operation on a pointer */
				expr->OP_TYPE->tp_fund == POINTER
			&&	/* the result is dereferenced, e.g. *p++; */
				expr->ex_type == expr->OP_TYPE->tp_up
			) {
				oper = '*';
			}
			break;

		case '/':
			/*	this is a specially weird case: the '/' may
				result from pointer subtraction
			*/
			if (	expr->OP_TYPE->tp_fund == INT
			&&	expr->OP_LEFT->OP_OPER == '-'
			&&	expr->OP_LEFT->OP_TYPE->tp_fund == POINTER
			) {
				oper = '-';
			}
			break;
		}
		if (oper) {
			hwarning("result of %s ignored", symbol2str(oper));
		}
		break;

	case Value:
		if (expr->VL_CLASS == Const) {
			hwarning("constant expression ignored");
		}
		else {
			hwarning("value ignored");
		}
		break;

	default:			/* String Float */
		hwarning("constant ignored");
		break;
	}
}

PRIVATE
add_expr_state(value, to_state, espp)
	struct value value;
	struct expr_state **espp;
{
	register struct expr_state *esp = *espp;

	ASSERT(value.vl_class == Name);

	/* try to find the esp */
	while (	esp
	&&	!(	esp->es_idf == value.vl_data.vl_idf
		&&	esp->es_offset == value.vl_value
		)
	) {
		esp = esp->next;
	}

	/* if not found, add it */
	if (!esp) {
		esp = new_expr_state();
		esp->es_idf = value.vl_data.vl_idf;
		esp->es_offset = value.vl_value;
		esp->next = *espp;
		*espp = esp;
	}

	/* set state */
	switch (to_state) {
	case USED:
		esp->es_used = 1;
		break;
	case REFERRED:
		esp->es_referred = 1;
		break;
	case SET:
		esp->es_set = 1;
		break;
	default:
		NOTREACHED();
		/* NOTREACHED */
	}
}

PRIVATE
referred_esp(esp)
	struct expr_state *esp;
{
	/* raises all REFERRED items to SET and USED status */
	while (esp) {
		if (esp->es_referred) {
			esp->es_set = 1;
			change_state(esp->es_idf, SET);
			esp->es_used = 1;
			change_state(esp->es_idf, USED);
			esp->es_referred = 0;
		}
		esp = esp->next;
	}
}

PRIVATE
free_expr_states(esp)
	register struct expr_state *esp;
{
	while (esp) {
		register struct expr_state *esp2 = esp;

		esp = esp->next;
		free_expr_state(esp2);
	}
}

#ifdef	DEBUG
print_esp(msg, esp)
	char *msg;
	struct expr_state *esp;
{
	print("%s: <", msg);
	while (esp) {
		print(" %s[%d]%c%c%c ",
			esp->es_idf->id_text, esp->es_offset,
			(esp->es_used ? 'U' : ' '),
			(esp->es_referred ? 'R' : ' '),
			(esp->es_set ? 'S' : ' ')
		);
		esp = esp->next;
	}
	print(">\n");
}
#endif	/* DEBUG */

#endif	/* LINT */
