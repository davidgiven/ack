#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

void chrefcount(int regno, int amount, int tflag) {
	struct reginfo *rp;
#if MAXMEMBERS != 0
	int i, tmp;
#endif

	rp= &machregs[regno];
#if MAXMEMBERS!=0
	if (rp->r_members[0]==0) {
#endif
		rp->r_refcount += amount;
		if (tflag)
			rp->r_tcount += amount;
		assert(rp->r_refcount >= 0);
#if MAXMEMBERS!=0
	} else
		for (i=0;i<MAXMEMBERS;i++)
			if ((tmp = rp->r_members[i])!=0)
				chrefcount(tmp,amount,tflag);
#endif
}

int getrefcount(int regno, int tflag) {
	struct reginfo *rp;
#if MAXMEMBERS != 0
	int i,maxcount, tmp;
#endif

	rp= &machregs[regno];
#if MAXMEMBERS!=0
	if (rp->r_members[0]==0)
#endif
		return(rp->r_refcount - (tflag ? rp->r_tcount : 0));
#if MAXMEMBERS!=0
	else {
		maxcount=0;
		for (i=0;i<MAXMEMBERS;i++)
			if ((tmp=rp->r_members[i])!=0) {
				tmp = getrefcount(tmp, tflag);
				if (tmp > maxcount) maxcount = tmp;
			}
		return(maxcount);
	}
#endif
}

void erasereg(int regno) {
	struct reginfo *rp = &machregs[regno];
	int i;
	byte *tdpb;

#if MAXMEMBERS==0
	rp->r_contents.t_token = 0;
	for (i=TOKENSIZE-1;i>=0;i--)
		rp->r_contents.t_att[i].aw = 0;

	/* Now erase recursively all registers containing
	 * something using this one
	 */
	for (rp=machregs;rp<machregs+NREGS;rp++) {
		if (rp->r_contents.t_token == -1) {
			if (rp->r_contents.t_att[0].ar == regno) {
				/* erasereg(rp-machregs);
				   replaced by the following three
				   lines
				*/
				rp->r_contents.t_token = 0;
				for (i=TOKENSIZE-1;i>=0;i--)
					rp->r_contents.t_att[i].aw = 0;
			}
		} else if (rp->r_contents.t_token > 0) {
			tdpb= & (tokens[rp->r_contents.t_token].t_type[0]);
			for (i=0;i<TOKENSIZE;i++)
				if (*tdpb++ == EV_REG && 
				    rp->r_contents.t_att[i].ar == regno) {
					/* erasereg(rp-machregs);
					   replaced by the following three
					   lines
					*/
					rp->r_contents.t_token = 0;
					for (i=TOKENSIZE-1;i>=0;i--)
						rp->r_contents.t_att[i].aw = 0;
					break;
				}
		}
	}
#else
	extern short clashlist[];
	short *sp = &clashlist[rp->r_iclash];

	rp->r_contents.t_token = 0;
	while (*sp) {
		rp = &machregs[*sp];
		rp->r_contents.t_token = 0;
		for (i=TOKENSIZE-1;i>=0;i--)
			rp->r_contents.t_att[i].aw = 0;
		regno = *sp++;
		/* Now erase recursively all registers containing
		 * something using this one
		 */
		for (rp=machregs;rp<machregs+NREGS;rp++) {
			if (rp->r_contents.t_token == -1) {
				if (rp->r_contents.t_att[0].ar == regno) {
					/* erasereg(rp-machregs);
					   replaced by the following three
					   lines
					*/
					rp->r_contents.t_token = 0;
					for (i=TOKENSIZE-1;i>=0;i--)
						rp->r_contents.t_att[i].aw = 0;
				}
			} else if (rp->r_contents.t_token > 0) {
				tdpb= & (tokens[rp->r_contents.t_token].t_type[0]);
				for (i=0;i<TOKENSIZE;i++)
					if (*tdpb++ == EV_REG && 
					    rp->r_contents.t_att[i].ar == regno) {
						/* erasereg(rp-machregs);
						   replaced by the following three
						   lines
						*/
						rp->r_contents.t_token = 0;
						for (i=TOKENSIZE-1;i>=0;i--)
							rp->r_contents.t_att[i].aw = 0;
						break;
					}
			}
		}
	}
#endif
}

void cleanregs(void) {
	struct reginfo *rp;
	int i;

	for (rp=machregs;rp<machregs+NREGS;rp++) {
		rp->r_contents.t_token = 0;
		for (i=TOKENSIZE-1;i>=0;i--)
			rp->r_contents.t_att[i].aw = 0;
	}
}

#ifndef NDEBUG
static void inctcount(int regno) {
	struct reginfo *rp;
	int i;

	rp = &machregs[regno];
#if MAXMEMBERS!=0
	if (rp->r_members[0] == 0) {
#endif
		rp->r_tcount++;
#if MAXMEMBERS!=0
	} else  {
		for (i=0;i<MAXMEMBERS;i++)
			if (rp->r_members[i] != 0)
				inctcount(rp->r_members[i]);
	}
#endif
}

void chkregs(void) {
	struct reginfo *rp;
	token_p tp;
	byte *tdpb;
	int i;

	for (rp=machregs+1;rp<machregs+NREGS;rp++) {
		assert(rp->r_tcount==0);
	}
	for (tp=fakestack;tp<fakestack+stackheight;tp++) {
		if (tp->t_token == -1)
			inctcount(tp->t_att[0].ar);
		else {
			tdpb = &(tokens[tp->t_token].t_type[0]);
			for (i=0;i<TOKENSIZE;i++)
				if (*tdpb++==EV_REG)
					inctcount(tp->t_att[i].ar);
		}
	}
#ifdef REGVARS
#include <em_reg.h>
	for(i=reg_any;i<=reg_float;i++) {
		int j;
		for(j=0;j<nregvar[i];j++)
			inctcount(rvnumbers[i][j]);
	}
#endif /* REGVARS */
	for (rp=machregs+1;rp<machregs+NREGS;rp++) {
		assert(rp->r_refcount==rp->r_tcount);
		rp->r_tcount=0;
	}
}
#endif
