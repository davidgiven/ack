
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L _ A U X . C
 */

#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "../../../h/em_spec.h"
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



rem_call(c)
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



/* rem_graph */

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



remunit(kind,p,l)
	short    kind;
	proc_p   p;
	line_p   l;
{
	register bblock_p b;
	bblock_p next;
	Lindex   pi;
	loop_p   lp;

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
	oldmap(lmap,llength);
	oldmap(lbmap,llength);
	oldmap(bmap,blength);
	oldmap(lpmap,lplength);
}
remcc(head)
	calcnt_p head;
{
	calcnt_p cc, next;

	for (cc = head; cc != (calcnt_p) 0; cc = next) {
		next = cc->cc_next;
		oldcalcnt(cc);
	}
}
