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
#include	<stb.h>

#include	"strict3rd.h"
#include	"dbsymtab.h"
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
#include	"bigresult.h"
#include	"use_insert.h"

extern arith		NewPtr();
extern arith		NewInt();
extern arith		TmpSpace();

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

#define REACH_FLAG	1
#define EXIT_FLAG	2

int
LblWalkNode(lbl, nd, exit, reach)
	label lbl, exit;
	t_node *nd;
{
	/*	Generate code for node "nd", after generating instruction
		label "lbl". "exit" is the exit label for the closest
		enclosing LOOP.
	*/

	def_ilb(lbl);
	return WalkNode(nd, exit, reach);
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
	/*	Instruction label definition. Forget about line number.
	*/
	C_df_ilb(l);
	oldlineno = 0;
}

DoLineno(nd)
	register t_node *nd;
{
	/*	Generate line number information, if necessary.
	*/
	if ((! options['L']
#ifdef DBSYMTAB
	     || options['g']
#endif /* DBSYMTAB */
	    ) &&
	    nd->nd_lineno &&
	    nd->nd_lineno != oldlineno) {
		oldlineno = nd->nd_lineno;
		if (! options['L']) C_lin((arith) nd->nd_lineno);
#ifdef DBSYMTAB
		if ( options['g']) {
			static int	ms_lineno;

			if (ms_lineno != nd->nd_lineno) {
				ms_lineno = nd->nd_lineno;
				C_ms_std((char *) 0, N_SLINE, ms_lineno);
			}
		}
#endif /* DBSYMTAB */
	}
}

DoFilename(needed)
{
	/*	Generate filename information, when needed.
		This routine is called at the generation of a
		procedure entry, and after generating a call to
		another procedure.
	*/
	static label	filename_label = 0;

	oldlineno = 0;	/* always invalidate remembered line number */
	if (needed && ! options['L']) {

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
		else if (! options['R']) {
			/* put funny value in BSS, in an attempt to detect
			   uninitialized variables
			*/
			C_cal("killbss");
		}

		for (; nd; nd = nd->nd_NEXT) {
			C_cal(nd->nd_def->mod_vis->sc_scope->sc_name);
		}
		DoFilename(1);
	}
	WalkDefList(sc->sc_def, MkCalls);
	proclevel++;
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std((char *) 0, N_LBRAC, proclevel);
	}
#endif /* DBSYMTAB */
	WalkNode(module->mod_body, NO_EXIT_LABEL, REACH_FLAG);
	DO_DEBUG(options['X'], PrNode(module->mod_body, 0));
	def_ilb(RETURN_LABEL);
	EndPriority();
	C_ret((arith) 0);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std((char *) 0, N_RBRAC, proclevel);
	}
#endif /* DBSYMTAB */
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
	register t_type *tp;
	register t_param *param;
	register t_scope *procscope = procedure->prc_vis->sc_scope;
	label too_big = 0;		/* returnsize larger than returnarea */
	arith StackAdjustment = 0;	/* space for conformant arrays */
	arith retsav = 0;		/* temporary space for return value */
	arith func_res_size = 0;
#ifdef USE_INSERT
	int partno = C_getid();
	int partno2 = C_getid();
#else
	label cd_init;
	label cd_body;
#endif

	proclevel++;
	CurrVis = procedure->prc_vis;

	/* Generate code for all local modules and procedures
	*/
	WalkDefList(procscope->sc_def, WalkDef);

	func_type = tp = RemoveEqual(ResultType(procedure->df_type));

	if (tp) {
		func_res_size = WA(tp->tp_size);
		if (TooBigForReturnArea(tp)) {
#ifdef BIG_RESULT_ON_STACK
			/* The result type of this procedure is too big.
			   The caller will have reserved space on its stack,
			   above the parameters, to store the result.
			*/
			too_big = 1;
#else
			/* The result type of this procedure is too big.
			   The actual procedure will return a pointer to a
			   global data area in which the function result is
			   stored.
			   Notice that this makes the code non-reentrant.
			   Here, we create the data area for the function
			   result.
			*/
			too_big = ++data_label;
			C_df_dlb(too_big);
			C_bss_cst(func_res_size, (arith)0, 0);
#endif /* BIG_RESULT_ON_STACK */
		}
	}

	/* Generate code for this procedure
	*/
	TmpOpen(procscope);
#ifdef USE_INSERT
	C_insertpart(partno2);	/* procedure header */
#else
	C_pro_narg(procedure->prc_name);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std((char *) 0, N_LBRAC, proclevel);
	}
#endif /* DBSYMTAB */
	C_ms_par(procedure->df_type->prc_nbpar
#ifdef BIG_RESULT_ON_STACK
		+ (too_big ? func_res_size : 0)
#endif
		);
#endif
	/* generate code for filename only when the procedure can be
	   exported, either directly or by taking the address.
	   This cannot be done if the level is bigger than one (because in
	   this case it is a nested procedure).
	*/
	DoFilename(procscope->sc_level == 1);
	DoPriority();

	text_label = 1;		/* label at end of procedure */

	/* Check if we must save the stack pointer */
	for (param = ParamList(procedure->df_type);
	     param;
	     param = param->par_next) {
		if (! IsVarParam(param)) {
			tp = TypeOfParam(param);

			if ( IsConformantArray(tp)) {
				/* First time we get here
				*/
				if (func_type && !too_big) {
					/* Some local space, only
					   needed if the value itself
					   is returned
					*/
					retsav= TmpSpace(func_res_size, 1);
				}
				StackAdjustment = NewPtr();
				C_lor((arith) 1);
				STL(StackAdjustment, pointer_size);
			}
		}
	}

#ifdef USE_INSERT
	C_insertpart(partno);
#else
	cd_init = ++text_label;
	cd_body = ++text_label;
	C_bra(cd_init);
	def_ilb(cd_body);
#endif

	if ((WalkNode(procedure->prc_body, NO_EXIT_LABEL, REACH_FLAG) & REACH_FLAG)) {
		if (func_res_size) {
			node_warning(procscope->sc_end,
				     W_ORDINARY,
				     "function procedure \"%s\" does not always return a value",
				     procedure->df_idf->id_text);
			c_loc(M2_NORESULT);
			C_trp();
			C_asp(-func_res_size);
		}
#ifndef USE_INSERT
		C_bra(RETURN_LABEL);
#endif
	}

#ifdef USE_INSERT
	C_beginpart(partno);
#else
	def_ilb(cd_init);
#endif

	/* Generate calls to initialization routines of modules defined within
	   this procedure
	*/
	WalkDefList(procscope->sc_def, MkCalls);

	/* Make sure that arguments of size < word_size are on a
	   fixed place.
	   Also make copies of parameters when neccessary.
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
					STL(param->par_def->var_off,
					    tp->tp_size);
				}
				continue;
			}
			/* Here, we have to make a copy of the
			   array. We must also remember how much
			   room is reserved for copies, because
			   we have to adjust the stack pointer before
			   a RET is done. This is even more complicated
			   when the procedure returns a value.
			   Then, the value must be saved,
			   the stack adjusted, the return value pushed
			   again, and then RET
			*/
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
#ifdef USE_INSERT
	C_endpart(partno);
#else
	C_bra(cd_body);
#endif
	DO_DEBUG(options['X'], PrNode(procedure->prc_body, 0));
	def_ilb(RETURN_LABEL);	/* label at end */
	if (too_big) {
		/* Fill the data area reserved for the function result
		   with the result
		*/
#ifdef BIG_RESULT_ON_STACK
		C_lal(procedure->df_type->prc_nbpar);
#else
		c_lae_dlb(too_big);
#endif /* BIG_RESULT_ON_STACK */
		C_sti(func_res_size);
		if (StackAdjustment) {
			/* Remove copies of conformant arrays
			*/
			LOL(StackAdjustment, pointer_size);
			C_str((arith) 1);
		}
#ifdef BIG_RESULT_ON_STACK
		func_res_size = 0;
#else
		c_lae_dlb(too_big);
		func_res_size = pointer_size;
#endif /* BIG_RESULT_ON_STACK */
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
#ifdef USE_INSERT
	C_beginpart(partno2);
	C_pro(procedure->prc_name, -procscope->sc_off);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std((char *) 0, N_LBRAC, proclevel);
	}
#endif /* DBSYMTAB */
	C_ms_par(procedure->df_type->prc_nbpar
#ifdef BIG_RESULT_ON_STACK
		+ (too_big ? func_res_size : 0)
#endif
		);
#endif
	if (! options['n']) WalkDefList(procscope->sc_def, RegisterMessage);
#ifdef USE_INSERT
	C_endpart(partno2);
#endif
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std((char *) 0, N_RBRAC, proclevel);
	}
#endif /* DBSYMTAB */
	C_end(-procscope->sc_off);
	if (! fit(procscope->sc_off, (int) word_size)) {
		node_error(procedure->prc_body,
			   "maximum local byte count exceeded");
	}
	TmpClose();
	CurrVis = savevis;
	proclevel--;
	WalkDefList(procscope->sc_def, UseWarnings);
}

static
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

static
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

WalkLink(nd, exit_label, end_reached)
	register t_node *nd;
	label exit_label;
{
	/*	Walk node "nd", which is a link.
		"exit_label" is set to a label number when inside a LOOP.
		"end_reached" maintains info about reachability (REACH_FLAG),
		and whether an EXIT statement was seen (EXIT_FLAG).
	*/

	while (nd && nd->nd_class == Link) {	 /* statement list */
		end_reached = WalkNode(nd->nd_LEFT, exit_label, end_reached);
		nd = nd->nd_RIGHT;
	}

	return WalkNode(nd, exit_label, end_reached);
}

STATIC
ForLoopVarExpr(nd)
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	CodePExpr(nd);
	CodeCoercion(tp, BaseType(tp));
}

int
WalkStat(nd, exit_label, end_reached)
	register t_node *nd;
	label exit_label;
{
	/*	Walk through a statement, generating code for it.
	*/
	register t_node *left = nd->nd_LEFT;
	register t_node *right = nd->nd_RIGHT;

	assert(nd->nd_class == Stat);

	if (nd->nd_symb == ';') return 1;

	if (! end_reached & REACH_FLAG) {
		node_warning(nd, W_ORDINARY, "statement not reached");
	}
	if (nd->nd_symb != WHILE ||
	    nd->nd_lineno != left->nd_lineno) {
		/* Avoid double linenumber generation in while statements */
		DoLineno(nd);
	}
	options['R'] = (nd->nd_flags & ROPTION);
	options['A'] = (nd->nd_flags & AOPTION);
	switch(nd->nd_symb) {
	case '(': {
		t_node *nd1 = nd;
		if (ChkCall(&nd1)) {
			assert(nd == nd1);
			if (nd->nd_type != 0) {
				node_error(nd, "procedure call expected instead of function call");
				break;
			}
			CodeCall(nd);
		}
		}
		break;

	case BECOMES:
		DoAssign(nd);
		break;

	case IF:
		{	label l1 = ++text_label, l3 = ++text_label;
			int end_r;

			ExpectBool(&(nd->nd_LEFT), l3, l1);
			assert(right->nd_symb == THEN);
			end_r = LblWalkNode(l3, right->nd_LEFT, exit_label, end_reached);

			if (right->nd_RIGHT) {	/* ELSE part */
				label l2 = ++text_label;

				C_bra(l2);
				end_reached = end_r | LblWalkNode(l1, right->nd_RIGHT, exit_label, end_reached);
				l1 = l2;
			}
			else	end_reached |= end_r;
			def_ilb(l1);
			break;
		}

	case CASE:
		end_reached = CaseCode(nd, exit_label, end_reached);
		break;

	case WHILE:
		{	label	loop = ++text_label,
				exit = ++text_label,
				dummy = ++text_label;

			C_bra(dummy);
			end_reached |= LblWalkNode(loop, right, exit_label, end_reached);
			def_ilb(dummy);
			ExpectBool(&(nd->nd_LEFT), loop, exit);
			def_ilb(exit);
			break;
		}

	case REPEAT:
		{	label loop = ++text_label, exit = ++text_label;

			end_reached = LblWalkNode(loop, left, exit_label, end_reached);
			ExpectBool(&(nd->nd_RIGHT), exit, loop);
			def_ilb(exit);
			break;
		}

	case LOOP:
		{	label loop = ++text_label, exit = ++text_label;

			if (LblWalkNode(loop, right, exit, end_reached) & EXIT_FLAG) {
				end_reached &= REACH_FLAG;
			}
			else	end_reached = 0;
			C_bra(loop);
			def_ilb(exit);
			break;
		}

	case FOR:
		{
			arith tmp = NewInt();
			arith tmp2 = NewInt();
			int good_forvar;
			label l1 = ++text_label;
			label l2 = ++text_label;
			int uns = 0;
			arith stepsize;
			t_type *bstp;
			t_node *loopid;

			good_forvar = DoForInit(left);
			loopid = left->nd_LEFT;
			if ((stepsize = right->nd_LEFT->nd_INT) == 0) {
				node_warning(right->nd_LEFT,
					     W_ORDINARY,
					     "zero stepsize in FOR loop");
			}
			if (good_forvar) {
				bstp = BaseType(loopid->nd_type);
				uns = bstp->tp_fund != T_INTEGER;
				CodePExpr(left->nd_RIGHT->nd_RIGHT);
				C_stl(tmp);
				CodePExpr(left->nd_RIGHT->nd_LEFT);
				C_dup(int_size);
				C_stl(tmp2);
				C_lol(tmp);
				if (uns) C_cmu(int_size);
				else C_cmi(int_size);
				if (stepsize >= 0) C_zgt(l2);
				else C_zlt(l2);
				C_lol(tmp2);
				RangeCheck(loopid->nd_type,
					   left->nd_RIGHT->nd_LEFT->nd_type);
				CodeDStore(loopid);
				if (stepsize >= 0) {
					C_lol(tmp);
					ForLoopVarExpr(loopid);
				}
				else {
					stepsize = -stepsize;
					ForLoopVarExpr(loopid);
					C_lol(tmp);
				}
				C_sbu(int_size);
				if (stepsize) {
					C_loc(stepsize);
					C_dvu(int_size);
				}
				C_stl(tmp);
				loopid->nd_def->df_flags |= D_FORLOOP;
				def_ilb(l1);
				if (! options['R']) {
					ForLoopVarExpr(loopid);
					C_stl(tmp2);
				}
				end_reached |= WalkNode(right->nd_RIGHT, exit_label, end_reached);
				if (! options['R']) {
					label x = ++text_label;
					C_lol(tmp2);
					ForLoopVarExpr(loopid);
					C_beq(x);
					c_loc(M2_FORCH);
					C_trp();
					def_ilb(x);
				}
				loopid->nd_def->df_flags &= ~D_FORLOOP;
				FreeInt(tmp2);
				if (stepsize) {
					C_lol(tmp);
					C_zeq(l2);
					C_lol(tmp);
					c_loc(1);
					C_sbu(int_size);
					C_stl(tmp);
					C_loc(right->nd_LEFT->nd_INT);
					ForLoopVarExpr(loopid);
					C_adu(int_size);
					RangeCheck(loopid->nd_type, bstp);
					CodeDStore(loopid);
				}
			}
			else {
				end_reached |= WalkNode(right->nd_RIGHT, exit_label, end_reached);
				loopid->nd_def->df_flags &= ~D_FORLOOP;
			}
			C_bra(l1);
			def_ilb(l2);
			FreeInt(tmp);
		}
		break;

	case WITH:
		{
			t_scopelist link;
			struct withdesig wds;
			t_desig ds;

			if (! WalkDesignator(&(nd->nd_LEFT), &ds, D_USED)) break;
			left = nd->nd_LEFT;
			if (left->nd_type->tp_fund != T_RECORD) {
				node_error(left, "record variable expected");
				break;
			}

			wds.w_next = WithDesigs;
			wds.w_flags = D_USED;
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
			end_reached = WalkNode(right, exit_label, end_reached);
			CurrVis = link.sc_next;
			WithDesigs = wds.w_next;
			FreePtr(ds.dsg_offset);
			ChkDesig(&(nd->nd_LEFT), wds.w_flags & (D_USED|D_DEFINED));
			break;
		}

	case EXIT:
		assert(exit_label != 0);

		if (end_reached & REACH_FLAG) end_reached = EXIT_FLAG;
		C_bra(exit_label);
		break;

	case RETURN:
		end_reached &= ~REACH_FLAG;
		if (right) {
			if (! ChkExpression(&(nd->nd_RIGHT))) break;
			/* The type of the return-expression must be
			   assignment compatible with the result type of the
			   function procedure (See Rep. 9.11).
			*/
			if (!ChkAssCompat(&(nd->nd_RIGHT), func_type, "RETURN")) {
				break;
			}
			right = nd->nd_RIGHT;
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
	return end_reached;
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
	NodeCrash,
	WalkLink,
};

extern t_desig null_desig;

ExpectBool(pnd, true_label, false_label)
	register t_node **pnd;
	label true_label, false_label;
{
	/*	"pnd" must indicate a boolean expression. Check this and
		generate code to evaluate the expression.
	*/
	t_desig ds;

	ds = null_desig;
	if (ChkExpression(pnd)) {
		if ((*pnd)->nd_type != bool_type &&
		    (*pnd)->nd_type != error_type) {
			node_error(*pnd, "boolean expression expected");
		}

		CodeExpr(*pnd, &ds,  true_label, false_label);
	}
}

int
WalkDesignator(pnd, ds, flags)
	t_node **pnd;
	t_desig *ds;
{
	/*	Check designator and generate code for it
	*/

	if (! ChkVariable(pnd, flags)) return 0;

	*ds = null_desig;
	CodeDesig(*pnd, ds);
	return 1;
}

DoForInit(nd)
	t_node *nd;
{
	register t_node *right = nd->nd_RIGHT;
	register t_def *df;
	t_type *base_tp;
	t_type *tpl, *tpr;
	int r;

	r = ChkVariable(&(nd->nd_LEFT), D_USED|D_DEFINED);
	r &= ChkExpression(&(right->nd_LEFT));
	r &= ChkExpression(&(right->nd_RIGHT));
	if (!r) return 0;

	df = nd->nd_LEFT->nd_def;
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

	base_tp = BaseType(df->df_type);
	tpl = right->nd_LEFT->nd_type;
	tpr = right->nd_RIGHT->nd_type;
#ifndef STRICT_3RD_ED
	if (! options['3']) {
	  if (!ChkAssCompat(&(right->nd_LEFT), base_tp, "FOR statement") ||
	      !ChkAssCompat(&(right->nd_RIGHT), base_tp, "FOR statement")) {
		return 1;
	  }
	  if (!TstCompat(df->df_type, tpl) ||
	      !TstCompat(df->df_type, tpr)) {
node_warning(nd, W_OLDFASHIONED, "compatibility required in FOR statement");
	  }
	} else
#endif
	if (!ChkCompat(&(right->nd_LEFT), base_tp, "FOR statement") ||
	    !ChkCompat(&(right->nd_RIGHT), base_tp, "FOR statement")) {
		return 1;
	}

	return 1;
}

DoAssign(nd)
	register t_node *nd;
{
	/* May we do it in this order (expression first) ???
	   The reference manual sais nothing about it, but the book does:
	   it sais that the left hand side is evaluated first.
	   DAMN THE BOOK!
	*/
	t_desig dsr;
	register t_type *tp;

	if (! (ChkExpression(&(nd->nd_RIGHT)) &
	       ChkVariable(&(nd->nd_LEFT), D_DEFINED))) return;
	tp = nd->nd_LEFT->nd_type;

	if (nd->nd_RIGHT->nd_symb == STRING) TryToString(nd->nd_RIGHT, tp);

	if (! ChkAssCompat(&(nd->nd_RIGHT), tp, "assignment")) {
		return;
	}
	dsr = null_desig;

#define StackNeededFor(ds)	((ds).dsg_kind == DSG_PLOADED \
				  || (ds).dsg_kind == DSG_INDEXED)
	CodeExpr(nd->nd_RIGHT, &dsr, NO_LABEL, NO_LABEL);
	tp = nd->nd_RIGHT->nd_type;
	if (complex(tp)) {
		if (StackNeededFor(dsr)) CodeAddress(&dsr);
	}
	else {
		CodeValue(&dsr, tp);
	}
	CodeMove(&dsr, nd->nd_LEFT, tp);
}

static
RegisterMessage(df)
	register t_def *df;
{
	register t_type *tp;

	if (df->df_kind == D_VARIABLE) {
		if ( !(df->df_flags & D_NOREG)) {
			/* Examine type and size
			*/
			tp = BaseType(df->df_type);
			if ((df->df_flags & D_VARPAR) ||
			    (tp->tp_fund&(T_POINTER|T_HIDDEN|T_EQUAL))) {
				C_ms_reg(df->var_off,
					 pointer_size,
					 reg_pointer,
					 0);
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

static
df_warning(nd, df, warning)
	t_node	*nd;
	t_def	*df;
	char	*warning;
{
	if (! (df->df_kind & (D_VARIABLE|D_PROCEDURE|D_TYPE|D_CONST|D_PROCHEAD))) {
		return;
	}
	if (warning) {
		node_warning(nd,
			     W_ORDINARY,
			     "%s \"%s\" %s",
			     (df->df_flags & D_VALPAR) ? "value parameter" :
			      (df->df_flags & D_VARPAR) ? "variable parameter" :
			       (df->df_kind == D_VARIABLE) ? "variable" :
				(df->df_kind == D_TYPE) ? "type" :
				 (df->df_kind == D_CONST) ? "constant" :
				  "procedure",
			     df->df_idf->id_text, warning);
	}
}

static
UseWarnings(df)
	register t_def *df;
{
	t_node	*nd = df->df_scope->sc_end;

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
			if (df->df_flags & (D_USED | D_DEFINED)) {
				return;
			}
			df_warning(nd,
				   df1,
				   df1->df_kind == D_VARIABLE ?
					"imported but not used/assigned" :
					"imported but not used");
			return;
		}
		df = df1;
		nd = df->df_scope->sc_end;
	}
	switch(df->df_flags & (D_USED|D_DEFINED|D_VALPAR|D_VARPAR)) {
	case 0:
	case D_VARPAR:
		df_warning(nd, df,"never used/assigned");
		break;
	case D_USED:
		df_warning(nd, df,"never assigned");
		break;
	case D_VALPAR:
	case D_DEFINED:
	case D_DEFINED|D_VALPAR:
		df_warning(nd, df,"never used");
		break;
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
