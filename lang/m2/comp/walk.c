/* P A R S E   T R E E   W A L K E R */

static char *RcsId = "$Header$";

/*	Routines to walk through parts of the parse tree, and generate
	code for these parts.
*/

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"main.h"
#include	"LLlex.h"
#include	"node.h"

#include	"debug.h"

extern arith	align();
static int	prclev = 0;

WalkModule(module)
	register struct def *module;
{
	/*	Walk through a module, and all its local definitions.
		Also generate code for its body.
	*/
	register struct def *df = module->mod_scope->sc_def;
	struct scope *scope;

	scope = CurrentScope;
	CurrentScope = module->mod_scope;
	if (!prclev && module->mod_number) {
		/* This module is a local module, but not within a
		   procedure. Generate code to allocate storage for its
		   variables
		*/
		arith size = align(CurrentScope->sc_off, word_size);

		if (size == 0) size = word_size;
		C_df_dnam(&(CurrentScope->sc_name[1]));
		C_bss_cst(size, (arith) 0, 0);
	}
	else if (CurrentScope == Defined->mod_scope) {
		/* This module is the module currently being compiled.
		   Again, generate code to allocate storage for its
		   variables, which all have an explicit name.
		*/
		while (df) {
			if (df->df_kind == D_VARIABLE) {
				C_df_dnam(df->var_name);
				C_bss_cst(df->df_type->tp_size, (arith) 0, 0);
			}
			df = df->df_nextinscope;
		}
	}

	/* Now, walk through it's local definitions
	*/
	WalkDef(CurrentScope->sc_def);

	/* Now, generate initialization code for this module.
	   First call initialization routines for modules defined within
	   this module.
	*/
	CurrentScope->sc_off = 0;
	C_pro_narg(CurrentScope->sc_name);
	MkCalls(CurrentScope->sc_def);
	WalkNode(module->mod_body);
	C_end(align(-CurrentScope->sc_off, word_size));

	CurrentScope = scope;
}

WalkProcedure(procedure)
	struct def *procedure;
{
	/*	Walk through the definition of a procedure and all its
		local definitions
	*/
	struct scope *scope = CurrentScope;
	register struct def *df;

	prclev++;
	CurrentScope = procedure->prc_scope;
	
	WalkDef(CurrentScope->sc_def);

	/* Generate code for this procedure
	*/
	C_pro_narg(procedure->prc_name);
	/* generate calls to initialization routines of modules defined within
	   this procedure
	*/
	MkCalls(CurrentScope->sc_def);
	WalkNode(procedure->prc_body);
	C_end(align(-CurrentScope->sc_off, word_size));
	CurrentScope = scope;
	prclev--;
}

WalkDef(df)
	register struct def *df;
{
	/*	Walk through a list of definitions
	*/
	while (df) {
		if (df->df_kind == D_MODULE) {
			WalkModule(df);
		}
		else if (df->df_kind == D_PROCEDURE) {
			WalkProcedure(df);
		}
		df = df->df_nextinscope;
	}
}

MkCalls(df)
	register struct def *df;
{
	/*	Generate calls to initialization routines of modules
	*/
	while (df) {
		if (df->df_kind == D_MODULE) {
			C_lxl((arith) 0);
			C_cal(df->df_scope->sc_name);
		}
		df = df->df_nextinscope;
	}
}

WalkNode(nd)
	struct node *nd;
{
	/*	Node "nd" represents either a statement or a statement list.
		Generate code for it.
	*/
	/* ??? */
}
