/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	EXPRESSION SYNTAX PARSER	*/

{
#include	<alloc.h>
#include	"parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"LLlex.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"code.h"
#include    "error.h"
#include    "ch3.h"
#include    "ch3bin.h"
#include    "ch3mon.h"
#include    "proto.h"
#include	"sizes.h"

extern struct expr *intexpr();
int InSizeof = 0;	/* inside a sizeof- expression */
int ResultKnown = 0;	/* result of the expression is already known */

/* Since the grammar in the standard is not LL(n), it is modified so that
 * it accepts basically the same grammar. This means that there is no 1-1
 * mapping from the grammar in the standard to the grammar given here.
 * Such is life.
 */
}

/* 3.3.1 */
primary(register struct expr **expp;) :
	IDENTIFIER
	{dot2expr(expp);}
|
	constant(expp)
|
	string(expp)
|
	'(' expression(expp) ')'
	{ (*expp)->ex_flags |= EX_PARENS; }
;


/* Character string literals that are adjacent tokens
 * are concatenated into a single character string
 * literal.
 */
string(register struct expr **expp;)
	{	register int i, len;
		register char *str;
		register int fund;
	}
:
	STRING
	{	str = dot.tk_bts;
		len = dot.tk_len;
		fund = dot.tk_fund;
	}
	[
		STRING
		{	/* A pasted string keeps the type of the first
			 * string literal.
			 * The pasting of normal strings and wide
			 * character strings are stated as having an
			 * undefined behaviour.
			 */
			if (dot.tk_fund != fund)
				warning("illegal pasting of string literals");
			str = Realloc(str, (unsigned) (--len + dot.tk_len));
			for (i = 0; i < dot.tk_len; i++)
				str[len++] = dot.tk_bts[i];
		}
	]*
	{ string2expr(expp, str, len); }
;

/* 3.3.2 */
postfix_expression(register struct expr **expp;)
	{ int oper; 
	  struct expr *e1 = 0;
	  struct idf *idf;
	}
:
	primary(expp)
	[
		'[' expression(&e1) ']'
			{ ch3bin(expp, '[', e1); e1 = 0; }
	|
		'(' parameter_list(&e1)? ')'
			{ ch3bin(expp, '(', e1); call_proto(expp); e1 = 0; }
	|
		[ '.' | ARROW ]			{ oper = DOT; }
		identifier(&idf)		{ ch3sel(expp, oper, idf); }
	|
		[
			PLUSPLUS	{ oper = POSTINCR; }
		|
			MINMIN		{ oper = POSTDECR; }
		]
		    { ch3incr(expp, oper); }
	]*
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
		{ch3bin(expp, PARCOMMA, e1);}
	]*
;

%first	first_of_type_specifier, type_specifier;

/* 3.3.3 & 3.3.4 */
unary(register struct expr **expp;)
	{struct type *tp; int oper;}
:
%if (first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)
	cast(&tp) unary(expp)
	{	ch3cast(expp, CAST, tp);
		(*expp)->ex_flags |= EX_CAST;
		if (int_size != pointer_size)
			(*expp)->ex_flags &= ~EX_PTRDIFF;
	}
|
	postfix_expression(expp)
|
	unop(&oper) unary(expp)
	{ch3mon(oper, expp);}
|
	size_of(expp)
;

/* When an identifier is used in a sizeof()-expression, we must stil not
 * mark it as used.
 * extern int i;  ....  sizeof(i)  .... need not have a definition for i
 */
size_of(register struct expr **expp;)
	{struct type *tp;}
:
	SIZEOF { InSizeof++; }	/* handle (sizeof(sizeof(int))) too */
	[%if (first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)
		cast(&tp)
		{
			*expp = intexpr(size_of_type(tp, "type"), UNSIGNED);
			(*expp)->ex_flags |= EX_SIZEOF;
		}
	|
		unary(expp)
		{ch3mon(SIZEOF, expp);}
	]
	{ InSizeof--; }
;

/* 3.3.5-3.3.17 */
/*	The set of operators in C is stratified in 15 levels, with level
	N being treated in RM 7.N (although this is not the standard
	anymore). The standard describes this in phrase-structure-grammar,
	which we are unable to parse. The description that follows comes
	from the old C-compiler.

	In principle each operator is assigned a rank, ranging
	from 1 to 15.  Such an expression can be parsed by a construct
	like:
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
	{int oper, OldResultKnown; struct expr *e1;}
:
	unary(expp)
	[%while (rank_of(DOT) <= maxrank )
		/*	'?', '=', and ',' are no binops
		 */
		binop(&oper)
		{ OldResultKnown = ResultKnown;
		  if (oper == OR || oper == AND) {
			  if (is_cp_cst(*expp) || is_fp_cst(*expp)) {
				  if (is_zero_cst(*expp)) {
					  if (oper == AND) ResultKnown++;
				  } else if (oper == OR) ResultKnown++;
			  }
		  }
		}
		binary_expression(rank_of(oper)-1, &e1)
		{
			ch3bin(expp, oper, e1);
			ResultKnown = OldResultKnown;
		}
	]*
;

/* 3.3.15 */
conditional_expression(struct expr **expp;)
	{struct expr *e1 = 0, *e2 = 0; int OldResultKnown, ConstExpr=0;}
:
	/* allow all binary operators */
	binary_expression(rank_of('?') - 1, expp)
	[	'?'
		{ OldResultKnown = ResultKnown;
		  if (is_cp_cst(*expp) || is_fp_cst(*expp)) {
			  ConstExpr++;
			  if (is_zero_cst(*expp)) ResultKnown++;
		  }
		}
		expression(&e1)
		':'
		{ if (ConstExpr) {
			if (OldResultKnown == ResultKnown) ResultKnown++;
			else ResultKnown = OldResultKnown;
		  }
		}
		conditional_expression(&e2)
		{	
			ResultKnown = OldResultKnown;
			ch3bin(&e1, ':', e2);
			opnd2test(expp, '?');
			ch3bin(expp, '?', e1);
		}
	]?
;

/* 3.3.16 */
assignment_expression(struct expr **expp;)
	{ int oper;
	  struct expr *e1 = 0;
	}
:
	conditional_expression(expp)
	[
		asgnop(&oper)
		assignment_expression(&e1)
		{ch3asgn(expp, oper, e1);}
	|
		empty		/* LLgen artefact ??? */
	]
;

/* 3.3.17 */
expression(struct expr **expp;)
	{struct expr *e1;}
:
	assignment_expression(expp)
	[	','
		assignment_expression(&e1)
		{
			ch3bin(expp, ',', e1);
		}
	]*
;

unop(int *oper;) :
	['*' | '&' | '-' | '+' | '!' | '~' | PLUSPLUS | MINMIN]
	{   if (DOT == '&') DOT = ADDRESSOF;
	    *oper = DOT;
	}
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
	[ '=' | PLUSAB | MINAB | TIMESAB | DIVAB | MODAB 
	| LEFTAB | RIGHTAB | ANDAB | XORAB | ORAB ]
	{ *oper = DOT; }

;

constant(struct expr **expp;) :
[
	INTEGER
|
	FLOATING
]	{dot2expr(expp);}
;

/* 3.4 */
constant_expression (struct expr **expp;) :
	conditional_expression(expp)
	{ chk_cst_expr(expp); }
;

identifier(struct idf **idfp;) :
[ IDENTIFIER
| TYPE_IDENTIFIER
]
	{ *idfp = dot.tk_idf; }
;
