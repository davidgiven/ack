/* S T A T E M E N T S */

{
#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"

static int	loopcount = 0;	/* Count nested loops */
}

statement(register struct node **pnd;)
{
	register struct node *nd;
} :
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
			{ if (!loopcount) error("EXIT not in a LOOP");
			  *pnd = MkLeaf(Stat, &dot);
			}
|
	ReturnStatement(pnd)
|
	/* empty */	{ *pnd = 0; }
]
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

StatementSequence(register struct node **pnd;)
{
	struct node *nd;
} :
	statement(pnd)
	[
		';' statement(&nd)
			{ if (nd) {
				*pnd = MkNode(Link, *pnd, nd, &dot);
				(*pnd)->nd_symb = ';';
			  	pnd = &((*pnd)->nd_right);
			  }
			}
	]*
;

IfStatement(struct node **pnd;)
{
	register struct node *nd;
} :
	IF		{ nd = MkLeaf(Stat, &dot);
			  *pnd = nd;
			}
	expression(&(nd->nd_left))
	THEN		{ nd->nd_right = MkLeaf(Link, &dot);
			  nd = nd->nd_right;
			}
	StatementSequence(&(nd->nd_left))
	[
		ELSIF	{ nd->nd_right = MkLeaf(Stat, &dot);
			  nd = nd->nd_right;
			  nd->nd_symb = IF;
			}
		expression(&(nd->nd_left))
		THEN	{ nd->nd_right = MkLeaf(Link, &dot);
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
	CASE		{ *pnd = nd = MkLeaf(Stat, &dot); }
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
	[ CaseLabelList(ptp, pnd)
	  ':'		{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot); }
	  StatementSequence(&((*pnd)->nd_right))
	]?
			{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);
			  (*pnd)->nd_symb = '|';
			}
;

WhileStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	WHILE		{ *pnd = nd = MkLeaf(Stat, &dot); }
	expression(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;

RepeatStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	REPEAT		{ *pnd = nd = MkLeaf(Stat, &dot); }
	StatementSequence(&(nd->nd_left))
	UNTIL
	expression(&(nd->nd_right))
;

ForStatement(struct node **pnd;)
{
	register struct node *nd;
	struct node *dummy;
}:
	FOR		{ *pnd = nd = MkLeaf(Stat, &dot); }
	IDENT		{ nd->nd_IDF = dot.TOK_IDF; }
	BECOMES		{ nd->nd_left = MkLeaf(Stat, &dot);
			  nd = nd->nd_left;
			}
	expression(&(nd->nd_left))
	TO
	expression(&(nd->nd_right))
	[
		BY
		ConstExpression(&dummy)
			{
			  if (!(dummy->nd_type->tp_fund & T_INTORCARD)) {
				error("illegal type in BY clause");
			  }
			  nd->nd_INT = dummy->nd_INT;
			  FreeNode(dummy);
			}
	|
			{ nd->nd_INT = 1; }
	]
	DO
	StatementSequence(&((*pnd)->nd_right))
	END
;

LoopStatement(struct node **pnd;):
	LOOP		{ *pnd = MkLeaf(Stat, &dot); }
	StatementSequence(&((*pnd)->nd_right))
	END
;

WithStatement(struct node **pnd;)
{
	register struct node *nd;
}:
	WITH		{ *pnd = nd = MkLeaf(Stat, &dot); }
	designator(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;

ReturnStatement(struct node **pnd;)
{
	register struct def *df = CurrentScope->sc_definedby;
	register struct node *nd;
	extern int return_occurred;
} :

	RETURN		{ *pnd = nd = MkLeaf(Stat, &dot);
			  return_occurred = 1;
			}
	[
		expression(&(nd->nd_right))
			{ if (scopeclosed(CurrentScope)) {
error("a module body has no result value");
			  }
			  else if (! df->df_type->next) {
error("procedure \"%s\" has no result value", df->df_idf->id_text);
			  }
			}
	|
			{ if (df->df_type->next) {
error("procedure \"%s\" must return a value", df->df_idf->id_text);
			  }
			}
	]
;
