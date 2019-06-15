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
#include "state.h"
#include "extern.h"
#include "subr.h"
#include "salloc.h"



extern int nstab;	/* salloc.c */

#ifndef STONSTACK
extern string myalloc();

state_p stlist=0;
#endif


static void bmove(register short *from, register short *to, register int nbytes)
{
	if (nbytes<=0)
		return;
	assert(sizeof(short)==2 && (nbytes&1)==0);
	nbytes>>=1;
	do
		*to++ = *from++;
	while (--nbytes);
}

#ifdef STONSTACK
state_p savestatus(register state_p sp)
{
#else
state_p state_p savestatus(void)
{
	register state_p sp;

	if ((sp=stlist)==0)
		sp = (state_p) myalloc( sizeof( *sp ) );
	else
		stlist=sp->st_next;
#endif
	sp->st_sh = stackheight;
	bmove((short *)fakestack,(short *)sp->st_fs,stackheight*sizeof(token_t));
	sp->st_na = nallreg;
	bmove((short *)allreg,(short *)sp->st_ar,nallreg*sizeof(int));
	sp->st_ct = curtoken;
	bmove((short *)dollar,(short *)sp->st_do,LONGESTPATTERN*sizeof(result_t));
	bmove((short *)machregs,(short *)sp->st_mr,NREGS*sizeof(struct reginfo));
	sp->st_ne = nemlines;
	bmove((short *)emlines,(short *)sp->st_el,nemlines*sizeof(struct emline));
	sp->st_em = emp;
	sp->st_se = saveemp;
	sp->st_tl = tokpatlen;
	sp->st_ns = nstab;
#ifndef STONSTACK
	return(sp);
#endif
}

void restorestatus(register state_p sp)
{
	stackheight = sp->st_sh;
	bmove((short *)sp->st_fs,(short *)fakestack,stackheight*sizeof(token_t));
	nallreg = sp->st_na;
	bmove((short *)sp->st_ar,(short *)allreg,nallreg*sizeof(int));
	curtoken = sp->st_ct;
	bmove((short *)sp->st_do,(short *)dollar,LONGESTPATTERN*sizeof(result_t));
	bmove((short *)sp->st_mr,(short *)machregs,NREGS*sizeof(struct reginfo));
	nemlines = sp->st_ne;
	bmove((short *)sp->st_el,(short *)emlines,nemlines*sizeof(struct emline));
	emp = sp->st_em;
	saveemp = sp->st_se;
	tokpatlen = sp->st_tl;
	popstr(sp->st_ns);
}

#ifndef STONSTACK
void freestatus(state_p sp)
{
	sp->st_next = stlist;
	stlist = sp;
}
#endif


