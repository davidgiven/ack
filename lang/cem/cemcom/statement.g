/* $Header$ */
/*	STATEMENT SYNTAX PARSER	*/

{
#include	"debug.h"
#include	"botch_free.h"

#include	"arith.h"
#include	"LLlex.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"expr.h"
#include	"code.h"
#include	"storage.h"
#include	"em.h"
#include	"stack.h"
#include	"def.h"

extern int level;
}

/*	Each statement construction is stacked in order to trace a
	statement to such a construction. Example: a case statement should
	be recognized as a piece of the most enclosing switch statement.
*/

/* 9 */
statement
:
[%if (AHEAD != ':')
	expression_statement
|
	label ':' statement
|
	compound_statement((arith *)0)
|
	if_statement
|
	while_statement
|
	do_statement
|
	for_statement
|
	switch_statement
|
	case_statement
|
	default_statement
|
	break_statement
|
	continue_statement
|
	return_statement
|
	jump
|
	';'
|
	asm_statement
]
;

expression_statement
	{	struct expr *expr;
	}
:
	expression(&expr)
	';'
		{
#ifdef	DEBUG
			print_expr("Full expression", expr);
#endif	DEBUG
			code_expr(expr, RVAL, FALSE, NO_LABEL, NO_LABEL);
			free_expression(expr);
		}
;

label
	{	struct idf *idf;
	}
:
	identifier(&idf)
	{
		/*	This allows the following absurd case:

				typedef int grz;
				main()	{
					grz: printf("A labelled statement\n");
				}
		*/
		define_label(idf);
		C_ilb((label)idf->id_def->df_address);
	}
;

if_statement
	{
		struct expr *expr;
		label l_true = text_label();
		label l_false = text_label();
		label l_end = text_label();
	}
:
	IF
	'('
	expression(&expr)
		{
			opnd2test(&expr, NOTEQUAL);
			if (expr->ex_class != Value)	{
				/*	What's happening here? If the
					expression consisted of a constant
					expression, the comparison has
					been optimized to a 0 or 1.
				*/
				code_expr(expr, RVAL, TRUE, l_true, l_false);
				C_ilb(l_true);
			}
			else	{
				if (expr->VL_VALUE == (arith)0)	{
					C_bra(l_false);
				}
			}
			free_expression(expr);
		}
	')'
	statement
	[%prefer
		ELSE
			{
				C_bra(l_end);
				C_ilb(l_false);
			}
		statement
			{	C_ilb(l_end);
			}
	|
		empty
			{	C_ilb(l_false);
			}
	]
;

while_statement
	{
		struct expr *expr;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
	}
:
	WHILE
		{
			stat_stack(l_break, l_continue);
			C_ilb(l_continue);
		}
	'('
	expression(&expr)
		{
			opnd2test(&expr, NOTEQUAL);
			if (expr->ex_class != Value)	{
				code_expr(expr, RVAL, TRUE, l_body, l_break);
				C_ilb(l_body);
			}
			else	{
				if (expr->VL_VALUE == (arith)0)	{
					C_bra(l_break);
				}
			}
		}
	')'
	statement
		{
			C_bra(l_continue);
			C_ilb(l_break);
			stat_unstack();
			free_expression(expr);
		}
;

do_statement
	{	struct expr *expr;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
	}
:
	DO
		{	C_ilb(l_body);
			stat_stack(l_break, l_continue);
		}
	statement
	WHILE
	'('
		{	C_ilb(l_continue);
		}
	expression(&expr)
		{
			opnd2test(&expr, NOTEQUAL);
			if (expr->ex_class != Value)	{
				code_expr(expr, RVAL, TRUE, l_body, l_break);
			}
			else	{
				if (expr->VL_VALUE == (arith)1)	{
					C_bra(l_body);
				}
			}
			C_ilb(l_break);
		}
	')'
	';'
		{
			stat_unstack();
			free_expression(expr);
		}
;

for_statement
	{	struct expr *e_init = 0, *e_test = 0, *e_incr = 0;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
		label l_test = text_label();
	}
:
	FOR
		{	stat_stack(l_break, l_continue);
		}
	'('
	[
		expression(&e_init)
		{	code_expr(e_init, RVAL, FALSE, NO_LABEL, NO_LABEL);
		}
	]?
	';'
		{	C_ilb(l_test);
		}
	[
		expression(&e_test)
		{
			opnd2test(&e_test, NOTEQUAL);
			if (e_test->ex_class != Value)	{
				code_expr(e_test, RVAL, TRUE, l_body, l_break);
				C_ilb(l_body);
			}
			else	{
				if (e_test->VL_VALUE == (arith)0)	{
					C_bra(l_break);
				}
			}
		}
	]?
	';'
	expression(&e_incr)?
	')'
	statement
		{
			C_ilb(l_continue);
			if (e_incr)
				code_expr(e_incr, RVAL, FALSE, NO_LABEL, NO_LABEL);
			C_bra(l_test);
			C_ilb(l_break);
			stat_unstack();
			free_expression(e_init);
			free_expression(e_test);
			free_expression(e_incr);
		}
;

switch_statement
	{
		struct expr *expr;
	}
:
	SWITCH
	'('
	expression(&expr)	/* this must be an integer expression!	*/
		{
			ch7cast(&expr, CAST, int_type);
			code_startswitch(expr);
		}
	')'
	statement
		{
			code_endswitch();
			free_expression(expr);
		}
;

case_statement
	{
		struct expr *expr;
	}
:
	CASE
	constant_expression(&expr)
		{
			code_case(expr->VL_VALUE);
			free_expression(expr);
		}
	':'
	statement
;

default_statement
:
	DEFAULT
		{
			code_default();
		}
	':'
	statement
;

break_statement
:
	BREAK
		{
			if (!do_break())
				error("invalid break");
		}
	';'
;

continue_statement
:
	CONTINUE
		{
			if (!do_continue())
				error("invalid continue");
		}
	';'
;

return_statement
	{	struct expr *expr = 0;
	}
:
	RETURN
	[
		expression(&expr)
		{
			do_return_expr(expr);
			free_expression(expr);
		}
	|
		empty
		{
			C_ret((arith)0);
		}
	]
	';'
;

jump
	{	struct idf *idf;
	}
:
	GOTO
	identifier(&idf)
	';'
		{
			apply_label(idf);
			C_bra((label)idf->id_def->df_address);
		}
;

compound_statement(arith *nbytes;):
	'{'
		{
			stack_level();
		}
	[%while (AHEAD != ':')		/* >>> conflict on TYPE_IDENTIFIER */
		declaration
	]*
	[%persistent
		statement
	]*
	'}'
		{
			if (nbytes)
				*nbytes = (- local_level->sl_max_block);
			unstack_level();
		}
;

asm_statement
	{	char *asm_string;
	}
:
	ASM
	'('
	STRING
		{	asm_string = dot.tk_str;
		}
	')'
	';'
		{	asm_seen(asm_string);
		}
;
