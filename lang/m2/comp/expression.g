/* E X P R E S S I O N S */

{
#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

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
}

number(struct node **p;) :
[
	%default
	INTEGER
|
	REAL
]			{ *p = MkLeaf(Value, &dot);
			  (*p)->nd_type = toktype;
			}
;

qualident(int types;
	  struct def **pdf;
	  char *str;
	  struct node **p;
	 )
{
	register struct def *df;
	struct node *nd;
} :
	IDENT	{ nd = MkLeaf(Name, &dot); }
	[
		selector(&nd)
	]*
		{ if (types) {
			df = ill_df;

			if (ChkDesignator(nd)) {
			    if (nd->nd_class != Def) {
				node_error(nd, "%s expected", str);
			    }
			    else {
				df = nd->nd_def;
		  		if ( !((types|D_ERROR) & df->df_kind)) {
				    if (df->df_kind == D_FORWARD) {
node_error(nd,"%s \"%s\" not declared", str, df->df_idf->id_text);
				    }
				    else {
node_error(nd,"identifier \"%s\" is not a %s", df->df_idf->id_text, str);
				    }
				}
			    }
			}
			*pdf = df;
		  }
		  if (!p) FreeNode(nd);
		  else *p = nd;
		}
;

selector(struct node **pnd;):
	'.'	{ *pnd = MkNode(Link,*pnd,NULLNODE,&dot); }
	IDENT	{ (*pnd)->nd_IDF = dot.TOK_IDF; }
;

ExpList(struct node **pnd;)
{
	register struct node *nd;
} :
	expression(pnd)		{ *pnd = nd = MkNode(Link,*pnd,NULLNODE,&dot);
				  (*pnd)->nd_symb = ',';
				}
	[
		','		{ nd->nd_right = MkLeaf(Link, &dot);
				  nd = nd->nd_right;
				}
		expression(&(nd->nd_left))
	]*
;

ConstExpression(struct node **pnd;):
	expression(pnd)
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
		{ DO_DEBUG(options['X'], print("CONSTANT EXPRESSION\n"));
		  DO_DEBUG(options['X'], PrNode(*pnd, 0));
		  if (ChkExpression(*pnd) &&
		      ((*pnd)->nd_class != Set && (*pnd)->nd_class != Value)) {
			error("Constant expression expected");
		  }
		  DO_DEBUG(options['X'], print("RESULTS IN\n"));
		  DO_DEBUG(options['X'], PrNode(*pnd, 0));
		}
;

expression(struct node **pnd;)
{
} :
	SimpleExpression(pnd)
	[
		/* relation */
		[ '=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN ]
			{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		SimpleExpression(&((*pnd)->nd_right))
	]?
;

/* Inline in expression
relation:
	'=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;
*/

SimpleExpression(struct node **pnd;)
{
} :
	[
		[ '+' | '-' ]
			{ *pnd = MkLeaf(Uoper, &dot);
			  pnd = &((*pnd)->nd_right);
			}
	]?
	term(pnd)
	[
		/* AddOperator */
		[ '+' | '-' | OR ]
			{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		term(&((*pnd)->nd_right))
	]*
;

/* Inline in "SimpleExpression"
AddOperator:
	'+' | '-' | OR
;
*/

term(struct node **pnd;)
{
}:
	factor(pnd)
	[
		/* MulOperator */
		[ '*' | '/' | DIV | MOD | AND | '&' ]
			{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		factor(&((*pnd)->nd_right))
	]*
;

/* inline in "term"
MulOperator:
	'*' | '/' | DIV | MOD | AND | '&'
;
*/

factor(register struct node **p;)
{
	struct def *df;
	struct node *nd;
} :
	qualident(0, &df, (char *) 0, p)
	[
		designator_tail(p)?
		[
			{ *p = MkNode(Call, *p, NULLNODE, &dot); }
			ActualParameters(&((*p)->nd_right))
		]?
	|
		bare_set(&nd)
			{ nd->nd_left = *p; *p = nd; }
	]
|
	bare_set(p)
| %default
	number(p)
|
	STRING	{ *p = MkLeaf(Value, &dot);
		  (*p)->nd_type = toktype;
		}
|
	'(' expression(p) ')'
|
	NOT		{ *p = MkLeaf(Uoper, &dot); }
	factor(&((*p)->nd_right))
;

bare_set(struct node **pnd;)
{
	register struct node *nd;
} :
	'{'		{ dot.tk_symb = SET;
			  *pnd = nd = MkLeaf(Xset, &dot);
			  nd->nd_type = bitset_type;
			}
	[
		element(nd)
		[	{ nd = nd->nd_right; }
			',' element(nd)
		]*
	]?
	'}'
;

ActualParameters(struct node **pnd;):
	'(' ExpList(pnd)? ')'
;

element(struct node *nd;)
{
	struct node *nd1;
} :
	expression(&nd1)
	[
		UPTO
			{ nd1 = MkNode(Link, nd1, NULLNODE, &dot);}
		expression(&(nd1->nd_right))
	]?
			{ nd->nd_right = MkNode(Link, nd1, NULLNODE, &dot);
			  nd->nd_right->nd_symb = ',';
			}
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
		selector(pnd)
	|
		visible_designator_tail(pnd)
	]*
;

visible_designator_tail(register struct node **pnd;):
	'['		{ *pnd = MkNode(Arrsel, *pnd, NULLNODE, &dot); }
		expression(&((*pnd)->nd_right))
		[
			','
			{ *pnd = MkNode(Arrsel, *pnd, NULLNODE, &dot);
			  (*pnd)->nd_symb = '[';
			}
			expression(&((*pnd)->nd_right))
		]*
	']'
|
	'^'		{ *pnd = MkNode(Arrow, NULLNODE, *pnd, &dot); }
;
