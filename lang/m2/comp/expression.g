/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N S */

/* $Header$ */

{
#include	"debug.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"node.h"
#include	"const.h"
#include	"type.h"
#include	"chk_expr.h"
#include	"warning.h"

extern char	options[];
}

/* inline, we need room for pdp/11
number(t_node **p;) :
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

qualident(t_node **p;)
{
} :
	IDENT	{ *p = dot2leaf(Name); }
	[
		selector(p)
	]*
;

selector(register t_node **pnd;):
	'.'	{ *pnd = dot2node(Link,*pnd,NULLNODE); }
	IDENT	{ (*pnd)->nd_IDF = dot.TOK_IDF; }
;

ExpList(t_node **pnd;)
{
	register t_node *nd;
} :
	expression(pnd)		{ *pnd = nd = dot2node(Link,*pnd,NULLNODE);
				  nd->nd_symb = ',';
				}
	[
		','		{ nd->nd_right = dot2leaf(Link);
				  nd = nd->nd_right;
				}
		expression(&(nd->nd_left))
	]*
;

ConstExpression(t_node **pnd;)
{
	register t_node *nd;
}:
	expression(pnd)
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
		{ nd = *pnd;
		  DO_DEBUG(options['C'], print("CONSTANT EXPRESSION\n"));
		  DO_DEBUG(options['C'], PrNode(nd, 0));

		  if (ChkExpression(nd) &&
		      nd->nd_class != Set &&
		      nd->nd_class != Value &&
		      ! (options['l'] && nd->nd_class == Def && IsProc(nd))) {
			error("constant expression expected");
		  }

		  DO_DEBUG(options['C'], print("RESULTS IN\n"));
		  DO_DEBUG(options['C'], PrNode(nd, 0));
		}
;

expression(register t_node **pnd;)
{
} :
	SimpleExpression(pnd)
	[
		/* relation */
		[ '=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN ]
			{ *pnd = dot2node(Oper, *pnd, NULLNODE); }
		SimpleExpression(&((*pnd)->nd_right))
	|
	]
;

/* Inline in expression
relation:
	'=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;
*/

SimpleExpression(register t_node **pnd;)
{
	register t_node *nd = 0;
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
				nd->nd_right = *pnd;
				*pnd = nd;
			  }
			  nd = *pnd;
			}
	[
		/* AddOperator */
		[ '+' | '-' | OR ]
			{ nd = dot2node(Oper, nd, NULLNODE); }
		term(&(nd->nd_right))
	]*
			{ *pnd = nd; }
;

/* Inline in "SimpleExpression"
AddOperator:
	'+' | '-' | OR
;
*/

term(t_node **pnd;)
{
	register t_node *nd;
}:
	factor(pnd)	{ nd = *pnd; }
	[
		/* MulOperator */
		[ '*' | '/' | DIV | MOD | AND ]
			{ nd = dot2node(Oper, nd, NULLNODE); }
		factor(&(nd->nd_right))
	]*
			{ *pnd = nd; }
;

/* inline in "term"
MulOperator:
	'*' | '/' | DIV | MOD | AND
;
*/

factor(register t_node **p;)
{
	register t_node *nd;
	t_node *nd1;
} :
	qualident(p)
	[
		designator_tail(p)
		[
			{ *p = dot2node(Call, *p, NULLNODE); }
			ActualParameters(&((*p)->nd_right))
		|
		]
	|
		bare_set(&nd1)
			{ nd = nd1; nd->nd_left = *p; *p = nd; }
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
		      class == Link) {
			nd->nd_right = *p;
			*p = nd;
		  }
		  else FreeNode(nd);
		}
	')'
|
	NOT		{ *p = dot2leaf(Uoper); }
	factor(&((*p)->nd_right))
;

bare_set(t_node **pnd;)
{
	register t_node *nd;
} :
	'{'		{ dot.tk_symb = SET;
			  *pnd = nd = dot2leaf(Xset);
			  nd->nd_type = bitset_type;
			}
	[
		element(nd)
		[	{ nd = nd->nd_right; }
			',' element(nd)
		]*
	|
	]
	'}'
;

ActualParameters(t_node **pnd;):
	'(' ExpList(pnd)? ')'
;

element(register t_node *nd;) :
	expression(&(nd->nd_right))
	[
		UPTO
			{ nd->nd_right = dot2node(Link, nd->nd_right, NULLNODE);}
		expression(&(nd->nd_right->nd_right))
	|
	]
			{ nd->nd_right = dot2node(Link, nd->nd_right, NULLNODE);
			  nd->nd_right->nd_symb = ',';
			}
;

designator(t_node **pnd;)
:
	qualident(pnd)
	designator_tail(pnd)
;

designator_tail(register t_node **pnd;):
	visible_designator_tail(pnd)
	[ %persistent
		%default
		selector(pnd)
	|
		visible_designator_tail(pnd)
	]*
|
;

visible_designator_tail(t_node **pnd;)
{
	register t_node *nd = *pnd;
}:
	'['		{ nd = dot2node(Arrsel, nd, NULLNODE); }
		expression(&(nd->nd_right))
		[
			','
			{ nd = dot2node(Arrsel, nd, NULLNODE);
			}
			expression(&(nd->nd_right))
		]*
	']'
			{ *pnd = nd; }
|
	'^'		{ *pnd = dot2node(Arrow, NULLNODE, nd); }
;
