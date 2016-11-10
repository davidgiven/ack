/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* C O P Y   P R O P A G A T I O N */

#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include "../share/types.h"
#include "ud.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/def.h"
#include "../share/aux.h"
#include "../share/locals.h"
#include "../ud/ud_defs.h"
#include "ud_copy.h"
#include "ud_const.h"
#include "ud_aux.h"



line_p *copies;		/* table of copies; every entry points to the
			 * store-instruction.
			 */
short *def_to_copynr;	/* table that maps a 'definition'-number to a 
			 * 'copy' number.
			 */
short nrcopies;		/* number of copies in the current procedure
			 * (length of copies-table)
			 */

#define COPY_NR(c)	def_to_copynr[c]
#define CHANGED(v,b) (Cis_elem(v,CHGVARS(b)) || Cis_elem(IMPLICIT_DEF(v),GEN(b)))


#define COUNT 0
#define MAP 1

STATIC void traverse_defs(p,action)
	proc_p p;
	int action;
{
	bblock_p b;
	line_p l;
	bool found;
	short defcnt,v,cnt;

	defcnt = 1;
	if (action == COUNT) {
		nrcopies = 0;
	} else {
		copies = (line_p *) newmap(nrcopies);
		def_to_copynr = newtable(nrdefs);
		cnt = 1;
	}
	if (defcnt > nrexpldefs) return;
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (defs[defcnt] == l) {
				if (is_copy(l)) {
					var_nr(PREV(l),&v,&found);
					if (found) {
						if (action == COUNT) {
							nrcopies++;
						} else {
							copies[cnt] = l;
							def_to_copynr[defcnt] =
							   cnt++;
						}
					}
				}
				if (++defcnt > nrexpldefs) return;
			}
		}
	}
}



STATIC make_copytab(p)
	proc_p p;
{
	/* Make a table of all copies appearing in procedure p.
	 * We first count how many there are, because we
	 * have to allocate a dynamic array of the correct size.
	 */

	traverse_defs(p,COUNT);
	traverse_defs(p,MAP);
}



STATIC bool is_changed(varl,start,stop)
	line_p varl, start, stop;
{
	/* See if the variable used by instruction varl
	 * is changed anywhere between 'start' and 'stop'
	 */

	register line_p l;
	short v;
	bool found;

	var_nr(varl,&v,&found);
	if (!found) {
		return TRUE; /* We don't maintain ud-info for this variable */
	}
	for (l = start; l != (line_p) 0 && l != stop; l = l->l_next) {
		if (does_expl_def(l) && same_var(varl,l)) return TRUE;
		if (does_impl_def(l) && affected(varl,v,l)) return TRUE;
	}
	return FALSE;
}



STATIC void gen_kill_copies(p)
	proc_p p;
{
	/* Compute C_GEN and C_KILL for every basic block
	 * of p.
	 */

	register line_p l;
	register bblock_p b,n;
	short v;
	bool found;
	short copycnt = 1, defcnt = 1;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		C_GEN(b) = Cempty_set(nrcopies);
		C_KILL(b) = Cempty_set(nrcopies);
	}
	if (nrcopies == 0) return;
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (copies[copycnt] == l) {
				var_nr(PREV(l),&v,&found);
				assert(found);
				for (n = p->p_start; n != (bblock_p) 0;
				     n = n->b_next) {
					if (n != b && CHANGED(v,n) &&
					    Cis_elem(EXPL_TO_DEFNR(defcnt),IN(n))) {
						Cadd(copycnt,&C_KILL(n));
					}
				}
				if (is_changed(PREV(l),l,(line_p) 0)) {
					Cadd(copycnt,&C_KILL(b));
				} else {
					Cadd(copycnt,&C_GEN(b));
				}
				if (++copycnt > nrcopies) return;
			}
			if (defs[defcnt] == l) defcnt++;
		}
	}
}



STATIC intersect_outs(bbset,setp,full_set)
	lset bbset;
	cset *setp,full_set;
{
	/* Take the intersection of C_OUT(b), for all b in bbset,
	 * and put the result in setp.
	 */

	Lindex i;

	Ccopy_set(full_set,setp);
	for (i = Lfirst(bbset); i != (Lindex) 0; i = Lnext(i,bbset)) {
		Cintersect(C_OUT((bblock_p) Lelem(i)), setp);
	}
}



STATIC init_cin(p,full_set)
	proc_p p;
	cset full_set;
{
	/* Initialize C_IN(b) and C_OUT(b), for every basic block b.
	 * C_IN of the root of the CFG (i.e. the procedure entry block)
	 * will contain every copy, as it trivially holds that for
	 * every copy "s: A := B" there is no assignment to B on any
	 * path from s to the beginning of the root (because PRED(root)=empty).
	 * C_IN and C_OUT of the root will never be changed.
	 * For all remaining blocks b, C_IN(b) is initialized to the set of
	 * all copies, and C_OUT is set to all copies but those killed in b.
	 */

	bblock_p b;
	bblock_p root = p->p_start;

	C_IN(root) = Cempty_set(nrcopies);
	Ccopy_set(full_set,&C_IN(root)); /* full_set is the set of all copies */
	/* C_OUT(root) = {all copies} - C_KILL(root) + C_GEN(root) */
	C_OUT(root) = Cempty_set(nrcopies);
	Ccopy_set(full_set,&C_OUT(root));
	Csubtract(C_KILL(root),&C_OUT(root));
	Cjoin(C_GEN(root),&C_OUT(root));
	for (b = root->b_next; b != (bblock_p) 0; b = b->b_next) {
		C_IN(b) = Cempty_set(nrcopies);
		Ccopy_set(full_set,&C_IN(b));
		C_OUT(b) = Cempty_set(nrcopies);
		Ccopy_set(full_set,&C_OUT(b));
		Csubtract(C_KILL(b),&C_OUT(b));
	}
}



STATIC solve_cin(p)
	proc_p p;
{
	/* Solve the data flow equations for reaching
	 * definitions of procedure p.
	 * These equations are:
	 *  (1)  C_OUT(b) = C_IN(b) - C_KILL(b) + C_GEN(b)
	 *  (2)  C_IN(b)  = C_OUT(p1) * .. * C_OUT(pn)
	 *  (3)  C_IN(root) = {all copies} ;
	 *       where PRED(b) = {p1, .. , pn}
	 *	 and '*' denotes set intersection.
	 * We use the iterative algorithm of Aho&Ullman to
	 * solve the equations.
	 */

	register bblock_p b;
	bool     change;
	cset     newin,full_set;
	short n;

	/* initializations */
	full_set = Cempty_set(nrcopies);
	for (n = 1; n <= nrcopies; n++) {
		Cadd(n,&full_set);
	}
	newin = Cempty_set(nrcopies);
	init_cin(p,full_set);
	change = TRUE;
	/* main loop */
	while (change) {
		change = FALSE;
		for (b = p->p_start->b_next; b != (bblock_p) 0; b = b->b_next) {
			intersect_outs(b->b_pred, &newin,full_set);
			/* newin = C_OUT(p1) * .. * C_OUT(pn) */
			if (!Cequal(newin,C_IN(b))) {
				change = TRUE;
				Ccopy_set(newin, &C_IN(b));
				Ccopy_set(C_IN(b),   &C_OUT(b));
				Csubtract(C_KILL(b), &C_OUT(b));
				Cjoin(C_GEN(b),      &C_OUT(b));
			}
		}
	}
	Cdeleteset(newin);
	Cdeleteset(full_set);
}



copy_analysis(p)
	proc_p p;
{
	/* Determine which copies procedure p has. Compute C_IN(b),
	 * for every basic block b.
	 */

	make_copytab(p); /* Make a table of all copies */
	gen_kill_copies(p); /* Compute C_GEN(b) and C_KILL(b), for every b */
	solve_cin(p); /* Solve equations for C_IN(b) */
}



bool is_copy(def)
	line_p def;
{
	/* See if the definition def is also a 'copy', i.e. an
	 * statement of the form 'A := B' (or, in EM terminology:
	 * a sequence 'Load Variable; Store Variable').
	 */


	line_p lhs;
	int instr;

	lhs = PREV(def);
	if (lhs == (line_p) 0) return FALSE;
	instr = INSTR(def);
	switch(INSTR(lhs)) {
		case op_lol:
		case op_loe:
			return instr == op_stl || instr == op_ste;
		case op_ldl:
		case op_lde:
			return instr == op_sdl || instr == op_sde;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



fold_var(old,new,b)
	line_p old, new;
	bblock_p b;
{
	/* The variable referenced by the EM instruction 'old'
	 * must be replaced by the variable referenced by 'new'.
	 */

	line_p l;

/* DEBUGGING: 
	local_p loc;
	short nr;
	bool ok;
	if (TYPE(old) == OPOBJECT) {
		printf("global var.");
	} else {
		printf("local var. with off. %ld",off_set(old));
		find_local(off_set(old),&nr,&ok);
		assert(ok);
		loc = locals[nr];
		printf(",score %ld",loc->lc_score);
	}
	printf(" replaced by ");
	if (TYPE(new) == OPOBJECT) {
		printf("global var.");
	} else {
		printf("local var. with off. %ld",off_set(new));
		find_local(off_set(new),&nr,&ok);
		assert(ok);
		loc = locals[nr];
		printf(",score %ld",loc->lc_score);
	}
	printf("\n");
END DEBUG */
	l = old;
	if (TYPE(l) != TYPE(new)) {
		l = newline(TYPE(new));
		l->l_instr = INSTR(new);
		repl_line(old,l,b);
	}
	switch(TYPE(new)) {
		case OPOBJECT:
			OBJ(l) = OBJ(new);
			break;
		case OPSHORT:
			SHORT(l) = SHORT(new);
			break;
		case OPOFFSET:
			OFFSET(l) = OFFSET(new);
			break;
		default:
			assert(FALSE);
	}
}



bool value_retained(copy,defnr,use,b)
	line_p copy,use;
	short  defnr;
	bblock_p b;
{
	/* See if the right hand side variable of the
	 * copy still has the same value at 'use'.
	 * If the copy and the use are in the same
	 * basic block (defnr = 0), search from the
	 * copy to the use, to see if the rhs variable
	 * is changed. If the copy is in another block,
	 * defnr is the definition-number of the copy.
	 * Search from the beginning of the block to
	 * the use, to see if the rhs is changed; if not,
	 * check that the copy is in C_IN(b).
	 */

	line_p rhs, start;

	rhs = PREV(copy);
	start = (defnr == 0 ? copy : b->b_start);
	return !is_changed(rhs,start,use) &&
	       (defnr == 0 || Cis_elem(COPY_NR(defnr), C_IN(b)));
}
