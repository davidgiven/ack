#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "state.h"
#include "equiv.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#define SHORTCUT	/* Stop searching at distance 0 */

#if NREGS >= MAXRULE
#define MAXPOS NREGS
#else
#define MAXPOS MAXRULE
#endif

#define MAXPATTERN 5
#define MAXREPLLEN 5    /* Max length of EM-replacement, should come from boot */

byte startupcode[] = { DO_NEXTEM };

byte *nextem();
unsigned costcalc();
unsigned docoerc();
unsigned stackupto();
string tostring();

#ifdef NDEBUG
#define DEBUG(xxxxx)
#else
#include <stdio.h>
#define DEBUG(string) {if(Debug) fprintf(stderr,"%-*d%s\n",4*level,level,string);}
#endif

#define BROKE() {assert(origcp!=startupcode);DEBUG("BROKE");goto doreturn;}
#define CHKCOST() {if (totalcost>=costlimit) BROKE();}

unsigned codegen(codep,ply,toplevel,costlimit,forced) byte *codep; unsigned costlimit; {
#ifndef NDEBUG
	byte *origcp=codep;
	static int level=0;
#endif
	unsigned totalcost = 0;
	byte *bp;
	int n;
	unsigned mindistance,dist;
	register int i;
	int cindex;
	int npos,npos2,pos[MAXPOS],pos2[MAXPOS];
#ifdef STONSTACK
	state_t state;
#define SAVEST	savestatus(&state)
#define RESTST	restorestatus(&state)
#define FREEST	/* nothing */
#else
	state_p state;
#define SAVEST	state=savestatus()
#define RESTST	restorestatus(state)
#define FREEST	freestatus(state)
#endif
	unsigned mincost,t;
	int texpno,nodeno;
	token_p tp;
	tkdef_p tdp;
	int tinstno;
	register struct reginfo *rp;
	struct reginfo **rpp;
	token_t token,mtoken,token2;
	int propno;
	int exactmatch;
	int j;
	int decision;
	int stringno;
	result_t result;
	cost_t cost;
	int size,lsize,repllen;
	int tokexp[MAXPATTERN];
	int nregneeded;
	token_p regtp[MAXCREG];
	c3_p regcp[MAXCREG];
	rl_p regls[MAXCREG];
	c3_p findcoerc();
	int sret;
	token_t reptoken[MAXREPLLEN];
	int emrepllen,eminstr;
	int inscoerc=0;
	int stackpad;
	struct perm *tup,*ntup,*besttup,*tuples();

#ifndef NDEBUG
	level++;
	DEBUG("Entering codegen");
#endif
	for (;;) {
	switch( (*codep++)&037 ) {
    default:
	assert(FALSE);
	/* NOTREACHED */
    case DO_NEXTEM:
	DEBUG("NEXTEM");
	tokpatlen = 0;
	nallreg=0;
	if (toplevel) {
		garbage_collect();
		totalcost=0;
	} else {
		if (--ply <= 0)
			goto doreturn;
	}
	if (stackheight>MAXFSTACK-7)	
		totalcost += stackupto(&fakestack[6],ply,toplevel);
	bp = nextem(toplevel);
	if (bp == 0) {
		/*
		 * No pattern found, can be pseudo or error
		 * in table.
		 */
		if (toplevel) {
			codep--;
			DEBUG("pseudo");
			dopseudo();
		} else
			goto doreturn;
	} else {
#ifndef NDEBUG
		chkregs();
#endif
		if (! toplevel) {
			ply -= emp-saveemp+1;
			if (ply <= 0) ply = 1;
		}
		n = *bp++;
		assert(n>0 && n<=MAXRULE);
		if (n>1) {
			mindistance = MAXINT; npos=0;
			for(i=0;i<n;i++) {
				getint(cindex,bp);
				dist=distance(cindex);
#ifndef NDEBUG
if (Debug)
	fprintf(stderr,"distance of pos %d is %u\n",i,dist);
#endif
				if (dist<=mindistance) {
					if (dist<mindistance) {
#ifdef SHORTCUT
						if(dist==0)
							goto gotit;
#endif
						npos=0;
						mindistance = dist;
					}
					pos[npos++] = cindex;
				}
			}
			assert(mindistance<MAXINT);
			if (npos>1) {
				/*
				 * More than 1 tokenpattern is a candidate.
				 * Decision has to be made by lookahead.
				 */
				SAVEST;
				mincost = costlimit-totalcost+1;
				for(i=0;i<npos;i++) {
					t=codegen(&coderules[pos[i]],ply,FALSE,
						  costlimit<MAXINT?mincost:MAXINT,0);
#ifndef NDEBUG
if (Debug)
	fprintf(stderr,"mincost %u,cost %u,pos %d\n",mincost,t,i);
#endif
					if (t<mincost) {
						mincost = t;
						cindex = pos[i];
					}
					RESTST;
				}
				FREEST;
				if (totalcost+mincost>costlimit) {
					totalcost += mincost;
					BROKE();
				}
			} else {
				cindex = pos[0];
			}
		} else {
			getint(cindex,bp);
		}

	gotit:
		/*
		 * Now cindex contains the code-index of the best candidate
		 * so proceed to use it.
		 */
		codep = &coderules[cindex];
	}
	break;
    case DO_COERC:
	DEBUG("COERC");
	tokpatlen=1;
	inscoerc=1;
	break;
    case DO_XXMATCH:
	DEBUG("XXMATCH");
    case DO_XMATCH:
	DEBUG("XMATCH");
	tokpatlen=(codep[-1]>>5)&07;
	for (i=0;i<tokpatlen;i++)
		getint(tokexp[i],codep);
	tokexp[i]=0;
	break;	/* match already checked by distance() */
    case DO_MATCH:
	DEBUG("MATCH");
	tokpatlen=(codep[-1]>>5)&07;
	for(i=0;i<tokpatlen;i++)
		getint(tokexp[i],codep);
	tokexp[i] = 0;
	tp = &fakestack[stackheight-1];
	i=0;
	while (i<tokpatlen && tp>=fakestack) {
		size=tsize(tp);
		while (i<tokpatlen && (lsize=ssize(tokexp[i]))<=size) {
			size -= lsize;
			i++;
		}
		if (i<tokpatlen && size!=0) {
			totalcost += stackupto(tp,ply,toplevel);
			CHKCOST();
			break;
		}
		tp--;
	}
	tp = &fakestack[stackheight-1];
	i=0;
	while (i<tokpatlen && tp >= fakestack) {
		size = tsize(tp);
		lsize= ssize(tokexp[i]);
		if (size != lsize) {    /* find coercion */
#ifdef MAXSPLIT
			sret = split(tp,&tokexp[i],ply,toplevel);
			if (sret==0) {
#endif /* MAXSPLIT */
				totalcost += stackupto(tp,ply,toplevel);
				CHKCOST();
				break;
#ifdef MAXSPLIT
			}
			i += sret;
#endif /* MAXSPLIT */
		} else
			i += 1;
		tp--;
	}
    nextmatch:
	tp = &fakestack[stackheight-1];
	i=0; nregneeded = 0;
	while (i<tokpatlen && tp>=fakestack) {
		if (!match(tp,&machsets[tokexp[i]],0)) {
			register c3_p cp = findcoerc(tp, &machsets[tokexp[i]]);
			if (cp==0) {
				for (j=0;j<nregneeded;j++)
					regtp[j] -= (tp-fakestack+1);
				totalcost += stackupto(tp,ply,toplevel);
				CHKCOST();
				break;
			} else {
				if (cp->c3_prop==0) {
					totalcost+=docoerc(tp,cp,ply,toplevel,0);
					CHKCOST();
				} else {
					assert(nregneeded<MAXCREG);
					regtp[nregneeded] = tp;
					regcp[nregneeded] = cp;
					regls[nregneeded] = curreglist;
					nregneeded++;
				}
			}
		}
		i++; tp--;
	}
	if (tokpatlen>stackheight) {
		int k;
		stackpad = tokpatlen-stackheight;
		for (j=stackheight-1, k = j + stackpad;j>=0;j--, k--) {
			fakestack[k] = fakestack[j];
			/* fakestack[j+stackpad] = fakestack[j]; does not
			   compile under Xenix
			*/
		}
		for (j=0;j<stackpad;j++)
			fakestack[j].t_token=0;
		stackheight += stackpad;
		for (j=0;j<nregneeded;j++)
			regtp[j] += stackpad;
		tp = &fakestack[stackpad-1];
		while (i<tokpatlen && tp>=fakestack) {
			register c3_p cp = findcoerc((token_p) 0, &machsets[tokexp[i]]);
			if (cp==0) {
				assert(!toplevel);
				for (j=0;j<nregneeded;j++)
					myfree(regls[j]);
				totalcost=INFINITY;
				BROKE();
			}
			if (cp->c3_prop==0) {
				totalcost+=docoerc(tp,cp,ply,toplevel,0);
				CHKCOST();
			} else {
				assert(nregneeded<MAXCREG);
				regtp[nregneeded] = tp;
				regcp[nregneeded] = cp;
				regls[nregneeded] = curreglist;
				nregneeded++;
			}
			i++; tp--;
		}
	} else
		stackpad=0;
	assert(i==tokpatlen);
	if (nregneeded==0)
		break;
	SAVEST;
	mincost=costlimit-totalcost+1;
	tup = tuples(regls,nregneeded);
	besttup=0;
	for (; tup != 0; tup = ntup) {
#ifndef NDEBUG
if(Debug>1) { fprintf(stderr,"Next tuple %d,%d,%d,%d\n",
                        tup->p_rar[0],
                        tup->p_rar[1],
                        tup->p_rar[2],
                        tup->p_rar[3]);
                fprintf(stderr, "totalcost = %u, costlimit = %u, mincost = %u\n",
                        totalcost, costlimit, mincost);
        }
#endif
		ntup = tup->p_next;
		for (i=0,t=0;i<nregneeded && t<mincost; i++)
			t += docoerc(regtp[i],regcp[i],ply,FALSE,tup->p_rar[i]);
#ifndef NDEBUG   
if (Debug > 1) fprintf(stderr, "cost after coercions: %u\n", t); 
#endif
		if (t<mincost)
			t += codegen(codep,ply,FALSE,mincost<MAXINT?mincost-t:MAXINT,0);
		if (t<mincost) {
			mincost = t;
			besttup = tup;
		} else
			myfree(tup);
		RESTST;
	}
	FREEST;
	for (i=0;i<nregneeded;i++)
		myfree(regls[i]);
	if (totalcost+mincost>costlimit) {
		if (besttup)
			myfree(besttup);
		if (stackpad!=tokpatlen) {
			if (stackpad) {
				if (costlimit<MAXINT) {
					totalcost = costlimit+1;
					BROKE();
				}
				for (i=0;i<stackheight-stackpad;i++)
					fakestack[i] = fakestack[i+stackpad];
				stackheight -= stackpad;
				totalcost += stackupto(&fakestack[stackheight-1],ply,toplevel);
			} else
				totalcost += stackupto(fakestack,ply,toplevel);
			CHKCOST();
			goto nextmatch;
		}
		totalcost += mincost;
		BROKE();
	}
	for (i=0;i<nregneeded;i++)
		totalcost += docoerc(regtp[i],regcp[i],ply,toplevel,besttup->p_rar[i]);
	myfree(besttup);
	break;
    case DO_REMOVE:
	DEBUG("REMOVE");
	if (codep[-1]&32) {
		getint(texpno,codep);
		getint(nodeno,codep);
	} else {
		getint(texpno,codep);
		nodeno=0;
	}
	for (tp= &fakestack[stackheight-tokpatlen-1];tp>=&fakestack[0];tp--)
		if (match(tp,&machsets[texpno],nodeno)) {
			/* investigate possible coercion to register */
			totalcost += stackupto(tp,ply,toplevel);
			CHKCOST();
			break;
		}
	for (rp=machregs+2;rp<machregs+NREGS;rp++)
		if (match(&rp->r_contents,&machsets[texpno],nodeno))
			rp->r_contents.t_token=0;
	break;
    case DO_RREMOVE:	/* register remove */
	DEBUG("RREMOVE");
	getint(nodeno,codep);
	result=compute(&enodes[nodeno]);
	assert(result.e_typ==EV_REG);
	for (tp= &fakestack[stackheight-tokpatlen-1];tp>=&fakestack[0];tp--)
		if (tp->t_token==-1) {
			if(tp->t_att[0].ar==result.e_v.e_reg)
				goto gotone;
		} else {
			tdp = &tokens[tp->t_token];
			for(i=0;i<TOKENSIZE;i++)
				if (tdp->t_type[i]==EV_REG &&
				    tp->t_att[i].ar==result.e_v.e_reg)
					goto gotone;
		}
	break;
    gotone:
	/* investigate possible coercion to register */
	totalcost += stackupto(tp,ply,toplevel);
	CHKCOST();
	break;
    case DO_DEALLOCATE:
	DEBUG("DEALLOCATE");
	getint(tinstno,codep);
	instance(tinstno,&token);
	if (token.t_token==-1)
		chrefcount(token.t_att[0].ar,-1,TRUE);
	else {
		tdp= &tokens[token.t_token];
		for (i=0;i<TOKENSIZE;i++)
			if (tdp->t_type[i]==EV_REG)
				chrefcount(token.t_att[i].ar,-1,TRUE);
	}
	break;
    case DO_REALLOCATE:
	DEBUG("REALLOCATE");
	for(rp=machregs;rp<machregs+NREGS;rp++)
		if(rp->r_tcount) {
			rp->r_refcount -= rp->r_tcount;
			rp->r_tcount = 0;
		}
	break;
    case DO_ALLOCATE:
	DEBUG("ALLOCATE");
	if (codep[-1]&32) {
		getint(propno,codep);
		getint(tinstno,codep);
	} else {
		getint(propno,codep);
		tinstno=0;
	}
	instance(tinstno,&token);
	if (!forced) {
		do {
			npos=exactmatch=0;
			for(rpp=reglist[propno];rp= *rpp; rpp++)
				if (getrefcount((int)(rp-machregs), FALSE)==0) {
					pos[npos++] = rp-machregs;
					if (eqtoken(&rp->r_contents,&token))
						exactmatch++;
				}
			/*
			 * Now pos[] contains all free registers with desired
			 * property. If none then some stacking has to take place.
			 */
			if (npos==0) {
				if (stackheight<=tokpatlen) {
					if (!toplevel) {
						totalcost = INFINITY;
						BROKE();
					} else {
						fatal("No regs available");
					}
				}
				totalcost += stackupto( &fakestack[0],ply,toplevel);
				CHKCOST();
			}
		} while (npos==0);
		if (!exactmatch) {
			npos2=npos;
			for(i=0;i<npos;i++)
				pos2[i]=pos[i];
		} else {
			/*
			 * Now we are reducing the number of possible registers.
			 * We take only one equally likely register out of every
			 * equivalence class as given by set of properties.
			 */
			mtoken = token;
			npos2=0;
			for(i=0;i<npos;i++)
				if (eqtoken(&machregs[pos[i]].r_contents,&mtoken)) {
					pos2[npos2++] = pos[i];
					for(j=0;j<npos2-1;j++)
						if (eqregclass(pos2[j],pos[i])) {
							npos2--;
							break;
						}
				}
		}
		/*
		 * Now pos2[] contains all possibilities to try, if more than
		 * one, lookahead is necessary.
		 */
		token2.t_token= -1;
		for (i=1;i<TOKENSIZE;i++)
			token2.t_att[i].aw=0;
		if (npos2==1)
			decision=pos2[0];
		else {
			SAVEST;
			mincost=costlimit-totalcost+1;
			for(j=0;j<npos2;j++) {
				chrefcount(pos2[j],1,FALSE);
				token2.t_att[0].ar=pos2[j];
				allreg[nallreg++] = pos2[j];
				if (token.t_token != 0)
					t=move(&token,&token2,ply,FALSE,mincost);
				else {
					t = 0;
					erasereg(pos2[j]);
				}
				if (t<mincost)
					t += codegen(codep,ply,FALSE,mincost<MAXINT?mincost-t:MAXINT,0);
				if (t<mincost) {
					mincost=t;
					decision=pos2[j];
				}
				RESTST;
			}
			FREEST;
			if (totalcost+mincost>costlimit) {
				totalcost = INFINITY;
				BROKE();
			}
		}
	} else {
		decision = forced;
		if (getrefcount(decision, FALSE)!=0) {
			totalcost = INFINITY;
			BROKE();
		}
		token2.t_token = -1;
	}
	chrefcount(decision,1,FALSE);
	token2.t_att[0].ar=decision;
	if (token.t_token != 0) {
		totalcost+=move(&token,&token2,ply,toplevel,MAXINT);
		CHKCOST();
	} else
		erasereg(decision);
	allreg[nallreg++]=decision;
	break;
    case DO_LOUTPUT:
	DEBUG("LOUTPUT");
	getint(stringno,codep);
	getint(nodeno,codep);
	if (toplevel) {
		gencode(codestrings[stringno]);
		genexpr(nodeno);
	}
	break;
    case DO_ROUTPUT:
	DEBUG("ROUTPUT");
	i=((codep[-1]>>5)&07);
	do {
		getint(stringno,codep);
		if (toplevel) {
			gencode(codestrings[stringno]);
			gennl();
		}
	} while (i--);
	break;
    case DO_MOVE:
	DEBUG("MOVE");
	getint(tinstno,codep);
	instance(tinstno,&token);
	getint(tinstno,codep);
	instance(tinstno,&token2);
	totalcost += move(&token,&token2,ply,toplevel,costlimit-totalcost+1);
	CHKCOST();
	break;
    case DO_ERASE:
	DEBUG("ERASE");
	getint(nodeno,codep);
	result=compute(&enodes[nodeno]);
	assert(result.e_typ==EV_REG);
	erasereg(result.e_v.e_reg);
	break;
    case DO_TOKREPLACE:
	DEBUG("TOKREPLACE");
	assert(stackheight>=tokpatlen);
	repllen=(codep[-1]>>5)&07;
	for(i=0;i<repllen;i++) {
		getint(tinstno,codep);
		instance(tinstno,&reptoken[i]);
		tref(&reptoken[i],1);
	}
	for(i=0;i<tokpatlen;i++) {
		if (!inscoerc)
			tref(&fakestack[stackheight-1],-1);
		stackheight--;
	}
	for (i=0;i<repllen;i++) {
		assert(stackheight<MAXFSTACK);
		fakestack[stackheight] = reptoken[i];
		stackheight++;
		/* do not combine previous two statements; that does not
		   compile under Xenix V3.2
		*/
	}
	for(i=0;i<nallreg;i++)
		chrefcount(allreg[i],-1,FALSE);
	break;
    case DO_EMREPLACE:
	DEBUG("EMREPLACE");
	emrepllen=(codep[-1]>>5)&07;
	j=emp-emlines;
	if (emrepllen>j) {
		int k = nemlines + emrepllen - j;
		assert(k<MAXEMLINES);
		for (i=nemlines;i>=0;i--, k--)
			emlines[k] = emlines[i];
		nemlines += emrepllen-j;
		emp += emrepllen-j;
	}
	emp -= emrepllen;
	for (i=0;i<emrepllen;i++) {
		getint(eminstr,codep);
		getint(nodeno,codep);
		emp[i].em_instr = eminstr;
		result = compute(&enodes[nodeno]);
		switch(result.e_typ) {
		default:
			assert(FALSE);
		case 0:
			emp[i].em_optyp = OPNO;
			emp[i].em_soper = 0;
			break;
		case EV_INT:
			emp[i].em_optyp = OPINT;
			emp[i].em_soper = tostring(result.e_v.e_con);
			emp[i].em_u.em_ioper = result.e_v.e_con;
			break;
		case EV_STR:
			emp[i].em_optyp = OPSYMBOL;
			emp[i].em_soper = result.e_v.e_str;
			break;
		}
	}
	if (!toplevel) {
		ply += emrepllen;
	}
	break;
    case DO_COST:
	DEBUG("COST");
	getint(cost.c_size,codep);
	getint(cost.c_time,codep);
	totalcost += costcalc(cost);
	CHKCOST();
	break;
#ifdef REGVARS
    case DO_PRETURN:
	if (toplevel) {
		swtxt();
		regreturn();	/* in mach.c */
	}
	break;
#endif
    case DO_RETURN:
	DEBUG("RETURN");
	assert(origcp!=startupcode);
    doreturn:
#ifndef NDEBUG
	level--;
#endif
	return(totalcost);
	}
	}
}
