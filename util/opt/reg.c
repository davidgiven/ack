#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "assert.h"
#include "param.h"
#include "types.h"
#include "line.h"
#include "tes.h"
#include "proinf.h"
#include "alloc.h"
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mes.h>
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

regvar(ap) register arg_p ap; {
	register reg_p rp;
	register i;

	rp = newreg();
	i=0;
	while (ap!=(arg_p)0 && ap->a_typ==ARGOFF && i<4) {
		rp->r_par[i++]=ap->a_a.a_offset;
		ap=ap->a_next;
	}
	/*
	 * Omit incomplete messages
	 */
	switch(i) {
	default:assert(FALSE);
	case 0:
	case 1:
	case 2:	oldreg(rp); return;
	case 3: rp->r_par[3]= (offset) 0; break;
	case 4: break;
	}
	rp->r_next = curpro.freg;
	curpro.freg = rp;
}

inreg(off) offset off; {
	register reg_p rp;

	for (rp=curpro.freg; rp != (reg_p) 0; rp=rp->r_next)
		if( rp->r_par[0] == off)
			return(TRUE);
	return(FALSE);
}

outregs() {
	register reg_p rp,tp;
	register i;

	for(rp=curpro.freg; rp != (reg_p) 0; rp = tp) {
		tp = rp->r_next;
		outinst(ps_mes);
		outoff((offset)ms_reg);
		for(i=0;i<4;i++)
			outoff(rp->r_par[i]);
		outinst(sp_cend);
		oldreg(rp);
	}
	/* List of register messages is followed by an empty ms_reg
	 * unless an ms_gto was in this procedure, then the ms_gto
	 * will be output. Kludgy.
	 */
	outinst(ps_mes);
	outoff((offset)(curpro.gtoproc? ms_gto : ms_reg));
	outinst(sp_cend);
	curpro.freg = (reg_p) 0;
}

/* outtes() handles the output of the top elt. messages */
outtes() {
	register lblst_p lp = est_list;

	while(lp != NULL) {
		if ((lp->ll_size != 0) && !(lp->ll_num->n_flags & NUMCOND)) {
			outinst(ps_mes);
			outoff((offset)ms_tes);
			outoff((offset)lp->ll_num->n_number);
			outoff((offset)lp->ll_size);
			outoff((offset)lp->ll_fallthrough);
			outinst(sp_cend);
		}
		lp = lp->ll_next;
	}
}

incregusage(off) offset off; {
	register reg_p rp;

#ifndef GLOBAL_OPT
	/* If we're optimizing the output of the global optimizer
	 * we must not change the count fields of the register messages.
	 */
	for(rp=curpro.freg; rp != (reg_p) 0; rp=rp->r_next)
		if (rp->r_par[0]==off) {
			rp->r_par[3]++;
			return;
		}
#endif
}
