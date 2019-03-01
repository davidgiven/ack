/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"
#include "util.h"
#include "eval.h"
#include "gencode.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif


List *forwardlabel=0;

Linerecord	*firstline, 
		*currline, 
		*lastline;



List *newlist(void)
{
	List *l;

	/* NOSTRICT */ l = (List *) salloc(sizeof(List));
	return(l);
}


/* Line management is handled here */

Linerecord *srchline(int nr)
{
	Linerecord *l;

	for(l=firstline;l && l->linenr<=nr;l= l->nextline)
		if ( l->linenr== nr) return(l);
	return(0);
}



List *srchforward(int nr)
{
	List *l;

	for(l=forwardlabel;l ;l=l->nextlist)
		if ( l->linenr== nr) return(l);
	return(0);
}



void linewarnings(void)
{
	List *l;
	extern int errorcnt;

	l= forwardlabel;
	while (l)
	{
	 	if ( !srchline(l->linenr))
		{
			fprint(STDERR, "ERROR: line %d not defined\n",l->linenr);
			errorcnt++;
		}
		l=l->nextlist;
	}
}



void newblock(int nr)
{
	Linerecord	*l;
	List		*frwrd;

	if ( debug) print("newblock at %d\n",nr);
	if ( nr>0 && currline && currline->linenr>= nr)
	{
		if ( debug) print("old line:%d\n",currline->linenr);
		error("Lines out of sequence");
	}

	frwrd=srchforward(nr);
	if ( frwrd && debug) print("forward found %d\n",frwrd->emlabel);
	l= srchline(nr);
	if ( l)
	{
		error("Line redefined");
		nr= -genlabel();
	}

	/* make new EM block structure */
	/* NOSTRICT */ l= (Linerecord *) salloc(sizeof(*l));
	l->emlabel= frwrd ? frwrd->emlabel : genlabel();
	l->linenr= nr;

	/* insert this record */
	if ( firstline)
	{
		currline->nextline=l;
		l->prevline= currline;
		lastline= currline=l;
	} else
		firstline = lastline =currline=l;
}



int gotolabel(int nr)
{
	/* simulate a goto statement in the line record table */
	Linerecord *l1;
	List	*ll;

	if (debug) print("goto label %d\n",nr);
	/* update currline */
	ll= newlist();
	ll-> linenr=nr;
	ll-> nextlist= currline->gotos;
	currline->gotos= ll;

	/* try to generate code */
	l1= srchline(nr);
	if ( (ll=srchforward(nr))!=0) 
		nr= ll->emlabel;
	else
		if ( l1==0)
		{
			/* declare forward label */
			if (debug) print("declare forward %d\n",nr);
			ll= newlist();
			ll->emlabel= genlabel();
			ll-> linenr=nr;
			ll->nextlist= forwardlabel;
			forwardlabel= ll;
			nr= ll->emlabel;
		} else nr= l1->emlabel;
	return(nr);
}



void gotostmt(int nr)
{
           C_bra((label) gotolabel(nr));
}

/* GOSUB-return, assume that proper entries are made to subroutines
   only. The return statement is triggered by a fake constant label */

List	*gosubhead, *gotail;
int	gosubcnt=1;



List *gosublabel(void)
{
	List *l;

	l= newlist();
	l->nextlist=0;
	l->emlabel=genlabel();
	if ( gotail){
		gotail->nextlist=l;
		gotail=l;
	} else gotail= gosubhead=l;
	gosubcnt++;
	return(l);
}



void gosubstmt(int lab)
{
	List *l;
	int nr,n;

	n=gosubcnt;
	l= gosublabel();
	nr=gotolabel(lab);
	/*return index */
        C_loc((arith) n);
	/* administer legal return */
        C_cal("_gosub");
        C_asp((arith) BEMINTSIZE);
        C_bra((label) nr);
	C_df_ilb((label)l->emlabel);
}



void genreturns(void)
{
	int count;
	int nr;

	nr= genlabel();
        C_df_dnam("returns");
        C_rom_ilb((label) nr);
        C_rom_cst((arith)1);
        C_rom_cst((arith) (gosubcnt-2));

	count = 0;
	while ( gosubhead)
	{
                C_rom_ilb((label) gosubhead->emlabel);
		gosubhead= gosubhead->nextlist;
		count++;
	}

	if (count != (gosubcnt-1))
		error("gosub count table mismatch");

        C_df_ilb((label) nr);
        C_loc((arith) 1);
        C_cal("error");
}




void returnstmt(void)
{
        C_cal("_retstmt");
        C_lfr((arith) BEMINTSIZE);
        C_lae_dnam("returns",(arith)0);
        C_csa((arith) BEMINTSIZE);
}



/* compound goto-gosub statements */
List	*jumphead,*jumptail;
int	jumpcnt;


void jumpelm(int nr)
{
	List *l;

	l= newlist();
	l->emlabel= gotolabel(nr);
	l->nextlist=0;
	if ( jumphead==0) jumphead = jumptail = l;
	else {
		jumptail->nextlist=l;
		jumptail=l;
	}
	jumpcnt++;
}



void ongotostmt(int type)
{
	/* generate the code itself, index in on top of the stack */
	/* blurh, store the number of entries in the descriptor */
	int firstlabel;
	int descr;
	List *l;

	/* create descriptor first */
	descr= genlabel();
	firstlabel=genlabel();
	C_df_dlb((label)descr);
        C_rom_ilb((label)firstlabel);
	C_rom_cst((arith) 1);
	C_rom_cst((arith)(jumpcnt-1));
	l= jumphead;
	while (l)
	{
		C_rom_ilb((label)l->emlabel);
		l= l->nextlist;
	}
	jumphead= jumptail=0; jumpcnt=0;
	if (debug) print("ongotst:%d labels\n", jumpcnt);
	conversion(type,INTTYPE);
	C_dup((arith) BEMINTSIZE);
	C_zlt(err_goto_label);
        C_lae_dlb((label) descr,(arith) 0);
        C_csa((arith) BEMINTSIZE);
	C_df_ilb((label)firstlabel);
}



void ongosubstmt(int type)
{
	List *l;
	int firstlabel;
	int descr;

	/* create descriptor first */
	descr= genlabel();
	firstlabel=genlabel();
	C_df_dlb((label)descr);
	C_rom_ilb((label)firstlabel);
	C_rom_cst((arith)1);
	C_rom_cst((arith)(jumpcnt-1));
	l= jumphead;

	while (l)
	{
		C_rom_ilb((label)l->emlabel);
		l= l->nextlist;
	}

	jumphead= jumptail=0; 
	jumpcnt=0;
	l= newlist();
	l->nextlist=0;
	l->emlabel=firstlabel;
	if ( gotail){
		gotail->nextlist=l;
		gotail=l;
	} else gotail=gosubhead=l;
	/* save the return point of the gosub */
        C_loc((arith) gosubcnt);
        C_cal("_gosub");
        C_asp((arith) BEMINTSIZE);
	gosubcnt++;
	/* generate gosub */
	conversion(type,INTTYPE);
	C_dup((arith) BEMINTSIZE);
	C_zlt(err_goto_label);
        C_lae_dlb((label) descr,(arith) 0);
        C_csa((arith)  BEMINTSIZE);
        C_df_ilb((label)firstlabel);
}




/* REGION ANALYSIS and FINAL VERSION GENERATION */


