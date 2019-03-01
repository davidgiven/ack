/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "symbols.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bem.h"
#include "eval.h"
#include "gencode.h"
#include "util.h"



#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

/* Symboltable management module */

int	deftype[128];		/* default type declarer */
				/* which may be set by OPTION BASE */


/* Local declarations */
static void get_space(int type,int size);


void initdeftype(void)
{
	int i;

	for(i='a';i<='z';i++) deftype[i]= DOUBLETYPE;
	for(i='A';i<='Z';i++) deftype[i]= DOUBLETYPE;
}


int indexbase=0;		/* start of array subscripting */

Symbol	*firstsym = NIL;
Symbol *alternate = NIL;



Symbol *srchsymbol(char* str)
{
	Symbol *s;

	/* search symbol table entry or create it */
	if (debug) print("srchsymbol %s\n",str);
	s=firstsym;

	while (s)
	{
		if ( strcmp(s->symname,str)==0) return(s);
		s= s->nextsym;
	}

	/* search alternate list */
	s=alternate;

	while (s)
	{
		if ( strcmp(s->symname,str)==0) return(s);
		s= s->nextsym;
	}

	/* not found, create an empty slot */
	s = (Symbol *) salloc(sizeof(Symbol));
	s->symtype= DEFAULTTYPE;
	s->nextsym= firstsym;
	s->symname= (char *) salloc((unsigned) strlen(str)+1);
	strcpy(s->symname,str);
	firstsym= s;
	if (debug) print("%s allocated\n",str);
	return(s);
}



void dcltype(Symbol *s)
{
	/* type declarer */
	int type;

	if ( s->isparam) return;
	type=s->symtype;
	if (type==DEFAULTTYPE)
		/* use the default rule */
		type= deftype[(int)(*s->symname)];
	/* generate the emlabel too */
	if ( s->symalias==0)
		s->symalias= dclspace(type);
	s->symtype= type;
	if (debug) print("symbol set to %d\n",type);
}



void dclarray(Symbol *s)
{
	int i; int size;

	if ( s->symtype==DEFAULTTYPE) s->symtype= DOUBLETYPE;
	if (debug) print("generate space and descriptors for %d\n",s->symtype);
	if (debug) print("dim %d\n",s->dimensions);
	s->symalias= genlabel();
	/* generate descriptors */
	size=1;

	for(i=0;i<s->dimensions;i++) {
		s->dimalias[i]= genlabel();
	}

	for(i=s->dimensions-1;i>=0;i--)
	{
		C_df_dlb((label)(s->dimalias[i]));
		C_rom_cst((arith)indexbase);
		C_rom_cst((arith)(s->dimlimit[i]-indexbase));
		C_rom_cst((arith)(size*typesize(s->symtype)));
		size = size* (s->dimlimit[i]+1-indexbase);
	}

	if (debug) print("size=%d\n",size);
	/* size of stuff */
	C_df_dlb((label)s->symalias);
	get_space(s->symtype,size); /* Van ons. */
}



static void get_space(int type,int size)
{

	switch ( type ) {
		case INTTYPE:
			C_bss_cst((arith)BEMINTSIZE*size,
			   	(arith)0,
			   	1);
			break;
		case FLOATTYPE:
		case DOUBLETYPE:
			C_bss_fcon((arith)BEMFLTSIZE*size,
			   	"0.0",
			   	(arith)BEMFLTSIZE,
			   	1);
			break;
		case STRINGTYPE: /* Note: this is ugly. Gertjan */
			C_bss_icon((arith)BEMPTRSIZE*size,
			   	"0",
                            	(arith)BEMPTRSIZE,
			    	1);
			break;
		default:
			error("Space allocated for unknown type. Coredump.");
			abort(); /* For debugging purposes */
		}
}



void defarray(Symbol *s)
{
	/* array is used without dim statement, set default limits */
	int i;
	for(i=0;i<s->dimensions;i++) s->dimlimit[i]=10;
	dclarray(s);
}



int dclspace(int type)
{
	int nr;

	nr= genemlabel();

	switch( type)
	{
		case STRINGTYPE:
			C_bss_icon((arith)BEMPTRSIZE,"0",(arith)BEMPTRSIZE,1);
			break;
		case INTTYPE:
			C_bss_cst((arith)BEMINTSIZE,(arith)0,1);
			break;
		case FLOATTYPE:
		case DOUBLETYPE:
			C_bss_fcon((arith)BEMFLTSIZE,"0.0",(arith)BEMFLTSIZE,1);
			break;
	}
	return(nr);
}



/* SOME COMPILE TIME OPTIONS */
void optionbase(int ival)
{
	if ( ival<0 || ival>1)
		error("illegal option base value");
	else indexbase=ival;
}



void setdefaulttype(int type)
{
	extern char *cptr;
	char	first,last,i;

	/* handcrafted parser for letter ranges */
	if (debug) print("deftype:%s\n",cptr);
	while ( isspace(*cptr)) cptr++;
	if ( !isalpha(*cptr))
		error("letter expected");
	first= *cptr++;
	if (*cptr=='-')
	{
		/* letter range */
		cptr++;
		last= *cptr;
		if ( !isalpha(last))
			error("letter expected");
		else for(i=first;i<=last;i++) deftype[i]= type;
		cptr++;
	} else deftype[first]=type;
	if ( *cptr== ',') 
	{
		cptr++;
		setdefaulttype(type);	/* try again */
	}
}


Symbol *fcn;



void newscope(Symbol *s)
{
	if (debug) print("new scope for %s\n",s->symname);
	alternate= firstsym;
	firstsym = NIL;
	fcn=s;
	s->isfunction=1;
	if ( fcn->dimensions)
		error("Array redeclared");
	if ( fcn->symtype== DEFAULTTYPE)
		fcn->symtype=DOUBLETYPE;
}



/* User defined functions */



void heading(void)
{
	char procname[50];

	(void) sprint(procname,"_%s",fcn->symname);
        C_pro_narg(procname);
	if ( fcn->symtype== DEFAULTTYPE)
		fcn->symtype= DOUBLETYPE;
}



static int fcnsize(void)
{
	/* generate portable function size */
	int	i,sum;  /* sum is NEW */

	sum = 0;
	for(i=0;i<fcn->dimensions;i++)
		sum += typesize(fcn->dimlimit[i]);
	return(sum);
}



void endscope(int type)
{
	Symbol *s;

	if ( debug) print("endscope");
	conversion(type,fcn->symtype);
        C_ret((arith) typestring(fcn->symtype));
	/* generate portable EM code */
	C_end( (arith)fcnsize() );
	s= firstsym;

	while (s)
	{
		firstsym = s->nextsym;
		(void) free((char *)s);
		s= firstsym;
	}

	firstsym= alternate;
	alternate = NIL;
	fcn=NIL;
}



void dclparm(Symbol *s)
{
	int size=0;

	if ( s->symtype== DEFAULTTYPE)
		s->symtype= DOUBLETYPE;
	s->isparam=1;
	fcn->dimlimit[fcn->dimensions]= s->symtype;
	fcn->dimensions++;
	s->symalias= -fcn->dimensions;
	if ( debug) print("parameter %d offset %d\n",fcn->dimensions-1,-size);
}



/* unfortunately function calls have to be stacked as  well */
#define MAXNESTING	50
Symbol	*fcntable[MAXNESTING];
int	fcnindex= -1;



int fcncall(Symbol *s)
{
	if ( !s->isfunction)
		error("Function not declared");
	else{
		fcn= s;
		fcnindex++;
		fcntable[fcnindex]=s;
	}
	return(s->symtype);
}



int fcnend(int parmcount)
{
	int type;
	static char concatbuf[50]; /* NEW */

	/* check number of arguments */
	if ( parmcount <fcn->dimensions)
		error("not enough parameters");
	if ( parmcount >fcn->dimensions)
		error("too many parameters");
	(void) sprint(concatbuf,"_%s",fcn->symname);
	C_cal(concatbuf);
	C_asp((arith)fcnsize());
        C_lfr((arith) typestring(fcn->symtype));
	type= fcn->symtype;
	fcnindex--;
	if ( fcnindex>=0)
		fcn= fcntable[fcnindex];
	return(type);
}



void callparm(int ind,int type)
{
	if ( fcnindex<0) error("unexpected parameter");
	if ( ind >= fcn->dimensions)
		error("too many parameters");
	else 
		conversion(type,fcn->dimlimit[ind]);
}
