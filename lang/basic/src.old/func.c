/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


/* expression types for predefined functions are assembled */
int	typetable[10];
int	exprlimit;

/* handle all predefined functions */
#define cv(X)	conversion(type,X); pop=X
#define cl(X)	emcode("cal",X);

parm(cnt)
int cnt;
{
	if( cnt> exprlimit)
		error("Not enough arguments");
	if( cnt < exprlimit)
		error("Too many arguments");
}

callfcn(fcnnr,cnt)
int fcnnr,cnt;
{
	int pop=DOUBLETYPE;
	int res=DOUBLETYPE;
	int type;


	type= typetable[0];
	exprlimit=cnt;
	if(debug) printf("fcn=%d\n",fcnnr);
	switch(fcnnr)
	{
	case ABSSYM: 	cv(DOUBLETYPE);
			cl("$_abr");
			parm(1);
			break;
	case ASCSYM:	cv(STRINGTYPE);
			cl("$_asc"); res=INTTYPE;
			parm(1);
			break;
	case ATNSYM:	cv(DOUBLETYPE);
			cl("$_atn");
			parm(1);
			break;
	case CDBLSYM:	cv(DOUBLETYPE);  return(DOUBLETYPE);;
	case CHRSYM:	cv(INTTYPE);
			cl("$_chr"); res=STRINGTYPE;
			parm(1);
			break;
	case CSNGSYM:
		cv(DOUBLETYPE); return(DOUBLETYPE);
	case CINTSYM:	cv(INTTYPE);  return(INTTYPE);
	case COSSYM:	cv(DOUBLETYPE);
			cl("$_cos");
			parm(1);
			break;
	case CVISYM:	cv(STRINGTYPE);
			cl("$_cvi"); res=INTTYPE;
			parm(1);
			break;
	case CVSSYM:	cv(STRINGTYPE);
			cl("$_cvd"); res=DOUBLETYPE;
			parm(1);
			break;
	case CVDSYM:	cv(STRINGTYPE);
			cl("$_cvd"); res=DOUBLETYPE;
			parm(1);
			break;
	case EOFSYM:	
			if( cnt==0)
			{
				res= INTTYPE;
				pop= INTTYPE;
				emcode("loc","-1");
			} else cv(INTTYPE);
			cl("$_ioeof"); res=INTTYPE;
			break;
	case EXPSYM:	cv(DOUBLETYPE);
			cl("$_exp");
			parm(1);
			break;
	case FIXSYM:	cv(DOUBLETYPE);
			cl("$_fix"); res=INTTYPE;
			parm(1);
			break;
	case INPSYM:
	case LPOSSYM:
	case FRESYM:	pop=0;
			warning("function not supported");
			parm(1);
			break;
	case HEXSYM:	cv(INTTYPE);
			cl("$_hex"); res=STRINGTYPE;
			parm(1);
			break;
	case OUTSYM:
	case INSTRSYM:	cv(DOUBLETYPE);
			cl("$_instr"); res=STRINGTYPE;
			parm(1);
			break;
	case INTSYM:	cv(DOUBLETYPE);
			cl("$_fcint");
			parm(1);
			break;
	case LEFTSYM:	parm(2);
			extraconvert(type, STRINGTYPE,typetable[1]);
			type= typetable[1];
			cv(INTTYPE);
			cl("$_left"); res=STRINGTYPE;
			emcode("asp",EMPTRSIZE);
			emcode("asp",EMINTSIZE);
			emcode("lfr",EMPTRSIZE);
			return(STRINGTYPE);
	case LENSYM:	cv(STRINGTYPE);
			cl("$_len"); res=INTTYPE;
			parm(1);
			break;
	case LOCSYM:	cv(INTTYPE);
			cl("$_loc"); res=INTTYPE;
			parm(1);
			break;
	case LOGSYM:	cv(DOUBLETYPE);
			cl("$_log");
			parm(1);
			break;
	case MKISYM:	cv(INTTYPE);
			cl("$_mki"); res=STRINGTYPE;
			parm(1);
			break;
	case MKSSYM:	cv(DOUBLETYPE);
			cl("$_mkd"); res=STRINGTYPE;
			parm(1);
			break;
	case MKDSYM:	cv(DOUBLETYPE);
			cl("$_mkd"); res=STRINGTYPE;
			parm(1);
			break;
	case OCTSYM:	cv(INTTYPE);
			cl("$_oct"); res=STRINGTYPE;
			parm(1);
			break;
	case PEEKSYM:	cv(INTTYPE);
			cl("$_peek"); res=INTTYPE;
			parm(1);
			break;
	case POSSYM:	emcode("asp",typestring(type));
			emcode("exa","_pos");
			emcode("loe","_pos");
			return(INTTYPE);
	case RIGHTSYM:	parm(2);
			extraconvert(type, STRINGTYPE,typetable[1]);
			type= typetable[1];
			cv(INTTYPE);
			cl("$_right"); res=STRINGTYPE;
			emcode("asp",EMINTSIZE);
			emcode("asp",EMPTRSIZE);
			emcode("lfr",EMPTRSIZE);
			return(STRINGTYPE);
	case RNDSYM:	if( cnt==1) pop=type; else pop=0;
			cl("$_rnd"); res= DOUBLETYPE;
			break;
	case SGNSYM:	cv(DOUBLETYPE);
			cl("$_sgn"); res=INTTYPE;
			parm(1);
			break;
	case SINSYM:	cv(DOUBLETYPE);
			cl("$_sin");
			parm(1);
			break;
	case SPACESYM:	cv(INTTYPE);
			cl("$_space"); res=STRINGTYPE;
			parm(1);
			break;
	case SPCSYM:	cv(INTTYPE);
			cl("$_spc"); res=0;
			parm(1);
			break;
	case SQRSYM:	cv(DOUBLETYPE);
			cl("$_sqt");
			parm(1);
			break;
	case STRSYM:	cv(DOUBLETYPE);
			cl("$_str");
			parm(1);
			break;
	case STRINGSYM:	cv(STRINGTYPE);
			cl("$_string"); res=STRINGTYPE;
			parm(1);
			break;
	case TABSYM:	cv(INTTYPE);
			cl("$_tab"); res=0;
			parm(1);
			break;
	case TANSYM:	cv(DOUBLETYPE);
			cl("$_tan");
			parm(1);
			break;
	case VALSYM:	cv(STRINGTYPE);
			cl("$atol"); res=INTTYPE;
			parm(1);
			break;
	case VARPTRSYM:	cv(DOUBLETYPE);
			cl("$_valptr");
			parm(1);
			break;
	default:	error("unknown function");
	}
	if(pop)
		emcode("asp",typestring(pop));
	if(res)
	emcode("lfr",typestring(res));
	return(res);
}

