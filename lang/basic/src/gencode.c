/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"
#include "graph.h"
#include "eval.h"
#include "util.h"
#include "gencode.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif


/* Here we find all routines dealing with pure EM code generation */

static int	emlabel=1;
label	err_goto_label;



int genlabel(void)
{
	 return(emlabel++);
}



int genemlabel(void)
{
	int l;

	l=genlabel();
	C_df_dlb((label)l);
	return(l);
}





int tronoff=0;
void newemblock(int nr)
{
	C_df_ilb((label)currline->emlabel);
	C_lin((arith)nr);
	if ( tronoff || traceflag) {
		C_loc((arith)nr);
		C_cal("_trace");
		C_asp((arith)BEMINTSIZE);
	}
}





/* Handle data statements */
List	*datalist=0;
void datastmt(void)
{
	List *l,*l1;


	/* NOSTRICT */ l= (List *) salloc(sizeof(List));
	l->linenr= currline->linenr;
        l->emlabel =  sys_filesize(datfname);
	if ( datalist==0) 
	{
		datalist=l;
	} else {
		l1= datalist;
		while (l1->nextlist) l1= l1->nextlist;
		l1->nextlist=l;
	}

}



void datatable(void)
{
	List *l;
	int line=0;

	/* called at end to generate the data seek table */
	C_exa_dnam("_seektab");
	C_df_dnam("_seektab");       /* VRAAGTEKEN */
	l= datalist;
	while (l)
	{
		C_rom_cst((arith)(l->linenr));
		C_rom_cst((arith)(line++));
		l= l->nextlist;
	}
	C_rom_cst((arith)0);
	C_rom_cst((arith)0);
}



/* ERROR and exception handling */
void exceptstmt(int lab)
{
	/* exceptions to subroutines are supported only */
	extern int gosubcnt;
	List	*l;

	C_loc((arith)gosubcnt);
	l= (List *) gosublabel();
	l->emlabel= gotolabel(lab);
	C_cal("_trpset");
	C_asp((arith)BEMINTSIZE);
}



void errorstmt(int exprtype)
{
	/* convert expression to a valid error number */
	/* obtain the message and print it */
	C_cal("error");
	C_asp((arith)typesize(exprtype));
}



/* BASIC IO */
void openstmt(int recsize)
{
	C_loc((arith)recsize);
	C_cal("_opnchn");
	C_asp((arith)(2*BEMPTRSIZE+BEMINTSIZE));
}



void printstmt(int exprtype)
{
	switch(exprtype)
	{
		case INTTYPE:
			C_cal("_prinum");
			C_asp((arith)typestring(INTTYPE));
			break;
		case FLOATTYPE:
		case DOUBLETYPE:
			C_cal("_prfnum");
			C_asp((arith)typestring(DOUBLETYPE));
			break;
		case STRINGTYPE:
			C_cal("_prstr");
			C_asp((arith)BEMPTRSIZE);
			break;
		case 0:	/* result of tab function etc */
			break;
		default:
			error("printstmt:unexpected");
	}
}



void zone(int i)
{
	if ( i) C_cal("_zone");
}



void writestmt(int exprtype, int comma)
{
	if ( comma) C_cal("_wrcomma");

	switch(exprtype)
	{
		case INTTYPE:
			C_cal("_wrint");
			break;
		case FLOATTYPE:
		case DOUBLETYPE:
			C_cal("_wrflt");
			break;
		case STRINGTYPE:
			C_cal("_wrstr");
			break;
		default:
			error("printstmt:unexpected");
	}
	C_asp((arith)BEMPTRSIZE);
}



void restore(int lab)
{
	/* save this information too */

	C_loc((arith)0);
	C_cal("_setchan");
	C_asp((arith)BEMINTSIZE);
	C_loc((arith)lab);
	C_cal("_restore");
	C_asp((arith)BEMINTSIZE);
}



void prompt(int qst)
{
	setchannel(-1);
	C_cal("_prstr");
	C_asp((arith)BEMPTRSIZE);
	if (qst) C_cal("_qstmark");
}



void linestmt(int type)
{
	if ( type!= STRINGTYPE)
		error("String variable expected");
	C_cal("_rdline");
	C_asp((arith)BEMPTRSIZE);
}



void readelm(int type)
{
	switch(type)
	{
		case INTTYPE:
			C_cal("_readint");
			break;
		case FLOATTYPE:
		case DOUBLETYPE:
			C_cal("_readflt");
			break;
		case STRINGTYPE:
			C_cal("_readstr");
			break;
		default:
			error("readelm:unexpected type");
	}
	C_asp((arith)BEMPTRSIZE);
}



/* Swap exchanges the variable values */
void swapstmt(int ltype,int rtype)
{
	if ( ltype!= rtype)
		error("Type mismatch");
	else
		switch(ltype)
		{
			case INTTYPE:
				C_cal("_intswap");
				break;
			case FLOATTYPE:
			case DOUBLETYPE:
				C_cal("_fltswap");
				break;
			case STRINGTYPE:
				C_cal("_strswap");
				break;
			default:
				error("swap:unexpected");
		}

	C_asp((arith)(2*BEMPTRSIZE));
}



/* input/output handling */
void setchannel(int val)
{
	/* obtain file descroption */
	C_loc((arith)val);
	C_cal("_setchan");
	C_asp((arith)BEMINTSIZE);
}



/* The if-then-else statements */
int ifstmt(int type)
{
	/* This BASIC follows the True= -1 rule */
	int nr;

	nr= genlabel();
	if ( type == INTTYPE)
		C_zeq((label)nr);
	else	
		if ( type == FLOATTYPE || type == DOUBLETYPE )
		{
			C_lae_dnam("fltnull",(arith)0);
			C_loi((arith)BEMFLTSIZE);
			C_cmf((arith)BEMFLTSIZE);
			C_zeq((label)nr);
		}
		else error("Integer or Float expected");

	return(nr);
}



int thenpart(int elselab)
{
	int nr;

	nr=genlabel();
	C_bra((label)nr);
	C_df_ilb((label)elselab);
	return(nr);
}



void elsepart(int lab)
{
	C_df_ilb((label)lab);
}



/* generate code for the for-statement */
#define MAXFORDEPTH 20

struct FORSTRUCT{
	Symbol	*loopvar;	/* loop variable */
	int	initaddress;
	int	limitaddress;
	int	stepaddress;
	int	fortst;		/* variable limit test */
	int	forinc;		/* variable increment code */
	int	forout;		/* end of loop */
} fortable[MAXFORDEPTH];

int	forcnt= -1;



void forinit(Symbol *s)
{
	int type;
	struct FORSTRUCT *f;

	dcltype(s);
	type= s->symtype;
	forcnt++;
	if ( (type!=INTTYPE && type!=FLOATTYPE && type!=DOUBLETYPE) ||
	    s->dimensions)
		error("Illegal loop variable");
	if ( forcnt >=MAXFORDEPTH)
		error("too many for statements");
	else {
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



void forexpr(int type)
{
	/* save start value of loop variable in a save place*/
	/* to avoid clashing with final value and step expression */
	int result;

	result= fortable[forcnt].loopvar->symtype;
	conversion(type,result);
	storevar(fortable[forcnt].initaddress, result);
}



void forlimit(int type)
{
	/* save the limit value too*/
	int result;

	result= fortable[forcnt].loopvar->symtype;
	conversion(type,result);
	storevar(fortable[forcnt].limitaddress, result);
}



void forskipped(struct FORSTRUCT *f)
{
	int type;

	type= f->loopvar->symtype;
	/* evaluate lower bound times sign of step */
	C_lae_dlb((label)f->initaddress,(arith)0);
	loadvar(type);
	conversion(type,DOUBLETYPE);
	C_lae_dlb((label)f->stepaddress,(arith)0);
	loadvar(type);
	conversion(type,DOUBLETYPE);
	C_cal("_forsgn");
	C_asp((arith)BEMFLTSIZE);
	C_lfr((arith)BEMINTSIZE);
	conversion(INTTYPE,DOUBLETYPE);
	C_mlf((arith)BEMFLTSIZE);
	/* evaluate higher bound times sign of step */
	C_lae_dlb((label)f->limitaddress,(arith)0);
	loadvar(type);
	conversion(type,DOUBLETYPE);
	C_lae_dlb((label)f->stepaddress,(arith)0);
	loadvar(type);
	conversion(type,DOUBLETYPE);
	C_cal("_forsgn");
	C_asp((arith)BEMFLTSIZE);
	C_lfr((arith)BEMINTSIZE);
	conversion(INTTYPE,DOUBLETYPE);
	C_mlf((arith)BEMFLTSIZE);
	/* skip condition */
	C_cmf((arith)BEMFLTSIZE);
	C_zgt((label)f->forout);
}



void forstep(int type)
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
	C_lae_dlb((label)f->initaddress,(arith)0);
	loadvar(result);
	C_lae_dlb((label)varaddress,(arith)0);
	C_sti((arith)typestring(result));
	C_bra((label)f->fortst);
	/* increment loop variable */
	C_df_ilb((label)f->forinc);
	C_lae_dlb((label)varaddress,(arith)0);
	loadvar(result);
	C_lae_dlb((label)f->stepaddress,(arith)0);
	loadvar(result);
	if (result == INTTYPE)
		C_adi((arith)BEMINTSIZE);
	else	C_adf((arith)BEMFLTSIZE);
	C_lae_dlb((label)varaddress,(arith)0);
	C_sti((arith)typestring(result));
	/* test boundary */
	C_df_ilb((label)f->fortst);
	C_lae_dlb((label)varaddress,(arith)0);
	loadvar(result);
        /* Start of NEW code */
	C_lae_dlb((label)f->stepaddress,(arith)0); 
	loadvar(result);                           
	conversion(result,DOUBLETYPE);            
	C_cal("_forsgn");                           
	C_asp((arith)BEMFLTSIZE);               
	C_lfr((arith)BEMINTSIZE);              
	conversion(INTTYPE,result);           
	if ( result == INTTYPE )
		C_mli((arith)BEMINTSIZE);
	else	C_mlf((arith)BEMFLTSIZE);    
        /* End of NEW code */
	C_lae_dlb((label)f->limitaddress,(arith)0);
	loadvar(result);
        /* Start NEW code */
	C_lae_dlb((label)f->stepaddress,(arith)0); 
	loadvar(result);                    
	conversion(result,DOUBLETYPE);     
	C_cal("_forsgn");                    
	C_asp((arith)BEMFLTSIZE);        
	C_lfr((arith)BEMINTSIZE);       
	conversion(INTTYPE,result);    
	if ( result == INTTYPE )
		C_mli((arith)BEMINTSIZE);
	else	C_mlf((arith)BEMFLTSIZE);
        /* End NEW code */
	if (result == INTTYPE)
		C_cmi((arith)BEMINTSIZE);
	else	C_cmf((arith)BEMFLTSIZE);
	C_zgt((label)f->forout);
}



void nextstmt(Symbol *s)
{
	if (forcnt>MAXFORDEPTH || forcnt<0 || 
	    (s && s!= fortable[forcnt].loopvar))
		error("NEXT without FOR");
	else {
		/* address of variable is on top of stack ! */
		C_bra((label)fortable[forcnt].forinc);
		C_df_ilb((label)fortable[forcnt].forout);
		forcnt--;
	}
}



void pokestmt(int type1,int type2)
{
	conversion(type1,INTTYPE);
	conversion(type2,INTTYPE);
	C_asp((arith)(2*BEMINTSIZE));
}



/* generate code for the while statement */
#define MAXDEPTH 20

int	whilecnt, whilelabels[MAXDEPTH][2]; /*0=head,1=out */

void whilestart(void)
{
	whilecnt++;
	if ( whilecnt==MAXDEPTH)
		fatal("too many nestings");
	/* gendummy label in graph */
	newblock(-1);
	whilelabels[whilecnt][0]= currline->emlabel;
	whilelabels[whilecnt][1]= genlabel();
	C_df_ilb((label)whilelabels[whilecnt][0]);
}



void whiletst(int exprtype)
{
	/* test expression type */
	conversion(exprtype,INTTYPE);
	C_zeq((label)whilelabels[whilecnt][1]);
}



void wend(void)
{
	if ( whilecnt<1)
		error("not part of while statement");
	else {
		C_bra((label)whilelabels[whilecnt][0]);
		C_df_ilb((label)whilelabels[whilecnt][1]);
		whilecnt--;
	}
}



/* generate code for the final version */
void prologcode(void)
{
	/* generate the EM prolog code */
	C_df_dnam("fltnull");
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_df_dnam("dummy2");
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	/* NEW variable we make */
	C_df_dnam("dummy3");
	C_bss_dnam((arith)BEMPTRSIZE,"dummy3",(arith)0,0);
	C_df_dnam("tronoff");
	C_con_cst((arith)0);
	C_df_dnam("dummy1");
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
	C_con_cst((arith)0);
        C_exa_dnam("_iomode");
        C_df_dnam("_iomode");
        C_rom_scon("O",(arith)2); 
	C_exa_dnam("_errsym");
	C_exa_dnam("_erlsym");
}



void prolog2(void)
{
	int result;
	label l = genlabel(), l2;

	err_goto_label = genlabel();
	C_exp("main");
	C_pro("main",(arith)0);
	C_ms_par((arith)0);
	/* Trap handling */
	C_cal("_ini_trp");

	l2 = genemlabel();
	C_rom_ilb(l);
	C_lae_dlb(l2, (arith) 0);
	C_loi((arith) BEMPTRSIZE);
	C_exa_dnam("trpbuf");
	C_lae_dnam("trpbuf",(arith)0);
	C_cal("__setjmp");
	C_df_ilb(l);
	C_asp((arith)(BEMPTRSIZE+BEMPTRSIZE));
	C_lfr((arith)BEMINTSIZE);
	C_dup((arith)BEMINTSIZE);
	C_zeq((label)0);
	C_lae_dnam("returns",(arith)0);
	C_csa((arith)BEMINTSIZE);
	C_df_ilb((label)0);
	C_asp((arith)BEMINTSIZE);
	result= sys_open(datfname, OP_WRITE, &datfile);
	if ( result==0 ) fatal("improper file creation permission");
        gendata();
}



/* NEW */
void gendata(void)
{
	C_loc((arith)0);
	C_cal("_setchan");
	C_asp((arith)BEMINTSIZE);
	C_df_dnam("datfname");
	C_rom_scon(datfname,(arith)strlen(datfname) + 1);	/* EHB */
	C_df_dnam("dattyp");
	C_rom_scon("i\\0",(arith)4);
	C_df_dnam("datfdes");
	C_rom_dnam("datfname",(arith)0);
	C_rom_cst((arith)1);
	C_rom_cst((arith)(myitoa(strlen(datfname))));
	C_df_dnam("dattdes");
	C_rom_dnam("dattyp",(arith)0);
	C_rom_cst((arith)1);
	C_rom_cst((arith)1);
	C_lae_dnam("dattdes",(arith)0);
	C_lae_dnam("datfdes",(arith)0);
	C_loc((arith)0);
	C_cal("_opnchn");
	C_asp((arith)(2*BEMPTRSIZE+BEMINTSIZE));
}



void epilogcode(void)
{
	/* finalization code */
	int nr;
	nr= genlabel();
	C_bra((label)nr);
	genreturns();
	C_df_ilb((label)nr);
	datatable(); /* NEW */
	C_loc((arith)0);
	C_cal("_hlt");
	C_df_ilb(err_goto_label);
	C_cal("_goto_err");
	C_end((arith)0);
}
