#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "param.h"
#include "tables.h"

#ifdef REGVARS

#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "regvar.h"
#include <em_reg.h>
#include "result.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
extern string myalloc();
struct regvar *rvlist;

struct regvar *
linkreg(long of, int sz, int tp, int sc) {
	register struct regvar *rvlp;

	rvlp= (struct regvar *) myalloc(sizeof *rvlp);
	rvlp->rv_next = rvlist;
	rvlist=rvlp;
	rvlp->rv_off	= of;
	rvlp->rv_size	= sz;
	rvlp->rv_type	= tp;
	rvlp->rv_score	= sc;
	rvlp->rv_reg	= 0;	/* no register assigned yet */
	return(rvlp);
}

void
tryreg(struct regvar *rvlp, int typ) {
	int score;
	register i;
	register struct regassigned *ra;
	struct regvar *save;

	if (typ != reg_any && nregvar[typ]!=0) {
		struct reginfo *ri = &machregs[rvnumbers[typ][0]];
		int size, wrong;

		size = ri->r_size;
		wrong = (size!=rvlp->rv_size);
#ifdef REGLAP
		/* reg_float may have one subregister */
		if (wrong && ri->r_members[0]!=0) {
			size = machregs[ri->r_members[0]].r_size;
			wrong = (size!=rvlp->rv_size);
		}
#endif /* REGLAP */
		if (wrong)
			score = -1;
		else
			score = regscore(rvlp->rv_off,
					 rvlp->rv_size,
					 rvlp->rv_type,
					 rvlp->rv_score,
					 typ);	/* machine dependent */
		ra = regassigned[typ];
		if (score>ra[nregvar[typ]-1].ra_score) {
			save = ra[nregvar[typ]-1].ra_rv;
			for (i=nregvar[typ]-1;i>0 && ra[i-1].ra_score<score;i--)
				ra[i] = ra[i-1];
			ra[i].ra_rv = rvlp;
			ra[i].ra_score = score;
			if((rvlp=save)==0)
				return;
		}
	}
	if (nregvar[reg_any]==0)
		return;
	if (machregs[rvnumbers[reg_any][0]].r_size!=rvlp->rv_size)
		score = -1;
	else
		score = regscore(rvlp->rv_off,
				 rvlp->rv_size,
				 rvlp->rv_type,
				 rvlp->rv_score,
				 reg_any);	/* machine dependent */
	ra = regassigned[reg_any];
	if (score>ra[nregvar[reg_any]-1].ra_score) {
		for (i=nregvar[reg_any]-1;i>0 && ra[i-1].ra_score<score;i--)
			ra[i] = ra[i-1];
		ra[i].ra_rv = rvlp;
		ra[i].ra_score = score;
	}
}

void
fixregvars(int saveall) {
	register struct regvar *rv;
	register rvtyp,i;
	
	swtxt();
	i_regsave();	/* machine dependent initialization */
	for (rvtyp=reg_any;rvtyp<=reg_float;rvtyp++) {
	    for(i=0;i<nregvar[rvtyp];i++)
		if (saveall) {
			struct reginfo *rp;
			rp= &machregs[rvnumbers[rvtyp][i]];
			regsave(codestrings[rp->r_repr],
				    (long)-TEM_WSIZE,rp->r_size);
#ifdef REGLAP
			/* reg_float may have one subregister */
			if (rp->r_members[0]!=0) {
				rp= &machregs[rp->r_members[0]];
				regsave(codestrings[rp->r_repr],
					    (long)-TEM_WSIZE,rp->r_size);
			}
#endif
		} else if(regassigned[rvtyp][i].ra_score>0) {
			rv=regassigned[rvtyp][i].ra_rv;
			rv->rv_reg=rvnumbers[rvtyp][i];
			rv->rv_type = rvtyp;
#ifdef REGLAP
			/* reg_float may have alternate size */
			if (machregs[rv->rv_reg].r_size != rv->rv_size) {
				rv->rv_reg = machregs[rv->rv_reg].r_members[0];
				assert(rv->rv_reg != 0);
				assert(machregs[rv->rv_reg].r_size ==
					   rv->rv_size);
			}
#endif
			regsave(codestrings[machregs[rv->rv_reg].r_repr],
				    rv->rv_off,rv->rv_size);
		}
	}
	f_regsave();
}

int
isregvar(long off) {
	register struct regvar *rvlp;

	for(rvlp=rvlist;rvlp!=0;rvlp=rvlp->rv_next)
		if(rvlp->rv_off == off)
			return(rvlp->rv_reg);
	return(-1);
}

int
isregtyp(long off) {
	register struct regvar *rvlp;

	for(rvlp=rvlist;rvlp!=0;rvlp=rvlp->rv_next)
		if(rvlp->rv_off == off)
			return(rvlp->rv_reg ? rvlp->rv_type+1 : 0);
	return(-1);
}

void
unlinkregs(void) {
	register struct regvar *rvlp,*t;
	register struct regassigned *ra;
	int rvtyp,i;

	for(rvlp=rvlist;rvlp!=0;rvlp=t) {
		t=rvlp->rv_next;
		myfree((string)rvlp);
	}
	rvlist=0;
	for (rvtyp=reg_any;rvtyp<=reg_float;rvtyp++) {
	    for(i=0;i<nregvar[rvtyp];i++) {
		ra= &regassigned[rvtyp][i];
		ra->ra_rv = 0;
		ra->ra_score = 0;
	    }
	}
}

#endif /* REGVARS */

/* nothing after this */
