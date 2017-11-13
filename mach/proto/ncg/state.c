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
#include "state.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

extern int nstab;	/* salloc.c */

static void bmove(short *, short *, int);

void savestatus(state_p sp) {

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
}

void restorestatus(state_p sp) {

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

static void bmove(short *from, short *to, int nbytes) {

	if (nbytes<=0)
		return;
	assert(sizeof(short)==2 && (nbytes&1)==0);
	nbytes>>=1;
	do
		*to++ = *from++;
	while (--nbytes);
}
