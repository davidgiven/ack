/* S T A T E M E N T S */

{
static char *RcsId = "$Header$";
}

statement:
[
	/*
	 * This part is not in the reference grammar. The reference grammar
	 * states : assignment | ProcedureCall | ...
	 * but this gives LL(1) conflicts
	 */
	designator
	[
		ActualParameters?
	|
		BECOMES expression
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
	RETURN expression?
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

IfStatement:
	IF expression THEN StatementSequence
	[ ELSIF expression THEN StatementSequence ]*
	[ ELSE StatementSequence ]?
	END
;

CaseStatement:
	CASE expression OF case [ '|' case ]*
	[ ELSE StatementSequence ]?
	END
;

case:
	[ CaseLabelList ':' StatementSequence ]?
				/* This rule is changed in new modula-2 */
;

WhileStatement:
	WHILE expression DO StatementSequence END
;

RepeatStatement:
	REPEAT StatementSequence UNTIL expression
;

ForStatement:
	FOR IDENT
	BECOMES expression
	TO expression
	[ BY ConstExpression ]?
	DO StatementSequence END
;

LoopStatement:
	LOOP StatementSequence END
;

WithStatement:
	WITH designator DO StatementSequence END
;
