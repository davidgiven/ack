/* $Id$ */

/*	The Return Status Block contains, in push order:
	FIL, LIN, LB, PC, PI, rsbcode
*/

#include	"logging.h"
#include	"global.h"
#include	"mem.h"
#include	"rsb.h"
#include	"proctab.h"
#include	"linfil.h"
#include	"shadow.h"
#include	"warn.h"

/* offsets to be added to a local base */
int rsb_rsbcode;
int rsb_PI;
int rsb_PC;
int rsb_LB;
int rsb_LIN;
int rsb_FIL;
int rsbsize;

init_rsb()
{
	rsb_rsbcode = 0;
	rsb_PI = wsize;
	rsb_PC = rsb_PI + psize;
	rsb_LB = rsb_PC + psize;
	rsb_LIN = rsb_LB + psize;
	rsb_FIL = rsb_LIN + LINSIZE;
	rsbsize = rsb_FIL + psize;
}

pushrsb(rsbcode)
	int rsbcode;
{
	/* fill Return Status Block */
	incSP((size)rsbsize);

	st_stdp(SP + rsb_FIL, getFIL());
	st_prot(SP + rsb_FIL, psize);

	st_stn(SP + rsb_LIN, (long)getLIN(), LINSIZE);
	st_prot(SP + rsb_LIN, LINSIZE);

	st_stdp(SP + rsb_LB, LB);
	st_prot(SP + rsb_LB, psize);

	st_stip(SP + rsb_PC, PC);
	st_prot(SP + rsb_PC, psize);

	st_stn(SP + rsb_PI, PI, psize);
	st_prot(SP + rsb_PI, psize);

	st_stw(SP + rsb_rsbcode, (long)rsbcode);
	st_prot(SP + rsb_rsbcode, wsize);

	newLB(SP);
}

/*ARGSUSED*/
int poprsb(rtt)
	int rtt;			/* set to 1 if working for RTT */
{
	/* pops the RSB and returns the rsbcode, for further testing */
	register int rsbcode;

#ifdef	LOGGING
	{
		/* check SP */
		register ptr properSP = LB - proctab[PI].pr_nloc;

		if (SP < properSP)
			warning(rtt ? WRTTSTL : WRETSTL);
		if (SP > properSP)
			warning(rtt ? WRTTSTS : WRETSTS);
	}
#endif	/* LOGGING */

	/* discard stack up to RSB */
	newSP(LB);

	/* get RSB code and test it for applicability */
	rsbcode = st_lduw(SP + rsb_rsbcode);
	if ((rsbcode & RSBMASK) != RSBCODE)	/* no RSB at all */
		return rsbcode;

	if (rsbcode != RSB_STP) {
		/*	Restore registers PI, PC, LB, LIN and FIL
			from Return Status Block
		*/
		PI = st_lds(SP + rsb_PI, psize);
		newPC(st_ldip(SP + rsb_PC));
		newLB(st_lddp(SP + rsb_LB));
		putLIN((long) st_ldu(SP + rsb_LIN, LINSIZE));
		putFIL(st_lddp(SP + rsb_FIL));

		/* remove RSB */
		st_dec(rsbsize);

		pop_frames();
	}

	return rsbcode;
}

