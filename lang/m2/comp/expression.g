/* E X P R E S S I O N S */

{
static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"scope.h"
#include	"node.h"
#include	"const.h"
#include	"type.h"
#include	"debug.h"
}

number(struct node **p;)
{
	struct type *tp;
} :
[
	INTEGER		{ tp = dot.TOK_INT <= max_int ?
				intorcard_type : card_type;
			}
|
	REAL		{ tp = real_type; }
]			{ *p = MkNode(Value, NULLNODE, NULLNODE, &dot);
			  (*p)->nd_type = tp;
			}
;

qualident(int types; struct def **pdf; char *str; struct node **p;)
{
	register struct def *df;
	register struct node **pnd;
	struct node *nd;
} :
	IDENT		{ nd = MkNode(Name, NULLNODE, NULLNODE, &dot);
			  pnd = &nd;
			}
	[
		selector(pnd)
	]*
			{ if (types) {
				findname(nd);
				assert(nd->nd_class == Def);
				*pdf = df = nd->nd_def;
			  	if ( !((types|D_ERROR) & df->df_kind)) {
					error("identifier \"%s\" is not a %s",
					df->df_idf->id_text, str);
				}
			  }
			  if (!p) FreeNode(nd);
			  else *p = nd;
			}
;

selector(struct node **pnd;):
	'.'	{ *pnd = MkNode(Link,*pnd,NULLNODE,&dot); }
	IDENT	{ (*pnd)->nd_right = MkNode(Name,NULLNODE,NULLNODE,&dot); }
;

ExpList(struct node **pnd;)
{
	struct node **nd;
} :
	expression(pnd)		{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);
				  (*pnd)->nd_symb = ',';
				  nd = &((*pnd)->nd_right);
				}
	[
		','		{ *nd = MkNode(Link, NULLNODE, NULLNODE, &dot);
				}
		expression(&(*nd)->nd_left)
				{ nd = &((*pnd)->nd_right); }
	]*
;

ConstExpression(struct node **pnd;):
	expression(pnd)
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
		{ DO_DEBUG(3,
		     ( debug("Constant expression:"),
		       PrNode(*pnd)));
		  if (chk_expr(*pnd) &&
		      ((*pnd)->nd_class != Set && (*pnd)->nd_class != Value)) {
			error("Constant expression expected");
		  }
		  DO_DEBUG(3, PrNode(*pnd));
		}
;

expression(struct node **pnd;)
{
} :
	SimpleExpression(pnd)
	[
		/* relation */
		[ '=' | '#' | UNEQUAL | '<' | LESSEQUAL | '>' |
		  GREATEREQUAL | IN
		]
			{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		SimpleExpression(&((*pnd)->nd_right))
	]?
;

/* Inline in expression
relation:
	'=' | '#' | UNEQUAL | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;
*/

SimpleExpression(struct node **pnd;)
{
} :
	[
		[ '+' | '-' ]
			{ *pnd = MkNode(Uoper, NULLNODE, NULLNODE, &dot);
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

factor(struct node **p;)
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
			{ nd->nd_left = *p;
			  *p = nd;
			}
	]
|
	bare_set(p)
| %default
	number(p)
|
	STRING		{ *p = MkNode(Value, NULLNODE, NULLNODE, &dot);
			  if (dot.TOK_SLE == 1) {
				dot.TOK_INT = *(dot.TOK_STR);
				(*p)->nd_type = char_type;
			  }
			  else	(*p)->nd_type = string_type;
			}
|
	'(' expression(p) ')'
|
	NOT		{ *p = MkNode(Uoper, NULLNODE, NULLNODE, &dot); }
	factor(&((*p)->nd_right))
;

bare_set(struct node **pnd;)
{
	register struct node *nd;
} :
	'{'		{
			  dot.tk_symb = SET;
			  *pnd = nd = MkNode(Xset, NULLNODE, NULLNODE, &dot);
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

visible_designator_tail(struct node **pnd;):
	'['		{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot); }
		expression(&((*pnd)->nd_right))
		[
			','
			{ *pnd = MkNode(Oper, *pnd, NULLNODE, &dot);
			  (*pnd)->nd_symb = '[';
			}
			expression(&((*pnd)->nd_right))
		]*
	']'
|
	'^'		{ *pnd = MkNode(Oper, NULLNODE, *pnd, &dot); }
;
