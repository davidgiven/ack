/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"
#include "util.h"
#include "eval.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif


/* expression types for predefined functions are assembled */
int	typetable[10];
int	exprlimit;

/* handle all predefined functions */
#define cv(X)	conversion(type,X); pop=X



void parm(int cnt)
{
	if( cnt> exprlimit)
		error("Not enough arguments");
	if( cnt < exprlimit)
		error("Too many arguments");
}



int callfcn(int fcnnr,int cnt,int *typetable)
{
	int pop=DOUBLETYPE;
	int res=DOUBLETYPE;
	int type;


	type= typetable[0];
	exprlimit=cnt;
	if(debug) print("fcn=%d\n",fcnnr);

	switch(fcnnr)
	{
		case ABSSYM: 	cv(DOUBLETYPE);
				C_cal("_abr");
				parm(1);
				break;
		case ASCSYM:	cv(STRINGTYPE);
				C_cal("_asc"); 
				res=INTTYPE;
				parm(1);
				break;
		case ATNSYM:	cv(DOUBLETYPE);
				C_cal("_atn");
				parm(1);
				break;
		case CDBLSYM:	cv(DOUBLETYPE);  
				return(DOUBLETYPE);;
		case CHRSYM:	cv(INTTYPE);
				C_cal("_chr"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case CSNGSYM:   cv(DOUBLETYPE); 
				return(DOUBLETYPE);
		case CINTSYM:	cv(INTTYPE);  
				return(INTTYPE);
		case COSSYM:	cv(DOUBLETYPE);
				C_cal("_cos");
				parm(1);
				break;
		case CVISYM:	cv(STRINGTYPE);
				C_cal("_cvi"); 
				res=INTTYPE;
				parm(1);
				break;
		case CVSSYM:	cv(STRINGTYPE);
				C_cal("_cvd"); 
				res=DOUBLETYPE;
				parm(1);
				break;
		case CVDSYM:	cv(STRINGTYPE);
				C_cal("_cvd"); 
				res=DOUBLETYPE;
				parm(1);
				break;
		case EOFSYM:	
				if( cnt==0)
				{
					res= INTTYPE;
					pop= INTTYPE;
                                   	C_loc((arith) -1);
				} else cv(INTTYPE);
				C_cal("_ioeof"); 
				res=INTTYPE;
				break;
		case EXPSYM:	cv(DOUBLETYPE);
				C_cal("_exp");
				parm(1);
				break;
		case FIXSYM:	cv(DOUBLETYPE);
				C_cal("_fix"); 
				res=INTTYPE;
				parm(1);
				break;
		case INPSYM:
		case LPOSSYM:
		case FRESYM:	pop=0;
				warning("function not supported");
				parm(1);
				break;
		case HEXSYM:	cv(INTTYPE);
				C_cal("_hex"); res=STRINGTYPE;
				parm(1);
				break;
		case OUTSYM:
		case INSTRSYM:	cv(DOUBLETYPE);
				C_cal("_instr"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case INTSYM:	cv(DOUBLETYPE);
				C_cal("_fcint");
				parm(1);
				break;
		case LEFTSYM:	parm(2);
				extraconvert(type, STRINGTYPE,typetable[1]);
				type= typetable[1];
				cv(INTTYPE);
				C_cal("_left"); 
				res=STRINGTYPE;
                           	C_asp((arith) BEMPTRSIZE);
                           	C_asp((arith) BEMINTSIZE);
                           	C_lfr((arith) BEMPTRSIZE);
				return(STRINGTYPE);
		case LENSYM:	cv(STRINGTYPE);
				C_cal("_length"); 
				res=INTTYPE;
				parm(1);
				break;
		case LOCSYM:	cv(INTTYPE);
				C_cal("_loc"); 
				res=INTTYPE;
				parm(1);
				break;
		case LOGSYM:	cv(DOUBLETYPE);
				C_cal("_log");
				parm(1);
				break;
		case MKISYM:	cv(INTTYPE);
				C_cal("_mki"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case MKSSYM:	cv(DOUBLETYPE);
				C_cal("_mkd"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case MKDSYM:	cv(DOUBLETYPE);
				C_cal("_mkd"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case OCTSYM:	cv(INTTYPE);
				C_cal("_oct"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case PEEKSYM:	cv(INTTYPE);
				C_cal("_peek"); 
				res=INTTYPE;
				parm(1);
				break;
		case POSSYM:	C_asp((arith) typestring(type));
	                        C_exa_dnam("_pos");
	                        C_loe_dnam("_pos",(arith) 0);
				return(INTTYPE);
		case RIGHTSYM:	parm(2);
				extraconvert(type, STRINGTYPE,typetable[1]);
				type= typetable[1];
				cv(INTTYPE);
				C_cal("_right"); 
				res=STRINGTYPE;
	                        C_asp((arith) BEMINTSIZE);
	                        C_asp((arith) BEMPTRSIZE);
	                        C_lfr((arith) BEMPTRSIZE);
				return(STRINGTYPE);
		case RNDSYM:	if( cnt==1) pop=type; 
				else pop=0;
				C_cal("_rnd"); 
				res= DOUBLETYPE;
				break;
		case SGNSYM:	cv(DOUBLETYPE);
				C_cal("_sgn"); 
				res=INTTYPE;
				parm(1);
				break;
		case SINSYM:	cv(DOUBLETYPE);
				C_cal("_sin");
				parm(1);
				break;
		case SPACESYM:	cv(INTTYPE);
				C_cal("_space"); 
				res=STRINGTYPE;
				parm(1);
				break;
		case SPCSYM:	cv(INTTYPE);
				C_cal("_spc"); 
				res=0;
				parm(1);
				break;
		case SQRSYM:	cv(DOUBLETYPE);
				C_cal("_sqt");
				parm(1);
				break;
		case STRSYM:	cv(DOUBLETYPE);
				C_cal("_nstr");
				res=STRINGTYPE; /* NEW */
				parm(1);
				break;
		case STRINGSYM:
				parm(2);        /* 2 is NEW */
				if (typetable[1] == STRINGTYPE) {
					C_cal("_asc");
					C_asp((arith)BEMPTRSIZE);
					C_lfr((arith)BEMINTSIZE);
					typetable[1] = INTTYPE;
				}
				extraconvert(type,
                                             DOUBLETYPE,
                                             typetable[1]);   /* NEW */
				type= typetable[1];
				cv(DOUBLETYPE);               /* NEW */
				C_cal("_string"); 
				res=STRINGTYPE;
				C_asp((arith)typestring(DOUBLETYPE)); /*NEW*/
				break;
		case TABSYM:	cv(INTTYPE);
				C_cal("_tab"); 
				res=0;
				parm(1);
				break;
		case TANSYM:	cv(DOUBLETYPE);
				C_cal("_tan");
				parm(1);
				break;
		case VALSYM:	cv(STRINGTYPE);
				C_loi((arith)BEMPTRSIZE);
				C_cal("atoi"); 
				res=INTTYPE;
				parm(1);
				break;
		case VARPTRSYM:	cv(DOUBLETYPE);
				C_cal("_valptr");
				parm(1);
				break;
		default:	error("unknown function");
	}

	if(pop) C_asp((arith) typestring(pop));
	if(res) C_lfr((arith) typestring(res));
	return(res);
}

