/* E X P R E S S I O N S */

{
static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"scope.h"
}

number:
	INTEGER
|
	REAL
;

qualident(int types; struct def **pdf; char *str;)
{
	int scope;
	int  module;
	register struct def *df;
	struct def *lookfor();
} :
	IDENT		{ if (types) {
				df = lookfor(dot.TOK_IDF, CurrentScope, 1);
				*pdf = df;
				if (df->df_kind == D_ERROR) types = 0;
			  }
			}
	[
			{ if (types &&!(scope = has_selectors(df))) {
				types = 0;
				*pdf = ill_df;
			  }
			}
		/* selector */
		'.' IDENT
			{ if (types) {
				module = (df->df_kind == D_MODULE);
				df = lookup(dot.TOK_IDF, scope);
				if (!df) {
					error("identifier \"%s\" not declared",
					      dot.TOK_IDF->id_text);
					types = 0;
					df = ill_df;
				}
				else
				if (module &&
				    !(df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
					error("identifier \"%s\" not exported from qualifying module", dot.TOK_IDF->id_text);
				}
			  }
			}
	]*
			{ if (types && !(types & df->df_kind)) {
				error("identifier \"%s\" is not a %s",
					df->df_idf->id_text, str);
			  }
			}
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

factor
{
	struct def *df;
} :
	qualident(0, &df, (char *) 0)
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

designator
{
	struct def *df;
} :
	qualident(0, &df, (char *) 0)
	designator_tail?
;

designator_tail:
	visible_designator_tail
	[
		selector
	|
		visible_designator_tail
	]*
;

visible_designator_tail:
	'[' ExpList ']'
|
	'^'
;
