/* The grammar of ISO-Pascal as given by the specification, BS6192: 1982. */

{
#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<stb.h>

#include	"LLlex.h"
#include	"def.h"
#include	"f_info.h"
#include	"idf.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
#include	"dbsymtab.h"
}

%lexical LLlex;

%start LLparse, Program;

/* ISO section 6.10, p. 137 */
Program
{
	struct def *df;
	arith dummy;
}:
	ProgramHeading(&df)
	';' Block(df) '.'
	  {
#ifdef DBSYMTAB
	    if (options['g']) {
		C_ms_stb_cst(df->df_idf->id_text, N_MAIN, 0, (arith) 0);
		stb_string(df, D_END);
	    }
#endif /* DBSYMTAB */
	  }
	| { df = new_def();
	    df->df_idf = str2idf(FileName, 1);
	    df->df_kind = D_MODULE;
	    open_scope();
	    GlobalScope = CurrentScope;
	    df->prc_vis = CurrVis;
	  }

	  Module(df, &dummy)
;

ProgramHeading(register struct def **df;):
	PROGRAM IDENT
			{ program = *df = new_def();
			  (*df)->df_idf = dot.TOK_IDF;
			  (*df)->df_kind = D_PROGRAM;
			  open_scope();
			  GlobalScope = CurrentScope;
			  (*df)->prc_vis = CurrVis;
#ifdef DBSYMTAB
	  		  if (options['g']) stb_string(*df, D_MODULE);
#endif /* DBSYMTAB */
			}
	[
		'('
		ProgramParameters
		')'
				{ make_extfl(); }
	]?
;

ProgramParameters
{
	struct node *Proglist;
}:
	IdentifierList(&Proglist)
				{ EnterProgList(Proglist); }
;
