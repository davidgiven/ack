/* The grammar of ISO-Pascal as given by the specification, BS6192: 1982. */

{
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
}

%lexical LLlex;

%start LLparse, Program;

/* ISO section 6.10, p. 137 */
Program
{
	struct def *df;
}:
	ProgramHeading(&df) ';' Block(df) '.'
;

ProgramHeading(register struct def **df;):
	PROGRAM IDENT
			{ program = *df = new_def();
			  (*df)->df_idf = dot.TOK_IDF;
			  (*df)->df_kind = D_PROGRAM;
			  open_scope();
			  GlobalScope = CurrentScope;
			  (*df)->prc_vis = CurrVis;
			}
	[
		'('
		ProgramParameters
		')'
	]?
;

ProgramParameters
{
	struct node *Proglist;
}:
	IdentifierList(&Proglist)
				{ EnterProgList(Proglist); }
;
