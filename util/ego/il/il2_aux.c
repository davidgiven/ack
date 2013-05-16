/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 2 _ A U X . C
 */

#include <stdlib.h>
#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "il_aux.h"
#include "il2_aux.h"
#include "../share/get.h"
#include "../share/aux.h"

#define USE_INDIR(p)	(p->p_use->u_flags & UF_INDIR)

#define OFTEN_USED(f)	((f->f_flags&FF_OFTENUSED) == FF_OFTENUSED)
#define CHANGE_EXT(p)	(Cnrelems(p->p_change->c_ext) > 0)
#define NOT_INLINE(a)	(a->ac_inl = FALSE)
#define  INLINE(a)	(a->ac_inl = TRUE)


#define CHANGED(p)	p->p_flags2 |= PF_CHANGED
#define IS_CHANGED(p)	(p->p_flags2 & PF_CHANGED)



STATIC bool match_pars(fm,act)
	formal_p fm;
	actual_p act;
{
	/* Check if every actual parameter has the same
	 * size as its corresponding formal. If not, the
	 * actual parameters should not be expanded in line.
	 */

	while (act != (actual_p) 0) {
		if (fm == (formal_p) 0 || tsize(fm->f_type) != act->ac_size) {
			return FALSE;
		}
		act = act->ac_next;
		fm = fm->f_next;
	}
	return (fm == (formal_p) 0 ? TRUE : FALSE);
}


STATIC bool change_act(p,act)
	proc_p p;
	actual_p act;
{
	/* See if a call to p migth change any of the
	 * operands of the actual parameter expression.
	 * If the parameter is to be expanded in line,
	 * we must be sure its value does not depend
	 * on the point in the program where it is
	 * evaluated.
	 */

	line_p l;

	for (l = act->ac_exp; l != (line_p) 0; l = l->l_next) {
		switch(INSTR(l)) {
			case op_lil:
			case op_lof:
			case op_loi:
			case op_los:
			case op_ldf:
				return TRUE;
				/* assume worst case */
			case op_lol:
			case op_ldl:
				if (CHANGE_INDIR(p)) {
					return TRUE;
				}
				break;
			case op_loe:
			case op_lde:
				if (CHANGE_INDIR(p) || CHANGE_EXT(p)) {
					return TRUE;
				}
				break;
		}
	}
	return FALSE;
}



STATIC bool is_simple(expr)
	line_p expr;
{
	/* See if expr is something simple, i.e. a constant or
	 * a variable. So the expression must consist of
	 * only one instruction.
	 */


	if (expr->l_next == (line_p) 0) {
		switch(INSTR(expr)) {
			case op_loc:
			case op_ldc:
			case op_lol:
			case op_ldl:
			case op_loe:
			case op_lde:
				return TRUE;
		}
	}
	return FALSE;
}



STATIC bool too_expensive(fm,act)
	formal_p fm;
	actual_p act;
{
	/* If the formal parameter is used often and the
	 * actual parameter is not something simple
	 * (i.e. an expression, not a constant or variable)
	 * it may be too expensive too expand the parameter
	 * in line.
	 */

	return (OFTEN_USED(fm) && !is_simple(act->ac_exp));
}
bool anal_params(c)
	call_p c;
{
	/* Determine which of the actual parameters of a
	 * call may be expanded in line.
	 */

	proc_p p;
	actual_p act;
	formal_p form;
	int inlpars = 0;

	p = c->cl_proc; /* the called procedure */
	if (!match_pars(p->P_FORMALS, c->cl_actuals)) return FALSE;
	if (!INLINE_PARS(p)) {
		for (act = c->cl_actuals; act != (actual_p) 0;
		     act = act->ac_next) {
			NOT_INLINE(act);
		}
		return TRUE; /* "# of inline pars." field in cl_flags remains 0 */
	}
	for (act = c->cl_actuals, form = p->P_FORMALS; act != (actual_p) 0;
	     act = act->ac_next, form = form->f_next) {
		if (form->f_flags & FF_BAD ||
		    change_act(p,act) || too_expensive(form,act)) {
			NOT_INLINE(act);
		} else {
			INLINE(act);
			inlpars++;
		}
	}
	if (inlpars > 15) inlpars = 15; /* We've only got 4 bits! */
	c->cl_flags |= inlpars; /* number of inline parameters */
	return TRUE;
}


STATIC short space_saved(c)
	call_p c;
{
	/* When a call gets expanded in line, the total size of the
	 * code usually gets incremented, because we have to
	 * duplicate the text of the called routine. However, we save
	 * ourselves a CAL instruction and possibly anASP instruction
	 * (if the called procedure has parameters). Moreover, if we
	 * can put some parameters in line, we don't have to push
	 * their results on the stack before doing the call, so we
	 * save some code here too. The routine estimates the amount of
	 * code saved, expressed in number of EM instructions.
	 */

	return (1 + (c->cl_flags & CLF_INLPARS) + (c->cl_proc->p_nrformals>0));
}

STATIC short param_score(c)
	call_p c;
{
	/* If a call has an inline parameter that is a constant,
	 * chances are high that other optimization techniques
	 * can do further optimizations, especially if the constant
	 * happens to be "0". So the call gets extra points for this.
	 */

	register actual_p act;
	line_p l;
	short score = 0;

	for (act = c->cl_actuals; act != (actual_p) 0; act = act->ac_next) {
		if (act->ac_inl) {
			l = act->ac_exp;
			if (l->l_next == (line_p) 0 &&
			    (INSTR(l) == op_loc || INSTR(l) == op_ldc)) {
				score += (off_set(l) == (offset) 0 ? 2 : 1);
				/* 0's count for two! */
			}
		}
	}
	return score;
}





assign_ratio(c)
	call_p c;
{
	/* This routine is one of the most important ones
	 * of the inline substitution phase. It assigns a number
	 * (a 'ratio') to a call, indicating how desirable
	 * it is to expand the call in line.
	 * Currently, a very simplified straightforward heuristic
	 * is used.
	 */

	short ll, loopfact, ratio;

	ll = c->cl_proc->P_SIZE - space_saved(c);
	if (ll <= 0) ll = 1;
	ratio = 1000 / ll;
	if (ratio == 0) ratio = 1;
	/* Add points if the called procedure falls through
	 * it's end (no BRA needed) or has formal parameters
	 * (ASP can be deleted).
	 */
	if (c->cl_proc->p_flags2 & PF_FALLTHROUGH) {
		ratio += 10;
	}
	if (c->cl_proc->p_nrformals > 0) {
		ratio += 10;
	}
	if (c->cl_caller->p_localbytes == 0) {
		ratio -= 10;
	}
	ratio += (10 *param_score(c));
	/* Extra points for constants as parameters */
	if (ratio <= 0) ratio = 1;
	ll = c->cl_looplevel+1; 
	if (ll == 1 && !IS_CALLED_IN_LOOP(c->cl_caller)) ll = 0;
	/* If the call is not in a loop and the called proc. is never called
	 * in a loop, ll is set to 0.
	*/
	loopfact = (ll > 3 ? 10 : ll*ll);
	ratio *= loopfact;
	if (c->cl_flags & CLF_FIRM) {
		ratio = 2*ratio;
	}
	c->cl_ratio = ratio;
}


call_p abstract(c)
	call_p c;
{
	/* Abstract information from the call that is essential
	 * for choosing the calls that will be expanded.
	 * Put the information is an 'abstracted call'.
	 */

	call_p a;

	a = newcall();
	a->cl_caller = c->cl_caller;
	a->cl_id = c->cl_id;
	a->cl_proc = c->cl_proc;
	a->cl_looplevel = c->cl_looplevel;
	a->cl_ratio = c->cl_ratio;
	a->cl_flags = c->cl_flags;
	return a;
}



STATIC adjust_counts(callee,ccf)
	proc_p callee;
	FILE   *ccf;
{
	/* A call to callee is expanded in line;
	 * the text of callee is not removed, so
	 * every proc called by callee gets its
	 * P_NRCALLED field incremented.
	 */

	calcnt_p cc, head;

	head = getcc(ccf,callee); /* get calcnt info of called proc */
	for (cc = head; cc != (calcnt_p) 0; cc = cc->cc_next) {
		cc->cc_proc->P_NRCALLED += cc->cc_count;
	}
	remcc(head); /* remove calcnt info */
}



STATIC bool is_dispensable(callee,ccf)
	proc_p callee;
	FILE   *ccf;
{
	/* A call to callee is expanded in line.
	 * Decrement its P_NRCALLED field and see if
	 * it can now be removed because it is no
	 * longer called. Procedures that ever have
	 * their address taken (via LPI) will never
	 * be removed, as they might be called indirectly.
	 */

	if ((--callee->P_NRCALLED) == 0 &&
	    (complete_program || (callee->p_flags1 & PF_EXTERNAL) == 0) &&
	    (callee->p_flags1 & PF_LPI) == 0) {
		DISPENSABLE(callee);
		OUTVERBOSE("dispensable: procedure %d can be removed",callee->p_id);
#ifdef VERBOSE
		Spremoved++;
#endif
		return TRUE;
	} else {
		adjust_counts(callee,ccf);
		return FALSE;
	}
}




STATIC call_p nested_calls(a)
	call_p a;
{
	/* Get a list of all calls that will appear in the
	 * EM text if the call 'a' is expanded in line.
	 * These are the calls in the P_CALS list of the
	 * called procedure.
	 */

	call_p c, cp, head, *cpp;

	head = (call_p) 0;
	cpp = &head;
	for (c = a->cl_proc->P_CALS; c != (call_p) 0; c = c->cl_cdr) {
		cp = abstract(c);
		cp->cl_looplevel += a->cl_looplevel;
		cp->cl_flags = (byte) 0;
		if (a->cl_flags & CLF_FIRM) {
			cp->cl_flags |= CLF_FIRM;
		}
		assign_ratio(cp);
		*cpp = cp;
		cpp = &cp->cl_cdr;
	}
	return head;
}




STATIC call_p find_origin(c)
	call_p c;
{
	/* c is a nested call. Find the original call.
	 * This origional must be in the P_CALS list
	 * of the calling procedure.
	 */

	register call_p x;

	for (x = c->cl_caller->P_CALS; x != (call_p) 0; x = x->cl_cdr) {
		if (x->cl_id == c->cl_id) return x;
	}
	assert(FALSE);
	/* NOTREACHED */
}



STATIC selected(a)
	call_p a;
{
	/* The call a is selected for in line expansion.
	 * Mark the call as being selected and get the
	 * calls nested in it; these will be candidates
	 * too now.
	 */

	SELECTED(a);
	EVER_EXPANDED(find_origin(a));
	a->cl_car = nested_calls(a);
}




STATIC compare(x,best,space)
	call_p x, *best;
	long  space;
{
	/* See if x is better than the current best choice */

	if (x != (call_p) 0 && !IS_CHANGED(x->cl_proc) &&
	    x->cl_proc->P_SIZE - space_saved(x) <= space) {
		if ((*best == (call_p) 0 && x->cl_ratio != 0) ||
		    (*best != (call_p) 0 && x->cl_ratio > (*best)->cl_ratio )) {
			*best = x;
		}
	}
}




STATIC call_p best_one(list,space)
	call_p list;
	long  space;
{
	/* Find the best candidate of the list
	 * that has not already been selected. The
	 * candidate must fit in the given space.
	 * We look in the cdr as well as in the car
	 * direction.
	 */

	call_p best = (call_p) 0;
	call_p c;

	for (c = list; c != (call_p) 0; c = c->cl_cdr) {
		if (IS_SELECTED(c)) {
			compare(best_one(c->cl_car,space),&best,space);
		} else {
			compare(c,&best,space);
		}
	}
	return best;
}



STATIC singles(cals)
	call_p cals;
{
	/* If a procedure is only called once, this call
	 * will be expanded in line, because it costs
	 * no extra space.
	 */

	call_p c;

	for (c = cals; c != (call_p) 0; c = c->cl_cdr) {
		if (IS_SELECTED(c)) {
			singles(c->cl_car);
		} else {
			if (c->cl_proc->P_NRCALLED == 1 &&
			    !IS_CHANGED(c->cl_proc) &&
			    (complete_program || 
			      (c->cl_proc->p_flags1 & PF_EXTERNAL) == 0) &&
			    (c->cl_proc->p_flags1 & PF_LPI) == 0) {
				c->cl_proc->P_NRCALLED = 0;
				SELECTED(c);
				EVER_EXPANDED(find_origin(c));
				DISPENSABLE(c->cl_proc);
				CHANGED(c->cl_caller);
				OUTVERBOSE("singles: procedure %d can be removed",
				  c->cl_proc->p_id);
#ifdef VERBOSE
				  Spremoved++;
#endif
			}
		}
	}
}



STATIC single_calls(proclist)
	proc_p proclist;
{
	proc_p p;

	for (p = proclist; p != (proc_p) 0; p = p->p_next) {
		if (!BIG_CALLER(p) && !IS_DISPENSABLE(p)) {
			/* Calls appearing in a large procedure or in
			 * a procedure that was already eliminated
			 * are not considered.
			 */
			singles(p->P_CALS);
		}
	}
}
			



select_calls(proclist,ccf,space)
	proc_p proclist;
	FILE   *ccf;
	long space ;
{
	/* Select all calls that are to be expanded in line. */

	proc_p p,chp;
	call_p best, x;

	for (;;) {
		best = (call_p) 0;
		chp = (proc_p) 0; /* the changed procedure */
		for (p = proclist; p != (proc_p) 0; p = p->p_next) {
			if (!BIG_CALLER(p) && !IS_DISPENSABLE(p)) {
				/* Calls appearing in a large procedure or in
				 * a procedure that was already eliminated
				 * are not considered.
				 */
				x = best_one(p->P_CALS,space);
				compare(x,&best,space);
				if (x == best) chp = p;
			}
		}
		if (best == (call_p) 0) break;
		if (!is_dispensable(best->cl_proc,ccf)) {
			space -= (best->cl_proc->P_SIZE - space_saved(best));
		}
		else space += space_saved(best);
#ifdef VERBOSE
		if (verbose_flag) fprintf(stderr, "space left: %ld\n", space);
#endif
		selected(best);
		CHANGED(chp);
	}
	single_calls(proclist);
#ifdef VERBOSE
	Sstat(proclist,space);
#endif
}




STATIC nonnested_calls(cfile)
	FILE *cfile;
{
	register call_p c,a;

	while((c = getcall(cfile)) != (call_p) 0) {
		/* find the call in the call list of the caller */
		for (a = c->cl_caller->P_CALS;
		     a != (call_p) 0 && c->cl_id != a->cl_id; a = a->cl_cdr);
		assert(a != (call_p) 0 && a->cl_proc == c->cl_proc);
		if (IS_EVER_EXPANDED(a)) {
			a->cl_actuals = c->cl_actuals;
			c->cl_actuals = (actual_p) 0;
		}
		rem_call(c);
	}
}



STATIC copy_pars(src,dest)
	call_p src, dest;
{
	/* Copy the actual parameters of src to dest. */

	actual_p as,ad, *app;

	app = &dest->cl_actuals;
	for (as = src->cl_actuals; as != (actual_p) 0; as = as->ac_next) {
		ad = newactual();
		ad->ac_exp = copy_expr(as->ac_exp);
		ad->ac_size = as->ac_size;
		ad->ac_inl = as->ac_inl;
		*app = ad;
		app = &ad->ac_next;
	}
}



STATIC nest_pars(cals)
	call_p cals;
{
	/* Recursive auxiliary procedure of add_actuals. */

	call_p c,org;

	for (c = cals; c != (call_p) 0; c = c->cl_cdr) {
		if (IS_SELECTED(c)) {
			org = find_origin(c);
			copy_pars(org,c);
			nest_pars(c->cl_car);
		}
	}
}



add_actuals(proclist,cfile)
	proc_p proclist;
	FILE   *cfile;
{
	/* Fetch the actual parameters of all selected calls.
	 * For all non-nested calls (i.e. those calls that
	 * appeared originally in the EM text), we get the
	 * parameters from the cal-file. 
	 * For nested calls (i.e. calls
	 * that are a result of in line substitution) we
	 * get the parameters from the original call.
	 */

	proc_p p;
	call_p a;

	nonnested_calls(cfile);
	for (p = proclist; p != (proc_p) 0; p = p->p_next) {
		for (a = p->P_CALS; a != (call_p) 0; a = a->cl_cdr) {
			nest_pars(a->cl_car);
		}
	}
}



STATIC clean(cals)
	call_p *cals;
{
	call_p c,next,*cpp;

	/* Recursive auxiliary routine of cleancals */

	cpp = cals;
	for (c = *cpp; c != (call_p) 0; c = next) {
		next = c->cl_cdr;
		if (IS_SELECTED(c)) {
			clean(&c->cl_car);
			cpp = &c->cl_cdr;
		} else {
			assert(c->cl_car == (call_p) 0);
			oldcall(c);
			*cpp = next;
		}
	}
}


cleancals(proclist)
	proc_p proclist;
{
	/* Remove all calls in the P_CALS list of p
	 * that were not selected for in line expansion.
	 */

	register proc_p p;

	for (p = proclist; p != (proc_p) 0; p = p->p_next) {
		clean(&p->P_CALS);
	}
}




append_abstract(a,p)
	call_p a;
	proc_p p;
{
	/* Append an abstract of a call-descriptor to
	 * the call-list of procedure p.
	 */

	call_p c;

	if (p->P_CALS  == (call_p) 0) {
		p->P_CALS = a;
	} else {
		for (c = p->P_CALS; c->cl_cdr != (call_p) 0; c = c->cl_cdr);
		c->cl_cdr = a;
	}
}


#ifdef VERBOSE

/* At the end, we traverse the entire call-list, to see why the
 * remaining calls were not expanded inline.
 */


Sstatist(list,space)
	call_p list;
	long space;
{
	call_p c;

	for (c = list; c != (call_p) 0; c = c->cl_cdr) {
		if (IS_SELECTED(c)) {
			Sstatist(c->cl_car,space);
		} else {
			if (IS_CHANGED(c->cl_proc)) Schangedcallee++;
			else if (BIG_PROC(c->cl_proc)) Sbigcallee++;
			else if (c->cl_proc->P_SIZE > space) Sspace++;
			else if (c->cl_ratio == 0) Szeroratio++;
			else assert(FALSE);
		}
	}
}

Sstat(proclist,space)
	proc_p proclist;
	long space;
{
	proc_p p;

	for (p = proclist; p != (proc_p) 0; p = p->p_next) {
		if (BIG_CALLER(p)) Sbig_caller++;
		else if (IS_DISPENSABLE(p)) Sdispensable++;
		else Sstatist(p->P_CALS,space);
	}
}
#endif
