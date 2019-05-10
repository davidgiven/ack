/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "subr.h"
#include "reg.h"

/* Froward local declarations */
static void awayreg(int);

void chrefcount(int regno, int amount, int tflag)
{
	register struct reginfo *rp;
	register int i;

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
			if (rp->r_members[i]!=0)
				chrefcount(rp->r_members[i],amount,tflag);
#endif
}

int getrefcount(int regno, int tflag)
{
	register struct reginfo *rp;
	register int i,maxcount;

	rp= &machregs[regno];
#if MAXMEMBERS!=0
	if (rp->r_members[0]==0)
#endif
		return(rp->r_refcount - (tflag ? rp->r_tcount : 0));
#if MAXMEMBERS!=0
	else {
		maxcount=0;
		for (i=0;i<MAXMEMBERS;i++)
			if (rp->r_members[i]!=0)
				maxcount=max(maxcount,getrefcount(rp->r_members[i], tflag));
		return(maxcount);
	}
#endif
}

void erasereg(int regno)
{
	register struct reginfo *rp;
	register int i;

	rp = &machregs[regno];
	rp->r_contents.t_token = 0;
	for (i=0;i<TOKENSIZE;i++)
		rp->r_contents.t_att[i].aw = 0;

#if MAXMEMBERS == 0
	awayreg(regno);

#else
	if (regno == 1)	 { /* condition codes */
	} else
	for (rp=machregs;rp<machregs+NREGS;rp++)
		if (rp->r_clash[regno>>4]&(1<<(regno&017))) {
			rp->r_contents.t_token = 0;
			for (i=0;i<TOKENSIZE;i++)
				rp->r_contents.t_att[i].aw = 0;
			awayreg((int)(rp-machregs));
		}
#endif
}

static void awayreg(int regno)
{
	register struct reginfo *rp;
	register tkdef_p tdp;
	register int i;

	/* Now erase recursively all registers containing
	 * something using this one
	 */
	for (rp=machregs;rp<machregs+NREGS;rp++) {
		if (rp->r_contents.t_token == -1) {
			if (rp->r_contents.t_att[0].ar == regno)
				erasereg((int)(rp-machregs));
		} else if (rp->r_contents.t_token > 0) {
			tdp= & tokens[rp->r_contents.t_token];
			for (i=0;i<TOKENSIZE;i++)
				if (tdp->t_type[i] == EV_REG && 
				    rp->r_contents.t_att[i].ar == regno) {
					erasereg((int)(rp-machregs));
					break;
				}
		}
	}
}

void cleanregs(void)
{
	register struct reginfo *rp;
	register int i;

	for (rp=machregs;rp<machregs+NREGS;rp++) {
		rp->r_contents.t_token = 0;
		for (i=0;i<TOKENSIZE;i++)
			rp->r_contents.t_att[i].aw = 0;
	}
}

#ifndef NDEBUG
void inctcount(int regno)
{
	register struct reginfo *rp;
	register int i;

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

void chkregs(void)
{
	register struct reginfo *rp;
	register token_p tp;
	register tkdef_p tdp;
	int i;

	for (rp=machregs;rp<machregs+NREGS;rp++) {
		assert(rp->r_tcount==0);
	}
	for (tp=fakestack;tp<fakestack+stackheight;tp++) {
		if (tp->t_token == -1)
			inctcount(tp->t_att[0].ar);
		else {
			tdp = &tokens[tp->t_token];
			for (i=0;i<TOKENSIZE;i++)
				if (tdp->t_type[i]==EV_REG)
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
	for (rp=machregs;rp<machregs+NREGS;rp++) {
		assert(rp->r_refcount==rp->r_tcount);
		rp->r_tcount=0;
	}
}
#endif
