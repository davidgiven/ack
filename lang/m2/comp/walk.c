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
#include	<em_reg.h>
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
#include	"chk_expr.h"
#include	"walk.h"

extern arith	NewPtr();
extern arith	NewInt();
extern int	proclevel;
label		text_label;
label		data_label;
static struct type *func_type;
struct withdesig *WithDesigs;
struct node	*Modules;
struct scope	*ProcScope;

STATIC
DoProfil()
{
	static label	filename_label = 0;

	if (! options['L']) {
		if (!filename_label) {
			filename_label = ++data_label;
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
	register struct scope *sc;
	struct scopelist *savevis = CurrVis;

	CurrVis = module->mod_vis;
	sc = CurrentScope;

	/* Walk through it's local definitions
	*/
	WalkDef(sc->sc_def);

	/* Now, generate initialization code for this module.
	   First call initialization routines for modules defined within
	   this module.
	*/
	sc->sc_off = 0;
	text_label = 1;
	ProcScope = sc;	
	C_pro_narg(sc->sc_name);
	DoProfil();
	if (module == Defined) {
		/* Body of implementation or program module.
		   Call initialization routines of imported modules.
		   Also prevent recursive calls of this one.
		*/
		register struct node *nd;

		if (state == IMPLEMENTATION) {
			label l1 = ++data_label;
			/* we don't actually prevent recursive calls,
			   but do nothing if called recursively
			*/
			C_df_dlb(l1);
			C_bss_cst(word_size, (arith) 0, 1);
			C_loe_dlb(l1, (arith) 0);
			C_zne((label) 1);
			C_loc((arith) 1);
			C_ste_dlb(l1, (arith) 0);
		}

		for (nd = Modules; nd; nd = nd->next) {
			C_cal(nd->nd_IDF->id_text);
		}
	}
	MkCalls(sc->sc_def);
	proclevel++;
	DO_DEBUG(options['X'], PrNode(module->mod_body, 0));
	WalkNode(module->mod_body, (label) 0);
	C_df_ilb((label) 1);
	C_ret((arith) 0);
	C_end(-sc->sc_off);
	proclevel--;
	TmpClose();

	CurrVis = savevis;
}

WalkProcedure(procedure)
	register struct def *procedure;
{
	/*	Walk through the definition of a procedure and all its
		local definitions, checking and generating code.
	*/
	struct scopelist *savevis = CurrVis;
	register struct scope *sc;
	register struct type *tp;
	register struct paramlist *param;
	label func_res_label = 0;

	proclevel++;
	CurrVis = procedure->prc_vis;
	ProcScope = sc = CurrentScope;

	/* Generate code for all local modules and procedures
	*/
	WalkDef(sc->sc_def);

	/* Generate code for this procedure
	*/
	C_pro_narg(sc->sc_name);
	DoProfil();

	/* Generate calls to initialization routines of modules defined within
	   this procedure
	*/
	MkCalls(sc->sc_def);

	/* Make sure that arguments of size < word_size are on a
	   fixed place.
	*/
	for (param = ParamList(procedure->df_type);
	     param;
	     param = param->next) {
		if (! IsVarParam(param)) {
			tp = TypeOfParam(param);

			if (!IsConformantArray(tp) && tp->tp_size < word_size) {
				C_lol(param->par_def->var_off);
				C_lal(param->par_def->var_off);
				C_sti(tp->tp_size);
			}
		}
	}

	text_label = 1;
	func_type = tp = ResultType(procedure->df_type);

	if (IsConstructed(tp)) {
		func_res_label = ++data_label;
		C_df_dlb(func_res_label);
		C_bss_cst(tp->tp_size, (arith) 0, 0);
	}

	DO_DEBUG(options['X'], PrNode(procedure->prc_body, 0));
	WalkNode(procedure->prc_body, (label) 0);
	C_ret((arith) 0);
	if (tp) {
		C_df_ilb((label) 1);
		if (func_res_label) {
			C_lae_dlb(func_res_label, (arith) 0);
			C_sti(tp->tp_size);
			C_lae_dlb(func_res_label, (arith) 0);
			C_ret(pointer_size);
		}
		else	C_ret(WA(tp->tp_size));
	}

	if (! options['n']) RegisterMessages(sc->sc_def);
	C_end(-sc->sc_off);
	TmpClose();
	CurrVis = savevis;
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
		else if (!proclevel && df->df_kind == D_VARIABLE) {
			C_df_dnam(df->var_name);
			C_bss_cst(
				WA(df->df_type->tp_size),
				(arith) 0, 0);
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

WalkLink(nd, lab)
	register struct node *nd;
	label lab;
{
	/*	Walk node "nd", which is a link.
		"lab" represents the label that must be jumped to on
		encountering an EXIT statement.
	*/

	while (nd && nd->nd_class == Link) {	 /* statement list */
		WalkNode(nd->nd_left, lab);
		nd = nd->nd_right;
	}

	WalkNode(nd, lab);
}

WalkCall(nd)
	register struct node *nd;
{
	assert(nd->nd_class == Call);

	if (! options['L']) C_lin((arith) nd->nd_lineno);
	if (chk_call(nd)) {
		if (nd->nd_type != 0) {
			node_error(nd, "procedure call expected");
			return;
		}
		CodeCall(nd);
	}
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

	assert(nd->nd_class == Stat);

	if (! options['L']) C_lin((arith) nd->nd_lineno);
	switch(nd->nd_symb) {
	case BECOMES:
		DoAssign(nd, left, right);
		break;

	case IF:
		{	label l1, l2, l3;

			l1 = ++text_label;
			l2 = ++text_label;
			l3 = ++text_label;
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

			l1 = ++text_label;
			l2 = ++text_label;
			l3 = ++text_label;
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

			l1 = ++text_label;
			l2 = ++text_label;
			C_df_ilb(l1);
			WalkNode(left, lab);
			ExpectBool(right, l2, l1);
			C_df_ilb(l2);
			break;
		}

	case LOOP:
		{	label l1, l2;

			l1 = ++text_label;
			l2 = ++text_label;
			C_df_ilb(l1);
			WalkNode(right, l2);
			C_bra(l1);
			C_df_ilb(l2);
			break;
		}

	case FOR:
		{
			arith tmp = 0;
			struct node *fnd;
			label l1 = ++text_label;
			label l2 = ++text_label;

			if (! DoForInit(nd, left)) break;
			fnd = left->nd_right;
			if (fnd->nd_class != Value) {
				CodePExpr(fnd);
				tmp = NewInt();
				C_stl(tmp);
			}
			C_bra(l1);
			C_df_ilb(l2);
			CheckAssign(nd->nd_type, int_type);
			CodeDStore(nd);
			WalkNode(right, lab);
			CodePExpr(nd);
			C_loc(left->nd_INT);
			C_adi(int_size);
			C_df_ilb(l1);
			C_dup(int_size);
			if (tmp) C_lol(tmp); else C_loc(fnd->nd_INT);
			if (left->nd_INT > 0) {
				C_ble(l2);
			}
			else	C_bge(l2);
			C_asp(int_size);
			if (tmp) FreeInt(tmp);
		}
		break;

	case WITH:
		{
			struct scopelist link;
			struct withdesig wds;
			struct desig ds;
			arith tmp = 0;

			WalkDesignator(left, &ds);
			if (left->nd_type->tp_fund != T_RECORD) {
				node_error(left, "record variable expected");
				break;
			}

			wds.w_next = WithDesigs;
			WithDesigs = &wds;
			wds.w_scope = left->nd_type->rec_scope;
			if (ds.dsg_kind != DSG_PFIXED) {
				/* In this case, we use a temporary variable
				*/
				CodeAddress(&ds);
				ds.dsg_kind = DSG_FIXED;
				/* Create a designator structure for the
				   temporary.
				*/
				ds.dsg_offset = tmp = NewPtr();
				ds.dsg_name = 0;
				CodeStore(&ds, pointer_size);
				ds.dsg_kind = DSG_PFIXED;
				/* the record is indirectly available */
			}
			wds.w_desig = ds;
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
			WalkExpr(right);
			/* The type of the return-expression must be
			   assignment compatible with the result type of the
			   function procedure (See Rep. 9.11).
			*/
			if (!TstAssCompat(func_type, right->nd_type)) {
node_error(right, "type incompatibility in RETURN statement");
			}
			C_bra((label) 1);
		}
		else	C_ret((arith) 0);
		break;

	default:
		crash("(WalkStat)");
	}
}

extern int	NodeCrash();

int (*WalkTable[])() = {
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	WalkCall,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	WalkStat,
	WalkLink,
	NodeCrash
};

ExpectBool(nd, true_label, false_label)
	register struct node *nd;
	label true_label, false_label;
{
	/*	"nd" must indicate a boolean expression. Check this and
		generate code to evaluate the expression.
	*/
	struct desig ds;

	if (!chk_expr(nd)) return;

	if (nd->nd_type != bool_type && nd->nd_type != error_type) {
		node_error(nd, "boolean expression expected");
	}

	ds = InitDesig;
	CodeExpr(nd, &ds,  true_label, false_label);
}

WalkExpr(nd)
	struct node *nd;
{
	/*	Check an expression and generate code for it
	*/

	if (! chk_expr(nd)) return;

	CodePExpr(nd);
}

WalkDesignator(nd, ds)
	struct node *nd;
	struct desig *ds;
{
	/*	Check designator and generate code for it
	*/

	if (! chk_variable(nd)) return;

	*ds = InitDesig;
	CodeDesig(nd, ds);
}

DoForInit(nd, left)
	register struct node *nd, *left;
{
	register struct def *df;

	nd->nd_left = nd->nd_right = 0;
	nd->nd_class = Name;
	nd->nd_symb = IDENT;

	if (! chk_variable(nd) ||
	    ! chk_expr(left->nd_left) ||
	    ! chk_expr(left->nd_right)) return 0;

	df = nd->nd_def;
	if (df->df_kind == D_FIELD) {
		node_error(nd, "FOR-loop variable may not be a field of a record");
		return 0;
	}

	if (!df->var_name && df->var_off >= 0) {
		node_error(nd, "FOR-loop variable may not be a parameter");
		return 0;
	}

	if (df->df_scope != CurrentScope) {
		register struct scopelist *sc = CurrVis;

		while (sc && sc->sc_scope != df->df_scope) {
			sc = nextvisible(sc);
		}

		if (!sc) {
			node_error(nd, "FOR-loop variable may not be imported");
			return 0;
		}
	}

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

	return 1;
}

DoAssign(nd, left, right)
	struct node *nd;
	register struct node *left, *right;
{
	/* May we do it in this order (expression first) ??? */
	struct desig dsl, dsr;

	if (!chk_expr(right)) return;
	if (! chk_variable(left)) return;
	TryToString(right, left->nd_type);
	dsr = InitDesig;
	CodeExpr(right, &dsr, NO_LABEL, NO_LABEL);

	if (! TstAssCompat(left->nd_type, right->nd_type)) {
		node_error(nd, "type incompatibility in assignment");
		return;
	}

	if (complex(right->nd_type)) {
		CodeAddress(&dsr);
	}
	else {
		CodeValue(&dsr, right->nd_type->tp_size);
		CheckAssign(left->nd_type, right->nd_type);
	}
	dsl = InitDesig;
	CodeDesig(left, &dsl);

	CodeAssign(nd, &dsr, &dsl);
}

RegisterMessages(df)
	register struct def *df;
{
	register struct type *tp;

	for (; df; df = df->df_nextinscope) {
		if (df->df_kind == D_VARIABLE && !(df->df_flags & D_NOREG)) {
			/* Examine type and size
			*/
			tp = BaseType(df->df_type);
			if ((df->df_flags & D_VARPAR) ||
				 tp->tp_fund == T_POINTER) {
				C_ms_reg(df->var_off, pointer_size,
					 reg_pointer, 0);
			}
			else if ((tp->tp_fund & T_NUMERIC) &&
			     tp->tp_size <= dword_size) {
				C_ms_reg(df->var_off,
					 tp->tp_size,
					 tp->tp_fund == T_REAL ?
					    reg_float : reg_any,
					 0);
			}
		}
	}
}
