/* E X P R E S S I O N S */

{
static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"scope.h"
#include	"node.h"
}

number(struct node **p;):
[
	INTEGER
|
	REAL
]			{ *p = MkNode(Value, NULLNODE, NULLNODE, dot); }
;

qualident(int types; struct def **pdf; char *str; struct node **p;)
{
	int scope;
	int  module;
	register struct def *df;
	struct def *lookfor();
} :
	IDENT		{ if (types) {
				df = lookfor(dot.TOK_IDF, CurrentScope, 1);
				*pdf = df;
				if (df->df_kind == D_ERROR) types = 0;
			  }
			  if (p) {
			  	*p = MkNode(Value, NULLNODE, NULLNODE,&dot);
			  }
			}
	[
			{ if (types &&!(scope = has_selectors(df))) {
				types = 0;
				*pdf = ill_df;
			  }
			}
		/* selector */
		'.'	{ if (p) *p = MkNode(Link, *p, NULLNODE, &dot); }
		IDENT
			{ if (p) {
				p = &((*p)->nd_right);
				*p = MkNode(Value, NULLNODE, NULLNODE,&dot);
			  }
			  if (types) {
				module = (df->df_kind == D_MODULE);
				df = lookup(dot.TOK_IDF, scope);
				if (!df) {
					types = 0;
					df = ill_df;
					id_not_declared(dot.TOK_IDF);
				}
				else
				if (module &&
				    !(df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
					error("identifier \"%s\" not exported from qualifying module", dot.TOK_IDF->id_text);
				}
			  }
			}
	]*
			{ if (types && !(types & df->df_kind)) {
				error("identifier \"%s\" is not a %s",
					df->df_idf->id_text, str);
			  }
			}
;

/* Inline substituted wherever it occurred
selector:
	'.' IDENT
;
*/

ExpList(struct node **pnd;)
{
	struct node **nd;
} :
	expression(pnd)		{ nd = pnd; }
	[
		','		{ *nd = MkNode(Link, *nd, NULLNODE, &dot);
				  nd = &(*nd)->nd_right;
				}
		expression(nd)
	]*
;

ConstExpression(struct node **pnd;):
	expression(pnd)
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
;

expression(struct node **pnd;)
{
	struct node *nd;
} :
	SimpleExpression(&nd)
	[
		/* relation */
		[ '=' | '#' | UNEQUAL | '<' | LESSEQUAL | '>' |
		  GREATEREQUAL | IN
		]
			{ nd = MkNode(Oper, nd, NULLNODE, &dot); }
		SimpleExpression(&(nd->nd_right))
	]?
			{ *pnd = nd; }
;

/* Inline in expression
relation:
	'=' | '#' | UNEQUAL | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;
*/

SimpleExpression(struct node **pnd;)
{
	register struct node *nd;
} :
	[ '+' | '-' ]?
	term(pnd)	{ nd = *pnd; }
	[
		/* AddOperator */
		[ '+' | '-' | OR ]
			{ *pnd = nd = MkNode(Oper, nd, NULLNODE, &dot); }
		term(&(nd->nd_right))
	]*
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
		[ '*' | '/' | DIV | MOD | AND | '&' ]
			{ *pnd = nd = MkNode(Oper, nd, NULLNODE, &dot); }
		factor(&(nd->nd_right))
	]*
;

/* inline in "term"
MulOperator:
	'*' | '/' | DIV | MOD | AND | '&'
;
*/

factor(struct node **p;)
{
	struct def *df;
} :
	qualident(0, &df, (char *) 0, p)
	[
		designator_tail(p)?
		[
				{ *p = MkNode(Call, p, NULLNODE, &dot); } 
			ActualParameters(&((*p)->nd_right))
		]?
	|			{ *p = MkNode(Call, p, NULLNODE, &dot); }
		bare_set(&((*p)->nd_right))
	]
|
	bare_set(p)
| %default
	number(p)
|
	STRING		{ *p = MkNode(Value, NULLNODE, NULLNODE, &dot); }
|
	'(' expression(p) ')'
|
	NOT		{ *p = MkNode(Uoper, NULLNODE, NULLNODE, &dot); }
	factor(&((*p)->nd_left))
;

bare_set(struct node **pnd;)
{
	struct node **nd;
} :
	'{'		{
			  dot.tk_symb = SET;
			  *pnd = MkNode(Link, NULLNODE, NULLNODE, &dot);
			  nd = &((*pnd)->nd_left);
			}
	[
		element(nd)
		[
			','	{ *nd = MkNode(Link, *nd, NULLNODE, &dot);
				  nd = &((*nd)->nd_right);
				}
			element(nd)
		]*
	]?
	'}'
;

ActualParameters(struct node **pnd;):
	'(' ExpList(pnd)? ')'
;

element(struct node **pnd;):
	expression(pnd)
	[
		UPTO		{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);}
		expression(&((*pnd)->nd_right))
	]?
;

designator(struct node **pnd;)
{
	struct def *df;
} :
	qualident(0, &df, (char *) 0, pnd)
	designator_tail(pnd)?
;

designator_tail(struct node **pnd;):
	visible_designator_tail(pnd)
	[
		/* selector */
		'.'	{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		IDENT	{ (*pnd)->nd_right =
				MkNode(Value, NULLNODE, NULLNODE, &dot);
			}
	|
		visible_designator_tail(pnd)
	]*
;

visible_designator_tail(struct node **pnd;):
	'['		{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
	ExpList(&((*pnd)->nd_right))
	']'
|
	'^'		{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
;
