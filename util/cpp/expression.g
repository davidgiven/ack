/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	EXPRESSION SYNTAX PARSER	*/

%lexical	LLlex;
%start		If_expr, if_expression;

{
#include	"LLlex.h"
#include	<em_arith.h>

extern arith ifval;
}

if_expression
:
	constant_expression(&ifval)
;

/* 7.1 */
primary(arith *pval;)
:
	constant(pval)
|
	'(' expression(pval) ')'
;

unary(arith *pval;)
	{int oper;}
:
	unop(&oper)
	unary(pval)
	{ ch7mon(oper, pval); }
|
	primary(pval)
;

binary_expression(int maxrank; arith *pval;)
	{int oper; arith val1;}
:
	unary(pval)
	[%while (rank_of(DOT) <= maxrank)
		binop(&oper)
		binary_expression(rank_of(oper)-1, &val1)
		{
			ch7bin(pval, oper, val1);
		}
	]*
;

/* 7.13 */
conditional_expression(arith *pval;)
	{arith val1 = 0, val2 = 0;}
:
	/* allow all binary operators */
	binary_expression(rank_of('?') - 1, pval)
	[	'?'
		expression(&val1)
		':'
		assignment_expression(&val2)
		{ *pval = (*pval ? val1 : val2); }
	]?
;

/* 7.14 */
assignment_expression(arith *pval;)
:
	conditional_expression(pval)
;

/* 7.15 */
expression(arith *pval;)
	{arith val1;}
:
	assignment_expression(pval)
	[	','
		assignment_expression(&val1)
		{
			ch7bin(pval, ',', val1);
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

constant(arith *pval;) :
	INTEGER
	{*pval = dot.tk_val;}
;

constant_expression (arith *pval;) :
	assignment_expression(pval)
;
