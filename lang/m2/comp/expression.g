{
static char *RcsId = "$Header$";
}

number:
	INTEGER
|
	REAL
;

qualident:
	IDENT selector*
;

selector:
	'.' /* field */ IDENT
;

ExpList:
	expression [ ',' expression ]*
;

ConstExpression:
	expression
	/*
	 * Changed rule in new Modula-2.
	 * Check that the expression is a constant expression and evaluate!
	 */
;

expression:
	SimpleExpression [ relation SimpleExpression ]?
;

relation:
	'=' | '#' | UNEQUAL | '<' | LESSEQUAL | '>' | GREATEREQUAL | IN
;

SimpleExpression:
	[ '+' | '-' ]? term [ AddOperator term ]*
;

AddOperator:
	'+' | '-' | OR
;

term:
	factor [ MulOperator factor ]*
;

MulOperator:
	'*' | '/' | DIV | MOD | AND | '&'
;

factor:
	qualident
	[
		designator_tail? ActualParameters?
	|
		bare_set
	]
|
	bare_set
| %default
	number
|
	STRING
|
	'(' expression ')'
|
	NOT factor
;

bare_set:
	'{' [ element [ ',' element ]* ]? '}'
;

ActualParameters:
	'(' ExpList? ')'
;

element:
	expression [ UPTO expression ]?
;

designator:
	qualident designator_tail?
;

designator_tail:
	visible_designator_tail
	[ selector | visible_designator_tail ]*
;

visible_designator_tail:
	'[' ExpList ']' | '^'
;
