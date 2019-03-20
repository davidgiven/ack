/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include        "ass00.h"
#include        "assex.h"



/*
** Make scans to do final assignment of instruction sizes & formats
** to those not already done. assign final values to labels
*/
void pass_4(void)
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
