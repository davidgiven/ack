/* P A R S E   T R E E   W A L K E R */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

/*	Routines to walk through parts of the parse tree, and generate
	code for these parts.
*/

#include	"debug.h"

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
#include	"desig.h"
#include	"f_info.h"
#include	"idf.h"

extern arith	NewPtr();
extern arith	NewInt();
extern int	proclevel;
static label	instructionlabel;
static char	return_expr_occurred;
static struct type *func_type;
struct withdesig *WithDesigs;
struct node	*Modules;

label
text_label()
{
	return instructionlabel++;
}

label
data_label()
{
	static label	datalabel = 0;

	return ++datalabel;
}

static
DoProfil()
{
	static label	filename_label = 0;

	if (! options['L']) {
		if (!filename_label) {
			filename_label = data_label();
			C_df_dlb(filename_label);
			C_rom_scon(FileName, (arith) (strlen(FileName) + 1));
		}

		C_fil_dlb(filename_label, (arith) 0);
	}
}

WalkModule(module)
	register struct def *module;
{
	/*	Walk through a module, and all its local definitions.
		Also generate code for its body.
	*/
	register struct def *df = module->mod_vis->sc_scope->sc_def;
	register struct scope *sc;
	struct scopelist *vis;

	vis = CurrVis;
	CurrVis = module->mod_vis;
	sc = CurrentScope;

	if (!proclevel && module != Defined) {
		/* This module is a local module, but not within a
		   procedure. Generate code to allocate storage for its
		   variables. This is done by generating a "bss",
		   with label "_<modulenumber><modulename>".
		*/
		arith size = align(sc->sc_off, word_align);

		if (size == 0) size = word_size;
		/* WHY ??? because we generated an INA for it ??? */

		C_df_dnam(&(sc->sc_name[1]));
		size = align(size, word_align);
		C_bss_cst(size, (arith) 0, 0);
		C_exp(sc->sc_name);
	}
	else if (CurrVis == Defined->mod_vis) {
		/* This module is the module currently being compiled.
		   Again, generate code to allocate storage for its
		   variables, which all have an explicit name.
		*/
		while (df) {
			if (df->df_kind == D_VARIABLE) {
				C_df_dnam(df->var_name);
				C_bss_cst(
					align(df->df_type->tp_size, word_align),
					(arith) 0, 0);
			}
			df = df->df_nextinscope;
		}
		if (state == PROGRAM) C_exp("main");
		else C_exp(sc->sc_name);
	}

	/* Now, walk through it's local definitions
	*/
	WalkDef(sc->sc_def);

	/* Now, generate initialization code for this module.
	   First call initialization routines for modules defined within
	   this module.
	*/
	sc->sc_off = 0;
	instructionlabel = 2;
	func_type = 0;
	C_pro_narg(state == PROGRAM ? "main" : sc->sc_name);
	DoProfil();
	if (CurrVis == Defined->mod_vis) {
		/* Body of implementation or program module.
		   Call initialization routines of imported modules.
		   Also prevent recursive calls of this one.
		*/
		struct node *nd;

		if (state == IMPLEMENTATION) {
			label l1 = data_label(), l2 = text_label();
			/* we don't actually prevent recursive calls,
			   but do nothing if called recursively
			*/
			C_df_dlb(l1);
			C_bss_cst(word_size, (arith) 0, 1);
			C_loe_dlb(l1, (arith) 0);
			C_zeq(l2);
			C_ret((arith) 0);
			C_df_ilb(l2);
			C_loc((arith) 1);
			C_ste_dlb(l1, (arith) 0);
		}

		nd = Modules;
		while (nd) {
			C_cal(nd->nd_IDF->id_text);
			nd = nd->next;
		}
	}
	MkCalls(sc->sc_def);
	proclevel++;
	WalkNode(module->mod_body, (label) 0);
	C_df_ilb((label) 1);
	C_ret((arith) 0);
	C_end(-sc->sc_off);
	proclevel--;
	TmpClose();

	CurrVis = vis;
}

WalkProcedure(procedure)
	register struct def *procedure;
{
	/*	Walk through the definition of a procedure and all its
		local definitions
	*/
	struct scopelist *vis = CurrVis;
	register struct scope *sc;
	register struct type *res_type;

	proclevel++;
	CurrVis = procedure->prc_vis;
	sc = CurrentScope;
	
	WalkDef(sc->sc_def);

	/* Generate code for this procedure
	*/
	C_pro_narg(sc->sc_name);
	DoProfil();
	/* generate calls to initialization routines of modules defined within
	   this procedure
	*/
	MkCalls(sc->sc_def);
	return_expr_occurred = 0;
	instructionlabel = 2;
	func_type = res_type = procedure->df_type->next;
	if (! returntype(res_type)) {
		node_error(procedure->prc_body, "illegal result type");
	}
	WalkNode(procedure->prc_body, (label) 0);
	C_df_ilb((label) 1);
	if (res_type) {
		if (! return_expr_occurred) {
node_error(procedure->prc_body,"function procedure does not return a value");
		}
		C_ret(align(res_type->tp_size, word_align));
	}
	else	C_ret((arith) 0);
	C_end(-sc->sc_off);
	TmpClose();
	CurrVis = vis;
	proclevel--;
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
			C_asp(pointer_size);
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
	struct node *nd;
	label lab;
{
	/*	Walk through a statement, generating code for it.
		"lab" represents the label that must be jumped to on
		encountering an EXIT statement.
	*/
	register struct node *left = nd->nd_left;
	register struct node *right = nd->nd_right;
	register struct desig *pds = &Desig;

	if (!nd) {
		/* Empty statement
		*/
		return;
	}

	if (! options['L']) C_lin((arith) nd->nd_lineno);

	if (nd->nd_class == Call) {
		if (chk_call(nd)) {
			if (nd->nd_type != 0) {
				node_error(nd, "procedure call expected");
				return;
			}
			CodeCall(nd);
		}
		return;
	}

	assert(nd->nd_class == Stat);

	switch(nd->nd_symb) {
	case BECOMES:
		DoAssign(nd, left, right);
		break;

	case IF:
		{	label l1, l2, l3;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			l3 = instructionlabel++;
			ExpectBool(left, l3, l1);
			assert(right->nd_symb == THEN);
			C_df_ilb(l3);
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
		CaseCode(nd, lab);
		break;

	case WHILE:
		{	label l1, l2, l3;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			l3 = instructionlabel++;
			C_df_ilb(l1);
			ExpectBool(left, l3, l2);
			C_df_ilb(l3);
			WalkNode(right, lab);
			C_bra(l1);
			C_df_ilb(l2);
			break;
		}

	case REPEAT:
		{	label l1, l2;

			l1 = instructionlabel++;
			l2 = instructionlabel++;
			C_df_ilb(l1);
			WalkNode(left, lab);
			ExpectBool(right, l2, l1);
			C_df_ilb(l2);
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
		{
			arith tmp = 0;
			struct node *fnd;
			label l1 = instructionlabel++;
			label l2 = instructionlabel++;
			arith size;

			if (! DoForInit(nd, left)) break;
			fnd = left->nd_right;
			size = fnd->nd_type->tp_size;
			if (fnd->nd_class != Value) {
				CodePExpr(fnd);
				tmp = NewInt();
				C_stl(tmp);
			}
			C_bra(l1);
			C_df_ilb(l2);
			WalkNode(right, lab);
			C_loc(left->nd_INT);
			CodePExpr(nd);
			C_adi(int_size);
			CodeDStore(nd);
			C_df_ilb(l1);
			CodePExpr(nd);
			if (tmp) C_lol(tmp); else C_loc(fnd->nd_INT);
			if (left->nd_INT > 0) {
				C_ble(l2);
			}
			else	C_bge(l2);
			if (tmp) FreeInt(tmp);
		}
		break;

	case WITH:
		{
			struct scopelist link;
			struct withdesig wds;
			arith tmp = 0;

			WalkDesignator(left);
			if (left->nd_type->tp_fund != T_RECORD) {
				node_error(left, "record variable expected");
				break;
			}

			wds.w_next = WithDesigs;
			WithDesigs = &wds;
			wds.w_scope = left->nd_type->rec_scope;
			if (pds->dsg_kind != DSG_PFIXED) {
				/* In this case, we use a temporary variable
				*/
				CodeAddress(pds);
				pds->dsg_kind = DSG_FIXED;
				/* Only for the store ... */
				pds->dsg_offset = tmp = NewPtr();
				pds->dsg_name = 0;
				CodeStore(pds, pointer_size);
				pds->dsg_kind = DSG_PFIXED;
				/* the record is indirectly available */
			}
			wds.w_desig = *pds;
			link.sc_scope = wds.w_scope;
			link.next = CurrVis;
			CurrVis = &link;
			WalkNode(right, lab);
			CurrVis = link.next;
			WithDesigs = wds.w_next;
			if (tmp) FreePtr(tmp);
			break;
		}

	case EXIT:
		assert(lab != 0);

		C_bra(lab);
		break;

	case RETURN:
		if (right) {
			WalkExpr(right, NO_LABEL, NO_LABEL);
			/* Assignment compatibility? Yes, see Rep. 9.11
			*/
			if (!TstAssCompat(func_type, right->nd_type)) {
node_error(right, "type incompatibility in RETURN statement");
			}
			return_expr_occurred = 1;
		}
		C_bra((label) 1);
		break;

	default:
		assert(0);
	}
}

ExpectBool(nd, true_label, false_label)
	register struct node *nd;
	label true_label, false_label;
{
	/*	"nd" must indicate a boolean expression. Check this and
		generate code to evaluate the expression.
	*/

	WalkExpr(nd, true_label, false_label);

	if (nd->nd_type != bool_type && nd->nd_type != error_type) {
		node_error(nd, "boolean expression expected");
	}
}

WalkExpr(nd, true_label, false_label)
	struct node *nd;
	label true_label, false_label;
{
	/*	Check an expression and generate code for it
	*/

	DO_DEBUG(1, (DumpTree(nd), print("\n")));

	if (! chk_expr(nd)) return;

	Desig = InitDesig;
	CodeExpr(nd, &Desig, true_label, false_label);
}

WalkDesignator(nd)
	struct node *nd;
{
	/*	Check designator and generate code for it
	*/

	DO_DEBUG(1, (DumpTree(nd), print("\n")));

	if (! chk_designator(nd, DESIGNATOR|VARIABLE, D_DEFINED)) return;

	Desig = InitDesig;
	CodeDesig(nd, &Desig);
}

DoForInit(nd, left)
	register struct node *nd, *left;
{

	nd->nd_left = nd->nd_right = 0;
	nd->nd_class = Name;
	nd->nd_symb = IDENT;

	if (! chk_designator(nd, VARIABLE, D_DEFINED) ||
	    ! chk_expr(left->nd_left) ||
	    ! chk_expr(left->nd_right)) return;

	if (nd->nd_type->tp_size > word_size ||
	    !(nd->nd_type->tp_fund & T_DISCRETE)) {
		node_error(nd, "illegal type of FOR loop variable");
		return 0;
	}

	if (!TstCompat(nd->nd_type, left->nd_left->nd_type) ||
	    !TstCompat(nd->nd_type, left->nd_right->nd_type)) {
		if (!TstAssCompat(nd->nd_type, left->nd_left->nd_type) ||
		    !TstAssCompat(nd->nd_type, left->nd_right->nd_type)) {
			node_error(nd, "type incompatibility in FOR statement");
			return 0;
		}
node_warning(nd, "old-fashioned! compatibility required in FOR statement");
	}

	CodePExpr(left->nd_left);
	CodeDStore(nd);
}

DoAssign(nd, left, right)
	struct node *nd;
	register struct node *left, *right;
{
	/* May we do it in this order (expression first) ??? */
	struct desig ds;

	if (!chk_expr(right)) return;
	if (! chk_designator(left, DESIGNATOR|VARIABLE, D_DEFINED)) return;
	TryToString(right, left->nd_type);
	Desig = InitDesig;
	CodeExpr(right, &Desig, NO_LABEL, NO_LABEL);

	if (! TstAssCompat(left->nd_type, right->nd_type)) {
		node_error(nd, "type incompatibility in assignment");
		return;
	}

	if (complex(right->nd_type)) {
		CodeAddress(&Desig);
	}
	else {
		CodeValue(&Desig, right->nd_type->tp_size);
		CheckAssign(left->nd_type, right->nd_type);
	}
	ds = Desig;
	Desig = InitDesig;
	CodeDesig(left, &Desig);

	CodeAssign(nd, &ds, &Desig);
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
