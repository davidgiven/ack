/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

/* Symboltable management module */

int	deftype[128];		/* default type declarer */
				/* which may be set by OPTION BASE */

initdeftype()
{
	int i;
	for(i='a';i<='z';i++) deftype[i]= DOUBLETYPE;
	for(i='A';i<='Z';i++) deftype[i]= DOUBLETYPE;
}

int indexbase=0;		/* start of array subscripting */

Symbol	*firstsym = NIL;
Symbol *alternate = NIL;

Symbol *srchsymbol(str)
char *str;
{
	Symbol *s;
	/* search symbol table entry or create it */
	if(debug) printf("srchsymbol %s\n",str);
	s=firstsym;
	while(s)
	{
		if( strcmp(s->symname,str)==0) return(s);
		s= s->nextsym;
	}
	/* search alternate list */
	s=alternate;
	while(s)
	{
		if( strcmp(s->symname,str)==0) return(s);
		s= s->nextsym;
	}
	/* not found, create an emty slot */
	/*NOSTRICT*/ s=  (Symbol *) salloc(sizeof(Symbol));
	s->symtype= DEFAULTTYPE;
	s->nextsym= firstsym;
	s->symname= (char *) salloc((unsigned)(strlen(str)+1));
	strcpy(s->symname,str);
	firstsym= s;
	if(debug) printf("%s allocated\n",str);
	return(s);
}

dcltype(s)
Symbol *s;
{
	/* type declarer */
	int type;
	if( s->isparam) return;
	type=s->symtype;
	if(type==DEFAULTTYPE)
		/* use the default rule */
		type= deftype[*s->symname];
	/* generate the emlabel too */
	if( s->symalias==0)
		s->symalias= dclspace(type);
	s->symtype= type;
	if(debug) printf("symbol set to %d\n",type);
}
dclarray(s)
Symbol *s;
{
	int i; int size;

	if( s->symtype==DEFAULTTYPE) s->symtype= DOUBLETYPE;
	if(debug) printf("generate space and descriptors for %d\n",s->symtype);
	if(debug) printf("dim %d\n",s->dimensions);
	s->symalias= genlabel();
	/* generate descriptors */
	size=1;
	for(i=0;i<s->dimensions;i++)
		s->dimalias[i]= genlabel();
	for(i=s->dimensions-1;i>=0;i--)
	{
		fprintf(emfile,"l%d\n rom %d,%d,%d*%s\n",
			s->dimalias[i],
			indexbase,
			s->dimlimit[i]-indexbase,
			size, typesize(s->symtype));
		size = size* (s->dimlimit[i]+1-indexbase);
	}
	if(debug) printf("size=%d\n",size);
	/* size of stuff */
	fprintf(emfile,"l%d\n bss %d*%s,0,1\n",
		s->symalias,size,typesize(s->symtype));
	/* Generate the range check descriptors */
	for( i= 0; i<s->dimensions;i++)
		fprintf(emfile,"r%d\n rom %d,%d\n",
			s->dimalias[i],
			indexbase,
			s->dimlimit[i]);

}
defarray(s)
Symbol *s;
{
	/* array is used without dim statement, set default limits */
	int i;
	for(i=0;i<s->dimensions;i++) s->dimlimit[i]=10;
	dclarray(s);
}
dclspace(type)
{
	int nr;
	nr= genemlabel();
	switch( type)
	{
	case STRINGTYPE:
		fprintf(emfile," bss %s,0,1\n",EMPTRSIZE);
		break;
	case INTTYPE:
		fprintf(emfile," bss %s,0,1\n",EMINTSIZE);
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		fprintf(emfile," bss 8,0.0F %s,1\n",EMFLTSIZE);
		break;
	}
	return(nr);
}

/* SOME COMPILE TIME OPTIONS */
optionbase(ival)
int	ival;
{
	if( ival<0 || ival>1)
		error("illegal option base value");
	else indexbase=ival;
}

setdefaulttype(type)
int	type;
{
	extern char *cptr;
	char	first,last,i;

	/* handcrafted parser for letter ranges */
	if(debug) printf("deftype:%s\n",cptr);
	while( isspace(*cptr)) cptr++;
	if( !isalpha(*cptr))
		error("letter expected");
	first= *cptr++;
	if(*cptr=='-')
	{
		/* letter range */
		cptr++;
		last= *cptr;
		if( !isalpha(last))
			error("letter expected");
		else for(i=first;i<=last;i++) deftype[i]= type;
		cptr++;
	} else deftype[first]=type;
	if( *cptr== ',') 
	{
		cptr++;
		setdefaulttype(type);	/* try again */
	}
}

Symbol *fcn;

newscope(s)
Symbol *s;
{
	if(debug) printf("new scope for %s\n",s->symname);
	alternate= firstsym;
	firstsym = NIL;
	fcn=s;
	s->isfunction=1;
	if( fcn->dimensions)
		error("Array redeclared");
	if( fcn->symtype== DEFAULTTYPE)
		fcn->symtype=DOUBLETYPE;
}
/* User defined functions */
heading( )
{
	char	procname[50];
	sprintf(procname,"$_%s",fcn->symname);
	emcode("pro",procname);
	if( fcn->symtype== DEFAULTTYPE)
		fcn->symtype= DOUBLETYPE;
}
fcnsize()
{
	/* generate portable function size */
	int	i;
	for(i=0;i<fcn->dimensions;i++)
		fprintf(Tmpfile,"%s+",typesize(fcn->dimlimit[i]));
	fprintf(Tmpfile,"0\n"); emlinecount++;
}
endscope(type)
int type;
{
	Symbol *s;

	if( debug) printf("endscope");
	conversion(type,fcn->symtype);
	emcode("ret", typestring(fcn->symtype));
	/* generate portable EM code */
	fprintf(Tmpfile," end ");
	fcnsize();
	s= firstsym;
	while(s)
	{
		firstsym = s->nextsym;
		/*NOSTRICT*/ free((char *)s);
		s= firstsym;
	}
	firstsym= alternate;
	alternate = NIL;
	fcn=NIL;
}

dclparm(s)
Symbol	*s;
{
	int size=0;
	if( s->symtype== DEFAULTTYPE)
		s->symtype= DOUBLETYPE;
	s->isparam=1;
	fcn->dimlimit[fcn->dimensions]= s->symtype;
	fcn->dimensions++;
	/*
	OLD STUFF
	for(i=fcn->dimensions;i>0;i--)
		fcn->dimalias[i]= fcn->dimalias[i-1];
	*/
	/*fcn->parmsize += typesize(s->symtype);*/
	/* fcn->dimalias[0]= -typesize(s->symtype)-fcn->dimalias[1];*/
	s->symalias= -fcn->dimensions;
	if( debug) printf("parameter %d offset %d\n",fcn->dimensions-1,-size);
}
/* unfortunately function calls have to be stacked as  well */
#define MAXNESTING	50
Symbol	*fcntable[MAXNESTING];
int	fcnindex= -1;

fcncall(s)
Symbol *s;
{
	if( !s->isfunction)
		error("Function not declared");
	else{
		fcn= s;
		fcnindex++;
		fcntable[fcnindex]=s;
	}
	return(s->symtype);
}
fcnend(parmcount)
int parmcount;
{
	int type;
	/* check number of arguments */
	if( parmcount <fcn->dimensions)
		error("not enough parameters");
	if( parmcount >fcn->dimensions)
		error("too many parameters");
	fprintf(Tmpfile," cal $_%s\n",fcn->symname);
	emlinecount++;
	fprintf(Tmpfile," asp ");
	fcnsize();
	emcode("lfr",typestring(fcn->symtype));
	type= fcn->symtype;
	fcnindex--;
	if( fcnindex>=0)
		fcn= fcntable[fcnindex];
	return(type);
}
callparm(ind,type)
int ind,type;
{
	if( fcnindex<0) error("unexpected parameter");

	if( ind >= fcn->dimensions)
		error("too many parameters");
	else 
		conversion(type,fcn->dimlimit[ind]);
}
