/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S T A T E M E N T S */

/* stripped down version from the one in the Modula-2 compiler */

/* $Id$ */

{
#include	"idf.h"
#include	"LLlex.h"

static int loopcount;
}

statement :
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
		[ BECOMES	
		| %erroneous
		  '='		{ error("':=' expected instead of '='");
				  DOT = BECOMES;
				}
		]
		expression
	]
	/*
	 * end of changed part
	 */
|
	IfStatement
|
	CaseStatement
|
	WHILE
	expression
	DO
	StatementSequence
	END
|
	REPEAT
	StatementSequence
	UNTIL
	expression
|
				{ loopcount++; }
	LOOP
	StatementSequence
	END
				{ loopcount--; }
|
	ForStatement
|
	WithStatement
|
	EXIT
			{ if (!loopcount) error("EXIT not in a LOOP"); }
|
	ReturnStatement
|
	/* empty */
]
;

StatementSequence :
	statement
	[ %persistent
		';'
		statement
	]*
;

IfStatement :
	IF expression
	THEN StatementSequence
	[
		ELSIF expression
		THEN StatementSequence
	]*
	[
		ELSE StatementSequence
	]?
	END
;

CaseStatement :
	CASE expression
	OF case
	[
		'|' case
	]*
	[ ELSE StatementSequence
	]?
	END
;

case :
	[ CaseLabelList ':'
	  StatementSequence
	]?
;

ForStatement :
	FOR IDENT BECOMES expression TO expression
	[
		BY ConstExpression
	|
	]
	DO StatementSequence
	END
;

WithStatement :
	WITH designator DO StatementSequence
	END
;

ReturnStatement :
	RETURN
	[
		expression
	|
	]
;
