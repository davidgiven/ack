/*
 * This file contains the main part of the stackheight computation phase. 
 *
 * Author: Hans van Eck. 
 */

#include <stdio.h>
#include <em_spec.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include "param.h"
#include "assert.h"
#include "types.h"
#include "shc.h"
#include "alloc.h"
#include "proinf.h"
#include "line.h"
#include "ext.h"
#include "pop_push.h"

extern char *pop_push[];
extern char flow_tab[];

#define NON_CONTINUABLE(i)	(flow_tab[i]&JUMP)
#define ISABRANCH(i)		(flow_tab[i]&HASLABEL)
#define ISCONDBRANCH(i)		(flow_tab[i]&CONDBRA)

lblst_p est_list = NULL;

#define INSTR(lnp)      (lnp->l_instr & BMASK)
#define TYPE(lnp)       lnp->l_optyp
#define PREV(lnp)       lnp->l_prev
#define SHORT(lnp)      lnp->l_a.la_short
#define MINI(lnp)	((lnp->l_optyp & BMASK) - Z_OPMINI)

#define IS_MINI(lnp)	(lnp->l_optyp >= OPMINI)
#define IS_LOC(l)	(l!=(line_p) 0 && INSTR(l)==op_loc && IS_MINI(l))

int state;
static int stack_height = 0;

init_state()
{
	stack_height = 0;
	change_state(KNOWN);
	est_list = NULL;
}

shc_pseudos()
{
	register line_p lp;

	for (lp = pseudos; lp != (line_p)0; lp = lp->l_next) {
		switch(INSTR(lp)) {
		case ps_con:
		case ps_rom:
			if (lp->l_optyp == OPLIST) {
				register arg_p ap = lp->l_a.la_arg;

				while (ap != (arg_p) 0) {
					if (ap->a_typ == ARGNUM) {
						assign_label(ap->a_a.a_np->n_repl);
					}
					ap = ap->a_next;
				}
			} else if (lp->l_optyp == OPNUMLAB)
				assign_label(lp->l_a.la_np->n_repl);
		}
	}
}

shc_instr(lnp)
line_p lnp;
{
	char *s;
	register instr = INSTR(lnp);
	register int mult, arg, argdef;
	line_p x = PREV(lnp);
	line_p y = (x == (line_p) 0 ? (line_p) 0 : PREV(x));

	if (state == NO_STACK_MES) return;

	if ( instr == op_lab) {
	    do_inst_label(lnp);
	    return;
	}
	if (instr < sp_fmnem || instr > sp_lmnem) {
		return;
	}

	if(state == NOTREACHED) return;	/* What else ? */
	s = pop_push[instr];

	if (*s != '0')
	    while (*s != '\0') {
		if (*s++ == '-') mult = -1;
		else mult = 1;
		if (TYPE(lnp) == OPSHORT) {
		    arg = SHORT(lnp);
		    if (arg < wordsize) arg = wordsize;
		    argdef = TRUE;
		} else if (IS_MINI(lnp)) {
		    arg = MINI(lnp);
		    if (arg > 0 && arg < wordsize) arg = wordsize;
		    if (arg < 0 && -arg < wordsize) arg = -wordsize;
		    argdef = TRUE;
		} else argdef = FALSE;
		switch (*s++) {
		case 'w': stack_height += mult * wordsize; break;
		case 'd': stack_height += mult * wordsize * 2; break;
		case 'p': stack_height += mult * pointersize; break;
		case 'a':
			    if (argdef == FALSE || instr == op_ass) {
				change_state(NO_STACK_MES);
				return;
			    }
			    stack_height += mult * arg;
			    break;
		case 'x':
			if (IS_LOC(x)) {
			    arg = MINI(x);
			    if (arg < wordsize) arg = wordsize;
			    stack_height += mult * arg;
			    break;
			}
			change_state(NO_STACK_MES);
			return;
		case 'y':
			if (IS_LOC(y)) {
			    arg = MINI(y);
			    if (arg < wordsize) arg = wordsize;
			    stack_height += mult * arg;
			    break;
			}
			change_state(NO_STACK_MES);
			return;
		case '?':
			/* Actually, the effect of a ret on the stack is
			 * known, but it has a '?' anyway. I think this
			 * should be changed in ~etc/em_table
			 */
			if (instr == op_ret)
				break;
			change_state(NO_STACK_MES);
			return;
		default:
			assert(FALSE);
		}
	    }

	if (ISABRANCH(instr)) do_inst_label(lnp);
	if (NON_CONTINUABLE(instr)) change_state(NOTREACHED);
}

change_state(mode)
int mode;
{
	state = mode;
	if (mode != KNOWN) stack_height = 0;
}

delete_labels()
{
	register lblst_p tmp;

	while ((tmp = est_list) != NULL) {
	    est_list = est_list->ll_next;
	    oldlblst(tmp);
	}
}

inst_old_label(lst_elt)
register lblst_p lst_elt;
{
	if (state != NOTREACHED) {
		if (stack_height < 0 || lst_elt->ll_height != stack_height) {
			change_state(NO_STACK_MES);
		}
	} else {				/* after a label */
		stack_height = lst_elt->ll_height;
		change_state(KNOWN);
	}
}

inst_new_label(label)
register num_p label;
{
	register lblst_p lst_elt;

	lst_elt = newlblst();
	lst_elt->ll_next = est_list;
	lst_elt->ll_num = label;
	lst_elt->ll_height = stack_height;

	est_list = lst_elt;

	label->n_lst_elt = lst_elt;
	label->n_flags |= NUMSET;
}

assign_label(label)
num_p label;
{
	if (label->n_flags & NUMSET)
	    inst_old_label(label->n_lst_elt);
	else inst_new_label(label);
}

do_inst_label(lnp)	/* (re-)install a label */
line_p lnp;
{
	num_p label = lnp->l_a.la_np->n_repl;
	int instr = INSTR(lnp);

	assign_label(label);

	if (instr == op_lab) {
	    if (state == NOTREACHED)  {
		label->n_lst_elt->ll_fallthrough = FALSE;
	    } else {
		label->n_lst_elt->ll_fallthrough = TRUE;
	    }
	} else if (ISCONDBRANCH(instr)) {	/* conditional branch */
	    label->n_flags |= NUMCOND;
	}
}
