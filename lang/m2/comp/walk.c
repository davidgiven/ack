/* P A R S E   T R E E   W A L K E R */

/*	Routines to walk through parts of the parse tree, and generate
	code for these parts.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_reg.h>
#include	<em_code.h>
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
#include	"warning.h"

extern arith	NewPtr();
extern arith	NewInt();
extern int	proclevel;
label		text_label;
label		data_label;
static struct type *func_type;
struct withdesig *WithDesigs;
struct node	*Modules;

#define	NO_EXIT_LABEL	((label) 0)
#define RETURN_LABEL	((label) 1)

STATIC
DoProfil()
{
	static label	filename_label = 0;

	if (! options['L']) {
		register label fn_label = filename_label;

		if (!fn_label) {
			filename_label = fn_label = ++data_label;
			C_df_dlb(fn_label);
			C_rom_scon(FileName, (arith) (strlen(FileName) + 1));
		}

		C_fil_dlb(fn_label, (arith) 0);
	}
}

WalkModule(module)
	register struct def *module;
{
	/*	Walk through a module, and all its local definitions.
		Also generate code for its body.
		This code is collected in an initialization routine.
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
	sc->sc_off = 0;		/* no locals (yet) */
	text_label = 1;		/* label at end of initialization routine */
	TmpOpen(sc);		/* Initialize for temporaries */
	C_pro_narg(sc->sc_name);
	DoProfil();
	if (module == Defined) {
		/* Body of implementation or program module.
		   Call initialization routines of imported modules.
		   Also prevent recursive calls of this one.
		*/
		register struct node *nd = Modules;

		if (state == IMPLEMENTATION) {
			label l1 = ++data_label;
			/* we don't actually prevent recursive calls,
			   but do nothing if called recursively
			*/
			C_df_dlb(l1);
			C_bss_cst(word_size, (arith) 0, 1);
			/* if this one is set to non-zero, the initialization
			   was already done.
			*/
			C_loe_dlb(l1, (arith) 0);
			C_zne(RETURN_LABEL);
			C_ine_dlb(l1, (arith) 0);
		}

		for (; nd; nd = nd->next) {
			C_cal(nd->nd_IDF->id_text);
		}
	}
	MkCalls(sc->sc_def);
	proclevel++;
	DO_DEBUG(options['X'], PrNode(module->mod_body, 0));
	WalkNode(module->mod_body, NO_EXIT_LABEL);
	C_df_ilb(RETURN_LABEL);
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
	register struct scope *sc = procedure->prc_vis->sc_scope;
	register struct type *tp;
	register struct paramlist *param;
	label func_res_label = 0;
	arith StackAdjustment = 0;
	arith retsav = 0;
	arith func_res_size = 0;

	proclevel++;
	CurrVis = procedure->prc_vis;

	/* Generate code for all local modules and procedures
	*/
	WalkDef(sc->sc_def);

	/* Generate code for this procedure
	*/
	C_pro_narg(sc->sc_name);
	DoProfil();
	TmpOpen(sc);

	func_type = tp = RemoveEqual(ResultType(procedure->df_type));

	if (tp && IsConstructed(tp)) {
		/* The result type of this procedure is constructed.
		   The actual procedure will return a pointer to a global
		   data area in which the function result is stored.
		   Notice that this does make the code non-reentrant.
		   Here, we create the data area for the function result.
		*/
		func_res_label = ++data_label;
		C_df_dlb(func_res_label);
		C_bss_cst(tp->tp_size, (arith) 0, 0);
	}

	if (tp) func_res_size = WA(tp->tp_size);

	/* Generate calls to initialization routines of modules defined within
	   this procedure
	*/
	MkCalls(sc->sc_def);

	/* Make sure that arguments of size < word_size are on a
	   fixed place.
	   Also make copies of conformant arrays when neccessary.
	*/
	for (param = ParamList(procedure->df_type);
	     param;
	     param = param->next) {
		if (! IsVarParam(param)) {
			tp = TypeOfParam(param);

			if (! IsConformantArray(tp)) {
				if (tp->tp_size < word_size) {
					C_lol(param->par_def->var_off);
					C_lal(param->par_def->var_off);
					C_sti(tp->tp_size);
				}
			}
			else {
				/* Here, we have to make a copy of the
				   array. We must also remember how much
				   room is reserved for copies, because
				   we have to adjust the stack pointer before
				   a RET is done. This is even more complicated
				   when the procedure returns a value.
				   Then, the value must be saved (in retval),
				   the stack adjusted, the return value pushed
				   again, and then RET
				*/
				arith tmpvar = NewInt();

				if (! StackAdjustment) {
					/* First time we get here
					*/
					if (tp && !func_res_label) {
						/* Some local space, only
						   needed if the value itself
						   is returned
						*/
						sc->sc_off -= func_res_size;
						retsav = sc->sc_off;
					}
					StackAdjustment = NewInt();
					C_loc((arith) 0);
					C_stl(StackAdjustment);
				}
				/* First compute the size of the array */
				C_lol(param->par_def->var_off +
				      pointer_size + word_size);
						/* upper - lower */
				C_inc();	/* gives number of elements */
				C_loc(tp->arr_elem->tp_size);
				C_cal("_wa");
				C_asp(dword_size);
				C_lfr(word_size);
						/* size in words */
				C_loc(word_size);
				C_mli(word_size);
						/* size in bytes */
				C_stl(tmpvar);
				C_lol(tmpvar);
				C_lol(tmpvar);
				C_lol(StackAdjustment);
				C_adi(word_size);
				C_stl(StackAdjustment);
						/* remember stack adjustments */
				C_ngi(word_size);
						/* Assumption: stack grows
						   downwards!! ???
						*/
				C_ass(word_size);
						/* adjusted stack pointer */
				C_lor((arith) 1);
						/* destination address (sp),
						   also assumes stack grows
						   downwards ???
						*/
				C_lal(param->par_def->var_off);
				C_loi(pointer_size);
						/* push source address */
				C_exg(pointer_size);
						/* exchange them */
				C_lol(tmpvar);	/* push size */
				C_bls(word_size);
						/* copy */
				C_lor((arith) 1);	
						/* push new address of array
						   ... downwards ... ???
						*/
				C_lal(param->par_def->var_off);
				C_sti(pointer_size);
				FreeInt(tmpvar);
			}
		}
	}

	text_label = 1;		/* label at end of procedure */

	DO_DEBUG(options['X'], PrNode(procedure->prc_body, 0));
	WalkNode(procedure->prc_body, NO_EXIT_LABEL);
	C_df_ilb(RETURN_LABEL);	/* label at end */
	tp = func_type;
	if (func_res_label) {
		/* Fill the data area reserved for the function result
		   with the result
		*/
		C_lae_dlb(func_res_label, (arith) 0);
		C_sti(tp->tp_size);
		if (StackAdjustment) {
			/* Remove copies of conformant arrays
			*/
			C_lol(StackAdjustment);
			C_ass(word_size);
		}
		C_lae_dlb(func_res_label, (arith) 0);
		C_ret(pointer_size);
	}
	else if (tp) {
		if (StackAdjustment) {
			/* First save the function result in a safe place.
			   Then remove copies of conformant arrays,
			   and put function result back on the stack
			*/
			C_lal(retsav);
			C_sti(func_res_size);
			C_lol(StackAdjustment);
			C_ass(word_size);
			C_lal(retsav);
			C_loi(func_res_size);
		}
		C_ret(func_res_size);
	}
	else	{
		if (StackAdjustment) {
			C_lol(StackAdjustment);
			C_ass(word_size);
		}
		C_ret((arith) 0);
	}
	if (StackAdjustment) FreeInt(StackAdjustment);
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

	for ( ; df; df = df->df_nextinscope) {
		switch(df->df_kind) {
		case D_MODULE:
			WalkModule(df);
			break;
		case D_PROCEDURE:
			WalkProcedure(df);
			break;
		case D_VARIABLE:
			if (!proclevel) {
				C_df_dnam(df->var_name);
				C_bss_cst(
					WA(df->df_type->tp_size),
					(arith) 0, 0);
			}
			break;
		default:
			/* nothing */
			;
		}
	}
}

MkCalls(df)
	register struct def *df;
{
	/*	Generate calls to initialization routines of modules
	*/

	for ( ; df; df = df->df_nextinscope) {
		if (df->df_kind == D_MODULE) {
			C_lxl((arith) 0);
			C_cal(df->mod_vis->sc_scope->sc_name);
			C_asp(pointer_size);
		}
	}
}

WalkLink(nd, exit_label)
	register struct node *nd;
	label exit_label;
{
	/*	Walk node "nd", which is a link.
	*/

	while (nd && nd->nd_class == Link) {	 /* statement list */
		WalkNode(nd->nd_left, exit_label);
		nd = nd->nd_right;
	}

	WalkNode(nd, exit_label);
}

WalkCall(nd)
	register struct node *nd;
{
	assert(nd->nd_class == Call);

	if (! options['L']) C_lin((arith) nd->nd_lineno);
	if (ChkCall(nd)) {
		if (nd->nd_type != 0) {
			node_error(nd, "procedure call expected");
			return;
		}
		CodeCall(nd);
	}
}

WalkStat(nd, exit_label)
	register struct node *nd;
	label exit_label;
{
	/*	Walk through a statement, generating code for it.
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
		{	label l1 = ++text_label, l3 = ++text_label;

			ExpectBool(left, l3, l1);
			assert(right->nd_symb == THEN);
			C_df_ilb(l3);
			WalkNode(right->nd_left, exit_label);

			if (right->nd_right) {	/* ELSE part */
				label l2 = ++text_label;

				C_bra(l2);
				C_df_ilb(l1);
				WalkNode(right->nd_right, exit_label);
				C_df_ilb(l2);
			}
			else	C_df_ilb(l1);
			break;
		}

	case CASE:
		CaseCode(nd, exit_label);
		break;

	case WHILE:
		{	label	loop = ++text_label,
				exit = ++text_label,
				dummy = ++text_label;

			C_df_ilb(loop);
			ExpectBool(left, dummy, exit);
			C_df_ilb(dummy);
			WalkNode(right, exit_label);
			C_bra(loop);
			C_df_ilb(exit);
			break;
		}

	case REPEAT:
		{	label loop = ++text_label, exit = ++text_label;

			C_df_ilb(loop);
			WalkNode(left, exit_label);
			ExpectBool(right, exit, loop);
			C_df_ilb(exit);
			break;
		}

	case LOOP:
		{	label loop = ++text_label, exit = ++text_label;

			C_df_ilb(loop);
			WalkNode(right, exit);
			C_bra(loop);
			C_df_ilb(exit);
			break;
		}

	case FOR:
		{
			arith tmp = 0;
			register struct node *fnd;
			int good_forvar;
			label l1 = ++text_label;
			label l2 = ++text_label;

			good_forvar = DoForInit(nd, left);
			fnd = left->nd_right;
			if (fnd->nd_class != Value) {
				/* Upperbound not constant.
				   The expression may only be evaluated once,
				   so generate a temporary for it
				*/
				CodePExpr(fnd);
				tmp = NewInt();
				C_stl(tmp);
			}
			C_df_ilb(l1);
			C_dup(int_size);
			if (tmp) C_lol(tmp); else C_loc(fnd->nd_INT);
			if (left->nd_INT > 0) {
				C_bgt(l2);
			}
			else	C_blt(l2);
			if (good_forvar) {
				RangeCheck(nd->nd_type, int_type);
				CodeDStore(nd);
			}
			WalkNode(right, exit_label);
			if (good_forvar) {	
				CodePExpr(nd);
				C_loc(left->nd_INT);
				C_adi(int_size);
				C_bra(l1);
				C_df_ilb(l2);
				C_asp(int_size);
			}
			if (tmp) FreeInt(tmp);
		}
		break;

	case WITH:
		{
			struct scopelist link;
			struct withdesig wds;
			struct desig ds;

			if (! WalkDesignator(left, &ds)) break;
			if (left->nd_type->tp_fund != T_RECORD) {
				node_error(left, "record variable expected");
				break;
			}

			wds.w_next = WithDesigs;
			WithDesigs = &wds;
			wds.w_scope = left->nd_type->rec_scope;
			CodeAddress(&ds);
			ds.dsg_kind = DSG_FIXED;
			/* Create a designator structure for the temporary.
			*/
			ds.dsg_offset = NewPtr();
			ds.dsg_name = 0;
			CodeStore(&ds, pointer_size);
			ds.dsg_kind = DSG_PFIXED;
			/* the record is indirectly available */
			wds.w_desig = ds;
			link.sc_scope = wds.w_scope;
			link.next = CurrVis;
			CurrVis = &link;
			WalkNode(right, exit_label);
			CurrVis = link.next;
			WithDesigs = wds.w_next;
			FreePtr(ds.dsg_offset);
			break;
		}

	case EXIT:
		assert(exit_label != 0);

		C_bra(exit_label);
		break;

	case RETURN:
		if (right) {
			if (! ChkExpression(right)) break;
			/* The type of the return-expression must be
			   assignment compatible with the result type of the
			   function procedure (See Rep. 9.11).
			*/
			if (!TstAssCompat(func_type, right->nd_type)) {
node_error(right, "type incompatibility in RETURN statement");
				break;
			}
			if (right->nd_type->tp_fund == T_STRING) {
				arith strsize = WA(right->nd_type->tp_size);

				C_zer(WA(func_type->tp_size) - strsize);
				CodePExpr(right);
				C_loi(strsize);
			}
			else	CodePExpr(right);
		}
		C_bra(RETURN_LABEL);
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

	if (!ChkExpression(nd)) return;

	if (nd->nd_type != bool_type && nd->nd_type != error_type) {
		node_error(nd, "boolean expression expected");
	}

	ds = InitDesig;
	CodeExpr(nd, &ds,  true_label, false_label);
}

int
WalkExpr(nd)
	register struct node *nd;
{
	/*	Check an expression and generate code for it
	*/

	if (! ChkExpression(nd)) return 0;

	CodePExpr(nd);
	return 1;
}

int
WalkDesignator(nd, ds)
	struct node *nd;
	struct desig *ds;
{
	/*	Check designator and generate code for it
	*/

	if (! ChkVariable(nd)) return 0;

	*ds = InitDesig;
	CodeDesig(nd, ds);
	return 1;
}

DoForInit(nd, left)
	register struct node *nd, *left;
{
	register struct def *df;

	nd->nd_left = nd->nd_right = 0;
	nd->nd_class = Name;
	nd->nd_symb = IDENT;

	if (!( ChkVariable(nd) &
	       WalkExpr(left->nd_left) &
	       ChkExpression(left->nd_right))) return 0;

	df = nd->nd_def;
	if (df->df_kind == D_FIELD) {
		node_error(nd,
			   "FOR-loop variable may not be a field of a record");
		return 1;
	}

	if (!df->var_name && df->var_off >= 0) {
		node_error(nd, "FOR-loop variable may not be a parameter");
		return 1;
	}

	if (df->df_scope != CurrentScope) {
		register struct scopelist *sc = CurrVis;

		for (;;) {
			if (!sc) {
				node_error(nd,
				      "FOR-loop variable may not be imported");
				return 1;
			}
			if (sc->sc_scope == df->df_scope) break;
			sc = nextvisible(sc);
		}
	}

	if (df->df_type->tp_size > word_size ||
	    !(df->df_type->tp_fund & T_DISCRETE)) {
		node_error(nd, "illegal type of FOR loop variable");
		return 1;
	}

	if (!TstCompat(df->df_type, left->nd_left->nd_type) ||
	    !TstCompat(df->df_type, left->nd_right->nd_type)) {
		if (!TstAssCompat(df->df_type, left->nd_left->nd_type) ||
		    !TstAssCompat(df->df_type, left->nd_right->nd_type)) {
			node_error(nd, "type incompatibility in FOR statement");
			return 1;
		}
node_warning(nd, W_OLDFASHIONED, "compatibility required in FOR statement");
	}

	return 1;
}

DoAssign(nd, left, right)
	struct node *nd;
	register struct node *left, *right;
{
	/* May we do it in this order (expression first) ???
	   The reference manual sais nothing about it, but the book does:
	   it sais that the left hand side is evaluated first.
	   DAMN THE BOOK!
	*/
	struct desig dsl, dsr;
	register struct type *rtp, *ltp;

	if (! (ChkExpression(right) & ChkVariable(left))) return;
	rtp = right->nd_type;
	ltp = left->nd_type;

	if (right->nd_symb == STRING) TryToString(right, ltp);
	dsr = InitDesig;

	if (! TstAssCompat(ltp, rtp)) {
		node_error(nd, "type incompatibility in assignment");
		return;
	}

	CodeExpr(right, &dsr, NO_LABEL, NO_LABEL);
	if (complex(rtp)) CodeAddress(&dsr);
	else {
		CodeValue(&dsr, rtp->tp_size);
		RangeCheck(ltp, rtp);
		CodeCoercion(rtp, ltp);
	}
	dsl = InitDesig;
	CodeDesig(left, &dsl);

	/*	Generate code for an assignment. Testing of type
		compatibility and the like is already done.
	*/

	if (dsr.dsg_kind == DSG_LOADED) {
		if (rtp->tp_fund == T_STRING) {
			CodeAddress(&dsl);
			C_loc(rtp->tp_size);
			C_loc(ltp->tp_size);
			C_cal("_StringAssign");
			C_asp((int_size << 1) + (pointer_size << 1));
			return;
		}
		CodeStore(&dsl, ltp->tp_size);
		return;
	}
	CodeAddress(&dsl);
	C_blm(ltp->tp_size);
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
				 (tp->tp_fund & (T_POINTER|T_HIDDEN|T_EQUAL))) {
				C_ms_reg(df->var_off, pointer_size,
					 reg_pointer, 0);
			}
			else if (tp->tp_fund & T_NUMERIC) {
				C_ms_reg(df->var_off,
					 tp->tp_size,
					 tp->tp_fund == T_REAL ?
					    reg_float : reg_any,
					 0);
			}
		}
	}
}
