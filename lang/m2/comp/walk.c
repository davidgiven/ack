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
#include	"Lpars.h"

#include	"debug.h"

extern arith	align();
static int	prclev = 0;
static label	instructionlabel = 0;
static label	datalabel = 0;
static label	return_label;
static char	return_expr_occurred;
static struct type *func_type;

WalkModule(module)
	register struct def *module;
{
	/*	Walk through a module, and all its local definitions.
		Also generate code for its body.
	*/
	register struct def *df = module->mod_vis->sc_scope->sc_def;
	struct scopelist *vis;

	vis = CurrVis;
	CurrVis = module->mod_vis;

	if (!prclev && module->mod_number) {
		/* This module is a local module, but not within a
		   procedure. Generate code to allocate storage for its
		   variables. This is done by generating a "bss",
		   with label "_<modulenumber><modulename>".
		*/
		arith size = align(CurrentScope->sc_off, word_align);

		if (size == 0) size = word_size;
		C_df_dnam(&(CurrentScope->sc_name[1]));
		C_bss_cst(size, (arith) 0, 0);
	}
	else if (CurrVis == Defined->mod_vis) {
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
	instructionlabel = 1;
	return_label = instructionlabel++;
	func_type = 0;
	C_pro_narg(CurrentScope->sc_name);
	MkCalls(CurrentScope->sc_def);
	WalkNode(module->mod_body, (label) 0);
	C_df_ilb(return_label);
	C_ret((label) 0);
	C_end(align(-CurrentScope->sc_off, word_align));

	CurrVis = vis;
}

WalkProcedure(procedure)
	struct def *procedure;
{
	/*	Walk through the definition of a procedure and all its
		local definitions
	*/
	struct scopelist *vis = CurrVis;

	prclev++;
	CurrVis = procedure->prc_vis;
	
	WalkDef(CurrentScope->sc_def);

	/* Generate code for this procedure
	*/
	C_pro_narg(CurrentScope->sc_name);
	/* generate calls to initialization routines of modules defined within
	   this procedure
	*/
	return_label = 1;
	instructionlabel = 2;
	func_type = procedure->df_type->next;
	MkCalls(CurrentScope->sc_def);
	WalkNode(procedure->prc_body, (label) 0);
	C_df_ilb(return_label);
	if (func_type) C_ret((arith) align(func_type->tp_size, word_align));
	else C_ret((arith) 0);
	C_end(align(-CurrentScope->sc_off, word_size));
	CurrVis = vis;
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
			C_cal(df->mod_vis->sc_scope->sc_name);
		}
		df = df->df_nextinscope;
	}
}

WalkNode(nd, lab)
	register struct node *nd;
	label lab;
{
	/*	Node "nd" represents either a statement or a statement list.
		Walk through it.
		"lab" represents the label that must be jumped to on
		encountering an EXIT statement.
	*/

	while (nd->nd_class == Link) {	 /* statement list */
		WalkStat(nd->nd_left, lab);
		nd = nd->nd_right;
	}

	WalkStat(nd, lab);
}

WalkStat(nd, lab)
	register struct node *nd;
	label lab;
{
	/*	Walk through a statement, generating code for it.
		"lab" represents the label that must be jumped to on
		encountering an EXIT statement.
	*/
	register struct node *left = nd->nd_left;
	register struct node *right = nd->nd_right;

	if (nd->nd_class == Call) {
		if (chk_call(nd)) {
			/* ??? */
		}
		return;
	}

	assert(nd->nd_class == Stat);

	switch(nd->nd_symb) {
	case BECOMES:
		WalkDesignator(left);
		WalkExpr(right);

		if (! TstAssCompat(left->nd_type, right->nd_type)) {
			node_error(nd, "type incompatibility in assignment");
			break;
		}
		/* ??? */
		break;

	case IF:
		{	label l1, l2;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			ExpectBool(left);
			assert(right->nd_symb == THEN);
			C_zeq(l1);
			WalkNode(right->nd_left, lab);

			if (right->nd_right) {	/* ELSE part */
				C_bra(l2);
				C_df_ilb(l1);
				WalkNode(right->nd_right, lab);
				C_df_ilb(l2);
			}
			else	C_df_ilb(l1);
			break;
		}

	case CASE:
		{
			WalkExpr(left);

			while (right) {
				if (right->nd_class == Link && right->nd_symb == '|') {
					WalkNode(right->nd_left->nd_right, lab);
					right = right->nd_right;
				}
				else	{
					WalkNode(right, lab);
					right = 0;
				}
			}

			/* ??? */
			break;
		}

	case WHILE:
		{	label l1, l2;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			C_df_ilb(l1);
			ExpectBool(left);
			C_zeq(l2);
			WalkNode(right, lab);
			C_bra(l1);
			C_df_ilb(l2);
			break;
		}

	case REPEAT:
		{	label l1;

			l1 = instructionlabel++;
			C_df_ilb(l1);
			WalkNode(left, lab);
			ExpectBool(right);
			C_zeq(l1);
			break;
		}

	case LOOP:
		{	label l1, l2;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			C_df_ilb(l1);
			WalkNode(left, l2);
			C_bra(l1);
			C_df_ilb(l2);
			break;
		}

	case FOR:
		/* ??? */
		WalkNode(right, lab);
		break;

	case WITH:
		{
			struct scopelist link;

			WalkDesignator(left);
			if (left->nd_type->tp_fund != T_RECORD) {
				node_error(left, "record variable expected");
				break;
			}

			link.sc_scope = left->nd_type->rec_scope;
			link.next = CurrVis;
			CurrVis = &link;
			WalkNode(right, lab);
			CurrVis = link.next;
			/* ??? */
			break;
		}

	case EXIT:
		assert(lab != 0);

		C_bra(lab);
		break;

	case RETURN:
		if (right) {
			WalkExpr(right);
			/* What kind of compatibility do we need here ???
			   assignment compatibility?
			*/
			if (!TstAssCompat(func_type, right->nd_type)) {
node_error(right, "type incompatibility in RETURN statement");
			}
		}
		C_bra(return_label);
		break;

	default:
		assert(0);
	}
}

ExpectBool(nd)
	struct node *nd;
{
	/*	"nd" must indicate a boolean expression. Check this and
		generate code to evaluate the expression.
	*/

	WalkExpr(nd);

	if (nd->nd_type != bool_type && nd->nd_type != error_type) {
		node_error(nd, "boolean expression expected");
	}
}

WalkExpr(nd)
	struct node *nd;
{
	/*	Check an expression and generate code for it
	*/

	DO_DEBUG(1, (DumpTree(nd), print("\n")));

	if (chk_expr(nd)) {
		/* ??? */
	}
}

WalkDesignator(nd)
	struct node *nd;
{
	/*	Check designator and generate code for it
	*/

	DO_DEBUG(1, (DumpTree(nd), print("\n")));

	if (chk_designator(nd, DESIGNATOR|VARIABLE)) {
		/* ??? */
	}
}

#ifdef DEBUG
DumpTree(nd)
	struct node *nd;
{
	char *s;
	extern char *symbol2str();
	
	if (!nd) {
		print("()");
		return;
	}

	print("(");
	DumpTree(nd->nd_left);
	switch(nd->nd_class) {
	case Def:	s = "Def"; break;
	case Oper:	s = "Oper"; break;
	case Uoper:	s = "Uoper"; break;
	case Name:	s = "Name"; break;
	case Set:	s = "Set"; break;
	case Value:	s = "Value"; break;
	case Call:	s = "Call"; break;
	case Xset:	s = "Xset"; break;
	case Stat:	s = "Stat"; break;
	case Link:	s = "Link"; break;
	default:	s = "ERROR"; break;
	}
	print("%s %s", s, symbol2str(nd->nd_symb));
	DumpTree(nd->nd_right);
	print(")");
}
#endif
