/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* P A R S E   T R E E   W A L K E R */

/* $Header$ */

/*	Routines to walk through parts of the parse tree, and generate
	code for these parts.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_reg.h>
#include	<em_code.h>
#include	<m2_traps.h>
#include	<assert.h>
#include	<alloc.h>

#include	"strict3rd.h"
#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"main.h"
#include	"node.h"
#include	"Lpars.h"
#include	"desig.h"
#include	"f_info.h"
#include	"idf.h"
#include	"chk_expr.h"
#include	"walk.h"
#include	"misc.h"
#include	"warning.h"

extern arith		NewPtr();
extern arith		NewInt();

extern int		proclevel;

label			text_label;
label			data_label = 1;
struct withdesig	*WithDesigs;
t_node			*Modules;

static t_type		*func_type;
static t_node		*priority;
static int		oldlineno;

static int		RegisterMessage();
static int		WalkDef();
static int		MkCalls();
static int		UseWarnings();

#define	NO_EXIT_LABEL	((label) 0)
#define RETURN_LABEL	((label) 1)

LblWalkNode(lbl, nd, exit)
	label lbl, exit;
	register t_node *nd;
{
	/*	Generate code for node "nd", after generating instruction
		label "lbl". "exit" is the exit label for the closest
		enclosing LOOP.
	*/

	def_ilb(lbl);
	WalkNode(nd, exit);
}

static arith tmpprio;

STATIC
DoPriority()
{
	/*	For the time being (???), handle priorities by calls to
		the runtime system
	*/
	if (priority) {
		tmpprio = NewInt();
		C_loc(priority->nd_INT);
		CAL("stackprio", (int) word_size);
		C_lfr(word_size);
		C_stl(tmpprio);
	}
}

STATIC
EndPriority()
{
	if (priority) {
		C_lol(tmpprio);
		CAL("unstackprio", (int) word_size);
		FreeInt(tmpprio);
	}
}

def_ilb(l)
	label l;
{
	C_df_ilb(l);
	oldlineno = 0;
}

DoLineno(nd)
	register t_node *nd;
{
	if (! options['L'] && nd->nd_lineno && nd->nd_lineno != oldlineno) {
		oldlineno = nd->nd_lineno;
		C_lin((arith) nd->nd_lineno);
	}
}

DoFilename()
{
	static label	filename_label = 0;

	oldlineno = 0;
	if (! options['L']) {

		if (! filename_label) {
			filename_label = 1;
			C_df_dlb((label) 1);
			C_rom_scon(FileName, (arith) (strlen(FileName) + 1));
		}

		C_fil_dlb((label) 1, (arith) 0);
	}
}

WalkModule(module)
	register t_def *module;
{
	/*	Walk through a module, and all its local definitions.
		Also generate code for its body.
		This code is collected in an initialization routine.
	*/
	register t_scope *sc;
	t_scopelist *savevis = CurrVis;

	CurrVis = module->mod_vis;
	priority = module->mod_priority;
	sc = CurrentScope;

	/* Walk through it's local definitions
	*/
	WalkDefList(sc->sc_def, WalkDef);

	/* Now, generate initialization code for this module.
	   First call initialization routines for modules defined within
	   this module.
	*/
	sc->sc_off = 0;		/* no locals (yet) */
	text_label = 1;		/* label at end of initialization routine */
	TmpOpen(sc);		/* Initialize for temporaries */
	C_pro_narg(sc->sc_name);
	DoPriority();
	if (module == Defined) {
		/* Body of implementation or program module.
		   Call initialization routines of imported modules.
		   Also prevent recursive calls of this one.
		*/
		register t_node *nd = Modules;

		if (state == IMPLEMENTATION) {
			/* We don't actually prevent recursive calls,
			   but do nothing if called recursively
			*/
			C_df_dlb(++data_label);
			C_con_cst((arith) 0);
			/* if this one is set to non-zero, the initialization
			   was already done.
			*/
			C_loe_dlb(data_label, (arith) 0);
			C_zne(RETURN_LABEL);
			C_ine_dlb(data_label, (arith) 0);
		}

		for (; nd; nd = nd->nd_left) {
			C_cal(nd->nd_def->mod_vis->sc_scope->sc_name);
		}
		DoFilename();
	}
	WalkDefList(sc->sc_def, MkCalls);
	proclevel++;
	WalkNode(module->mod_body, NO_EXIT_LABEL);
	DO_DEBUG(options['X'], PrNode(module->mod_body, 0));
	def_ilb(RETURN_LABEL);
	EndPriority();
	C_ret((arith) 0);
	C_end(-sc->sc_off);
	proclevel--;
	TmpClose();

	CurrVis = savevis;
	WalkDefList(sc->sc_def, UseWarnings);
}

WalkProcedure(procedure)
	register t_def *procedure;
{
	/*	Walk through the definition of a procedure and all its
		local definitions, checking and generating code.
	*/
	t_scopelist *savevis = CurrVis;
	register t_scope *procscope = procedure->prc_vis->sc_scope;
	register t_type *tp;
	register t_param *param;
	label func_res_label = 0;
	arith StackAdjustment = 0;
	arith retsav = 0;
	arith func_res_size = 0;

	proclevel++;
	CurrVis = procedure->prc_vis;

	/* Generate code for all local modules and procedures
	*/
	WalkDefList(procscope->sc_def, WalkDef);

	/* Generate code for this procedure
	*/
	C_pro_narg(procscope->sc_name);
	C_ms_par(procedure->df_type->prc_nbpar);
	TmpOpen(procscope);
	DoPriority();
	DoFilename();		/* ??? only when this procedure is exported? */

	func_type = tp = RemoveEqual(ResultType(procedure->df_type));

	if (tp) {
		func_res_size = WA(tp->tp_size);
		if (IsConstructed(tp)) {
			/* The result type of this procedure is constructed.
			   The actual procedure will return a pointer to a
			   global data area in which the function result is
			   stored.
			   Notice that this does make the code non-reentrant.
			   Here, we create the data area for the function
			   result.
			*/
			func_res_label = ++data_label;
			C_df_dlb(func_res_label);
			C_bss_cst(func_res_size, (arith) 0, 0);
		}
	}

	/* Generate calls to initialization routines of modules defined within
	   this procedure
	*/
	WalkDefList(procscope->sc_def, MkCalls);

	/* Make sure that arguments of size < word_size are on a
	   fixed place.
	   Also make copies of conformant arrays when neccessary.
	*/
	for (param = ParamList(procedure->df_type);
	     param;
	     param = param->par_next) {
		if (! IsVarParam(param)) {
			tp = TypeOfParam(param);

			if (! IsConformantArray(tp)) {
				if (tp->tp_size < word_size &&
				    (int) word_size % (int) tp->tp_size == 0) {
					C_lol(param->par_def->var_off);
					STL(param->par_def->var_off, tp->tp_size);
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
				if (! StackAdjustment) {
					/* First time we get here
					*/
					if (func_type && !func_res_label) {
						/* Some local space, only
						   needed if the value itself
						   is returned
						*/
						procscope->sc_off -=
							func_res_size;
						retsav = procscope->sc_off;
					}
					StackAdjustment = NewPtr();
					C_lor((arith) 1);
					STL(StackAdjustment, pointer_size);
				}
				/* First compute new stackpointer */
				C_lal(param->par_def->var_off);
				CAL("new_stackptr", (int)pointer_size);
				C_lfr(pointer_size);
				C_ass(pointer_size);
						/* adjusted stack pointer */
				LOL(param->par_def->var_off, pointer_size);
						/* push source address */
				CAL("copy_array", (int)pointer_size);
						/* copy */
			}
		}
	}

	text_label = 1;		/* label at end of procedure */

	WalkNode(procedure->prc_body, NO_EXIT_LABEL);
	DO_DEBUG(options['X'], PrNode(procedure->prc_body, 0));
	if (func_res_size) {
		c_loc(M2_NORESULT);
		C_trp();
		C_asp(-func_res_size);
	}
	def_ilb(RETURN_LABEL);	/* label at end */
	if (func_res_label) {
		/* Fill the data area reserved for the function result
		   with the result
		*/
		c_lae_dlb(func_res_label);
		C_sti(func_res_size);
		if (StackAdjustment) {
			/* Remove copies of conformant arrays
			*/
			LOL(StackAdjustment, pointer_size);
			C_str((arith) 1);
		}
		c_lae_dlb(func_res_label);
		func_res_size = pointer_size;
	}
	else if (StackAdjustment) {
		/* First save the function result in a safe place.
		   Then remove copies of conformant arrays,
		   and put function result back on the stack
		*/
		if (func_type) {
			STL(retsav, func_res_size);
		}
		LOL(StackAdjustment, pointer_size);
		C_str((arith) 1);
		if (func_type) {
			LOL(retsav, func_res_size);
		}
	}
	EndPriority();
	C_ret(func_res_size);
	if (! options['n']) WalkDefList(procscope->sc_def, RegisterMessage);
	C_end(-procscope->sc_off);
	if (! fit(procscope->sc_off, (int) word_size)) {
		node_error(procedure->prc_body, "maximum local byte count exceeded");
	}
	TmpClose();
	CurrVis = savevis;
	proclevel--;
	WalkDefList(procscope->sc_def, UseWarnings);
}

static int
WalkDef(df)
	register t_def *df;
{
	/*	Walk through a list of definitions
	*/

	switch(df->df_kind) {
	case D_MODULE:
		WalkModule(df);
		break;
	case D_PROCEDURE:
		WalkProcedure(df);
		break;
	case D_VARIABLE:
		if (!proclevel  && !(df->df_flags & D_ADDRGIVEN)) {
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

static int
MkCalls(df)
	register t_def *df;
{
	/*	Generate calls to initialization routines of modules
	*/

	if (df->df_kind == D_MODULE) {
		C_lxl((arith) 0);
		CAL(df->mod_vis->sc_scope->sc_name, (int)pointer_size);
	}
}

WalkLink(nd, exit_label)
	register t_node *nd;
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

STATIC
ForLoopVarExpr(nd)
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	CodePExpr(nd);
	CodeCoercion(tp, BaseType(tp));
}

WalkStat(nd, exit_label)
	register t_node *nd;
	label exit_label;
{
	/*	Walk through a statement, generating code for it.
	*/
	register t_node *left = nd->nd_left;
	register t_node *right = nd->nd_right;

	assert(nd->nd_class == Stat);

	DoLineno(nd);
	options['R'] = (nd->nd_flags & ROPTION);
	options['A'] = (nd->nd_flags & AOPTION);
	switch(nd->nd_symb) {
	case '(':
		if (ChkCall(nd)) {
			if (nd->nd_type != 0) {
				node_error(nd, "procedure call expected instead of function call");
				break;
			}
			CodeCall(nd);
		}
		break;

	case ';':
		break;

	case BECOMES:
		DoAssign(left, right);
		break;

	case IF:
		{	label l1 = ++text_label, l3 = ++text_label;

			ExpectBool(left, l3, l1);
			assert(right->nd_symb == THEN);
			LblWalkNode(l3, right->nd_left, exit_label);

			if (right->nd_right) {	/* ELSE part */
				label l2 = ++text_label;

				C_bra(l2);
				LblWalkNode(l1, right->nd_right, exit_label);
				l1 = l2;
			}
			def_ilb(l1);
			break;
		}

	case CASE:
		CaseCode(nd, exit_label);
		break;

	case WHILE:
		{	label	loop = ++text_label,
				exit = ++text_label,
				dummy = ++text_label;

			def_ilb(loop);
			ExpectBool(left, dummy, exit);
			LblWalkNode(dummy, right, exit_label);
			C_bra(loop);
			def_ilb(exit);
			break;
		}

	case REPEAT:
		{	label loop = ++text_label, exit = ++text_label;

			LblWalkNode(loop, left, exit_label);
			ExpectBool(right, exit, loop);
			def_ilb(exit);
			break;
		}

	case LOOP:
		{	label loop = ++text_label, exit = ++text_label;

			LblWalkNode(loop, right, exit);
			C_bra(loop);
			def_ilb(exit);
			break;
		}

	case FOR:
		{
			arith tmp = NewInt();
			arith tmp2 = 0;
			register t_node *fnd;
			int good_forvar;
			label l1 = ++text_label;
			label l2 = ++text_label;
			int uns = 0;
			arith stepsize;
			t_type *bstp;

			good_forvar = DoForInit(nd);
			if ((stepsize = left->nd_INT) == 0) {
				node_warning(left,
					     W_ORDINARY,
					     "zero stepsize in FOR loop");
			}
			fnd = left->nd_right;
			if (good_forvar) {
				bstp = BaseType(nd->nd_type);
				uns = bstp->tp_fund != T_INTEGER;
				C_dup(int_size);
				CodeDStore(nd);
				CodePExpr(fnd);
				C_stl(tmp);
				C_lol(tmp);
				if (uns) C_cmu(int_size);
				else C_cmi(int_size);
				if (left->nd_INT >= 0) {
					C_zgt(l2);
					C_lol(tmp);
					ForLoopVarExpr(nd);
				}
				else {
					stepsize = -stepsize;
					C_zlt(l2);
					ForLoopVarExpr(nd);
					C_lol(tmp);
				}
				C_sbu(int_size);
				if (stepsize) {
					C_loc(stepsize);
					C_dvu(int_size);
				}
				C_stl(tmp);
				nd->nd_def->df_flags |= D_FORLOOP;
				def_ilb(l1);
				if (! options['R']) {
					tmp2 = NewInt();
					ForLoopVarExpr(nd);
					C_stl(tmp2);
				}
			}
			WalkNode(right, exit_label);
			nd->nd_def->df_flags &= ~D_FORLOOP;
			if (good_forvar) {
				if (tmp2 != 0) {
					label x = ++text_label;
					C_lol(tmp2);
					ForLoopVarExpr(nd);
					C_beq(x);
					c_loc(M2_FORCH);
					C_trp();
					def_ilb(x);
					FreeInt(tmp2);
				}
				if (stepsize) {
					C_lol(tmp);
					C_zeq(l2);
					C_lol(tmp);
					c_loc(1);
					C_sbu(int_size);
					C_stl(tmp);
					C_loc(left->nd_INT);
					ForLoopVarExpr(nd);
					C_adu(int_size);
					RangeCheck(nd->nd_type, bstp);
					CodeDStore(nd);
				}
			}
			C_bra(l1);
			def_ilb(l2);
			FreeInt(tmp);
#ifdef DEBUG
			nd->nd_left = left;
			nd->nd_right = right;
#endif
		}
		break;

	case WITH:
		{
			t_scopelist link;
			struct withdesig wds;
			t_desig ds;

			if (! WalkDesignator(left, &ds, D_USED|D_DEFINED)) break;
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
			CodeStore(&ds, address_type);
			ds.dsg_kind = DSG_PFIXED;
			/* the record is indirectly available */
			wds.w_desig = ds;
			link.sc_scope = wds.w_scope;
			link.sc_next = CurrVis;
			CurrVis = &link;
			WalkNode(right, exit_label);
			CurrVis = link.sc_next;
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
			if (!ChkAssCompat(&(nd->nd_right), func_type, "RETURN")) {
				break;
			}
			right = nd->nd_right;
			if (right->nd_type->tp_fund == T_STRING) {
				CodePString(right, func_type);
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
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	WalkStat,
	WalkLink,
};

ExpectBool(nd, true_label, false_label)
	register t_node *nd;
	label true_label, false_label;
{
	/*	"nd" must indicate a boolean expression. Check this and
		generate code to evaluate the expression.
	*/
	register t_desig *ds = new_desig();

	if (ChkExpression(nd)) {
		if (nd->nd_type != bool_type && nd->nd_type != error_type) {
			node_error(nd, "boolean expression expected");
		}

		CodeExpr(nd, ds,  true_label, false_label);
	}
	free_desig(ds);
}

int
WalkDesignator(nd, ds, flags)
	t_node *nd;
	t_desig *ds;
{
	/*	Check designator and generate code for it
	*/

	if (! ChkVariable(nd, flags)) return 0;

	clear((char *) ds, sizeof(t_desig));
	CodeDesig(nd, ds);
	return 1;
}

DoForInit(nd)
	register t_node *nd;
{
	register t_node *left = nd->nd_left;
	register t_def *df;
	t_type *tpl, *tpr;

	nd->nd_left = nd->nd_right = 0;
	nd->nd_class = Name;
	nd->nd_symb = IDENT;

	if (!( ChkVariable(nd, D_USED|D_DEFINED) &
	       ChkExpression(left->nd_left) &
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
		register t_scopelist *sc = CurrVis;

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

	tpl = left->nd_left->nd_type;
	tpr = left->nd_right->nd_type;
#ifndef STRICT_3RD_ED
	if (! options['3']) {
	  if (!ChkAssCompat(&(left->nd_left), df->df_type, "FOR statement") ||
	      !ChkAssCompat(&(left->nd_right), BaseType(df->df_type), "FOR statement")) {
		return 1;
	  }
	  if (!TstCompat(df->df_type, tpl) ||
	      !TstCompat(df->df_type, tpr)) {
node_warning(nd, W_OLDFASHIONED, "compatibility required in FOR statement");
	  }
	} else
#endif
	if (!ChkCompat(&(left->nd_left), df->df_type, "FOR statement") ||
	    !ChkCompat(&(left->nd_right), BaseType(df->df_type), "FOR statement")) {
		return 1;
	}

	CodePExpr(left->nd_left);
	return 1;
}

DoAssign(left, right)
	register t_node *left;
	t_node *right;
{
	/* May we do it in this order (expression first) ???
	   The reference manual sais nothing about it, but the book does:
	   it sais that the left hand side is evaluated first.
	   DAMN THE BOOK!
	*/
	register t_desig *dsr;
	register t_type *tp;

	if (! (ChkExpression(right) & ChkVariable(left, D_DEFINED))) return;
	tp = left->nd_type;

	if (right->nd_symb == STRING) TryToString(right, tp);

	if (! ChkAssCompat(&right, tp, "assignment")) {
		return;
	}
	dsr = new_desig();

#define StackNeededFor(ds)	((ds)->dsg_kind == DSG_PLOADED \
				  || (ds)->dsg_kind == DSG_INDEXED)
	CodeExpr(right, dsr, NO_LABEL, NO_LABEL);
	tp = right->nd_type;
	if (complex(tp)) {
		if (StackNeededFor(dsr)) CodeAddress(dsr);
	}
	else {
		CodeValue(dsr, tp);
	}
	CodeMove(dsr, left, tp);
	free_desig(dsr);
}

static int
RegisterMessage(df)
	register t_def *df;
{
	register t_type *tp;
	arith sz;
	int regtype;

	if (df->df_kind == D_VARIABLE) {
		if ( !(df->df_flags & D_NOREG)) {
			/* Examine type and size
			*/
			regtype = -1;
			tp = BaseType(df->df_type);
			if ((df->df_flags & D_VARPAR) ||
			    (tp->tp_fund&(T_POINTER|T_HIDDEN|T_EQUAL))) {
				sz = pointer_size;
				regtype = reg_pointer;
			}
			else if (tp->tp_fund & T_NUMERIC) {
				sz = tp->tp_size;
				regtype = tp->tp_fund == T_REAL ?
					    reg_float : reg_any;
			}
			if (regtype >= 0) {
				C_ms_reg(df->var_off, sz, regtype, 0);
			}
		}
	}
}

static int
UseWarnings(df)
	register t_def *df;
{
	char *warning = 0;

	if (is_anon_idf(df->df_idf) ||
	    !(df->df_kind&(D_IMPORTED|D_VARIABLE|D_PROCEDURE|D_CONST|D_TYPE)) ||
	    (df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
		return;
	}

	if (df->df_kind & D_IMPORTED) {
		register t_def *df1 = df->imp_def;

		df1->df_flags |= df->df_flags & (D_USED|D_DEFINED);
		if (df->df_kind == D_INUSE) return;
		if ( !(df->df_flags & D_IMP_BY_EXP)) {
			if (! (df->df_flags & (D_USED | D_DEFINED))) {
				if (df1->df_kind == D_VARIABLE) {
					warning = "imported but not used/assigned";
				}
				else	warning = "imported but not used";
				goto warn;
			}
			return;
		}
		df = df1;
	}
	if (! (df->df_kind & (D_VARIABLE|D_PROCEDURE|D_TYPE|D_CONST))) {
		return;
	}
	switch(df->df_flags & (D_USED|D_DEFINED)) {
	case 0:
		warning = "never used/assigned";
		break;
	case D_USED:
		warning = "never assigned";
		break;
	case D_DEFINED:
		warning = "never used";
		break;
	case D_USED|D_DEFINED:
		return;
	}
warn:
	if (warning) {
		node_warning(df->df_scope->sc_end,
			     W_ORDINARY,
			     "identifier \"%s\" %s",
			     df->df_idf->id_text, warning);
	}
}

WalkDefList(df, proc)
	register t_def *df;
	int (*proc)();
{
	for (; df; df = df->df_nextinscope) {
		(*proc)(df);
	}
}
