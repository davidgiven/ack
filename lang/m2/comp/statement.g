/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S T A T E M E N T S */

/* $Id$ */

{
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include "parameters.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"

static int	loopcount = 0;	/* Count nested loops */
extern t_node *EmptyStatement;
}

statement(register t_node **pnd;)
{
	register t_node *nd;
	extern int return_occurred;
} :
	/*
	 * This part is not in the reference grammar. The reference grammar
	 * states : assignment | ProcedureCall | ...
	 * but this gives LL(1) conflicts
	 */
	designator(pnd)
	[			{ nd = dot2node(Stat, *pnd, NULLNODE);
				  nd->nd_symb = '(';
				  nd->nd_lineno = (*pnd)->nd_lineno;
				}
		ActualParameters(&(nd->nd_RIGHT))?
	|
		[ BECOMES	
		| %erroneous '='
				{ error("':=' expected instead of '='");
				  DOT = BECOMES;
				}
		]
				{ nd = dot2node(Stat, *pnd, NULLNODE); }
		expression(&(nd->nd_RIGHT))
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
	WHILE		{ *pnd = nd = dot2leaf(Stat); }
	expression(&(nd->nd_LEFT))
	DO
	StatementSequence(&(nd->nd_RIGHT))
	END
|
	REPEAT		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&(nd->nd_LEFT))
	UNTIL
	expression(&(nd->nd_RIGHT))
|
			{ loopcount++; }
	LOOP		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&((*pnd)->nd_RIGHT))
	END
			{ loopcount--; }
|
	ForStatement(pnd)
|
	WITH		{ *pnd = nd = dot2leaf(Stat); }
	designator(&(nd->nd_LEFT))
	DO
	StatementSequence(&(nd->nd_RIGHT))
	END
|
	EXIT
			{ if (!loopcount) error("EXIT not in a LOOP");
			  *pnd = dot2leaf(Stat);
			}
|
	ReturnStatement(pnd)
			{ return_occurred = 1; }
|
	/* empty */	{ *pnd = EmptyStatement; }
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

StatementSequence(register t_node **pnd;)
{
	t_node *nd;
	register t_node *nd1;
} :
	statement(pnd)
	[ %persistent
		';'
		statement(&nd)
			{ if (nd != EmptyStatement) {
			  	nd1 = dot2node(Link, *pnd, nd);
			  	*pnd = nd1;
			  	nd1->nd_symb = ';';
			  	pnd = &(nd1->nd_RIGHT);
			  }
			}
	]*
;

IfStatement(t_node **pnd;)
{
	register t_node *nd;
} :
	IF		{ nd = dot2leaf(Stat);
			  *pnd = nd;
			}
	expression(&(nd->nd_LEFT))
	THEN		{ nd->nd_RIGHT = dot2leaf(Link);
			  nd = nd->nd_RIGHT;
			}
	StatementSequence(&(nd->nd_LEFT))
	[
		ELSIF	{ nd->nd_RIGHT = dot2leaf(Stat);
			  nd = nd->nd_RIGHT;
			  nd->nd_symb = IF;
			}
		expression(&(nd->nd_LEFT))
		THEN	{ nd->nd_RIGHT = dot2leaf(Link);
			  nd = nd->nd_RIGHT;
			}
		StatementSequence(&(nd->nd_LEFT))
	]*
	[
		ELSE
		StatementSequence(&(nd->nd_RIGHT))
	|
	]
	END
;

CaseStatement(t_node **pnd;)
{
	register t_node *nd;
	t_type *tp = 0;
} :
	CASE		{ *pnd = nd = dot2leaf(Stat); }
	expression(&(nd->nd_LEFT))
	OF
	case(&(nd->nd_RIGHT), &tp)
			{ nd = nd->nd_RIGHT; }
	[
		'|'
		case(&(nd->nd_RIGHT), &tp)
			{ nd = nd->nd_RIGHT; }
	]*
	[ ELSE StatementSequence(&(nd->nd_RIGHT))
	|
	]
	END
;

case(t_node **pnd; t_type **ptp;) :
	[ CaseLabelList(ptp, pnd)
	  ':'		{ *pnd = dot2node(Link, *pnd, NULLNODE); }
	  StatementSequence(&((*pnd)->nd_RIGHT))
	|
	]
			{ *pnd = dot2node(Link, *pnd, NULLNODE);
			  (*pnd)->nd_symb = '|';
			}
;

/* inline in statement; lack of space 
WhileStatement(t_node **pnd;)
{
	register t_node *nd;
}:
	WHILE		{ *pnd = nd = dot2leaf(Stat); }
	expression(&(nd->nd_LEFT))
	DO
	StatementSequence(&(nd->nd_RIGHT))
	END
;

RepeatStatement(t_node **pnd;)
{
	register t_node *nd;
}:
	REPEAT		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&(nd->nd_LEFT))
	UNTIL
	expression(&(nd->nd_RIGHT))
;
*/

ForStatement(t_node **pnd;)
{
	register t_node *nd, *nd1;
}:
	FOR		{ *pnd = nd = dot2leaf(Stat); }
	IDENT		{ nd1 = dot2leaf(Name); }
	BECOMES		{ nd->nd_LEFT = dot2node(Stat, nd1, dot2leaf(Link));
			  nd1 = nd->nd_LEFT->nd_RIGHT;
			  nd1->nd_symb = TO;
			}
	expression(&(nd1->nd_LEFT))
	TO
	expression(&(nd1->nd_RIGHT))
			{ nd->nd_RIGHT = nd1 = dot2leaf(Link); 
			  nd1->nd_symb = BY;
			}
	[
		BY
		ConstExpression(&(nd1->nd_LEFT))
			{ if (!(nd1->nd_LEFT->nd_type->tp_fund & T_INTORCARD)) {
				error("illegal type in BY clause");
			  }
			}
	|
			{ nd1->nd_LEFT = dot2leaf(Value);
			  nd1->nd_LEFT->nd_INT = 1;
			}
	]
	DO
	StatementSequence(&(nd1->nd_RIGHT))
	END
;

/* inline in Statement; lack of space
LoopStatement(t_node **pnd;):
	LOOP		{ *pnd = dot2leaf(Stat); }
	StatementSequence(&((*pnd)->nd_RIGHT))
	END
;

WithStatement(t_node **pnd;)
{
	register t_node *nd;
}:
	WITH		{ *pnd = nd = dot2leaf(Stat); }
	designator(&(nd->nd_LEFT))
	DO
	StatementSequence(&(nd->nd_RIGHT))
	END
;
*/

ReturnStatement(t_node **pnd;)
{
	register t_def *df = CurrentScope->sc_definedby;
	register t_type *tp = df->df_type ? ResultType(df->df_type) : 0;
	register t_node *nd;
} :

	RETURN		{ *pnd = nd = dot2leaf(Stat); }
	[
		expression(&(nd->nd_RIGHT))
			{ if (scopeclosed(CurrentScope)) {
error("a module body cannot return a value");
			  }
			  else if (! tp) {
error("procedure \"%s\" is not a function, so cannot return a value", df->df_idf->id_text);
			  }
			}
	|
			{ if (tp) {
error("function procedure \"%s\" must return a value", df->df_idf->id_text);
			  }
			}
	]
;
