/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ C H A N G E . C
 */


#include <stdlib.h>
#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include <em_mes.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/def.h"
#include "../share/lset.h"
#include "../share/aux.h"
#include "../share/get.h"
#include "../share/put.h"
#include "il_aux.h"
#include "il3_change.h"
#include "il3_aux.h"

/* chg_callseq */




STATIC line_p par_expr(l,expr)
	line_p l, expr;
{
	/* Find the first line of the expression of which
	 * l is the last line; expr contains a pointer
	 * to a copy of that expression; effectively we
	 * just have to tally lines.
	 */

	line_p lnp;

	for (lnp = expr->l_next; lnp != (line_p) 0; lnp = lnp->l_next) {
		assert(l != (line_p) 0);
		l = PREV(l);
	}
	return l;
}



STATIC rem_text(l1,l2)
	line_p l1,l2;
{
	/* Remove the lines from l1 to l2 (inclusive) */

	line_p l, lstop;
	l = PREV(l1);
	lstop = l2->l_next;
	while (l->l_next != lstop) {
		rem_line(l->l_next);
	}
}



STATIC store_tmp(p,l,size)
	proc_p p;
	line_p l;
	offset  size;
{
	/* Emit code to store a 'size'-byte value in a new
	 * temporary local variable in the stack frame of p.
	 * Put this code after line l.
	 */

	line_p lnp;

	lnp = int_line(tmplocal(p,size)); /* line with operand temp. */
	if (size == ws) {
		lnp->l_instr = op_stl;  /* STL temp. */
	} else {
		if (size == 2*ws) {
			lnp->l_instr = op_sdl; /* SDL temp. */
		} else {
			/* emit 'LAL temp; STI size' */
			lnp->l_instr = op_lal;
			appnd_line(lnp,l);
			l = lnp;
			assert ((short) size == size);
			lnp = newline(OPSHORT);
			SHORT(lnp) = size;
			lnp->l_instr = op_sti;
		}
	}
	appnd_line(lnp,l);
}



STATIC chg_actuals(c,cal)
	call_p c;
	line_p cal;
{
	/* Change the actual parameter expressions of the call. */

	actual_p act;
	line_p llast,lfirst,l;

	llast = PREV(cal);
	for (act = c->cl_actuals; act != (actual_p) 0; act = act->ac_next) {
		lfirst = par_expr(llast,act->ac_exp);
		/* the code from lfirst to llast is a parameter expression */
		if (act->ac_inl) {
			/* in line parameter; remove it */
			l = llast;
			llast = PREV(lfirst);
			rem_text(lfirst,l);
		} else {
			store_tmp(curproc,llast,act->ac_size);
			/* put a "STL tmp" -like instruction after the code */
			llast = PREV(lfirst);
		}
	}
}



STATIC rm_callpart(c,cal)
	call_p c;
	line_p cal;
{
	/* Remove the call part, consisting of a CAL,
	 * an optional ASP and an optional LFR.
	 */

	line_p l;

	l= PREV(cal);
	rem_line(cal);
	if (c->cl_proc->p_nrformals > 0) {
		/* called procedure has parameters */
		assert (INSTR(l->l_next) == op_asp);
		rem_line(l->l_next);
	}
	if (INSTR(l->l_next) == op_lfr) {
		rem_line(l->l_next);
	}
}



chg_callseq(c,cal,l_out)
	call_p c;
	line_p cal,*l_out;
{
	/* Change the calling sequence. The actual parameter
	 * expressions are changed (in line parameters are
	 * removed, all other ones now store their result
	 * in a temporary local of the caller);
	 * the sequence "CAL ; ASP ; LFR" is removed.
	 */


	chg_actuals(c,cal);
	*l_out = PREV(cal); /* last instr. of new parameter part */
	rm_callpart(c,cal);
}


/* make_label */

line_p make_label(l,p)
	line_p l;
	proc_p p;
{
	/* Make sure that the instruction after l
	 * contains an instruction label. If this is
	 * not already the case, create a new label.
	 */

	line_p lab;

	if (l->l_next != (line_p) 0 && INSTR(l->l_next) == op_lab) {
		return l->l_next;
	}
	lab = newline(OPINSTRLAB);
	lab->l_instr = op_lab;
	p->p_nrlabels++;
	INSTRLAB(lab) = p->p_nrlabels;
	appnd_line(lab,l);
	return lab;
}



/* modify */

STATIC void act_info(off,acts,ab_off,act_out,off_out)
	offset off, ab_off, *off_out;
	actual_p acts, *act_out;
{
	/* Find the actual parameter that corresponds to
	 * the formal parameter with the given offset.
	 * Return it via act_out. If the actual is not
	 * an in-line actual, determine which temporary
	 * local is used for it; return the offset of that
	 * local via off_out.
	 */

	offset sum = 0, tmp = 0;
	actual_p act;

	for (act = acts; act != (actual_p) 0; act = act->ac_next) {
		if (!act->ac_inl) {
			tmp -= act->ac_size;
		}
		if (sum >= off) {
			/* found */
			*act_out = act;
			if (!act->ac_inl) {
				*off_out = tmp + sum - off + ab_off;
			} else {
				assert (sum == off);
			}
			return;
		}
		sum += act->ac_size;
	}
	assert(FALSE);
}



STATIC store_off(off,l)
	offset off;
	line_p l;
{
	if (TYPE(l) == OPSHORT) {
		assert ((short) off == off);
		SHORT(l) = (short) off;
	} else {
		OFFSET(l) = off;
	}
}



STATIC inl_actual(l,expr)
	line_p l, expr;
{
	/* Expand an actual parameter in line.
	 * A LOL or LDL instruction is replaced
	 * by an expression.
	 * A SIL or LIL is replaced by the expression
	 * followed by a STI or LOI.
	 */

	line_p e, lnp, s;
	short  instr;

	instr = INSTR(l);
	assert(expr != (line_p) 0);
	e = copy_expr(expr); /* make a copy of expr. */
	if (instr == op_sil || instr == op_lil) {
		s = int_line((offset) ws);
		s->l_instr = (instr == op_sil ? op_sti : op_loi);
		appnd_line(s,last_line(e));
	} else {
		assert(instr == op_lol || instr == op_ldl);
	}
	lnp = PREV(l);
	rem_line(l);
	app_list(e,lnp);
}



STATIC localref(l,c,ab_off,lb_off)
	line_p l;
	call_p c;
	offset ab_off, lb_off;
{
	/* Change a reference to a local variable or parameter
	 * of the called procedure.
	 */

	offset off, tmpoff;
	actual_p act;

	off = off_set(l);
	if (off < 0) {
		/* local variable, only the offset changes */
		store_off(lb_off + off,l);
	} else {
		act_info(off,c->cl_actuals,ab_off,&act,&tmpoff); /* find actual */
		if (act->ac_inl) {
			/* inline actual parameter */
			inl_actual(l,act->ac_exp);
		} else {
			/* parameter stored in temporary local */
			store_off(tmpoff,l);
		}
	}
}



STATIC chg_mes(l,c,ab_off,lb_off)
	line_p l;
	call_p c;
	offset ab_off, lb_off;
{
	/* The register messages of the called procedure
	 * must be changed. If the message applies to a
	 * local variable or to a parameter that is not
	 * expanded in line, the offset of the variable
	 * is changed; else the entire message is deleted.
	 */

	offset off, tmpoff;
	actual_p act;
	arg_p arg;

	arg = ARG(l);
	switch ((int) arg->a_a.a_offset) {
	   case ms_reg:
		if ((arg = arg->a_next) != (arg_p) 0) {
			/* "mes 3" without further argument is not changed */
			off = arg->a_a.a_offset;
			if (off < 0) {
				/* local variable */
				arg->a_a.a_offset += lb_off;
			} else {
				act_info(off,c->cl_actuals,ab_off,&act,&tmpoff);
				if (act->ac_inl) {
					/* in line actual */
					rem_line(l);
				} else {
					arg->a_a.a_offset = tmpoff;
				}
			}
		}
		break;
	   case ms_par:
		rem_line(l);
		break;
	}
}



STATIC chg_ret(l,c,lab)
	line_p l,lab;
	call_p c;
{
	/* Change the RET instruction appearing in the
	 * expanded text of a call. If the called procedure
	 * falls through, the RET is just deleted; else it
	 * is replaced by a branch.
	 */

	line_p lnp, bra;

	lnp = PREV(l);
	rem_line(l);
	if (!FALLTHROUGH(c->cl_proc)) {
		bra = newline(OPINSTRLAB);
		bra->l_instr = op_bra;
		INSTRLAB(bra) = INSTRLAB(lab);
		appnd_line(bra,lnp);
	}
}



STATIC mod_instr(l,c,lab,ab_off,lb_off,lab_off)
	line_p l,lab;
	call_p c;
	offset ab_off,lb_off;
	int    lab_off;
{
	if (TYPE(l) == OPINSTRLAB) {
		INSTRLAB(l) += lab_off;
	} else {
	    switch(INSTR(l)) {
		case op_stl:
		case op_inl:
		case op_del:
		case op_zrl:
		case op_sdl:
		case op_lol:
		case op_ldl:
		case op_sil:
		case op_lil:
		case op_lal:
			localref(l,c,ab_off,lb_off);
			break;
		case op_ret:
			chg_ret(l,c,lab);
			break;
		case ps_pro:
		case ps_end:
		case ps_sym:
		case ps_hol:
		case ps_bss:
		case ps_con:
		case ps_rom:
			rem_line(l);
			break;
		case ps_mes:
			chg_mes(l,c,ab_off,lb_off);
			break;
	    }
	}
}


modify(text,c,lab,ab_off,lb_off,lab_off)
	line_p text,lab;
	call_p c;
	offset ab_off,lb_off;
	int    lab_off;
{
	/* Modify the EM text of the called procedure.
	 * References to locals and parameters are
	 * changed; RETs are either deleted or replaced
	 * by a BRA to the given label; PRO and END pseudos
	 * are removed; instruction labels are changed, in
	 * order to make them different from any label used
	 * by the caller; some messages need to be changed too.
	 * Note that the first line of the text is a dummy instruction.
	 */

	register line_p l;
	line_p next;

	for (l = text->l_next; l != (line_p) 0; l = next) {
		next = l->l_next;
		/* This is rather tricky. An instruction like
		 * LOL 2 may be replaced by a number of instructions
		 * (if the parameter is expanded in line). This inserted
		 * code, however, should not be modified!
		 */
		mod_instr(l,c,lab,ab_off,lb_off,lab_off);
	}
}



mod_actuals(nc,c,lab,ab_off,lb_off,lab_off)
	call_p nc,c;
	line_p lab;
	offset ab_off,lb_off;
	int    lab_off;
{
	actual_p act;
	line_p l, next, dum;

	dum = newline(OPNO);
	PREV(dum) = (line_p) 0;
	for (act = nc->cl_actuals; act != (actual_p) 0; act = act->ac_next) {
		l = act->ac_exp;
		assert(l != (line_p) 0);
		/* Insert a dummy instruction before l */
		dum->l_next = l;
		PREV(l) = dum;
		while(l != (line_p) 0) {
			next = l->l_next;
			mod_instr(l,c,lab,ab_off,lb_off,lab_off);
			l = next;
		}
		act->ac_exp = dum->l_next;
		PREV(dum->l_next) = (line_p) 0;
	}
	oldline(dum);
}



/* insert */

STATIC line_p first_nonpseudo(l)
	line_p l;
{
	/* Find the first non-pseudo instruction of
	 * a list of instructions.
	 */

	while (l != (line_p) 0 && INSTR(l) >= sp_fpseu &&
		INSTR(l) <= ps_last) l = l->l_next;
	return l;
}



void
insert(text,l,firstline)
	line_p text,l,firstline;
{
	/* Insert the modified EM text of the called
	 * routine in the calling routine. Pseudos are
	 * put after the pseudos of the caller; all
	 * normal instructions are put at the place
	 * where the CAL originally was.
	 */

	line_p l1,l2,lastpseu;

	l1 = text->l_next;
	oldline(text);  /* remove dummy head instruction */
	if (l1 == (line_p) 0) return; /* no text at all! */
	l2 = first_nonpseudo(l1);
	if (l2 == (line_p) 0) {
		/* modified code consists only  of pseudos */
		app_list(l1,PREV(first_nonpseudo(firstline)));
	} else {
		if (l1 == l2) {
			/* no pseudos */
			app_list(l2,l);
		} else {
			lastpseu = PREV(first_nonpseudo(firstline));
			PREV(l2)->l_next = (line_p) 0; /* cut link */
			app_list(l2,l);  /* insert normal instructions */
			app_list(l1,lastpseu);
		}
	}
}



liquidate(p,text)
	proc_p p;
	line_p text;
{
	/* All calls to procedure p were expanded in line, so
	 * p is no longer needed. However, we must not throw away
	 * any data declarations appearing in p.
	 * The proctable entry of p is not removed, as we do not
	 * want to create holes in this table; however the PF_BODYSEEN
	 * flag is cleared, so p gets the same status as a procedure
	 * whose body is unmkown.
	 */

	line_p l, nextl, lastkept = (line_p) 0;
	call_p c, nextc;

	for (l = text; l != (line_p) 0; l = nextl) {
		nextl = l->l_next;
		switch(INSTR(l)) {
			case ps_sym:
			case ps_hol:
			case ps_bss:
			case ps_con:
			case ps_rom:
				lastkept = l;
				break;
			default:
				rem_line(l);
		}
	}
	if (lastkept != (line_p) 0) {
		/* There were some data declarations in p,
		 * so we'll turn p into a data-unit; we'll
		 * have to append an end-pseudo for this
		 * purpose.
		 */
		lastkept->l_next = newline(OPNO);
		lastkept->l_next->l_instr = (byte) ps_end;
	}
	/* There may be some calls in the body of p that
	 * ought to be expanded in line. As p is removed
	 * anyway, there is no use in really performing
	 * these substitutions, so the call-descriptors
	 * are just thrown away.
	 */

	 for (c = p->P_CALS; c != (call_p) 0; c = nextc) {
		nextc = c->cl_cdr;
		rem_call(c);
	}
	/* change the proctable entry */
	p->p_flags1 &= (byte) ~PF_BODYSEEN;
	oldchange(p->p_change);
	olduse(p->p_use);
}
