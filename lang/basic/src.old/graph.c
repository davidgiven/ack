#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


List *forwardlabel=0;

Linerecord	*firstline, 
		*currline, 
		*lastline;

List *newlist()
{
	List *l;
	/*NOSTRICT*/ l= (List *) salloc(sizeof(List));
	return(l);
}

/* Line management is handled here */

Linerecord *srchline(nr)
int nr;
{
	Linerecord *l;
	for(l=firstline;l && l->linenr<=nr;l= l->nextline)
	if( l->linenr== nr) return(l);
	return(0);
}
List *srchforward(nr)
int nr;
{
	List *l;
	for(l=forwardlabel;l ;l=l->nextlist)
	if( l->linenr== nr) return(l);
	return(0);
}
linewarnings()
{
	List *l;
	extern int errorcnt;
	l= forwardlabel;
	while(l)
	{
	 	if( !srchline(l->linenr))
		{
			printf("ERROR: line %d not defined\n",l->linenr);
			errorcnt++;
		}
		l=l->nextlist;
	}
}

newblock(nr)
int	nr;
{
	Linerecord	*l;
	List		*frwrd;

	if( debug) printf("newblock at %d\n",nr);
	if( nr>0 && currline && currline->linenr>= nr)
	{
		if( debug) printf("old line:%d\n",currline->linenr);
		error("Lines out of sequence");
	}

	frwrd=srchforward(nr);
	if( frwrd && debug) printf("forward found %d\n",frwrd->emlabel);
	l= srchline(nr);
	if( l)
	{
		error("Line redefined");
		nr= -genlabel();
	}

	/* make new EM block structure */
	/*NOSTRICT*/ l= (Linerecord *) salloc(sizeof(*l));
	l->emlabel= frwrd? frwrd->emlabel: genlabel();
	l->linenr= nr;
	/* save offset into tmpfile too */
	l->offset = (long) ftell(tmpfile);
	l->codelines= emlinecount;

	/* insert this record */
	if( firstline)
	{
		currline->nextline=l;
		l->prevline= currline;
		lastline= currline=l;
	} else
		firstline = lastline =currline=l;
}

gotolabel(nr)
int nr;
{
	/* simulate a goto statement in the line record table */
	Linerecord *l1;
	List	*ll;

	if(debug) printf("goto label %d\n",nr);
	/* update currline */
	ll= newlist();
	ll-> linenr=nr;
	ll-> nextlist= currline->gotos;
	currline->gotos= ll;

	/* try to generate code */
	l1= srchline(nr);
	if( (ll=srchforward(nr))!=0) 
		nr= ll->emlabel;
	else
	if( l1==0)
	{
		/* declare forward label */
		if(debug) printf("declare forward %d\n",nr);
		ll= newlist();
		ll->emlabel= genlabel();
		ll-> linenr=nr;
		ll->nextlist= forwardlabel;
		forwardlabel= ll;
		nr= ll->emlabel;
	} else 
		nr= l1->emlabel;
	return(nr);
}
gotostmt(nr)
int nr;
{
	emcode("bra",instrlabel(gotolabel(nr)));
}
/* GOSUB-return, assume that proper entries are made to subroutines
   only. The return statement is triggered by a fake constant label */

List	*gosubhead, *gotail;
int	gosubcnt=1;

List *gosublabel()
{
	List *l;

	l= newlist();
	l->nextlist=0;
	l->emlabel=genlabel();
	if( gotail){
		gotail->nextlist=l;
		gotail=l;
	} else gotail= gosubhead=l;
	gosubcnt++;
	return(l);
}
gosubstmt(lab)
int lab;
{
	List *l;
	int nr,n;

	n=gosubcnt;
	l= gosublabel();
	nr=gotolabel(lab);
	emcode("loc",itoa(n));	/*return index */
	emcode("cal","$_gosub");	/* administer legal return */
	emcode("asp",EMINTSIZE);
	emcode("bra",instrlabel(nr));
	fprintf(tmpfile,"%d\n",l->emlabel);
	emlinecount++;
}
genreturns()
{
	int nr;
	nr= genlabel();
	fprintf(emfile,"returns\n");
	fprintf(emfile," rom *%d,1,%d\n",nr,gosubcnt-1);
	while( gosubhead)
	{
		fprintf(emfile," rom *%d\n",gosubhead->emlabel);
		gosubhead= gosubhead->nextlist;
	}
	fprintf(emfile,"%d\n",nr);
	fprintf(emfile," loc 1\n");
	fprintf(emfile," cal $error\n");
}
returnstmt()
{
	emcode("cal","$_retstmt");	/* ensure legal return*/
	emcode("lfr",EMINTSIZE);
	fprintf(tmpfile," lae returns\n");
	emlinecount++;
	emcode("csa",EMINTSIZE);
}
/* compound goto-gosub statements */
List	*jumphead,*jumptail;
int	jumpcnt;

jumpelm(nr)
int nr;
{
	List *l;

	l= newlist();
	l->emlabel= gotolabel(nr);
	l->nextlist=0;
	if( jumphead==0) jumphead= jumptail= l;
	else {
		jumptail->nextlist=l;
		jumptail=l;
	}
	jumpcnt++;
}
ongotostmt(type)
int type;
{
	/* generate the code itself, index in on top of the stack */
	/* blurh, store the number of entries in the descriptor */
	int firstlabel;
	int descr;
	List *l;
	/* create descriptor first */
	descr= genlabel();
	firstlabel=genlabel();
	fprintf(tmpfile,"l%d\n",descr); emlinecount++;
	fprintf(tmpfile," rom *%d,1,%d\n",firstlabel,jumpcnt); emlinecount++;
	l= jumphead;
	while( l)
	{
		fprintf(tmpfile," rom *%d\n",l->emlabel); emlinecount++;
		l= l->nextlist;
	}
	jumphead= jumptail=0; jumpcnt=0;
	if(debug) printf("ongotst:%d labels\n", jumpcnt);
	conversion(type,INTTYPE);
	emcode("lae",datalabel(descr));
	emcode("csa",EMINTSIZE);
	fprintf(tmpfile,"%d\n",firstlabel); emlinecount++;
}
ongosubstmt(type)
int type;
{
	List *l;
	int firstlabel;
	int descr;
	/* create descriptor first */
	descr= genlabel();
	firstlabel=genlabel();
	fprintf(tmpfile,"l%d\n",descr); emlinecount++;
	fprintf(tmpfile," rom *%d,1,%d\n",firstlabel,jumpcnt); emlinecount++;
	l= jumphead;
	while( l)
	{
		fprintf(tmpfile," rom *%d\n",l->emlabel); emlinecount++;
		l= l->nextlist;
	}
	jumphead= jumptail=0; jumpcnt=0;

	l= newlist();
	l->nextlist=0;
	l->emlabel=firstlabel;
	if( gotail){
		gotail->nextlist=l;
		gotail=l;
	} else gotail= gosubhead=l;
	/* save the return point of the gosub */
	emcode("loc",itoa(gosubcnt));
	emcode("cal","$_gosub");
	emcode("asp",EMINTSIZE);
	gosubcnt++;
	/* generate gosub */
	conversion(type,INTTYPE);
	emcode("lae",datalabel(descr));
	emcode("csa",EMINTSIZE);
	fprintf(tmpfile,"%d\n",firstlabel);
	emlinecount++;
}

/* REGION ANALYSIS and FINAL VERSION GENERATION */

simpleprogram()
{
	char	buf[512];
	int length;

	/* a small EM programs has been found */
	prologcode();
	prolog2();
	(void) fclose(tmpfile);
	tmpfile= fopen(tmpfname,"r");
	if( tmpfile==NULL)
		fatal("tmp file disappeared");
	while( (length=fread(buf,1,512,tmpfile)) != 0)
		(void) fwrite(buf,1,length,emfile);
	epilogcode();
	(void) unlink(tmpfname);
}
