/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S T A T E M E N T S */

/* $Header$ */

{
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"

static int	loopcount = 0;	/* Count nested loops */
int		Roption;
extern char	options[];
extern t_node *EmptyStatement;
}

statement(register t_node **pnd;)
{
	register t_node *nd;
	extern int return_occurred;
} :
	/* We need some method for making sure lookahead is done, so ...
	*/
	[	PROGRAM
		/* LLlex never returns this */
	| %default
				{ if (options['R'] != Roption) {
					Roption = options['R'];
					nd = dot2leaf(Option);
					nd->nd_symb = 'R';
					nd->nd_INT = Roption;
					*pnd = nd =
					    dot2node(Link, nd, NULLNODE);
					nd->nd_symb = ';';
					pnd = &(nd->nd_right);
				  }
				}
	]
[
	/*
	 * This part is not in the reference grammar. The reference grammar
	 * states : assignment | ProcedureCall | ...
	 * but this gives LL(1) conflicts
	 */
	designator(pnd)
	[			{ nd = dot2node(Stat, *pnd, NULLNODE);
				  nd->nd_symb = '(';
				}
		ActualParameters(&(nd->nd_right))?
	|
		[ BECOMES	
		| '='		{ error("':=' expected instead of '='");
				  DOT = BECOMES;
				}
		]
				{ nd = dot2node(Stat, *pnd, NULLNODE); }
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
	WHILE		{ *pnd = nd = dot2leaf(Stat); }
	expression(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
|
	REPEAT		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&(nd->nd_left))
	UNTIL
	expression(&(nd->nd_right))
|
			{ loopcount++; }
	LOOP		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&((*pnd)->nd_right))
	END
			{ loopcount--; }
|
	ForStatement(pnd)
|
	WithStatement(pnd)
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

StatementSequence(register t_node **pnd;)
{
	t_node *nd;
	register t_node *nd1;
} :
	statement(pnd)
	[ %persistent
		';'
		statement(&nd)
			{ nd1 = dot2node(Link, *pnd, nd);
			  *pnd = nd1;
			  nd1->nd_symb = ';';
			  pnd = &(nd1->nd_right);
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
	expression(&(nd->nd_left))
	THEN		{ nd->nd_right = dot2leaf(Link);
			  nd = nd->nd_right;
			}
	StatementSequence(&(nd->nd_left))
	[
		ELSIF	{ nd->nd_right = dot2leaf(Stat);
			  nd = nd->nd_right;
			  nd->nd_symb = IF;
			}
		expression(&(nd->nd_left))
		THEN	{ nd->nd_right = dot2leaf(Link);
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

CaseStatement(t_node **pnd;)
{
	register t_node *nd;
	t_type *tp = 0;
} :
	CASE		{ *pnd = nd = dot2leaf(Stat); }
	expression(&(nd->nd_left))
	OF
	case(&(nd->nd_right), &tp)
			{ nd = nd->nd_right; }
	[
		'|'
		case(&(nd->nd_right), &tp)
			{ nd = nd->nd_right; }
	]*
	[ ELSE StatementSequence(&(nd->nd_right))
	]?
	END
;

case(t_node **pnd; t_type **ptp;) :
	[ CaseLabelList(ptp, pnd)
	  ':'		{ *pnd = dot2node(Link, *pnd, NULLNODE); }
	  StatementSequence(&((*pnd)->nd_right))
	]?
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
	expression(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;

RepeatStatement(t_node **pnd;)
{
	register t_node *nd;
}:
	REPEAT		{ *pnd = nd = dot2leaf(Stat); }
	StatementSequence(&(nd->nd_left))
	UNTIL
	expression(&(nd->nd_right))
;
*/

ForStatement(t_node **pnd;)
{
	register t_node *nd, *nd1;
	t_node *dummy;
}:
	FOR		{ *pnd = nd = dot2leaf(Stat); }
	IDENT		{ nd->nd_IDF = dot.TOK_IDF; }
	BECOMES		{ nd->nd_left = nd1 = dot2leaf(Stat); }
	expression(&(nd1->nd_left))
	TO
	expression(&(nd1->nd_right))
	[
		BY
		ConstExpression(&dummy)
			{ if (!(dummy->nd_type->tp_fund & T_INTORCARD)) {
				error("illegal type in BY clause");
			  }
			  nd1->nd_INT = dummy->nd_INT;
			  FreeNode(dummy);
			}
	|
			{ nd1->nd_INT = 1; }
	]
	DO
	StatementSequence(&(nd->nd_right))
	END
;

/* inline in Statement; lack of space
LoopStatement(t_node **pnd;):
	LOOP		{ *pnd = dot2leaf(Stat); }
	StatementSequence(&((*pnd)->nd_right))
	END
;
*/

WithStatement(t_node **pnd;)
{
	register t_node *nd;
}:
	WITH		{ *pnd = nd = dot2leaf(Stat); }
	designator(&(nd->nd_left))
	DO
	StatementSequence(&(nd->nd_right))
	END
;

ReturnStatement(t_node **pnd;)
{
	register t_def *df = CurrentScope->sc_definedby;
	register t_node *nd;
} :

	RETURN		{ *pnd = nd = dot2leaf(Stat); }
	[
		expression(&(nd->nd_right))
			{ if (scopeclosed(CurrentScope)) {
error("a module body has no result value");
			  }
			  else if (! ResultType(df->df_type)) {
error("procedure \"%s\" has no result value", df->df_idf->id_text);
			  }
			}
	|
			{ if (ResultType(df->df_type)) {
error("procedure \"%s\" must return a value", df->df_idf->id_text);
			  }
			}
	]
;
