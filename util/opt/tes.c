#ifndef NORCSID
static char rcsid[] = "$Id$";
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

#define INSTR(lnp)      (lnp->l_instr & BMASK)
#define TYPE(lnp)       lnp->l_optyp
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

tes_instr(lnp, x, y)
line_p lnp, x, y;
{
	char *s;
	register instr = INSTR(lnp);
	register int arg, argdef;
	int neg = 0;

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
			neg = (*s++ == '-');

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
		/*
		 * When the last argument was negative, the element size
		 * must be negated.  This is to catch 'asp -4'.
		 */
		if (neg) stacktop = -stacktop;
	}

	if (stacktop < 0) stacktop = 0;

	if (ISABRANCH(instr)) do_inst_label(lnp);
	if (NON_CONTINUABLE(instr)) {
		state = NOTREACHED;
		stacktop = 0;
	}
}

assign_label(label)
register num_p label;
{
	if (label->n_flags & NUMSET) {
		if (state == NOTREACHED || stacktop > label->n_size) {
			stacktop = label->n_size;
		} else if ( stacktop < label->n_size) {
			label->n_size = stacktop;
		}
	} else {
		label->n_size = stacktop;
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
		} else {
			label->n_flags |= NUMFALLTHROUGH;
		}
	} else if (ISCONDBRANCH(instr)) {	/* conditional branch */
		label->n_flags |= NUMCOND;
	}
	state = KNOWN;
}
