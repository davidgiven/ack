#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "assert.h"
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

string myalloc();
unsigned codegen();

match(tp,tep,optexp) register token_p tp; register set_p tep; {
	register bitno;
	token_p ct;
	result_t result;

	if (tp->t_token == -1) {        /* register frame */
		bitno = tp->t_att[0].ar;
		if (tep->set_val[bitno>>4]&(1<<(bitno&017)))
			if (tep->set_val[0]&1 || getrefcount(bitno, FALSE)<=1)
				goto oklabel;
		return(0);
	} else {                /* token frame */
		bitno = tp->t_token+NREGS;
		if ((tep->set_val[bitno>>4]&(1<<(bitno&017)))==0)
			return(0);
	}
    oklabel:
	if (optexp==0)
		return(1);
	ct=curtoken;
	curtoken=tp;
	compute(&enodes[optexp], &result);
	curtoken=ct;
	return(result.e_v.e_con);
}

instance(instno,token) register token_p token; {
	register inst_p inp;
	int i;
	register token_p tp;
#if MAXMEMBERS != 0
	struct reginfo *rp;
#endif
	int regno;
	result_t result;

	if (instno==0) {
		token->t_token = 0;
		for (i=TOKENSIZE-1;i>=0;i--)
			token->t_att[i].aw=0;
		return;
	}
	inp= &tokeninstances[instno];
	switch(inp->in_which) {
	default:
		assert(FALSE);
	case IN_COPY:
		if (inp->in_info[0] == 0)
			if (curtoken) tp = curtoken;
			else tp = &fakestack[stackheight-1];
		else	tp= &fakestack[stackheight-inp->in_info[0]];
		if (inp->in_info[1]==0) {
			*token = *tp;
		} else {
			token->t_token= -1;
#if MAXMEMBERS!=0
			assert(tp->t_token == -1);
			rp = &machregs[tp->t_att[0].ar];
			token->t_att[0].ar=rp->r_members[inp->in_info[1]-1];
#else
			assert(FALSE);
#endif
		}
		return;
	case IN_MEMB:
		if (inp->in_info[0] == 0)
			if (curtoken) tp = curtoken;
			else tp = &fakestack[stackheight-1];
		else	tp= &fakestack[stackheight-inp->in_info[0]];
		assert(inp->in_info[1]!=0);
		assert(tp->t_token>0);
		token->t_token= -1;
		assert(tokens[tp->t_token].t_type[inp->in_info[1]-1] == EV_REG);
		token->t_att[0].ar=tp->t_att[inp->in_info[1]-1].ar;
		return;
	case IN_RIDENT:
		token->t_token= -1;
		token->t_att[0].ar= inp->in_info[0];
		return;
	case IN_ALLOC:
		token->t_token= -1;
		regno=allreg[inp->in_info[0]];
#if MAXMEMBERS!=0
		if (inp->in_info[1])
			regno=machregs[regno].r_members[inp->in_info[1]-1];
#endif
		token->t_att[0].ar = regno;
		return;
#ifdef REGVARS
	case IN_S_DESCR:
	case IN_D_DESCR:
		compute(&enodes[inp->in_info[1]], &result);
		assert(result.e_typ==EV_INT);
		if ((regno=isregvar(result.e_v.e_con)) > 0) {
			token->t_token = -1;
			token->t_att[0].ar = regno;
			for (i=TOKENSIZE-1;i>0;i--)
				token->t_att[i].aw = 0;
			return;
		}
		/* fall through */
#endif
	case IN_DESCR:
		token->t_token=inp->in_info[0];
		for (i=0;i<TOKENSIZE;i++)
			if (inp->in_info[i+1]==0) {
				assert(tokens[token->t_token].t_type[i]==0);
				token->t_att[i].aw=0;
			} else {
				compute(&enodes[inp->in_info[i+1]], &result);
				assert(tokens[token->t_token].t_type[i]==result.e_typ);
				if (result.e_typ==EV_INT)
					token->t_att[i].aw=result.e_v.e_con;
				else if (result.e_typ==EV_ADDR)
					token->t_att[i].aa= result.e_v.e_addr;
				else
					token->t_att[i].ar=result.e_v.e_reg;
			}
		return;
	}
}

cinstance(instno,token,tp,regno) register token_p token,tp; {
	register inst_p inp;
	int i;
#if MAXMEMBERS != 0
	struct reginfo *rp;
#endif
	result_t result;
	int sh; /* saved stackheight */

	assert(instno!=0);
	inp= &tokeninstances[instno];
	switch(inp->in_which) {
	default:
		assert(FALSE);
	case IN_COPY:
		assert(inp->in_info[0] <= 1);
		if (inp->in_info[1]==0) {
			*token = *tp;
		} else {
			token->t_token= -1;
#if MAXMEMBERS!=0
			assert(tp->t_token == -1);
			rp = &machregs[tp->t_att[0].ar];
			token->t_att[0].ar=rp->r_members[inp->in_info[1]-1];
#else
			assert(FALSE);
#endif
		}
		return;
	case IN_MEMB:
		assert(inp->in_info[0] <= 1);
		token->t_token= -1;
		assert(tp->t_token>0);
		assert(tokens[tp->t_token].t_type[inp->in_info[1]-1] == EV_REG);
		token->t_att[0].ar=tp->t_att[inp->in_info[1]-1].ar;
		return;
	case IN_RIDENT:
		token->t_token= -1;
		token->t_att[0].ar= inp->in_info[0];
		return;
	case IN_ALLOC:
		token->t_token= -1;
		assert(inp->in_info[0]==0);
#if MAXMEMBERS!=0
		if (inp->in_info[1])
			regno=machregs[regno].r_members[inp->in_info[1]-1];
#endif
		token->t_att[0].ar = regno;
		return;
#ifdef REGVARS
	case IN_S_DESCR:
	case IN_D_DESCR:
		{	token_p ct = curtoken;

			curtoken = tp;
			compute(&enodes[inp->in_info[1]], &result);
			curtoken = ct;
			assert(result.e_typ==EV_INT);
			if ((regno=isregvar(result.e_v.e_con)) > 0) {
				token->t_token = -1;
				token->t_att[0].ar = regno;
				for (i=TOKENSIZE-1;i>0;i--)
					token->t_att[i].aw = 0;
				return;
			}
		}
		/* fall through */
#endif
	case IN_DESCR:
		sh = stackheight;
		stackheight = tp - fakestack + 1;
		token->t_token=inp->in_info[0];
		for (i=0;i<TOKENSIZE;i++)
			if (inp->in_info[i+1]==0) {
				assert(tokens[token->t_token].t_type[i]==0);
				token->t_att[i].aw=0;
			} else {
				token_p ct = curtoken;

				curtoken = tp;
				compute(&enodes[inp->in_info[i+1]], &result);
				curtoken = ct;
				assert(tokens[token->t_token].t_type[i]==result.e_typ);
				if (result.e_typ==EV_INT)
					token->t_att[i].aw=result.e_v.e_con;
				else if (result.e_typ==EV_ADDR)
					token->t_att[i].aa= result.e_v.e_addr;
				else
					token->t_att[i].ar=result.e_v.e_reg;
			}
		stackheight = sh;
		return;
	}
}

eqtoken(tp1,tp2) token_p tp1,tp2; {
	register i;
	register tkdef_p tdp;

	if (tp1->t_token!=tp2->t_token)
		return(0);
	if (tp1->t_token==0)
		return(1);
	if (tp1->t_token==-1) {
		if (tp1->t_att[0].ar!=tp2->t_att[0].ar)
			return(0);
		return(1);
	}
	tdp = &tokens[tp1->t_token];
	for (i=0;i<TOKENSIZE;i++)
		switch(tdp->t_type[i]) {
		default:
			return(1);
		case EV_INT:
			if (tp1->t_att[i].aw != tp2->t_att[i].aw)
				return(0);
			break;
		case EV_REG:
			if (tp1->t_att[i].ar != tp2->t_att[i].ar)
				return(0);
			break;
		case EV_ADDR:
			if (strcmp(tp1->t_att[i].aa.ea_str, tp2->t_att[i].aa.ea_str))
				return(0);
			if (tp1->t_att[i].aa.ea_off!=tp2->t_att[i].aa.ea_off)
				return(0);
			break;
		}
	return(1);
}

distance(cindex) {
	register char *bp;
	register i;
	register token_p tp;
	int tokexp,tpl;
	int expsize,toksize,exact;
	int xsekt=0;
	int fromstackneeded=0;

	bp = &coderules[cindex];
#ifndef NDEBUG
	if (*bp==DO_DLINE) {
		++bp;
		getint(i,bp);
	}
#endif
	switch( (*bp)&037 ) {
	default:
		return(stackheight==0 ? 0 : 100);
	case DO_MATCH:
		break;
	case DO_XXMATCH:
		xsekt++;
	case DO_XMATCH:
		xsekt++;
		break;
	}
	tpl= ((*bp++)>>5)&07;
	if (stackheight < tpl) {
		if (xsekt)
			return(MAXINT);
		fromstackneeded = tpl - stackheight;
		tpl = stackheight;
	} else
		if (stackheight != tpl && xsekt==2)
			return(MAXINT);
	exact=0;
	tp= &fakestack[stackheight-1];
	for (i=0;i<tpl;i++,tp--) {
		getint(tokexp,bp);
		if (!match(tp, &machsets[tokexp], 0)) {
			if (xsekt)
				return(MAXINT);
			expsize = ssize(tokexp);
			toksize = tsize(tp);
			if (expsize>toksize)
				return(100);
			if (expsize<toksize)
				return(99-i);

			/* Now we have a match in size, but it is not exact.
			   Therefore, make sure that we can at least
			   create it from the stack!
			*/
			if (! from_stack(&machsets[tokexp])) {
				return MAXINT;
			}
		} else
			exact++;
	}
	for (;i<tpl+fromstackneeded;i++) {
		/*	Make sure that any pattern we pick can be
			made from the stack
		*/
		getint(tokexp,bp);
		if (! from_stack(&machsets[tokexp])) {
			return(MAXINT);
		}
	}
	if (exact==tpl && ! fromstackneeded) {
		if (xsekt)
			return(0);
		return(10-exact);
	}
	return(20-2*exact+fromstackneeded);
}

extern set_t unstackset;

int from_stack(s1)
	register set_p s1;
{
	register set_p s2 = &unstackset;
	register int i;
	for (i = 0; i < SETSIZE; i++) {
		if ((s1->set_val[i] & s2->set_val[i]) != 0) return 1;
	}
	return 0;
}

unsigned costcalc(cost) cost_t cost; {
	extern unsigned cc1,cc2,cc3,cc4;

	return(cost.ct_space*cc1/cc2 + cost.ct_time*cc3/cc4);
}

ssize(tokexpno) {

	return(machsets[tokexpno].set_size);
}

tsize(tp) register token_p tp; {

	if (tp->t_token==-1)
		return(machregs[tp->t_att[0].ar].r_size);
	return(tokens[tp->t_token].t_size);
}

#ifdef MAXSPLIT
instsize(tinstno,tp) token_p tp; {
	inst_p inp;
	struct reginfo *rp;

	inp = &tokeninstances[tinstno];
	switch(inp->in_which) {
	default:
		assert(FALSE);
	case IN_COPY:
		assert(inp->in_info[0]<=1);
#if MAXMEMBERS!=0
		if (inp->in_info[1]==0)
#endif
			return(tsize(tp));
#if MAXMEMBERS!=0
		else {
			assert(tp->t_token == -1);
			rp = &machregs[tp->t_att[0].ar];
			return(machregs[rp->r_members[inp->in_info[1]-1]].r_size);
		}
#endif
	case IN_RIDENT:
		return(machregs[inp->in_info[0]].r_size);
	case IN_ALLOC:
		assert(FALSE);  /* cannot occur in splitting coercion */
	case IN_DESCR:
	case IN_S_DESCR:
	case IN_D_DESCR:
		return(tokens[inp->in_info[0]].t_size);
	}
}
#endif MAXSPLIT

tref(tp,amount) register token_p tp; {
	register i;
	register byte *tdpb;

	if (tp->t_token==-1)
		chrefcount(tp->t_att[0].ar,amount,FALSE);
	else {
		tdpb= &tokens[tp->t_token].t_type[0];
		for(i=0;i<TOKENSIZE;i++)
			if (*tdpb++==EV_REG)
				chrefcount(tp->t_att[i].ar,amount,FALSE);
	}
}

#define MAXSAVE 10
/* A few routines to save the top of the current stack,
   restore it and check whether a certain register is present in the
   saved stack
*/
token_t aside[MAXSAVE] ;
int	aside_length = -1 ;

save_stack(tp) register token_p tp ; {
	int i ;
	token_p tmp = &fakestack[stackheight - 1];

	/*aside_length = &fakestack[stackheight-1] -tp;
	  This did not compile on Xenix V3.2: CODE GENERATION ERROR!
	*/
	aside_length = tmp - tp;
	assert(aside_length<=MAXSAVE);
#ifndef NDEBUG
	if (aside_length!=0 && Debug>1)
		fprintf(stderr,"Saving %d items from fakestack\n",aside_length);
#endif
	for (i=1;i<=aside_length;i++)
		aside[i-1] = tp[i];
	stackheight -= aside_length;
}

in_stack(reg) {
	register token_p tp ;
	register i ;
	register tkdef_p tdp ;

	for ( i=0, tp=aside ; i<aside_length ; i++, tp++ )
		if (tp->t_token==-1) {
			if(tp->t_att[0].ar==reg)
				goto gotone ;
		} else {
			tdp = &tokens[tp->t_token];
			for(i=0;i<TOKENSIZE;i++)
				if (tdp->t_type[i]==EV_REG &&
				    tp->t_att[i].ar==reg)
					goto gotone ;
		}
	return 0 ;
gotone:
#ifndef NDEBUG
	if ( Debug>2 )
		fprintf(stderr,"Register %d present on non-visible stack\n",
			reg ) ;
#endif
	return 1 ;
}

rest_stack() {
	register int i ;

	assert(aside_length!= -1); 
#ifndef NDEBUG
	if (aside_length!=0 && Debug>1)
		fprintf(stderr,"Restoring %d items to fakestack(%d)\n",
			aside_length,stackheight);
#endif
	for (i=0;i<aside_length;i++)
		fakestack[stackheight++] = aside[i];
	aside_length= -1 ;
}

#ifdef MAXSPLIT
split(tp,ip,ply,toplevel) token_p tp; register int *ip; {
	register c2_p cp;
	token_t savestack[MAXSAVE];
	int ok;
	register i;
	int diff;
	token_p stp;
	int tpl;

	for (cp=c2coercs;cp->c2_texpno>=0; cp++) {
		if (!match(tp,&machsets[cp->c2_texpno],0))
			continue;
		ok=1;
		for (i=0; ok && i<cp->c2_nsplit;i++) {
			if (ip[i]==0)
				goto found;
			if (instsize(cp->c2_repl[i],tp) != ssize(ip[i]))
				ok=0;
		}
		goto found;
	}
	return(0);
found:
	assert(stackheight+cp->c2_nsplit-1<MAXFSTACK);
	save_stack(tp);
	tpl = tokpatlen;
	tokpatlen = 1;
	codegen(&coderules[cp->c2_codep],ply,toplevel,MAXINT,0);
	tokpatlen = tpl;
	rest_stack();
	return(cp->c2_nsplit);
}
#endif MAXSPLIT

unsigned docoerc(tp,cp,ply,toplevel,forced) token_p tp; register c3_p cp; {
	unsigned cost;
	int tpl;        /* saved tokpatlen */

	save_stack(tp) ;
	tpl = tokpatlen;
	tokpatlen = 1;
	cost = codegen(&coderules[cp->c3_codep],ply,toplevel,MAXINT,forced);
	tokpatlen = tpl;
	rest_stack() ;
	nallreg = 0;
	return(cost);
}

unsigned stackupto(limit,ply,toplevel) token_p limit; {
	token_t savestack[MAXFSTACK];
	token_p stp;
	int i,diff;
	int tpl;        /* saved tokpatlen */
	int nareg;	/* saved nareg */
	int areg[MAXALLREG];
	register c1_p cp;
	register token_p tp;
	unsigned totalcost=0;
	struct reginfo *rp,**rpp;

	for (tp=fakestack;tp<=limit;limit--) {
		for (cp=c1coercs;cp->c1_texpno>=0; cp++) {
			if (match(tp,&machsets[cp->c1_texpno],cp->c1_expr)) {
				if (cp->c1_prop>=0) {
					for (rpp=reglist[cp->c1_prop];
					       (rp = *rpp)!=0 &&
					       getrefcount((int)(rp-machregs), TRUE)!=0;
						  rpp++)
						;
					if (rp==0)
						continue;
						/* look for other possibility */
				}
				stp = &fakestack[stackheight-1];
				diff = stp -tp;
				assert(diff<=MAXFSTACK);
				for (i=1;i<=diff;i++)
					savestack[i-1] = tp[i];
				stackheight -= diff;
				tpl = tokpatlen;
				tokpatlen = 1;
				nareg = nallreg;
				for (i=0;i<nareg;i++)
					areg[i] = allreg[i];
				if (cp->c1_prop>=0) {
					nallreg=1; allreg[0] = rp-machregs;
					chrefcount(allreg[0],1,FALSE);
				} else 
					nallreg=0;
				totalcost+= codegen(&coderules[cp->c1_codep],ply,toplevel,MAXINT,0);
				tokpatlen = tpl;
				for (i=0;i<diff;i++) {
					fakestack[stackheight] = savestack[i];
					stackheight++;
					/* not cobmined in one statement;
					   this poor Xenix C compiler sometimes
					   gets failed assertions when you do
					   that!
					*/
				}
				nallreg=nareg;
				for (i=0;i<nareg;i++)
					allreg[i] = areg[i];
				goto contin;
			}
		}
		assert(FALSE);
	contin: ;
	}
	return(totalcost);
}

c3_p findcoerc(tp,tep) token_p tp; set_p tep; {
	register c3_p cp;
	token_t rtoken;
	register i;
	register struct reginfo **rpp;

	for (cp=c3coercs;cp->c3_texpno>=0; cp++) {
		if (tp!=(token_p) 0) {
			if (cp->c3_texpno==0)
				continue;
			if (!match(tp,&machsets[cp->c3_texpno],cp->c3_expr))
				continue;
		} else {
			if (cp->c3_texpno!=0)
				continue;
		}
		if (cp->c3_prop<0) {   /* no reg needed */
			cinstance(cp->c3_repl,&rtoken,tp,0);
			if (match(&rtoken,tep,0))
				return(cp);
		} else {
			curreglist = (rl_p) myalloc(sizeof (rl_t));
			curreglist->rl_n = 0;
			for (rpp=reglist[cp->c3_prop];*rpp;rpp++) {
				i = *rpp - machregs;
				cinstance(cp->c3_repl,&rtoken,tp,i);
				if (match(&rtoken,tep,0))
					curreglist->rl_list[curreglist->rl_n++] = i;
			}
			if (curreglist->rl_n != 0)
				return(cp);
			myfree((string)curreglist);
		}
	}
	return(0);      /* nothing found */
}

itokcost() {
	register tkdef_p tdp;

	for(tdp=tokens+1;tdp->t_size!=0;tdp++)
		tdp->t_cost.ct_space = costcalc(tdp->t_cost);
}

/*VARARGS1*/
error(s,a1,a2,a3,a4,a5,a6,a7,a8) char *s; {

	fprintf(stderr,"Error: ");
	fprintf(stderr,s,a1,a2,a3,a4,a5,a6,a7,a8);
	fprintf(stderr,"\n");
#ifdef TABLEDEBUG
	ruletrace();
#endif
	out_finish();
	exit(-1);
}

/*VARARGS1*/
fatal(s,a1,a2,a3,a4,a5,a6,a7,a8) char *s; {

	fprintf(stderr,"Fatal: ");
	fprintf(stderr,s,a1,a2,a3,a4,a5,a6,a7,a8);
	fprintf(stderr,"\n");
#ifdef TABLEDEBUG
	ruletrace();
#endif
	out_finish();
	abort();
	exit(-1);
}

#ifdef TABLEDEBUG

ruletrace() {
	register i;
	extern int tablelines[MAXTDBUG];
	extern int ntableline;
	extern char *tablename;

	fprintf(stderr,"Last code rules used\n");
	i=ntableline-1;
	while(i!=ntableline) {
		if (i<0)
			i += MAXTDBUG;
		if (tablelines[i]!=0)
			fprintf(stderr,"\%d: \"%s\", line %d\n",i,tablename,tablelines[i]);
		i--;
	}
}
#endif

#ifndef NDEBUG
badassertion(asstr,file,line) char *asstr, *file; {

	fatal("\"%s\", line %d:Assertion \"%s\" failed",file,line,asstr);
}
#endif
