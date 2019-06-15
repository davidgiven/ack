/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N S */

/* $Id$ */

{
#include 	"parameters.h"
#include	"debug.h"

#include	"alloc.h"
#include	"em_arith.h"
#include	"em_label.h"
#include	<assert.h>

#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"node.h"
#include	"type.h"
#include	"error.h"
#include	"chk_expr.h"
#include	"warning.h"

extern char	options[];
}

/* inline, we need room for pdp/11
number(struct node **p;) :
[
	%default
	INTEGER
|
	REAL
]			{ *p = dot2leaf(Value);
			  (*p)->nd_type = toktype;
			}
;
*/

qualident(struct node **p;)
{
} :
	IDENT	{ *p = dot2leaf(Name); }
	[
		selector(p)
	]*
;

selector(register struct node **pnd;)
{ struct node *nd;
} :
	'.'	{ nd = dot2leaf(Select); nd->nd_NEXT = *pnd; *pnd = nd; }
	IDENT	{ (*pnd)->nd_IDF = dot.TOK_IDF; }
;

ExpList(struct node **pnd;)
{
	register struct node *nd;
} :
	expression(pnd)		{ *pnd = nd = dot2node(Link,*pnd,NULLNODE);
				  nd->nd_symb = ',';
				}
	[
		','		{ nd->nd_RIGHT = dot2leaf(Link);
				  nd = nd->nd_RIGHT;
				}
		expression(&(nd->nd_LEFT))
	]*
;

ConstExpression(register struct node **pnd;)
{
}:
	expression(pnd)
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
		{
		  DO_DEBUG(options['C'], print("CONSTANT EXPRESSION\n"));
		  DO_DEBUG(options['C'], PrNode(*pnd, 0));

		  if (ChkExpression(pnd) &&
		      (*pnd)->nd_class != Set &&
		      (*pnd)->nd_class != Value &&
		      ! (options['l'] && (*pnd)->nd_class == Def && IsProc((*pnd)))) {
			error("constant expression expected");
		  }

		  DO_DEBUG(options['C'], print("RESULTS IN\n"));
		  DO_DEBUG(options['C'], PrNode(*pnd, 0));
		}
;

expression(register struct node **pnd;)
{
} :
	SimpleExpression(pnd)
	[
		/* relation */
		[ '=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN ]
			{ *pnd = dot2node(Oper, *pnd, NULLNODE); }
		SimpleExpression(&((*pnd)->nd_RIGHT))
	|
	]
;

/* Inline in expression
relation:
	'=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;
*/

SimpleExpression(register struct node **pnd;)
{
	register struct node *nd = 0;
} :
	[
		[ '+' | '-' ]
			{ nd = dot2leaf(Uoper);
			  /* priority of unary operator ??? */
			}
	|
	]
	term(pnd)
			{ if (nd) {
				nd->nd_RIGHT = *pnd;
				*pnd = nd;
			  }
			  nd = *pnd;
			}
	[
		/* AddOperator */
		[ '+' | '-' | OR ]
			{ nd = dot2node(Oper, nd, NULLNODE); }
		term(&(nd->nd_RIGHT))
	]*
			{ *pnd = nd; }
;

/* Inline in "SimpleExpression"
AddOperator:
	'+' | '-' | OR
;
*/

term(struct node **pnd;)
{
	register struct node *nd;
}:
	factor(pnd)	{ nd = *pnd; }
	[
		/* MulOperator */
		[ '*' | '/' | DIV | MOD | AND ]
			{ nd = dot2node(Oper, nd, NULLNODE); }
		factor(&(nd->nd_RIGHT))
	]*
			{ *pnd = nd; }
;

/* inline in "term"
MulOperator:
	'*' | '/' | DIV | MOD | AND
;
*/

factor(register struct node **p;)
{
	register struct node *nd;
	struct node *nd1;
} :
	qualident(p)
	[
		designator_tail(p)
		[
			{ *p = dot2node(Call, *p, NULLNODE); }
			ActualParameters(&((*p)->nd_RIGHT))
		|
		]
	|
		bare_set(&nd1)
			{ nd = nd1; nd->nd_LEFT = *p; *p = nd; }
	]
|
	bare_set(p)
| %default
	[
		%default
		INTEGER
	|
		REAL
	|
		STRING
	]		{ *p = dot2leaf(Value);
			  (*p)->nd_type = toktype;
			}
|
	'(' 	{ nd = dot2leaf(Uoper); }
	expression(p)
		{ /*	In some cases we must leave the '(' as an unary
			operator, because otherwise we cannot see that the
			factor was not a designator
		  */
		  register int class = (*p)->nd_class;

		  if (class == Arrsel ||
		      class == Arrow ||
		      class == Name ||
		      class == Select) {
			nd->nd_RIGHT = *p;
			*p = nd;
		  }
		  else FreeNode(nd);
		}
	')'
|
	NOT		{ *p = dot2leaf(Uoper); }
	factor(&((*p)->nd_RIGHT))
;

bare_set(struct node **pnd;)
{
	register struct node *nd;
} :
	'{'		{ DOT = SET;
			  *pnd = nd = dot2leaf(Xset);
			  nd->nd_type = bitset_type;
			}
	[
		element(nd)
		[	{ nd = nd->nd_RIGHT; }
			',' element(nd)
		]*
	|
	]
	'}'
;

ActualParameters(struct node **pnd;):
	'(' ExpList(pnd)? ')'
;

element(register struct node *nd;) :
	expression(&(nd->nd_RIGHT))
	[
		UPTO
			{ nd->nd_RIGHT = dot2node(Link, nd->nd_RIGHT, NULLNODE);}
		expression(&(nd->nd_RIGHT->nd_RIGHT))
	|
	]
			{ nd->nd_RIGHT = dot2node(Link, nd->nd_RIGHT, NULLNODE);
			  nd->nd_RIGHT->nd_symb = ',';
			}
;

designator(struct node **pnd;)
:
	qualident(pnd)
	designator_tail(pnd)
;

designator_tail(register struct node **pnd;):
	visible_designator_tail(pnd)
	[ %persistent
		%default
		selector(pnd)
	|
		visible_designator_tail(pnd)
	]*
|
;

visible_designator_tail(struct node **pnd;)
{
	register struct node *nd = *pnd;
}:
	'['		{ nd = dot2node(Arrsel, nd, NULLNODE); }
		expression(&(nd->nd_RIGHT))
		[
			','
			{ nd = dot2node(Arrsel, nd, NULLNODE);
			}
			expression(&(nd->nd_RIGHT))
		]*
	']'
			{ *pnd = nd; }
|
	'^'		{ *pnd = dot2node(Arrow, NULLNODE, nd); }
;
