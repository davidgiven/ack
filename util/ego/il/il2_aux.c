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
#include "ego/share/types.h"
#include "il.h"
#include "ego/share/debug.h"
#include "ego/share/alloc.h"
#include "ego/share/global.h"
#include "ego/share/cset.h"
#include "ego/share/lset.h"
#include "il_aux.h"
#include "il2_aux.h"
#include "ego/share/get.h"
#include "ego/share/utils.h"

#define USE_INDIR(p) (p->p_use->u_flags & UF_INDIR)

#define OFTEN_USED(f) ((f->f_flags & FF_OFTENUSED) == FF_OFTENUSED)
#define CHANGE_EXT(p) (Cnrelems(p->p_change->c_ext) > 0)
#define NOT_INLINE(a) (a->ac_inl = false)
#define INLINE(a) (a->ac_inl = true)

#define CHANGED(p) p->p_flags2 |= PF_CHANGED
#define IS_CHANGED(p) (p->p_flags2 & PF_CHANGED)

#ifdef VERBOSE
static void Sstat(proc_p proclist, long space);
#endif

static bool match_pars(formal_p fm, actual_p act)
{
	/* Check if every actual parameter has the same
	 * size as its corresponding formal. If not, the
	 * actual parameters should not be expanded in line.
	 */

	while (act != (actual_p)0)
	{
		if (fm == (formal_p)0 || tsize(fm->f_type) != act->ac_size)
		{
			return false;
		}
		act = act->ac_next;
		fm = fm->f_next;
	}
	return (fm == (formal_p)0 ? true : false);
}

static bool change_act(proc_p p, actual_p act)
{
	/* See if a call to p migth change any of the
	 * operands of the actual parameter expression.
	 * If the parameter is to be expanded in line,
	 * we must be sure its value does not depend
	 * on the point in the program where it is
	 * evaluated.
	 */

	line_p l;

	for (l = act->ac_exp; l != (line_p)0; l = l->l_next)
	{
		switch (INSTR(l))
		{
			case op_lil:
			case op_lof:
			case op_loi:
			case op_los:
			case op_ldf:
				return true;
				/* assume worst case */
			case op_lol:
			case op_ldl:
				if (CHANGE_INDIR(p))
				{
					return true;
				}
				break;
			case op_loe:
			case op_lde:
				if (CHANGE_INDIR(p) || CHANGE_EXT(p))
				{
					return true;
				}
				break;
		}
	}
	return false;
}

static bool is_simple(line_p expr)
{
	/* See if expr is something simple, i.e. a constant or
	 * a variable. So the expression must consist of
	 * only one instruction.
	 */

	if (expr->l_next == (line_p)0)
	{
		switch (INSTR(expr))
		{
			case op_loc:
			case op_ldc:
			case op_lol:
			case op_ldl:
			case op_loe:
			case op_lde:
				return true;
		}
	}
	return false;
}

static bool too_expensive(formal_p fm, actual_p act)
{
	/* If the formal parameter is used often and the
	 * actual parameter is not something simple
	 * (i.e. an expression, not a constant or variable)
	 * it may be too expensive too expand the parameter
	 * in line.
	 */

	return (OFTEN_USED(fm) && !is_simple(act->ac_exp));
}
bool anal_params(call_p c)
{
	/* Determine which of the actual parameters of a
	 * call may be expanded in line.
	 */

	proc_p p;
	actual_p act;
	formal_p form;
	int inlpars = 0;

	p = c->cl_proc; /* the called procedure */
	if (!match_pars(p->P_FORMALS, c->cl_actuals))
		return false;
	if (!INLINE_PARS(p))
	{
		for (act = c->cl_actuals; act != (actual_p)0; act = act->ac_next)
		{
			NOT_INLINE(act);
		}
		return true; /* "# of inline pars." field in cl_flags remains 0 */
	}
	for (act = c->cl_actuals, form = p->P_FORMALS; act != (actual_p)0;
	     act = act->ac_next, form = form->f_next)
	{
		if (form->f_flags & FF_BAD || change_act(p, act) || too_expensive(form, act))
		{
			NOT_INLINE(act);
		}
		else
		{
			INLINE(act);
			inlpars++;
		}
	}
	if (inlpars > 15)
		inlpars = 15; /* We've only got 4 bits! */
	c->cl_flags |= inlpars; /* number of inline parameters */
	return true;
}

static short space_saved(call_p c)
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

	return (1 + (c->cl_flags & CLF_INLPARS) + (c->cl_proc->p_nrformals > 0));
}

static short param_score(call_p c)
{
	/* If a call has an inline parameter that is a constant,
	 * chances are high that other optimization techniques
	 * can do further optimizations, especially if the constant
	 * happens to be "0". So the call gets extra points for this.
	 */

	actual_p act;
	line_p l;
	short score = 0;

	for (act = c->cl_actuals; act != (actual_p)0; act = act->ac_next)
	{
		if (act->ac_inl)
		{
			l = act->ac_exp;
			if (l->l_next == (line_p)0 && (INSTR(l) == op_loc || INSTR(l) == op_ldc))
			{
				score += (off_set(l) == (offset)0 ? 2 : 1);
				/* 0's count for two! */
			}
		}
	}
	return score;
}

void assign_ratio(call_p c)
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
	if (ll <= 0)
		ll = 1;
	ratio = 1000 / ll;
	if (ratio == 0)
		ratio = 1;
	/* Add points if the called procedure falls through
	 * it's end (no BRA needed) or has formal parameters
	 * (ASP can be deleted).
	 */
	if (c->cl_proc->p_flags2 & PF_FALLTHROUGH)
	{
		ratio += 10;
	}
	if (c->cl_proc->p_nrformals > 0)
	{
		ratio += 10;
	}
	if (c->cl_caller->p_localbytes == 0)
	{
		ratio -= 10;
	}
	ratio += (10 * param_score(c));
	/* Extra points for constants as parameters */
	if (ratio <= 0)
		ratio = 1;
	ll = c->cl_looplevel + 1;
	if (ll == 1 && !IS_CALLED_IN_LOOP(c->cl_caller))
		ll = 0;
	/* If the call is not in a loop and the called proc. is never called
	 * in a loop, ll is set to 0.
	 */
	loopfact = (ll > 3 ? 10 : ll * ll);
	ratio *= loopfact;
	if (c->cl_flags & CLF_FIRM)
	{
		ratio = 2 * ratio;
	}
	c->cl_ratio = ratio;
}

call_p abstract(call_p c)
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

static void adjust_counts(proc_p callee, FILE* ccf)
{
	/* A call to callee is expanded in line;
	 * the text of callee is not removed, so
	 * every proc called by callee gets its
	 * P_NRCALLED field incremented.
	 */

	calcnt_p cc, head;

	head = getcc(ccf, callee); /* get calcnt info of called proc */
	for (cc = head; cc != (calcnt_p)0; cc = cc->cc_next)
	{
		cc->cc_proc->P_NRCALLED += cc->cc_count;
	}
	remcc(head); /* remove calcnt info */
}

static bool is_dispensable(proc_p callee, FILE* ccf)
{
	/* A call to callee is expanded in line.
	 * Decrement its P_NRCALLED field and see if
	 * it can now be removed because it is no
	 * longer called. Procedures that ever have
	 * their address taken (via LPI) will never
	 * be removed, as they might be called indirectly.
	 */

	if ((--callee->P_NRCALLED) == 0 && (complete_program || (callee->p_flags1 & PF_EXTERNAL) == 0)
	    && (callee->p_flags1 & PF_LPI) == 0)
	{
		DISPENSABLE(callee);
		OUTVERBOSE("dispensable: procedure %d can be removed", callee->p_id, 0);
#ifdef VERBOSE
		Spremoved++;
#endif
		return true;
	}
	else
	{
		adjust_counts(callee, ccf);
		return false;
	}
}

static call_p nested_calls(call_p a)
{
	/* Get a list of all calls that will appear in the
	 * EM text if the call 'a' is expanded in line.
	 * These are the calls in the P_CALS list of the
	 * called procedure.
	 */

	call_p c, cp, head, *cpp;

	head = (call_p)0;
	cpp = &head;
	for (c = a->cl_proc->P_CALS; c != (call_p)0; c = c->cl_cdr)
	{
		cp = abstract(c);
		cp->cl_looplevel += a->cl_looplevel;
		cp->cl_flags = (byte)0;
		if (a->cl_flags & CLF_FIRM)
		{
			cp->cl_flags |= CLF_FIRM;
		}
		assign_ratio(cp);
		*cpp = cp;
		cpp = &cp->cl_cdr;
	}
	return head;
}

static call_p find_origin(call_p c)
{
	/* c is a nested call. Find the original call.
	 * This origional must be in the P_CALS list
	 * of the calling procedure.
	 */

	call_p x;

	for (x = c->cl_caller->P_CALS; x != (call_p)0; x = x->cl_cdr)
	{
		if (x->cl_id == c->cl_id)
			return x;
	}
	assert(false);
	UNREACHABLE_CODE;
}

static void selected(call_p a)
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

static void compare(call_p x, call_p* best, long space)
{
	/* See if x is better than the current best choice */

	if (x != (call_p)0 && !IS_CHANGED(x->cl_proc) && x->cl_proc->P_SIZE - space_saved(x) <= space)
	{
		if ((*best == (call_p)0 && x->cl_ratio != 0)
		    || (*best != (call_p)0 && x->cl_ratio > (*best)->cl_ratio))
		{
			*best = x;
		}
	}
}

static call_p best_one(call_p list, long space)
{
	/* Find the best candidate of the list
	 * that has not already been selected. The
	 * candidate must fit in the given space.
	 * We look in the cdr as well as in the car
	 * direction.
	 */

	call_p best = (call_p)0;
	call_p c;

	for (c = list; c != (call_p)0; c = c->cl_cdr)
	{
		if (IS_SELECTED(c))
		{
			compare(best_one(c->cl_car, space), &best, space);
		}
		else
		{
			compare(c, &best, space);
		}
	}
	return best;
}

static void singles(call_p cals)
{
	/* If a procedure is only called once, this call
	 * will be expanded in line, because it costs
	 * no extra space.
	 */

	call_p c;

	for (c = cals; c != (call_p)0; c = c->cl_cdr)
	{
		if (IS_SELECTED(c))
		{
			singles(c->cl_car);
		}
		else
		{
			if (c->cl_proc->P_NRCALLED == 1 && !IS_CHANGED(c->cl_proc)
			    && (complete_program || (c->cl_proc->p_flags1 & PF_EXTERNAL) == 0)
			    && (c->cl_proc->p_flags1 & PF_LPI) == 0)
			{
				c->cl_proc->P_NRCALLED = 0;
				SELECTED(c);
				EVER_EXPANDED(find_origin(c));
				DISPENSABLE(c->cl_proc);
				CHANGED(c->cl_caller);
				OUTVERBOSE("singles: procedure %d can be removed", c->cl_proc->p_id, 0);
#ifdef VERBOSE
				Spremoved++;
#endif
			}
		}
	}
}

static void single_calls(proc_p proclist)
{
	proc_p p;

	for (p = proclist; p != (proc_p)0; p = p->p_next)
	{
		if (!BIG_CALLER(p) && !IS_DISPENSABLE(p))
		{
			/* Calls appearing in a large procedure or in
			 * a procedure that was already eliminated
			 * are not considered.
			 */
			singles(p->P_CALS);
		}
	}
}

static void unused(proc_p proclist)
{
	/* See if any procedures are defined but will
	 * never be called. These can be safely removed.
	 * https://github.com/davidgiven/ack/issues/230
	 *
	 * FIXME:
	 *  1. This does not remove all unused procedures,
	 *     but only those which would be considered
	 *     suitable for inlining if used. It is not
	 *     yet safe to remove "unsuitable" procedures,
	 *     since the IL pass does not maintain correct
	 *     caller counts for these.
	 *  2. Static data structures defined inside
	 *     unused procedures are not yet removed.
	 *  3. The "whole program" (-a) setting is ignored.
	 */

	proc_p p;

	for (p = proclist; p != (proc_p)0; p = p->p_next)
	{
		if (!BIG_CALLER(p) && !IS_DISPENSABLE(p) && SUITABLE(p) && p->P_NRCALLED == 0
		    && (p->p_flags1 & (PF_EXTERNAL | PF_LPI)) == 0)
		{
			DISPENSABLE(p);
			OUTVERBOSE("unused: procedure %d can be removed", p->p_id, 0);
#ifdef VERBOSE
			Spremoved++;
#endif
		}
	}
}

void select_calls(proc_p proclist, FILE* ccf, long space)
{
	/* Select all calls that are to be expanded in line. */

	proc_p p, chp;
	call_p best, x;

	for (;;)
	{
		best = (call_p)0;
		chp = (proc_p)0; /* the changed procedure */
		for (p = proclist; p != (proc_p)0; p = p->p_next)
		{
			if (!BIG_CALLER(p) && !IS_DISPENSABLE(p))
			{
				/* Calls appearing in a large procedure or in
				 * a procedure that was already eliminated
				 * are not considered.
				 */
				x = best_one(p->P_CALS, space);
				compare(x, &best, space);
				if (x == best)
					chp = p;
			}
		}
		if (best == (call_p)0)
			break;
		if (!is_dispensable(best->cl_proc, ccf))
		{
			space -= (best->cl_proc->P_SIZE - space_saved(best));
		}
		else
			space += space_saved(best);
#ifdef VERBOSE
		if (verbose_flag)
			fprintf(stderr, "space left: %ld\n", space);
#endif
		selected(best);
		CHANGED(chp);
	}
	single_calls(proclist);
	unused(proclist);
#ifdef VERBOSE
	Sstat(proclist, space);
#endif
}

static void nonnested_calls(FILE* cfile)
{
	call_p c, a;

	while ((c = getcall(cfile)) != (call_p)0)
	{
		/* find the call in the call list of the caller */
		for (a = c->cl_caller->P_CALS; a != (call_p)0 && c->cl_id != a->cl_id; a = a->cl_cdr)
			;
		assert(a != (call_p)0 && a->cl_proc == c->cl_proc);
		if (IS_EVER_EXPANDED(a))
		{
			a->cl_actuals = c->cl_actuals;
			c->cl_actuals = (actual_p)0;
		}
		rem_call(c);
	}
}

static void copy_pars(call_p src, call_p dest)
{
	/* Copy the actual parameters of src to dest. */

	actual_p as, ad, *app;

	app = &dest->cl_actuals;
	for (as = src->cl_actuals; as != (actual_p)0; as = as->ac_next)
	{
		ad = newactual();
		ad->ac_exp = copy_expr(as->ac_exp);
		ad->ac_size = as->ac_size;
		ad->ac_inl = as->ac_inl;
		*app = ad;
		app = &ad->ac_next;
	}
}

static void nest_pars(call_p cals)
{
	/* Recursive auxiliary procedure of add_actuals. */

	call_p c, org;

	for (c = cals; c != (call_p)0; c = c->cl_cdr)
	{
		if (IS_SELECTED(c))
		{
			org = find_origin(c);
			copy_pars(org, c);
			nest_pars(c->cl_car);
		}
	}
}

void add_actuals(proc_p proclist, FILE* cfile)
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
	for (p = proclist; p != (proc_p)0; p = p->p_next)
	{
		for (a = p->P_CALS; a != (call_p)0; a = a->cl_cdr)
		{
			nest_pars(a->cl_car);
		}
	}
}

static void clean(call_p* cals)
{
	call_p c, next, *cpp;

	/* Recursive auxiliary routine of cleancals */

	cpp = cals;
	for (c = *cpp; c != (call_p)0; c = next)
	{
		next = c->cl_cdr;
		if (IS_SELECTED(c))
		{
			clean(&c->cl_car);
			cpp = &c->cl_cdr;
		}
		else
		{
			assert(c->cl_car == (call_p)0);
			oldcall(c);
			*cpp = next;
		}
	}
}

void cleancals(proc_p proclist)
{
	/* Remove all calls in the P_CALS list of p
	 * that were not selected for in line expansion.
	 */

	proc_p p;

	for (p = proclist; p != (proc_p)0; p = p->p_next)
	{
		clean(&p->P_CALS);
	}
}

void append_abstract(call_p a, proc_p p)
{
	/* Append an abstract of a call-descriptor to
	 * the call-list of procedure p.
	 */

	call_p c;

	if (p->P_CALS == (call_p)0)
	{
		p->P_CALS = a;
	}
	else
	{
		for (c = p->P_CALS; c->cl_cdr != (call_p)0; c = c->cl_cdr)
			;
		c->cl_cdr = a;
	}
}

#ifdef VERBOSE

/* At the end, we traverse the entire call-list, to see why the
 * remaining calls were not expanded inline.
 */

static void Sstatist(call_p list, long space)
{
	call_p c;

	for (c = list; c != (call_p)0; c = c->cl_cdr)
	{
		if (IS_SELECTED(c))
		{
			Sstatist(c->cl_car, space);
		}
		else
		{
			if (IS_CHANGED(c->cl_proc))
				Schangedcallee++;
			else if (BIG_PROC(c->cl_proc))
				Sbigcallee++;
			else if (c->cl_proc->P_SIZE > space)
				Sspace++;
			else if (c->cl_ratio == 0)
				Szeroratio++;
			else
				assert(false);
		}
	}
}

static void Sstat(proc_p proclist, long space)
{
	proc_p p;

	for (p = proclist; p != (proc_p)0; p = p->p_next)
	{
		if (BIG_CALLER(p))
			Sbig_caller++;
		else if (IS_DISPENSABLE(p))
			Sdispensable++;
		else
			Sstatist(p->P_CALS, space);
	}
}
#endif
