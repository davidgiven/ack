/* S T A T E M E N T S */

{
static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	<em_label.h>
#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"

static int	loopcount = 0;	/* Count nested loops */
extern struct def *currentdef;
}

statement(struct node **pnd;)
{
	register struct node *nd;
} :
				{ *pnd = 0; }
[
	/*
	 * This part is not in the reference grammar. The reference grammar
	 * states : assignment | ProcedureCall | ...
	 * but this gives LL(1) conflicts
	 */
	designator(pnd)
	[			{ nd = MkNode(Call, *pnd, NULLNODE, &dot);
				  nd->nd_symb = '(';
				}
		ActualParameters(&(nd->nd_right))?
	|
		BECOMES		{ nd = MkNode(Stat, *pnd, NULLNODE, &dot); }
		expression(&(nd->nd_right))
	]
				{ *pnd = nd; }
	/*
	 * end of changed part
	 */
|
	IfStatement(pnd)
|
	CaseStatement(pnd)
|
	WhileStatement(pnd)
|
	RepeatStatement(pnd)
|
			{ loopcount++; }
	LoopStatement(pnd)
			{ loopcount--; }
|
	ForStatement(pnd)
|
	WithStatement(pnd)
|
	EXIT
			{ if (!loopcount) {
error("EXIT not in a LOOP");
			  }
			  *pnd = MkNode(Stat, NULLNODE, NULLNODE, &dot);
			}
|
	RETURN		{ *pnd = nd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	[
		expression(&(nd->nd_right))
			{ if (scopeclosed(CurrentScope)) {
error("a module body has no result value");
			  }
			  else if (! currentdef->df_type->next) {
error("procedure \"%s\" has no result value", currentdef->df_idf->id_text);
			  }
			}
	|
			{ if (currentdef->df_type->next) {
error("procedure \"%s\" must return a value", currentdef->df_idf->id_text);
			  }
			}
	]
]?
;

/*
 * The next two rules in-line in "Statement", because of an LL(1) conflict

assignment:
	designator BECOMES expression
;

ProcedureCall:
	designator ActualParameters?
;
*/

StatementSequence(struct node **pnd;):
	statement(pnd)
	[
		';'	{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);
			  pnd = &((*pnd)->nd_right);
			}
		statement(pnd)
	]*
;

IfStatement(struct node **pnd;)
{
	register struct node *nd;
} :
	IF		{ nd = MkNode(Stat, NULLNODE, NULLNODE, &dot);
			  *pnd = nd;
			}
	expression(&(nd->nd_left))
	THEN		{ nd = MkNode(Link, NULLNODE, NULLNODE, &dot);
			  (*pnd)->nd_right = nd;
			}
	StatementSequence(&(nd->nd_left))
	[
		ELSIF	{ nd->nd_right = MkNode(Stat,NULLNODE,NULLNODE,&dot);
			  nd = nd->nd_right;
			  nd->nd_symb = IF;
			}
		expression(&(nd->nd_left))
		THEN	{ nd->nd_right = MkNode(Link,NULLNODE,NULLNODE,&dot);
			  nd = nd->nd_right;
			}
		StatementSequence(&(nd->nd_left))
	]*
	[
		ELSE
		StatementSequence(&(nd->nd_right))
	]?
	END
;

CaseStatement(struct node **pnd;)
{
	register struct node *nd;
	struct type *tp = 0;
} :
	CASE		{ *pnd = nd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	expression(&(nd->nd_left))
	OF
	case(&(nd->nd_right), &tp)
			{ nd = nd->nd_right; }
	[
		'|'
		case(&(nd->nd_right), &tp)
			{ nd = nd->nd_right; }
	]*
	[ ELSE StatementSequence(&(nd->nd_right)) ]?
	END
;

case(struct node **pnd; struct type **ptp;) :
			{ *pnd = 0; }
	[ CaseLabelList(ptp, pnd)
	  ':'		{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot); }
	  StatementSequence(&((*pnd)->nd_right))
	]?
				/* This rule is changed in new modula-2 */
			{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);
			  (*pnd)->nd_symb = '|';
			}
;

WhileStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	WHILE		{ *pnd = nd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	expression(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;

RepeatStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	REPEAT		{ *pnd = nd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	StatementSequence(&(nd->nd_left))
	UNTIL
	expression(&(nd->nd_right))
;

ForStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	FOR		{ *pnd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	IDENT		{ nd = MkNode(Name, NULLNODE, NULLNODE, &dot); }
	BECOMES		{ nd = MkNode(BECOMES, nd, NULLNODE, &dot); }
	expression(&(nd->nd_right))
	TO		{ (*pnd)->nd_left=nd=MkNode(Link,nd,NULLNODE,&dot); }
	expression(&(nd->nd_right))
	[
		BY	{ nd->nd_right=MkNode(Link,NULLNODE,nd->nd_right,&dot);
			}
		ConstExpression(&(nd->nd_right->nd_left))
	|
	]
	DO
	StatementSequence(&((*pnd)->nd_right))
	END
;

LoopStatement(struct node **pnd;):
	LOOP		{ *pnd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	StatementSequence(&((*pnd)->nd_right))
	END
;

WithStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	WITH		{ *pnd = nd = MkNode(Stat, NULLNODE, NULLNODE, &dot); }
	designator(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;
