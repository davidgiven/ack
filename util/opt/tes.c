#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif
/*
 * This file contains the main part of the top element size computation phase. 
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
#include "tes.h"
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
static int stacktop = 0;

init_state()
{
	stacktop = 0;
	state = KNOWN;
	est_list = NULL;
}

tes_pseudos()
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

tes_instr(lnp)
line_p lnp;
{
	char *s;
	register instr = INSTR(lnp);
	register int arg, argdef;
	line_p x = PREV(lnp);
	line_p y = (x == (line_p) 0 ? (line_p) 0 : PREV(x));

	if (instr == op_lab) {
		do_inst_label(lnp);
		return;
	}
	if (instr < sp_fmnem || instr > sp_lmnem) {
		return;
	}

	if (state == NOTREACHED) return;	/* What else ? */
	s = pop_push[instr];

	if (*s != '0') {
		while (*s != '\0') {
			 if (*s++ == '-') {		/* ignores asp -4 */
				stacktop = 0;
				continue;
			 }

			if (TYPE(lnp) == OPSHORT) {
				arg = SHORT(lnp);
				if (arg < wordsize) arg = wordsize;
				argdef = TRUE;
			} else if (IS_MINI(lnp)) {
				arg = MINI(lnp);
				if (arg > 0 && arg < wordsize) arg = wordsize;
				if (arg < 0 && -arg < wordsize) arg = -wordsize;
				argdef = TRUE;
			} else {
				argdef = FALSE;
			}
			switch (*s++) {
			case 'w': stacktop = wordsize; break;
			case 'd': stacktop = wordsize * 2; break;
			case 'p': stacktop = pointersize; break;
			case 'a':
				if (argdef == FALSE || instr == op_ass) {
					stacktop = 0;
				} else {
					stacktop = arg;
				}
				break;
			case 'x':
				if (IS_LOC(x)) {
					arg = MINI(x);
					if (arg < wordsize) arg = wordsize;
					stacktop = arg;
				} else {
					stacktop = 0;
				}
				break;
			case 'y':
				if (IS_LOC(y)) {
					arg = MINI(y);
					if (arg < wordsize) arg = wordsize;
					stacktop = arg;
				} else {
					stacktop = 0;
				}
				break;
			case '?':
				stacktop = 0;
				break;
			default:
				assert(FALSE);
			}
		}
	}

	if (stacktop < 0) stacktop = 0;

	if (ISABRANCH(instr)) do_inst_label(lnp);
	if (NON_CONTINUABLE(instr)) {
		state = NOTREACHED;
		stacktop = 0;
	}
}

delete_labels()
{
	register lblst_p tmp;

	while ((tmp = est_list) != NULL) {
	    est_list = est_list->ll_next;
	    oldlblst(tmp);
	}
}

assign_label(label)
num_p label;
{
	register lblst_p lst_elt;

	if (label->n_flags & NUMSET) {
		lst_elt = label->n_lst_elt;
		if (state == NOTREACHED || stacktop > lst_elt->ll_size) {
			stacktop = lst_elt->ll_size;
		} else if ( stacktop < lst_elt->ll_size) {
			lst_elt->ll_size = stacktop;
		}
	} else {
		lst_elt = newlblst();
		lst_elt->ll_next = est_list;
		lst_elt->ll_num = label;
		lst_elt->ll_size = stacktop;

		est_list = lst_elt;

		label->n_lst_elt = lst_elt;
		label->n_flags |= NUMSET;
	}
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
	state = KNOWN;
}
