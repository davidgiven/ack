#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
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

#define ALLOW_NEXTEM	/* code generator is allowed new try of NEXTEM
			   in exceptional cases */

byte startupcode[] = { DO_NEXTEM };

byte *nextem();
unsigned costcalc();
unsigned docoerc();
unsigned stackupto();
string tostring();
string ad2str();

#ifdef NDEBUG
#define DEBUG(string)
#else
#include <stdio.h>
#define DEBUG(string) {if(Debug) fprintf(stderr,"%-*d%s\n",4*level,level,string);}
#endif

#define BROKE() {assert(origcp!=startupcode || !paniced);DEBUG("BROKE");totalcost=INFINITY;goto doreturn;}
#define CHKCOST() {if (totalcost>=costlimit) BROKE();}

#ifdef TABLEDEBUG
int tablelines[MAXTDBUG];
int ntableline;
int set_fd,set_size;
short *set_val;
char *set_flag;
#endif

unsigned codegen(codep,ply,toplevel,costlimit,forced) byte *codep; unsigned costlimit; {
#ifndef NDEBUG
	byte *origcp=codep;
	static int level=0;
#endif
	unsigned totalcost = 0;
	int inscoerc=0;
	int procarg[MAXPROCARG+1];
#ifdef ALLOW_NEXTEM
	static int paniced;
	char *savebp = 0;
#endif
	state_t state;
#define SAVEST	savestatus(&state)
#define RESTST	restorestatus(&state)
#define FREEST	/* nothing */
#ifdef TABLEDEBUG
	extern char *tablename;
#endif

#ifndef NDEBUG
	assert(costlimit <= INFINITY);
	level++;
	DEBUG("Entering codegen");
	if (Debug > 1) fprintf(stderr, "toplevel = %d\n", toplevel);
#endif
	for (;;) {
	switch( (*codep++)&037 ) {
    default:
	assert(FALSE);
	/* NOTREACHED */
#ifdef TABLEDEBUG
    case DO_DLINE: {
	int n;

	getint(n,codep);
	tablelines[ntableline++] = n;
	if (ntableline>=MAXTDBUG)
		ntableline -= MAXTDBUG;
	if (set_fd)
		set_val[n>>4] &= ~(1<<(n&017));
#ifndef NDEBUG
	if (Debug)
		fprintf(stderr,"code from \"%s\", line %d\n",tablename,n);
#endif
	break;
    }
#endif
    case DO_NEXTEM: {
	byte *bp;
	int n;
	unsigned mindistance,dist;
	register i;
	int cindex;
	int npos,pos[MAXRULE];
	unsigned mincost,t;

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
	if (stackheight>MAXFSTACK-7) {
#ifndef NDEBUG
		if (Debug)
			fprintf(stderr,"Fakestack overflow threatens(%d), action ...\n",stackheight);
#endif
		totalcost += stackupto(&fakestack[6],ply,toplevel);
	}
#ifndef ALLOW_NEXTEM
	bp = nextem(toplevel);
#else
	if (toplevel) paniced=0;
	savebp = nextem(toplevel);
    panic:
	if (toplevel) totalcost = 0;
	bp = savebp;
#endif
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
		if (n==0) {	/* "procedure" */
			int j, nargs;
			getint(i,bp);
			getint(nargs,bp);
			assert(nargs <= MAXPROCARG);
			for (j = 0; j < nargs; j++) {
				getint(procarg[j],bp);
			}
			bp= &pattern[i];
			n = *bp++;
			DEBUG("PROC_CALL");
		}
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
				if (dist<=mindistance
#ifdef ALLOW_NEXTEM
				    || paniced
#endif
				   ) {
					if (dist<mindistance) {
						if(dist==0)
							goto gotit;
						npos=0;
						mindistance = dist;
					}
#ifdef ALLOW_NEXTEM
					if (dist < MAXINT)
#endif
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
				assert(mincost <= INFINITY);
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
    }
    case DO_COERC: {
	DEBUG("COERC");
	tokpatlen=1;
	inscoerc=1;
	break;
    }
    case DO_XXMATCH:
	DEBUG("XXMATCH");
    case DO_XMATCH: {
	register i;
	int temp;

	DEBUG("XMATCH");
	tokpatlen=(codep[-1]>>5)&07;
	for (i=0;i<tokpatlen;i++)
		getint(temp,codep);
	break;	/* match already checked by distance() */
    }
    case DO_MATCH: {
	register i;
	int j;
	unsigned mincost,t;
	token_p tp;
	int size,lsize;
	int tokexp[MAXPATLEN];
	int nregneeded;
	token_p regtp[MAXCREG];
	c3_p regcp[MAXCREG];
	rl_p regls[MAXCREG];
	c3_p cp,findcoerc();
#ifdef MAXSPLIT
	int sret;
#endif
	int stackpad = 0;
	struct perm *tup,*ntup,*besttup,*tuples();

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
			cp = findcoerc(tp, &machsets[tokexp[i]]);
#ifndef NDEBUG
if (Debug>1) fprintf(stderr,"findcoerc returns 0x%x at position %d\n",(unsigned)cp,i);
#endif
			if (cp==0) {
				for (j=0;j<nregneeded;j++)
					regtp[j] -= (tp-fakestack+1);
				totalcost += stackupto(tp,ply,toplevel);
				CHKCOST();
				break;
			} else {
				if (cp->c3_prop<0) {
					totalcost+=docoerc(tp,cp,ply,toplevel,0);
					CHKCOST();
				} else {
#ifndef NDEBUG
if(Debug>1) fprintf(stderr,"Register of type %d needed, remembering...\n",cp->c3_prop);
#endif
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
#ifndef NDEBUG
if(Debug>1) fprintf(stderr,"Pattern too long, %d with only %d items on stack\n",
		tokpatlen,stackheight);
#endif
		stackpad = tokpatlen-stackheight;
		for (j=stackheight-1;j>=0;j--)
			fakestack[j+stackpad] = fakestack[j];
		for (j=0;j<stackpad;j++)
			fakestack[j].t_token=0;
		stackheight += stackpad;
		for (j=0;j<nregneeded;j++)
			regtp[j] += stackpad;
		for (tp = &fakestack[stackpad-1];i<tokpatlen && tp>=fakestack;i++,tp--) {
			cp = findcoerc((token_p) 0, &machsets[tokexp[i]]);
			if (cp==0) {
				for (j=0;j<nregneeded;j++)
					myfree((string) (regls[j]));
#ifndef ALLOW_NEXTEM
				assert(!toplevel);
				BROKE();
#else
				assert(!(toplevel&&paniced));
				if (paniced) goto normalfailed;
				totalcost = INFINITY;
				for (i=0;i<stackheight-stackpad;i++)
					fakestack[i] = fakestack[i+stackpad];
				stackheight -= stackpad;
				goto doreturn;
#endif
			}
			if (cp->c3_prop<0) {
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
		if ( t<mincost && tokpatlen<=stackheight ) {
#ifndef NDEBUG
			if (Debug>2)
				fprintf(stderr,"Continuing match after coercions\n");
#endif
			t += codegen(codep,ply,FALSE,mincost<MAXINT?mincost-t:MAXINT,0);
		}
		if ( t<mincost && tokpatlen<=stackheight ) {
			mincost = t;
			besttup = tup;
		} else
			myfree((string) tup);
		RESTST;
	}
	FREEST;
	for (i=0;i<nregneeded;i++)
		myfree((string)(regls[i]));
	if (totalcost+mincost>costlimit) {
		if (besttup)
			myfree((string)besttup);
normalfailed:	if (stackpad!=tokpatlen) {
			if (stackpad) {
				for (i=0;i<stackheight-stackpad;i++)
					fakestack[i] = fakestack[i+stackpad];
				stackheight -= stackpad;
				if (costlimit<MAXINT)
					BROKE();
				totalcost += stackupto(&fakestack[stackheight-1],ply,toplevel);
			} else
				totalcost += stackupto(fakestack,ply,toplevel);
			CHKCOST();
			goto nextmatch;
		}
		totalcost += mincost;
		for (i=0;i<stackheight-stackpad;i++)
			fakestack[i] = fakestack[i+stackpad];
		stackheight -= stackpad;
                BROKE();
	}
	for (i=0;i<nregneeded;i++)
		totalcost += docoerc(regtp[i],regcp[i],ply,toplevel,besttup->p_rar[i]);
	assert(totalcost <= costlimit);
	myfree((string)besttup);
	break;
    }
    case DO_TOSTACK:
    case DO_REMOVE: {
	int texpno,nodeno;
	token_p tp;
	struct reginfo *rp;
	int doremove = (codep[-1] & 037) == DO_REMOVE;
	extern int allsetno;

	DEBUG(doremove ? "REMOVE" : "TOSTACK");
	if (codep[-1]&32) {
		getint(texpno,codep);
		getint(nodeno,codep);
	} else {
		getint(texpno,codep);
		nodeno=0;
	}
	if (texpno == allsetno) {
		totalcost += stackupto(&fakestack[stackheight-tokpatlen-1],ply,toplevel);
		CHKCOST();
		if (doremove) for (rp=machregs;rp<machregs+NREGS;rp++)
			rp->r_contents.t_token=0;
		break;
	}
	for (tp= &fakestack[stackheight-tokpatlen-1];tp>=&fakestack[0];tp--)
		if (match(tp,&machsets[texpno],nodeno)) {
			/* investigate possible coercion to register */
			totalcost += stackupto(tp,ply,toplevel);
			CHKCOST();
			break;
		}
	if (doremove) for (rp=machregs;rp<machregs+NREGS;rp++) {
		if (rp->r_contents.t_token != 0 &&
		    match(&rp->r_contents,&machsets[texpno],nodeno)) {
#ifndef NDEBUG
			if (Debug > 1) fprintf(stderr, "killing reg %ld (%s)\n", (long)(rp-machregs), rp->r_repr ? codestrings[rp->r_repr] : "cc");
#endif
			rp->r_contents.t_token=0;
		}
	}
	break;
    }
    case DO_KILLREG:
    case DO_RREMOVE: {	/* register remove */
	register i;
	int nodeno;
	token_p tp;
	tkdef_p tdp;
	result_t result;
	int dokill = (codep[-1] & 037) == DO_KILLREG;

	DEBUG(dokill ? "KILLREG" : "RREMOVE");
	getint(nodeno,codep);
	compute(&enodes[nodeno], &result);
	if (result.e_typ!=EV_REG)
		break;
	if ( in_stack(result.e_v.e_reg) ) BROKE() ; /* Check aside-stack */
	if (dokill) {
		/* kill register, and kill condition codes if they are set to
		   this register
		*/
		machregs[result.e_v.e_reg].r_contents.t_token = 0;
		if (machregs[0].r_contents.t_token == -1 &&
		    machregs[0].r_contents.t_att[0].ar == result.e_v.e_reg) {
			machregs[0].r_contents.t_token = 0;	
		}
	}
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
    }
    case DO_DEALLOCATE: {
	register i;
	tkdef_p tdp;
	int tinstno;
	token_t token;

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
    }
    case DO_REALLOCATE: {
	struct reginfo *rp;

	DEBUG("REALLOCATE");
	for(rp=machregs+1;rp<machregs+NREGS;rp++)
		if(rp->r_tcount) {
			rp->r_refcount -= rp->r_tcount;
			rp->r_tcount = 0;
		}
	break;
    }
    case DO_ALLOCATE: {
	register i;
	int j;
	int tinstno;
	int npos,npos2,pos[NREGS],pos2[NREGS];
	unsigned mincost,t;
	struct reginfo *rp,**rpp;
	token_t token,token2;
	int propno;
	int exactmatch;
	int decision;

	if (codep[-1]&32) {
		getint(propno,codep);
		getint(tinstno,codep);
		DEBUG("ALLOCATE,INIT");
	} else {
		getint(propno,codep);
		tinstno=0;
		DEBUG("ALLOCATE,EMPTY");
	}
	instance(tinstno,&token);
	if (!forced) {
		do {
			npos=exactmatch=0;
			for(rpp=reglist[propno];rp= *rpp; rpp++)
				if (getrefcount((int)(rp-machregs), FALSE)==0) {
					pos[npos++] = rp-machregs;
					if (eqtoken(&rp->r_contents,&token))
						pos2[exactmatch++] = rp-machregs;
				}
			/*
			 * Now pos[] contains all free registers with desired
			 * property. If none then some stacking has to take place.
			 */
			if (npos==0) {
  				if (stackheight<=tokpatlen) {
  					if (!toplevel) {
  						BROKE();
 					} else {
 						if (paniced)
 							fatal("No regs available");
 						totalcost += stackupto( &fakestack[0],ply,toplevel);
 						goto panic;
 					}
  				}
  				totalcost += stackupto( &fakestack[0],ply,toplevel);
  				CHKCOST();
			}
		} while (npos==0);

		if (!exactmatch && tinstno!=0) {
			/*
			 * No exact match, but we were looking for a particular
			 * token. Now try to find registers of which no
			 * known contents is available (the others might still
			 * be useful).
			 */
			for (i=0;i<npos;i++)
				if (machregs[pos[i]].r_contents.t_token == 0) {
					pos2[exactmatch++] = pos[i];
				}
		}

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
			npos2=0;
			for(i=0;i<exactmatch;i++) {
				pos2[npos2++] = pos2[i];
				for(j=0;j<npos2-1;j++)
					if (eqregclass(pos2[j],pos2[i])) {
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
		decision=pos2[0];
		if (npos2!=1) {
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
			if (totalcost+mincost>costlimit)
				BROKE();
		}
	} else {
		decision = forced;
		if (getrefcount(decision, FALSE)!=0)
			BROKE();
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
    }
    case DO_INSTR: {
	register i;
	int n;
	int tinstno;
	token_t token;
	int stringno;

    	DEBUG("INSTR");
	n=((codep[-1]>>5)&07);
	getint(stringno,codep);
	if (toplevel) {
		swtxt();
		if (stringno>10000) {
			assert(stringno < 100001 + MAXPROCARG);
			genstr(procarg[stringno-10001]);
		} else
			genstr(stringno);
	}
	for(i=0;i<n;i++) {
		getint(tinstno,codep);
		instance(tinstno,&token);
		if (toplevel)
			prtoken(&token,i==0 ? ' ' : ',');
		if (token.t_token>0)
			totalcost += tokens[token.t_token].t_cost.ct_space;
	}
	if (toplevel)
		gennl();
	CHKCOST();
	break;		
    }
    case DO_MOVE: {
	int tinstno;
	token_t token,token2;

	DEBUG("MOVE");
	getint(tinstno,codep);
	instance(tinstno,&token);
	getint(tinstno,codep);
	instance(tinstno,&token2);
	totalcost += move(&token,&token2,ply,toplevel,costlimit-totalcost+1);
	CHKCOST();
	break;
    }
    case DO_TEST: {
	int tinstno;
	token_t token;

	DEBUG("TEST");
	getint(tinstno,codep);
	instance(tinstno,&token);
	totalcost += test(&token,ply,toplevel,costlimit-totalcost+1);
	CHKCOST();
	break;
    }
    case DO_SETCC: {
	int tinstno;
	token_t token;

    	DEBUG("SETCC");
	getint(tinstno,codep);
	instance(tinstno,&token);
	setcc(&token);
	break;
    }
    case DO_ERASE: {
	int nodeno;
	result_t result;

	DEBUG("ERASE");
	getint(nodeno,codep);
	compute(&enodes[nodeno], &result);
	assert(result.e_typ!=EV_INT && result.e_typ!=EV_ADDR);
	if (result.e_typ==EV_REG)
		erasereg(result.e_v.e_reg);
	break;
    }
    case DO_TOKREPLACE: {
	register i;
	int tinstno;
	int repllen;
	token_t reptoken[MAXREPLLEN];

	DEBUG("TOKREPLACE");
	assert(stackheight>=tokpatlen);
	repllen=(codep[-1]>>5)&07;
#ifndef NDEBUG
	if (Debug>2)
		fprintf(stderr,"Stackheight=%d, tokpatlen=%d, repllen=%d %s\n",
			stackheight,tokpatlen,repllen,inscoerc ? "(inscoerc)":"");
#endif
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
		fakestack[stackheight++] = reptoken[i];
	}
	for(i=0;i<nallreg;i++)
		chrefcount(allreg[i],-1,FALSE);
	break;
    }
    case DO_EMREPLACE: {
	register i;
	int j;
	int nodeno;
	result_t result[MAXEMREPLLEN];
	int emrepllen,eminstr;

	DEBUG("EMREPLACE");
	emrepllen=(codep[-1]>>5)&07;
	j=emp-emlines;
	if (emrepllen>j) {
		assert(nemlines+emrepllen-j<MAXEMLINES);
		for (i=nemlines;i>=0;i--)
			emlines[i+emrepllen-j] = emlines[i];
		nemlines += emrepllen-j;
		emp += emrepllen-j;
	}
	emp -= emrepllen;
	for (i=0;i<emrepllen;i++) {
		getint(eminstr,codep);
		getint(nodeno,codep);
		emp[i].em_instr = eminstr;
		compute(&enodes[nodeno], &result[i]);
	}
	for (i=0;i<emrepllen;i++) {
		switch(result[i].e_typ) {
		default:
			assert(FALSE);
		case 0:
			emp[i].em_optyp = OPNO;
			emp[i].em_soper = 0;
			break;
		case EV_INT:
			emp[i].em_optyp = OPINT;
			emp[i].em_soper = tostring(result[i].e_v.e_con);
			emp[i].em_u.em_ioper = result[i].e_v.e_con;
			break;
		case EV_ADDR:
			emp[i].em_optyp = OPSYMBOL;
			emp[i].em_soper = ad2str(result[i].e_v.e_addr);
			break;
		}
	}
	if (!toplevel) {
		ply += emrepllen;
#ifndef NDEBUG
		if (Debug > 4) 
			fprintf(stderr, "ply becomes %d\n", ply);
#endif
	}
	break;
    }
    case DO_COST: {
	cost_t cost;

	DEBUG("COST");
	getint(cost.ct_space,codep);
	getint(cost.ct_time,codep);
	totalcost += costcalc(cost);
	CHKCOST();
	break;
    }
#ifdef REGVARS
    case DO_PRETURN: {
	if (toplevel) {
		swtxt();
		regreturn();	/* in mach.c */
	}
	break;
    }
#endif
    case DO_RETURN:
	DEBUG("RETURN");
	assert(origcp!=startupcode);
#ifndef NDEBUG
	level--;
#endif
	return(totalcost);
#ifdef USE_TES
    case DO_LABDEF: {
	int index;

	DEBUG("LABDEF");
	getint(index,codep);
	if (toplevel) {
		swtxt();
		printlabel(index);
	}

	break;
    }
#endif
	}
	}
    doreturn:
#ifdef ALLOW_NEXTEM
	if (toplevel && totalcost == INFINITY && ! paniced) {
		DEBUG("PANIC!");
		totalcost += stackupto(&fakestack[stackheight-1], ply, toplevel);
#ifndef NDEBUG
		if (Debug > 2)
			fprintf(stderr, "Stackheight = %d\n", stackheight);
#endif
		paniced = 1;
		tokpatlen = 0;
		goto panic;
	}
#endif
#ifndef NDEBUG
	level--;
#endif
	return(totalcost);
}

readcodebytes() {
#ifndef CODEINC
	register fd;
	extern int ncodebytes;

	if ((fd=open("code",0))<0) {
		error("Can't open code");
	}
	if (read(fd,coderules,ncodebytes)!=ncodebytes) {
		error("Short read from code");
	}
	close(fd);
#endif
}

#ifdef TABLEDEBUG
initlset(f) char *f; {
	extern char *myalloc();

	set_flag = f;
	if ((set_fd=open(f+1,2))<0)
		error("Can't open %s rw",f+1);
	read(set_fd,&set_size,sizeof(int));
	set_val=( short *) myalloc(set_size);
	read(set_fd,set_val,set_size);
}

termlset() {

	if (set_fd) {
		lseek(set_fd,(long) sizeof(int),0);
		write(set_fd,set_val,set_size);
		close(set_fd);
		if (set_flag[0]=='u') {
			register i;
			
			fprintf(stderr,"Unused code rules:\n\n");
			for(i=0;i<8*set_size;i++)
				if(set_val[i>>4]&(1<<(i&017)))
					fprintf(stderr,"\"%s\", line %d\n",tablename,i);
		}
	}
}
#endif
