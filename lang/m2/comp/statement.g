/* S T A T E M E N T S */

{
static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	"LLlex.h"
#include	"node.h"
}

statement
{
	struct node *nd1, *nd2;
} :
[
	/*
	 * This part is not in the reference grammar. The reference grammar
	 * states : assignment | ProcedureCall | ...
	 * but this gives LL(1) conflicts
	 */
	designator(&nd1)
	[
		ActualParameters(&nd2)?
	|
		BECOMES expression(&nd2)
	]
	/*
	 * end of changed part
	 */
|
	IfStatement
|
	CaseStatement
|
	WhileStatement
|
	RepeatStatement
|
	LoopStatement
|
	ForStatement
|
	WithStatement
|
	EXIT
|
	RETURN
	[
		expression(&nd1)
	]?
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

StatementSequence:
	statement [ ';' statement ]*
;

IfStatement
{
	struct node *nd1;
} :
	IF expression(&nd1) THEN StatementSequence
	[ ELSIF expression(&nd1) THEN StatementSequence ]*
	[ ELSE StatementSequence ]?
	END
;

CaseStatement
{
	struct node *nd;
} :
	CASE expression(&nd) OF case [ '|' case ]*
	[ ELSE StatementSequence ]?
	END
;

case:
	[ CaseLabelList ':' StatementSequence ]?
				/* This rule is changed in new modula-2 */
;

WhileStatement
{
	struct node *nd;
}:
	WHILE expression(&nd) DO StatementSequence END
;

RepeatStatement
{
	struct node *nd;
}:
	REPEAT StatementSequence UNTIL expression(&nd)
;

ForStatement
{
	struct node *nd1, *nd2, *nd3;
}:
	FOR IDENT
	BECOMES expression(&nd1)
	TO expression(&nd2)
	[ BY ConstExpression(&nd3) ]?
	DO StatementSequence END
;

LoopStatement:
	LOOP StatementSequence END
;

WithStatement
{
	struct node *nd;
}:
	WITH designator(&nd) DO StatementSequence END
;
