#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


/* Here we find all routines dealing with pure EM code generation */

static int	emlabel=1;
genlabel() { return(emlabel++);}


genemlabel()
{
	int l;

	l=genlabel();
	fprintf( emfile,"l%d\n",l);
	return(l);
}
genrom()
{
	int l;
	l= genemlabel();
	fprintf(emfile," rom ");
	return(l);
}

where()
{
	return(emlinecount);
}
exchange(blk1,blk2)
int blk1,blk2;
{
	/* exchange assembler blocks */
	if(debug) printf("exchange %d %d %d\n",blk1,blk2,emlinecount);
	fprintf(Tmpfile," exc %d,%d\n",blk2-blk1,emlinecount-blk2);
	emlinecount++;
}

/* routines to manipulate the Tmpfile */
int	emlinecount;		/* count number of lines generated */
				/* this value can be used to generate EXC */
int tronoff=0;
newemblock(nr)
int nr;
{
	/* save location on Tmpfile */
	currline->offset= ftell(Tmpfile);
	fprintf(Tmpfile,"%d\n",currline->emlabel);
	emlinecount++;
	if (! nolins) {
		fprintf(Tmpfile," lin %d\n",nr);
		emlinecount++;
	}
	if( tronoff || traceflag) {
		emcode("loc",itoa(nr));
		emcode("cal","$_trace");
		emcode("asp","EM_WSIZE");
	}
}

emcode(operation,params)
char *operation,*params;
{
	fprintf(Tmpfile," %s %s\n",operation,params);
	emlinecount++;
}
/* Handle data statements */
int	dataused=0;
List	*datalist=0;
datastmt()
{
	List *l,*l1;
	/*NOSTRICT*/ l= (List *) salloc(sizeof(List));
	l->linenr= currline->linenr;
	/*NOSTRICT?*/ l->emlabel= (long) ftell(datfile);
	if( datalist==0) 
	{
		datalist=l;
		datfile= fopen(datfname,"w");
		if( datfile==NULL) fatal("improper file creation permission");
	}else{
		l1= datalist;
		while(l1->nextlist) l1= l1->nextlist;
		l1->nextlist=l;
	}

	dataused=1;
}
datatable()
{
	List *l;
	int line=0;

	/* called at end to generate the data seek table */
	fprintf(emfile," exa _seektab\n");
	fprintf(emfile,"_seektab\n");
	l= datalist;
	while(l)
	{
		fprintf(emfile," rom %d,%d\n", l->linenr,line++);
		l= l->nextlist;
	}
	fprintf(emfile," rom 0,0\n");
}

/* ERROR and exception handling */
exceptstmt(lab)
int lab;
{
	/* exceptions to subroutines are supported only */
	extern int gosubcnt;
	List	*l;

	emcode("loc",itoa(gosubcnt));
	l= (List *) gosublabel();
	l->emlabel= gotolabel(lab);
	emcode("cal","$_trpset");
	emcode("asp",EMINTSIZE);
}

errorstmt(exprtype)
int	exprtype;
{
	/* convert expression to a valid error number */
	/* obtain the message and print it */
	emcode("cal","$error");
	emcode("asp",typesize(exprtype));
}

/* BASIC IO */
openstmt(recsize)
int recsize;
{
	emcode("loc",itoa(recsize));
	emcode("cal","$_opnchn");
	emcode("asp",EMPTRSIZE);
	emcode("asp",EMPTRSIZE);
	emcode("asp",EMINTSIZE);
}


printstmt(exprtype)
int	exprtype;
{
	switch(exprtype)
	{
	case INTTYPE:
		emcode("cal","$_prinum");
		emcode("asp",typestring(INTTYPE));
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		emcode("cal","$_prfnum");
		emcode("asp",typestring(DOUBLETYPE));
		break;
	case STRINGTYPE:
		emcode("cal","$_prstr");
		emcode("asp",EMPTRSIZE);
		break;
	case 0:	/* result of tab function etc */
		break;
	default:
		error("printstmt:unexpected");
	}
}
zone(i)
int i;
{
	if( i)emcode("cal","$_zone");
}
writestmt(exprtype,comma)
int	exprtype,comma;
{
	if( comma) emcode("cal","$_wrcomma");
	switch(exprtype)
	{
	case INTTYPE:
		emcode("cal","$_wrint");
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		emcode("cal","$_wrint");
		break;
	case STRINGTYPE:
		emcode("cal","$_wrstr");
		break;
	default:
		error("printstmt:unexpected");
	}
	emcode("asp",EMPTRSIZE);
}
restore(lab)
int lab;
{
	/* save this information too */

	 emcode("loc",itoa(0));
	 emcode("cal","$_setchan");
	 emcode("asp",EMINTSIZE);
	 emcode("loc",itoa(lab));
	 emcode("cal","$_restore");
	 emcode("asp",EMINTSIZE);
}
prompt(qst)
int qst;
{
	setchannel(-1);
	emcode("cal","$_prstr");
	emcode("asp",EMPTRSIZE);
	if(qst) emcode("cal","$_qstmark");
}
linestmt(type)
int type;
{
	if( type!= STRINGTYPE)
		error("String variable expected");
	emcode("cal","$_rdline");
	emcode("asp",EMPTRSIZE);
}
readelm(type)
int type;
{
	switch(type)
	{
	case INTTYPE:
		emcode("cal","$_readint");
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		emcode("cal","$_readflt");
		break;
	case STRINGTYPE:
		emcode("cal","$_readstr");
		break;
	default:
		error("readelm:unexpected type");
	}
	emcode("asp",EMPTRSIZE);
}

/* Swap exchanges the variable values */
swapstmt(ltype,rtype)
int	ltype, rtype;
{
	if( ltype!= rtype)
		error("Type mismatch");
	else
	switch(ltype)
	{
	case INTTYPE:
		emcode("cal","$_intswap");
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		emcode("cal","$_fltswap");
		break;
	case STRINGTYPE:
		emcode("cal","$_strswap");
		break;
	default:
		error("swap:unexpected");
	}
	emcode("asp",EMPTRSIZE);
	emcode("asp",EMPTRSIZE);
}

/* input/output handling */
setchannel(val)
int val;
{	/* obtain file descroption */
	emcode("loc",itoa(val));
	emcode("cal","$_setchan");
	emcode("asp",EMINTSIZE);
}
/* The if-then-else statements */
ifstmt(type)
int type;
{
	/* This BASIC follows the True= -1 rule */
	int nr;

	nr= genlabel();
	if( type == INTTYPE)
		emcode("zeq",instrlabel(nr));
	else	
	if( type == FLOATTYPE)
	{
		emcode("lae","fltnull");
		emcode("loi",EMFLTSIZE);
		emcode("cmf",EMFLTSIZE);
		emcode("zeq",instrlabel(nr));
	}
	else error("Integer or Float expected");
	return(nr);
}
thenpart( elselab)
int elselab;
{
	int nr;

	nr=genlabel();
	emcode("bra",instrlabel(nr));
	fprintf(Tmpfile,"%d\n",elselab);
	emlinecount++;
	return(nr);
}
elsepart(lab)int lab;
{
	fprintf(Tmpfile,"%d\n",lab); emlinecount++;
}
/* generate code for the for-statement */
#define MAXFORDEPTH 20
struct FORSTRUCT{
	Symbol	*loopvar;		/* loop variable */
	int	initaddress;
	int	limitaddress;
	int	stepaddress;
	int	fortst;		/* variable limit test */
	int	forinc;		/* variable increment code */
	int	forout;		/* end of loop */
} fortable[MAXFORDEPTH];
int	forcnt= -1;

forinit(s)
Symbol *s;
{
	int type;
	struct FORSTRUCT *f;

	dcltype(s);
	type= s->symtype;
	forcnt++;
	if( (type!=INTTYPE && type!=FLOATTYPE && type!=DOUBLETYPE) ||
	    s->dimensions)
		error("Illegal loop variable");
	if( forcnt >=MAXFORDEPTH)
		error("too many for statements");
	else{
		f=fortable+forcnt; 
		f->loopvar=s;
		f->fortst=genlabel();
		f->forinc=genlabel();
		f->forout=genlabel();
		/* generate space for temporary objects */
		f->initaddress= dclspace(type);
		f->limitaddress= dclspace(type);
		f->stepaddress= dclspace(type);
	}
}
forexpr(type)
int type;
{
	/* save start value of loop variable in a save place*/
	/* to avoid clashing with final value and step expression */
	int result;
	result= fortable[forcnt].loopvar->symtype;
	conversion(type,result);
	storevar(fortable[forcnt].initaddress, result);
}
forlimit(type)
int type;
{
	/* save the limit value too*/
	int result;
	result= fortable[forcnt].loopvar->symtype;
	conversion(type,result);
	storevar(fortable[forcnt].limitaddress, result);
}
forskipped(f)
struct FORSTRUCT *f;
{
	int type;
	type= f->loopvar->symtype;
	/* evaluate lower bound times sign of step */
	emcode("lae",datalabel(f->initaddress));
	loadvar(type);
	conversion(type,DOUBLETYPE);
	emcode("lae",datalabel(f->stepaddress));
	loadvar(type);
	conversion(type,DOUBLETYPE);
	emcode("cal","$_sgn");
	emcode("asp",EMFLTSIZE);
	emcode("lfr",EMINTSIZE);
	conversion(INTTYPE,DOUBLETYPE);
	emcode("mlf",EMFLTSIZE);
	/* evaluate higher bound times sign of step */
	emcode("lae",datalabel(f->limitaddress));
	loadvar(type);
	conversion(type,DOUBLETYPE);
	emcode("lae",datalabel(f->stepaddress));
	loadvar(type);
	conversion(type,DOUBLETYPE);
	emcode("cal","$_sgn");
	emcode("asp",EMFLTSIZE);
	emcode("lfr",EMINTSIZE);
	conversion(INTTYPE,DOUBLETYPE);
	emcode("mlf",EMFLTSIZE);
	/* skip condition */
	emcode("cmf",EMFLTSIZE);
	emcode("zgt",instrlabel(f->forout));
}
forstep(type)
int type;
{
	int result;
	int varaddress;
	struct FORSTRUCT *f;

	f= fortable+forcnt;
	result= f->loopvar->symtype;
	varaddress= f->loopvar->symalias;
	conversion(type,result);
	storevar(f->stepaddress, result);
	/* all information available, generate for-loop head */
	/* test for ingoring loop */
	forskipped(f);
	/* set initial value */
	emcode("lae",datalabel(f->initaddress));
	loadvar(result);
	emcode("lae",datalabel(varaddress));
	emcode("sti",typestring(result));
	emcode("bra",instrlabel(f->fortst)); 
	/* increment loop variable */
	fprintf(Tmpfile,"%d\n",f->forinc);
	emlinecount++;
	emcode("lae",datalabel(varaddress));
	loadvar(result);
	emcode("lae",datalabel(f->stepaddress));
	loadvar(result);
	if(result == INTTYPE)
		emcode("adi",EMINTSIZE);
	else	emcode("adf",EMFLTSIZE);
	emcode("lae",datalabel(varaddress));
	emcode("sti",typestring(result));
	/* test boundary */
	fprintf(Tmpfile,"%d\n",f->fortst);
	emlinecount++;
	emcode("lae",datalabel(varaddress));
	loadvar(result);
	emcode("lae",datalabel(f->limitaddress));
	loadvar(result);
	if(result == INTTYPE)
		emcode("cmi",EMINTSIZE);
	else	emcode("cmf",EMFLTSIZE);
	emcode("zgt",instrlabel(f->forout));
}
nextstmt(s)
Symbol *s;
{
	if(forcnt>MAXFORDEPTH || forcnt<0 || 
	  ( s && s!= fortable[forcnt].loopvar))
		error("NEXT without FOR");
	else{
		/* address of variable is on top of stack ! */
		emcode("bra",instrlabel(fortable[forcnt].forinc));
		fprintf(Tmpfile,"%d\n",fortable[forcnt].forout);
		forcnt--;
	}
}

pokestmt(type1,type2)
int	type1,type2;
{
	conversion(type1,INTTYPE);
	conversion(type2,INTTYPE);
	emcode("cal","$_poke");
	emcode("asp",EMINTSIZE);
	emcode("asp",EMINTSIZE);
}

/* generate code for the while statement */
#define MAXDEPTH 20

int	whilecnt, whilelabels[MAXDEPTH][2]; /*0=head,1=out */

whilestart()
{
	whilecnt++;
	if( whilecnt==MAXDEPTH)
		fatal("too many nestings");
	/* gendummy label in graph */
	newblock(-1);
	whilelabels[whilecnt][0]= currline->emlabel;
	whilelabels[whilecnt][1]= genlabel();
	fprintf(Tmpfile,"%d\n", whilelabels[whilecnt][0]);
	emlinecount++;
}
whiletst(exprtype)
int exprtype;
{
	/* test expression type */
	conversion(exprtype,INTTYPE);
	fprintf(Tmpfile," zeq *%d\n",whilelabels[whilecnt][1]);
	emlinecount++;
}
wend()
{
	if( whilecnt<1)
		error("not part of while statement");
	else{
		fprintf(Tmpfile," bra *%d\n",whilelabels[whilecnt][0]);
		fprintf(Tmpfile,"%d\n",whilelabels[whilecnt][1]);
		emlinecount++;
		emlinecount++;
		whilecnt--;
	}
}

/* generate code for the final version */
prologcode()
{
	/* generate the EM prolog code */
	fprintf(emfile,"fltnull\n con 0,0,0,0\n");
	fprintf(emfile,"dummy2\n con 0,0,0,0\n");
	fprintf(emfile,"tronoff\n con 0\n");
	fprintf(emfile,"dummy1\n con 0,0,0,0\n");
	fprintf(emfile," exa _iomode\n_iomode\n rom \"O\"\n");
	fprintf(emfile," exa _errsym\n");
	fprintf(emfile,"_errsym\n bss EM_WSIZE,0,1\n");
	fprintf(emfile," exa _erlsym\n");
	fprintf(emfile,"_erlsym\n bss EM_WSIZE,0,1\n");
}

prolog2()
{
	fprintf(emfile," exp $main\n");
	fprintf(emfile," pro $main,0\n");
	fprintf(emfile," mes 3\n");
	fprintf(emfile," mes 9,0\n");
	/* Trap handling */
	fprintf(emfile," cal $_ini_trp\n");
	fprintf(emfile," exa trpbuf\n");
	fprintf(emfile," lae trpbuf\n");
	fprintf(emfile," cal $setjmp\n");
	fprintf(emfile," asp 4\n");
	fprintf(emfile," lfr %s\n",EMINTSIZE);
	fprintf(emfile," dup %s\n",EMINTSIZE);
	fprintf(emfile," zeq *0\n");
	fprintf(emfile," lae returns\n");
	fprintf(emfile," csa %s\n",EMINTSIZE);
	fprintf(emfile,"0\n");
	fprintf(emfile," asp EM_WSIZE\n");
	/* when data lists are used open its file */
	if( dataused)
	{
		fprintf(emfile," loc 0\n");
		fprintf(emfile," cal $_setchan\n");
		fprintf(emfile," asp EM_WSIZE\n");
		fprintf(emfile,"datfname\n rom \"%s\\0\"\n", datfname);
		fprintf(emfile,"dattyp\n rom \"i\\0\"\n");
		fprintf(emfile,"datfdes\n rom datfname,1,%d\n",
			strlen(datfname));
		fprintf(emfile,"dattdes\n rom dattyp,1,1\n");
		fprintf(emfile," lae dattdes\n");
		fprintf(emfile," lae datfdes\n");
		fprintf(emfile," loc 0\n");
		fprintf(emfile," cal $_opnchn\n");
		fprintf(emfile," asp EM_PSIZE\n");
		fprintf(emfile," asp EM_PSIZE\n");
		fprintf(emfile," asp EM_WSIZE\n");
	}
	datatable();
}

epilogcode()
{
	/* finalization code */
	int nr;
	nr= genlabel();
	fprintf(emfile," bra *%d\n",nr);
	genreturns();
	fprintf(emfile,"%d\n",nr);
	fprintf(emfile," loc 0\n");
	fprintf(emfile," cal $_hlt\n");
	fprintf(emfile," end 0\n");
	fprintf(emfile," mes 4,4\n");
}
