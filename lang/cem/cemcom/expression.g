/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	EXPRESSION SYNTAX PARSER	*/

{
#include	"arith.h"
#include	"LLlex.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"expr.h"
#include	"code.h"
#include	"noRoption.h"

extern char options[];
extern struct expr *intexpr();
}

/* 7.1 */
primary(register struct expr **expp;) :
	IDENTIFIER
	{dot2expr(expp);}
|
	constant(expp)
|
	STRING
	{dot2expr(expp);}
|
	'(' expression(expp) ')'
	{(*expp)->ex_flags |= EX_PARENS;}
;

secundary(register struct expr **expp;) :
	primary(expp)
	[
		index_pack(expp)
	|
		parameter_pack(expp)
	|
		selection(expp)
	]*
;

index_pack(struct expr **expp;)
	{struct expr *e1;}
:
	'[' expression(&e1) ']'
	{ch7bin(expp, '[', e1);}
;

parameter_pack(struct expr **expp;)
	{struct expr *e1 = 0;}
:
	'(' parameter_list(&e1)? ')'
	{ch7bin(expp, '(', e1);}
;

selection(struct expr **expp;)
	{int oper; struct idf *idf;}
:
	[ '.' | ARROW ]
	{oper = DOT;}
	identifier(&idf)
	{ch7sel(expp, oper, idf);}
;

parameter_list(struct expr **expp;)
	{struct expr *e1 = 0;}
:
	assignment_expression(expp)
	{any2opnd(expp, PARCOMMA);}
	[ %persistent
		','
		assignment_expression(&e1)
		{any2opnd(&e1, PARCOMMA);}
		{ch7bin(expp, PARCOMMA, e1);}
	]*
;

/* 7.2 */
postfixed(struct expr **expp;)
	{int oper;}
:
	secundary(expp)
	[
		postop(&oper)
		{ch7incr(expp, oper);}
	|
		empty
	]
;

%first	first_of_type_specifier, type_specifier;

unary(register struct expr **expp;)
	{struct type *tp; int oper;}
:
%if (first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)
	cast(&tp) unary(expp)
	{	ch7cast(expp, CAST, tp);
		(*expp)->ex_flags |= EX_CAST;
	}
|
	postfixed(expp)
|
	unop(&oper) unary(expp)
	{ch7mon(oper, expp);}
|
	size_of(expp)
;

size_of(register struct expr **expp;)
	{struct type *tp;}
:
	SIZEOF
	[%if (first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)
		cast(&tp)
		{
			*expp = intexpr(size_of_type(tp, "type"), INT);
			(*expp)->ex_flags |= EX_SIZEOF;
		}
	|
		unary(expp)
		{ch7mon(SIZEOF, expp);}
	]
;

/* 7.3-7.12 */
/*	The set of operators in C is stratified in 15 levels, with level
	N being treated in RM 7.N.  In principle each operator is
	assigned a rank, ranging from 1 to 15.  Such an expression can
	be parsed by a construct like:
		binary_expression(int maxrank;)
			{int oper;}
		:
			binary_expression(maxrank - 1)
			[%if (rank_of(DOT) <= maxrank)
				binop(&oper)
				binary_expression(rank_of(oper)-1)
			]?
		;
	except that some call of 'unary' is necessary, depending on the
	grammar.
	
	This simple view is marred by three complications:
	1.	Level 15 (comma operator) is not allowed in many
		contexts and is different.
	2.	Level 13 (conditional operator) is a ternary operator,
		which does not fit this scheme at all.
	3.	Level 14 (assignment operators) group right-to-left, as
		opposed to 2-12, which group left-to-right (or are
		immaterial).
	4.	The operators in level 14 start with operators in levels
		2-13 (RM 7.14: The two parts of a compound assignment
		operator are separate tokens.)  This causes LL1 problems.
	This forces us to have four rules:
		binary_expression	for level 2-12
		conditional_expression	for level 13
		assignment_expression	for level 14 and
		expression		for the most general expression
*/

binary_expression(int maxrank; struct expr **expp;)
	{int oper; struct expr *e1;}
:
	unary(expp)
	[%while (rank_of(DOT) <= maxrank && AHEAD != '=')
		/*	'?', '=', and ',' are no binops, and the test
			for AHEAD != '=' keeps the other assignment
			operators out
		*/
		binop(&oper)
		binary_expression(rank_of(oper)-1, &e1)
		{
			ch7bin(expp, oper, e1);
		}
	]*
;

/* 7.13 */
conditional_expression(struct expr **expp;)
/*	There is some unfortunate disagreement about what is allowed
	between the '?' and the ':' of a conditional_expression.
	Although the Ritchie compiler does not even allow
	conditional_expressions there, some other compilers (e.g., VAX)
	accept a full assignment_expression there, and programs
	(like, e.g., emacs) rely on it. So we have little choice.
*/
	{struct expr *e1 = 0, *e2 = 0;}
:
	/* allow all binary operators */
	binary_expression(rank_of('?') - 1, expp)
	[	'?'
		expression(&e1)
		{
#ifndef NOROPTION
			check_conditional(e1, '?', "between ? and :");
#endif
		}
		':'
		assignment_expression(&e2)
		{	
#ifndef NOROPTION
			check_conditional(e2, '=', "after :");
#endif
			ch7bin(&e1, ':', e2);
			opnd2test(expp, '?');
			ch7bin(expp, '?', e1);
		}
	]?
;

/* 7.14 */
assignment_expression(struct expr **expp;)
	{
		int oper;
		struct expr *e1 = 0;
	}
:
	conditional_expression(expp)
	[%prefer	/* (rank_of(DOT) <= maxrank) for any asgnop */
		asgnop(&oper)
		assignment_expression(&e1)
		{ch7asgn(expp, oper, e1);}
	|
		empty		/* LLgen artefact ??? */
	]
;

/* 7.15 */
expression(struct expr **expp;)
	{struct expr *e1;}
:
	assignment_expression(expp)
	[	','
		assignment_expression(&e1)
		{
			ch7bin(expp, ',', e1);
		}
	]*
;

unop(int *oper;) :
	['*' | '&' | '-' | '!' | '~' | PLUSPLUS | MINMIN]
	{*oper = DOT;}
;

postop(int *oper;):
	PLUSPLUS {*oper = POSTINCR;}
|
	MINMIN {*oper = POSTDECR;}
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

asgnop(register int *oper;):
	'=' {*oper = DOT;}
|
	'+' '=' {*oper = PLUSAB;}
|
	'-' '=' {*oper = MINAB;}
|
	'*' '=' {*oper = TIMESAB;}
|
	'/' '=' {*oper = DIVAB;}
|
	'%' '=' {*oper = MODAB;}
|
	LEFT '=' {*oper = LEFTAB;}
|
	RIGHT '=' {*oper = RIGHTAB;}
|
	'&' '=' {*oper = ANDAB;}
|
	'^' '=' {*oper = XORAB;}
|
	'|' '=' {*oper = ORAB;}
;

constant(struct expr **expp;) :
[
	INTEGER
|
	FLOATING
]	{dot2expr(expp);}
;

/* 15 */
constant_expression (struct expr **expp;) :
	assignment_expression(expp)
	{chk_cst_expr(expp);}
;

identifier(struct idf **idfp;) :
[
	IDENTIFIER
|
	TYPE_IDENTIFIER
]
	{
		*idfp = dot.tk_idf;
	}
;
