/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ S U B S T . C
 */

#include <stdio.h>
#include <em_mnem.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/get.h"
#include "il_aux.h"
#include "il3_aux.h"
#include "il3_change.h"
#include "il3_subst.h"

STATIC line_p fetch_text(lf,c)
	FILE *lf;
	call_p c;
{
	/* Read the EM text of the called procedure.
	 * We use random access I/O here.
	 */

	line_p l;
	proc_p p;
	lset   savmes;

	savmes = mesregs;
	mesregs = Lempty_set();
	fseek(lf,c->cl_proc->P_LADDR,0);
	l = get_text(lf,&p);
	assert (p == c->cl_proc);
	Ldeleteset(mesregs);
	mesregs = savmes;
	return l;
}




line_p scan_to_cal(lines,n)
	line_p lines;
	short  n;
{
	/* Find the n-th CAL instruction */

	register line_p l;

	for (l = lines; l != (line_p) 0; l = l->l_next) {
		if (INSTR(l) == op_cal) {
			if (--n == 0) return l;
		}
	}
	return (line_p) 0; /* CAL not found */
}



substitute(lf,c,cal,firstline)
	FILE *lf;
	call_p c;
	line_p cal,firstline;
{
	/* Perform in line substitution of the call described
	 * by c. The EM text of the called routine is fetched
	 * and modified, the calling sequence is changed,
	 * the modified routine is put at the place of the call
	 * and all global information (proctable etc.) is kept
	 * up to date.
	 */

	line_p l, text, lab;
	offset ab_off, lb_off;
	line_p startscan, ncal;
	short lastcid;
	call_p nc;

	Ssubst++;
	ab_off = - curproc->p_localbytes;
	/* offset of temporaries for parameters
	 * that are not expanded in line.
	 */
	chg_callseq(c,cal,&l);
	/* Change the calling sequence; l points to the place
	 * where the expanded text must be put
	 */
	text = fetch_text(lf,c);	/* fetch EM text of called routine */
	lb_off = - curproc->p_localbytes;
	/* offset of temps. for locals of called proc. */
	curproc->p_localbytes += c->cl_proc->P_ORGLOCALS;
	/* locals of called routine are put in stack frame of caller */
	if (!FALLTHROUGH(c->cl_proc)) {
		/* The called proc contains one or more RETurns
		 * somewhere in the middle of its text; these
		 * should be changed into a jump to the end
		 * of the text. We create a label for this
		 * purpose (if there was no one already).
		 */
		lab = make_label(l,curproc);
	}
	modify(text,c,lab,ab_off,lb_off,curproc->p_nrlabels);
	curproc->p_nrlabels += c->cl_proc->P_ORGLABELS;
	insert(text,l,firstline);
	/* insert text; instructions are put after l, pseudos
	 * are put at beginning of caller.
	 */
	/* Now take care of the nested calls */
	startscan = l->l_next;
	lastcid = 0;
	for (nc = c->cl_car; nc != (call_p) 0; nc = nc->cl_cdr) {
		mod_actuals(nc,c,lab,ab_off,lb_off,curproc->p_nrlabels);
		ncal = scan_to_cal(startscan,nc->cl_id - lastcid);
		assert(ncal != (line_p) 0);
		startscan = scan_to_cal(ncal->l_next,1);
		lastcid = nc->cl_id;
		substitute(lf,nc,ncal,firstline);
	}
}
