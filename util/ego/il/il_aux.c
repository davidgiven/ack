/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L _ A U X . C
 */

#include <stdlib.h>
#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "il_aux.h"


int tsize(type)
	int type;
{
	/* Determine the size of a variable of the
	 *  given type.
	 */

	switch(type) {
		case SINGLE:	return ws;
		case DOUBLE:	return 2*ws;
		case POINTER:	return ps;
		default:	assert(FALSE);
	}
	/* NOTREACHED */
}



line_p duplicate(lnp)
	line_p lnp;
{
	/* Make a duplicate of an EM instruction.
	 * Pseudos may not be passed as argument.
	 */

	line_p l;

	l = newline(TYPE(lnp));
	l->l_instr = INSTR(lnp);
	switch(TYPE(l)) {
		case OPNO:
			break;
		case OPSHORT:
			SHORT(l) = SHORT(lnp);
			break;
		case OPOFFSET:
			OFFSET(l) = OFFSET(lnp);
			break;
		case OPINSTRLAB:
			INSTRLAB(l) = INSTRLAB(lnp);
			break;
		case OPOBJECT:
			OBJ(l) = OBJ(lnp);
			break;
		case OPPROC:
			PROC(l) = PROC(lnp);
			break;
		default:
			assert(FALSE); /* cannot copy pseudo */
	}
	return l;
}




line_p copy_expr(l1)
	line_p l1;
{
	/* copy the expression */

	line_p head, tail, l, lnp;

	head = (line_p) 0;
	for (lnp = l1; lnp != (line_p) 0; lnp = lnp->l_next) {
		l = duplicate(lnp);
		if (head == (line_p) 0) {
			head = tail = l;
			PREV(l) = (line_p) 0;
		} else {
			tail->l_next = l;
			PREV(l) = tail;
			tail = l;
		}
	}
	return head;
}



void rem_call(c)
	call_p c;
{
	actual_p act, nexta;
	call_p   nc,nextc;
	line_p   l,   nextl;

	for (act = c->cl_actuals; act != (actual_p) 0; act = nexta) {
		nexta = act->ac_next;
		for (l = act->ac_exp; l != (line_p) 0; l = nextl) {
			nextl = l->l_next;
			oldline(l);
		}
		oldactual(act);
	}
	nc = c->cl_car;
	oldcall(c);
	for (; nc != (call_p) 0; nc = nextc) {
		/* Take care of nested calls */
		nextc = nc->cl_cdr;
		rem_call(nc);
	}
}



/* remunit */

STATIC short remlines(l)
	line_p l;
{

	register line_p lnp;
	line_p next;

	for (lnp = l; lnp != (line_p) 0; lnp = next) {
		next = lnp->l_next;
		oldline(lnp);
	}
}



void remunit(short kind, proc_p p, line_p l)
{
	register bblock_p b;
	bblock_p next;
	Lindex   pi;

	if (kind == LDATA) {
		remlines(l);
		return;
	}
	for (b = p->p_start; b != (bblock_p) 0; b = next) {
		next = b->b_next;
		remlines(b->b_start);
		Ldeleteset(b->b_loops);
		Ldeleteset(b->b_succ);
		Ldeleteset(b->b_pred);
		oldbblock(b);
	}
	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
					 pi = Lnext(pi,p->p_loops)) {
		oldloop(Lelem(pi));
	}
	Ldeleteset(p->p_loops);
	oldmap((void **) lmap,llength);
	oldmap((void **) lbmap,llength);
	oldmap((void **) bmap,blength);
	oldmap((void **) lpmap,lplength);
}

void remcc(head)
	calcnt_p head;
{
	calcnt_p cc, next;

	for (cc = head; cc != (calcnt_p) 0; cc = next) {
		next = cc->cc_next;
		oldcalcnt(cc);
	}
}


/* Extra I/O routines */

call_p getcall(cf)
	FILE *cf;
{
	/* read a call from the call-file */

	call_p c;
	proc_p voided;
	actual_p act,*app;
	short n,m;

	curinp = cf;
	c = newcall();
	n = getshort(); /* void nesting level */
	if (feof(curinp)) return (call_p) 0;
	c->cl_caller = pmap[getshort()];
	c->cl_id     = getshort();
	c->cl_proc   = pmap[getshort()];
	c->cl_looplevel = getbyte();
	c->cl_flags = getbyte();
	c->cl_ratio  = getshort();
	app = &c->cl_actuals;
	n = getshort();
	while(n--) {
		act = newactual();
		m = getshort();
		act->ac_size = getoff();
		act->ac_inl = getbyte();
		act->ac_exp = getlines(cf,m,&voided,FALSE);
		*app = act;
		app = &act->ac_next;
	}
	*app = (actual_p) 0;
	return c;
}



line_p get_text(lf,p_out)
	FILE *lf;
	proc_p *p_out;
{
	/* Read the EM text of one unit
	 * If it is a procedure, set p_out to
	 * the proc. just read. Else set p_out
	 * to 0.
	 */

	line_p dumhead, l, lprev;
	loop_p *oldlpmap = lpmap;
	line_p *oldlmap = lmap;
	short oldllength = llength;
	short oldlastlabid = lastlabid;

	curinp = lf;
	*p_out = (proc_p) 0;
	dumhead = newline(OPNO);
	/* The list of instructions is preceeded by a dummy
	 * line, to simplify list manipulation
	 */
	dumhead->l_instr = op_nop; /* just for fun */
	lprev = dumhead;
	for (;;) {
		l = read_line(p_out);
		if (feof(curinp)) return (line_p) 0;
		lprev->l_next = l;
		PREV(l) = lprev;
		if (INSTR(l) == ps_end) break;
		if (INSTR(l) == ps_mes) {
			message(l);
		}
		lprev = l;
	}
	/* The tables that map labels to instructions
	 * and labels to basic blocks are not used.
	 */
	if (*p_out != (proc_p) 0) {
		oldmap((void **) lmap,llength);
		oldmap((void **) lbmap,llength);
		lmap = oldlmap;
		lpmap = oldlpmap;
	}
	llength = oldllength;
	lastlabid = oldlastlabid;
	return dumhead;
}



calcnt_p getcc(ccf,p)
	FILE *ccf;
	proc_p p;
{
	/* Get call-count info of procedure p */

	calcnt_p head,cc,*ccp;
	short i;

	fseek(ccf,p->p_extend->px_il.p_ccaddr,0);
	curinp = ccf;
	head = (calcnt_p) 0;
	ccp = &head;
	for (i = getshort(); i != (short) 0; i--) {
		cc = *ccp = newcalcnt();
		cc->cc_proc = pmap[getshort()];
		cc->cc_count = getshort();
		ccp = &cc->cc_next;
	}
	return head;
}


/* The following routines are only used by the Inline Substitution phase */


STATIC void putactuals(alist,cfile)
	actual_p alist;
	FILE     *cfile;
{
	/* output a list of actual parameters */

	actual_p a,next;
	line_p l;
	int count;

	count = 0;
	for (a = alist; a != (actual_p) 0; a = a->ac_next) count++;
	outshort(count); /* number of actuals */
	for (a = alist; a != (actual_p) 0; a = next) {
		next = a->ac_next;
		count = 0;
		for (l = a->ac_exp; l != (line_p) 0; l= l->l_next) count++;
		outshort(count); /* length of actual */
		outoff(a->ac_size);
		outbyte(a->ac_inl);
		count = putlines(a->ac_exp,cfile);
		oldactual(a);
	}
}



void putcall(call_p c, FILE *cfile, short level)
{
	/* output a call */

	call_p nc,nextc;


	curoutp = cfile;
	outshort(level);  /* nesting level */
	outshort(c->cl_caller->p_id);	/* calling proc */
	outshort(c->cl_id);
	outshort(c->cl_proc->p_id);	/* called proc */
	outbyte(c->cl_looplevel);
	outbyte(c->cl_flags);
	outshort(c->cl_ratio);
	putactuals(c->cl_actuals,cfile);
	nc = c->cl_car;
	oldcall(c);
	for (; nc != (call_p) 0; nc = nextc) {
		/* take care of nested calls */
		nextc = nc->cl_cdr;
		putcall(nc,cfile,level+1);
	}
}

long putcc(head,ccf)
	calcnt_p head;
	FILE     *ccf;
{
	/* Write call-count information to file ccf.
	 * Return the disk address of the info written.
	 */

	calcnt_p cc;
	long addr;
	short cnt;

	addr = ftell(ccf);
	curoutp = ccf;
	cnt = 0;
	for (cc = head; cc != (calcnt_p) 0;cc = cc->cc_next) cnt++;
	outshort(cnt);
	for (cc = head; cc != (calcnt_p) 0; cc = cc->cc_next) {
		outproc(cc->cc_proc);
		outshort(cc->cc_count);
	}
	return addr;
}
