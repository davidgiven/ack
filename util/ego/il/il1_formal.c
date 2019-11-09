/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ F O R M A L . C
 */

#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "il1_aux.h"
#include "il1_formal.h"

#define NOT_USED(f)	(!(f->f_flags & USEMASK))
#define USED_ONCE(f)	f->f_flags |= FF_ONCEUSED
#define USED_OFTEN(f)	f->f_flags |= FF_OFTENUSED
#define BADFORMAL(f)	f->f_flags |= FF_BAD

#define OUTSIDE_LOOP(b)	(Lnrelems(b->b_loops) == 0)
#define IS_FORMAL(x)	(x >= 0)



formal_p find_formal(p,type,off)
	proc_p  p;
	int	type;
	offset  off;
{
	/* Find a formal parameter of p
	 * If the formal overlaps with an existing formal
	 * or has an unknown type (i.e. its address is used)
	 * 0 is returned.
	 */

	formal_p f,prev,nf;

	if (type == UNKNOWN) return (formal_p) 0;
	prev = (formal_p) 0;
	for (f = p->P_FORMALS; f != (formal_p) 0; f = f->f_next) {
		if (f->f_offset >= off) break;
		prev = f;
	}
	if (f != (formal_p) 0 && f->f_offset == off) {
		return (same_size(f->f_type,type) ? f : (formal_p) 0);
	}
	if (f != (formal_p) 0 && par_overlap(off,type,f->f_offset,f->f_type)) {
		return (formal_p) 0;
	}
	if (prev != (formal_p) 0 && par_overlap(prev->f_offset,prev->f_type,
					off,type)) {
		return (formal_p) 0;
	}
	nf = newformal();
	nf->f_type = type;
	nf->f_offset = off;
	if (prev == (formal_p) 0) {
		p->P_FORMALS = nf;
	} else {
		prev->f_next = nf;
	}
	nf->f_next = f;
	return nf;
}



STATIC void no_inl_pars(p)
	proc_p p;
{
	/* p may not have any in line parameters */

	p->p_flags2 |= PF_NO_INLPARS;
	remov_formals(p);
}



STATIC void inc_use(f,b)
	formal_p f;
	bblock_p b;
{
	/* Increment the use count of formal f.
	 * The counter has only three states: not used,
	 * used once, used more than once.
	 * We count the number of times the formal
	 * is used dynamically (rather than statically),
	 * so if it is used in a loop, the counter
	 * is always set to more than once.
	 */

	if (NOT_USED(f) && OUTSIDE_LOOP(b)) {
		USED_ONCE(f);
	} else {
		USED_OFTEN(f);
	}
}



void
formal(p,b,off,type,usage)
	proc_p    p;
	bblock_p  b;
	offset    off;
	int       type,
		  usage;
{
	/* Analyze a reference to a parameter of p
	 * (occurring within basic block b).
	 * The parameter has offset off. If this
	 * offset is less than 0, it is not a
	 * parameter, but a local.
	 * The type can be SINGLE (1 word), DOUBLE
	 * (2 words), POINTER or UNKNOWN.
	 */

	formal_p f;

	if (!IS_FORMAL(off) || !SUITABLE(p) || !INLINE_PARS(p)) return;
	/* We are not interested in formal parameters of
	 * proccedures that will never be expanded in line,
	 * or whose parameters will not be expanded in line.
	 */
	f = find_formal(p,type,off);
	/* Find the formal; if not found, create one;
	 * if inconsistent with previous formals (e.g.
	 * overlapping formals) then return 0;
	 * also fills in its type.
	 */
	if (f == (formal_p) 0) {
		no_inl_pars(p);
		/* parameters of p may not be expanded in line */
	} else {
		if (usage == CHANGE) {
			/* don't expand f in line */
			BADFORMAL(f);
		} else {
			inc_use(f,b); /* increment use count */
		}
	}
}
