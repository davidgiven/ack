/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N S */

/* stripped down version of the one in the Modula-2 compiler */

/* $Id$ */

qualident :
	IDENT
	[
		selector
	]*
;

selector :
	'.' IDENT
;

ExpList :
	expression
	[
		',' expression
	]*
;

ConstExpression :
	expression
	/*
	 * Changed rule in new Modula-2.
	 */
;

expression :
	SimpleExpression
	[
		/* relation */
		[ '=' | '#' | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN ]
		SimpleExpression
	|
		/* empty */
	]
;

SimpleExpression :
	[
		'+' 
	|
		'-'
	|
		/* empty */
	]
	term
	[
		/* AddOperator */
		[ '+' | '-' | OR ] term
	]*
;

term :
	factor
	[
		/* MulOperator */
		[ '*' | '/' | DIV | MOD | AND ] factor
	]*
;

factor :
	qualident
	[
		designator_tail?
		[
			ActualParameters
		|
			/* empty */
		]
	|
		bare_set
	]
|
	bare_set
| %default
	[ %default
		INTEGER
	|
		REAL
	|
		STRING
	]
|
	'(' expression ')'
|
	NOT factor
;

bare_set :
	'{'
	[
		element
		[
			',' element
		]*
	|
		/* empty */
	]
	'}'
;

ActualParameters :
	'(' ExpList? ')'
;

element :
	expression
	[
		UPTO expression
	|
		/* empty */
	]
;

designator :
	qualident designator_tail?
;

designator_tail :
	visible_designator_tail
	[ %persistent
	    %default
		selector
	|
		visible_designator_tail
	]*
;

visible_designator_tail :
	'['
		expression
		[
			',' expression
		]*
	']'
|
	'^'
;
