/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	EXPRESSION SYNTAX PARSER	*/

%lexical	LLlex;
%start		If_expr, if_expression;

{
#include	"arith.h"
#include	"LLlex.h"

extern arith ifval;
}

if_expression
{ int is_unsigned = 0; }
:
	constant_expression(&ifval, &is_unsigned)
;

/* 7.1 */
primary(arith *pval; int *is_uns;)
:
	constant(pval, is_uns)
|
	'(' expression(pval, is_uns) ')'
;

unary(arith *pval; int *is_uns;)
	{int oper;}
:
	unop(&oper)
	unary(pval, is_uns)
	{ ch3mon(oper, pval, is_uns); }
|
	primary(pval, is_uns)
;

binary_expression(int maxrank; arith *pval; int *is_uns;)
	{int oper; arith val1; int u;}
:
	unary(pval, is_uns)
	[%while (rank_of(DOT) <= maxrank)
		binop(&oper)
		binary_expression(rank_of(oper)-1, &val1, &u)
		{
			ch3bin(pval, is_uns, oper, val1, u);
		}
	]*
;

/* 7.13 */
conditional_expression(arith *pval; int *is_uns)
	{arith val1 = 0, val2 = 0; int u;}
:
	/* allow all binary operators */
	binary_expression(rank_of('?') - 1, pval, is_uns)
	[	'?'
		expression(&val1, is_uns)
		':'
		assignment_expression(&val2, &u)
		{ if (*pval) *pval = val1;
		  else { *pval = val2; *is_uns = u; }
		}
	]?
;

/* 7.14 */
assignment_expression(arith *pval; int *is_uns)
:
	conditional_expression(pval, is_uns)
;

/* 7.15 */
expression(arith *pval; int *is_uns)
	{arith val1;
	 int is_uns1;
	}
:
	assignment_expression(pval,is_uns)
	[	','
		assignment_expression(&val1, &is_uns1)
		{
			ch3bin(pval, is_uns, ',', val1, is_uns1);
		}
	]*
;

unop(int *oper;) :
	[ '-' | '!' | '~' ]
	{*oper = DOT;}
;

multop:
	'*' | '/' | '%'
;

addop:
	'+' | '-'
;

shiftop:
	LEFT | RIGHT
;

relop:
	'<' | '>' | LESSEQ | GREATEREQ
;

eqop:
	EQUAL | NOTEQUAL
;

arithop:
	multop | addop | shiftop
|
	'&' | '^' | '|'
;

binop(int *oper;) :
	[ arithop | relop | eqop | AND | OR ]
	{*oper = DOT;}
;

constant(arith *pval; int *is_uns) :
	INTEGER
	{*pval = dot.tk_val;
	 *is_uns = dot.tk_unsigned;
	}
;

constant_expression (arith *pval; int *is_uns) :
	assignment_expression(pval, is_uns)
;
