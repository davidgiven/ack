/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

#include        "ass00.h"
#include        "assex.h"

#ifndef NORCSID
static char rcs_id[] = "$Header$" ;
#endif

/*
** Make scans to do final assignment of instruction sizes & formats
** to those not already done. assign final values to labels
*/
pass_4()
{
	register line_t *lnp;
	register locl_t *lbp;
	int     min_l, max_l;
	int     instr;

	pass = 4;
	prog_size= 0 ;
	for (lnp = pstate.s_fline ; lnp ; lnp= lnp->l_next, line_num++) {
		instr = ctrunc(lnp->instr_num);
		if ( instr==sp_fpseu ) {
			line_num = lnp->ad.ad_ln.ln_first ;
			continue ;
		}
		if ( instr==sp_ilb1 ) {
			lbp = lnp->ad.ad_lp;
			lbp->l_min= prog_size; lbp->l_defined = YES;
			continue ;
		}

		if (lnp->opoff == NO_OFF)
		{
			determine_props(lnp, &min_l, &max_l);
			if (min_l != max_l)
				fatal("no size known");
		} else {
			min_l = oplength(*(lnp->opoff)) ;
		}
		prog_size += min_l ;
	}
}
