/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "assert.h"
#include "param.h"
#include "set.h"
#include "property.h"
#include "reg.h"
#include "token.h"
#include "varinfo.h"
#include "iocc.h"
#include <cgg_cg.h>
#include "pseudo.h"
#include "extern.h"

extern set_t l_sets[];

int nmoves;
move_t l_moves[MAXMOVES];
short posmoves[MAXREGS+MAXTOKENS][SETSIZE];

void
n_move(s1,e1,s2,e2,vi) struct varinfo *vi; {
	register move_p mp;
	register i,j;

	NEXT(nmoves,MAXMOVES,"Moves");
	mp = &l_moves[nmoves-1];
	mp->m_set1 = s1;
	mp->m_expr1 = e1;
	mp->m_set2 = s2;
	mp->m_expr2 = e2;
	mp->m_cindex = codeindex;
	dopattern(0,VI_NULL,VI_NULL,vi,VI_NULL,VI_NULL);
	if (mp->m_expr1!=0 || mp->m_expr2!=0)
		return;
	for (i=0;i<MAXREGS+MAXTOKENS;i++)
	    if (BIT(l_sets[mp->m_set1].set_val,i))
	    	for(j=0;j<SETSIZE;j++)
		    posmoves[i][j] |= l_sets[mp->m_set2].set_val[j];
}

existmove(from,sp) iocc_t from; short *sp; {
	register i;

	for (i=0;i<MAXREGS+MAXTOKENS;i++)
	    if(BIT(from.in_set,i))
		if (!subset(sp,posmoves[i],SETSIZE))
			return(0);
	return(1);
}

existalmove(from,prpno) iocc_t from; {
	short s[SETSIZE];
	register i;
	
	for (i=0;i<SETSIZE;i++)
		s[i] = i<SZOFSET(MAXREGS) ? l_props[prpno].pr_regset[i] : 0;
	return(existmove(from,s));
}

struct varinfo *gen_move(from,to) iocc_t from,to; {
	register struct varinfo *vp;

	if (existmove(from,to.in_set)==0) {
		error("No such move defined");
		return(VI_NULL);
	}
	NEW(vp,struct varinfo);
	vp->vi_int[0] = INSMOVE;
	vp->vi_int[1] = from.in_index;
	vp->vi_int[2] = to.in_index;
	return(vp);
}

int ntests;
test_t l_tests[MAXTESTS];
short postests[SETSIZE];

void
n_test(s,e,vi) struct varinfo *vi; {
	register test_p tp;
	register i;

	NEXT(ntests,MAXTESTS,"Tests");
	tp = &l_tests[ntests-1];
	tp->t_set = s;
	tp->t_expr = e;	
	tp->t_cindex = codeindex;
	dopattern(0,VI_NULL,VI_NULL,vi,VI_NULL,VI_NULL);
	if (tp->t_expr!=0)
		return;
	for(i=0;i<SETSIZE;i++)
		postests[i] |= l_sets[tp->t_set].set_val[i];
}

struct varinfo *gen_test(from) iocc_t from; {
	register struct varinfo *vp;

	if (!subset(from.in_set,postests,SETSIZE)) {
		error("No such test");
		return(0);
	}
	NEW(vp,struct varinfo);
	vp->vi_int[0] = INSTEST;
	vp->vi_int[1] = from.in_index;
	return(vp);
}

struct varinfo *gen_label(arg) int arg; {
	register struct varinfo *vp;

	NEW(vp,struct varinfo);
	vp->vi_int[0] = INSLABDEF;
	vp->vi_int[1] = arg;
	return(vp);
}

struct varinfo *gen_preturn() {
	register struct varinfo *vp;

	NEW(vp,struct varinfo);
	vp->vi_int[0] = INSPRETURN;
	return(vp);
}

struct varinfo *gen_tlab(n) {
	register struct varinfo *vp;

	assert(n>=0 && n<=9);
	NEW(vp,struct varinfo);
	vp->vi_int[0] = INSTLAB;
	vp->vi_int[1] = n;
	return(vp);
}

int nstacks;
c1_t l_stacks[MAXSTACKS];
set_t ustackset,cstackset;

n_stack(s,e,p,vi) struct varinfo *vi; {
	register c1_p c1p;
	register short *sp;
	register i;

	NEXT(nstacks,MAXSTACKS,"Stacks");
	c1p= & l_stacks[nstacks-1];
	c1p->c1_texpno = s;
	c1p->c1_expr = e;
	c1p->c1_prop = p;
	c1p->c1_codep = codeindex;
	dopattern(0,VI_NULL,VI_NULL,vi,VI_NULL,VI_NULL);

	if (e==0 && p== -1)
		sp = ustackset.set_val;
	else
		sp = cstackset.set_val;
	for(i=0;i<SETSIZE;i++)
		sp[i] |= l_sets[s].set_val[i];
}

void
checkstacking(sp) register short *sp; {
	register i;
	register short *chkset;
	char *warn;

	if (subset(sp,ustackset.set_val,SETSIZE))
		return;
	chkset = ustackset.set_val; warn = "";
	for (i=1;i<nregs;i++)
		if (BIT(sp,i) && !BIT(chkset,i))
			error("No %sstacking rule for register %s",warn,
				l_regs[i].ri_name);
	for(;i<nregs+MAXTOKENS;i++)
		if (BIT(sp,i) && !BIT(chkset,i))
			error("No %sstacking rule for token %s",warn,
				l_tokens[i-nregs]->tk_name);
}

int ncoercs;
c3_t l_coercs[MAXCOERCS];
set_t unstackset;

/*VARARGS5*/

void
n_coerc(ti,be,al,ge,rp,in) struct varinfo *al,*ge,*rp; iocc_t in; {
	register c3_p c3p;
	register i;
	register struct varinfo *vi;

	if (ti!=0) {
		for(i=0,vi=rp;vi!=0;vi=vi->vi_next,i++)
			;
		if (i>1) {
			n_split(ti,be,al,ge,rp,i);
			return;
		} else {
			if (i==0) {
				error("Coercion should have a result!");
				return;
			}
		}
	} else {
		NEW(rp,struct varinfo);
		rp->vi_next = 0;
		rp->vi_int[0] = in.in_index;
	}
	if (nallreg>1)
		error("More than 1 register may not be allocated");
	NEXT(ncoercs,MAXCOERCS,"Coercions");
	c3p = & l_coercs[ncoercs-1];
	c3p->c3_texpno = ti;
	c3p->c3_expr = be;
	c3p->c3_prop = nallreg==0 ? -1 : allreg[0];
	c3p->c3_repl = rp->vi_int[0];
	c3p->c3_codep = codeindex;
	dopattern(ti==0,VI_NULL,al,ge,rp,VI_NULL);
	if (ti==0)
		for(i=0;i<SETSIZE;i++)
			unstackset.set_val[i] |= in.in_set[i];
	freevi(rp);
}

checkunstacking(setno) {
	register short *sp;
	register i;
	short hallset[SETSIZE];
	
	sp = l_sets[setno].set_val;
	for (i=0;i<SETSIZE;i++)
		hallset[i]=sp[i]&unstackset.set_val[i];
	nexthall(hallset);
}

int nsplit,maxsplit;
c2_t l_split[MAXSPLCOERC];

n_split(ti,be,al,ge,rp,n) struct varinfo *al,*ge,*rp; {
	register c2_p c2p;
	register i;
	register struct varinfo *vi;

	NEXT(nsplit,MAXSPLCOERC,"Splitting coercions");
	c2p = &l_split[nsplit-1];
	if (n>MAXSPLIT) {
		error("Maximum split factor is %d",MAXSPLIT);
		n = MAXSPLIT;
	}
	if (n>maxsplit) maxsplit=n;
	c2p->c2_texpno = ti;
	c2p->c2_expr = be;
	if (nallreg)
		error("No register uses allowed in splitting coercion");
	c2p->c2_nsplit = n;
	for (i=0,vi=rp; i<n; i++,vi=vi->vi_next)
		c2p->c2_repl[i] = vi->vi_int[0];
	c2p->c2_codep = codeindex;
	dopattern(0,VI_NULL,al,ge,rp,VI_NULL);
}
