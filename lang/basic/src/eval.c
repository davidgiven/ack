/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Id$" ;
#endif


/* Here you find all routines to evaluate expressions and
   generate code for assignment statements
*/

exprtype(ltype,rtype)
int	ltype,rtype;
{
	/* determine the result type of an expression */
	if ( ltype==STRINGTYPE || rtype==STRINGTYPE)
	{
		if ( ltype!=rtype)
			error("type conflict, string expected");
		return( STRINGTYPE);
	}
	/* take maximum */
	if ( ltype<rtype) return(rtype);
	return(ltype);
}



void
conversion(oldtype,newtype)
int oldtype,newtype;
{
	/* the value on top of the stack should be converted */
	if ( oldtype==newtype) return;

	switch( oldtype)
	{
	case INTTYPE:
		if ( newtype==FLOATTYPE || newtype==DOUBLETYPE)
		{
			C_loc((arith)BEMINTSIZE);
			C_loc((arith)BEMFLTSIZE);
			C_cif ();
		} else {
			if (debug) 
				print("type n=%d o=%d\n",newtype,oldtype);
			error("conversion error");
		}
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		if ( newtype==INTTYPE)
		{
			/* rounded ! */
			C_cal("_cint");
			C_asp((arith)BEMFLTSIZE);
			C_lfr((arith)BEMINTSIZE);
			break;
		} else if ( newtype==FLOATTYPE || newtype==DOUBLETYPE)
			break;
	default:
		if (debug) 
			print("type n=%d o=%d\n",newtype,oldtype);
		error("conversion error");
	}
}



void
extraconvert(oldtype,newtype,topstack)
int oldtype,newtype,topstack;
{
	/* the value below the top of the stack should be converted */
	if ( oldtype==newtype ) return;
	if ( debug) print("extra convert %d %d %d\n",oldtype,newtype,topstack);
	/* save top in dummy */

	switch( topstack)
	{
	case INTTYPE:
		C_ste_dnam("dummy1",(arith)0);
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		/* rounded ! */
		C_lae_dnam("dummy1",(arith)0);
		C_sti((arith)BEMFLTSIZE);
		break;
	default:
		error("conversion error");
		return;
	}
	/* now its on top of the stack */

	conversion(oldtype,newtype);
	/* restore top */

	switch( topstack)
	{
	case INTTYPE:
		C_loe_dnam("dummy1",(arith)0);
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		/* rounded ! */
		C_lae_dnam("dummy1",(arith)0);
		C_loi((arith)BEMFLTSIZE);
	}
}

	

boolop(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	if ( operator != NOTSYM)
	{
		extraconvert(ltype,INTTYPE,rtype);
		conversion(rtype,INTTYPE);
	} else conversion(ltype,INTTYPE);

	switch( operator)
	{
	case NOTSYM:
		C_com((arith)BEMINTSIZE);
		break;
	case ANDSYM:
		C_and((arith)BEMINTSIZE);
		break;
	case ORSYM:
		C_ior((arith)BEMINTSIZE);
		break;
	case XORSYM:
		C_xor((arith)BEMINTSIZE);
		break;
	case EQVSYM:
		C_xor((arith)BEMINTSIZE);
		C_com((arith)BEMINTSIZE);
		break;
	case IMPSYM:
		/* implies */
		C_com((arith)BEMINTSIZE);
		C_and((arith)BEMINTSIZE);
		C_com((arith)BEMINTSIZE);
		break;
	default:	
		error("boolop:unexpected");
	}

	return(INTTYPE);
}



genbool(operator)
int operator;
{
	int l1,l2;

	l1= genlabel();
	l2= genlabel();

	switch(operator)
	{
		case '<':	C_zlt((label)l1); break;
		case '>':	C_zgt((label)l1); break;
		case '=':	C_zeq((label)l1); break;
		case NESYM:	C_zne((label)l1); break;
		case LESYM:	C_zle((label)l1); break;
		case GESYM:	C_zge((label)l1); break;
		default:	error("relop:unexpected operator");
	}

	C_loc((arith)0);
	C_bra((label)l2);
	C_df_ilb((label)l1);
	C_loc((arith)-1);
	C_df_ilb((label)l2);
}



relop( ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int	result;

	if (debug) print("relop %d %d op=%d\n",ltype,rtype,operator);
	result= exprtype(ltype,rtype);
	extraconvert(ltype,result,rtype);
	conversion(rtype,result);
	/* compare the objects */
	if ( result==INTTYPE)
		C_cmi((arith)BEMINTSIZE);
	else if ( result==FLOATTYPE || result==DOUBLETYPE)
		  C_cmf((arith)BEMFLTSIZE);
	     else if ( result==STRINGTYPE)
		  {
			  C_cal("_strcomp");
			  C_asp((arith)(2*BEMPTRSIZE));
                	  C_lfr((arith)BEMINTSIZE);
		  } else error("relop:unexpected");
	/* handle the relational operators */
	genbool(operator);
	return(INTTYPE);
}



plusmin(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int result;

	result= exprtype(ltype,rtype);
	if ( result== STRINGTYPE)
	{
		if ( operator== '+')
		{
                        C_cal("_concat");
                        C_asp((arith)(2*BEMPTRSIZE));
                        C_lfr((arith)BEMPTRSIZE);
		} else error("illegal operator");
	} else {
		extraconvert(ltype,result,rtype);
		conversion(rtype,result);
		if ( result== INTTYPE)
		{
			if ( operator=='+') 
                                C_adi((arith)BEMINTSIZE);
			else C_sbi((arith)BEMINTSIZE);
		} else {
			if ( operator=='+') 
                                C_adf((arith)BEMFLTSIZE);
			else C_sbf((arith)BEMFLTSIZE);
		}
	}
	return(result);
}



muldiv(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int result;

	result=exprtype(ltype,rtype);
	if (operator==MODSYM || operator== '\\') result=INTTYPE;
	extraconvert(ltype,result,rtype);
	conversion(rtype,result);
	if ( result== INTTYPE)
	{
		if ( operator=='/') 
		{
			result=DOUBLETYPE;
			extraconvert(ltype,result,rtype);
			conversion(rtype,result);
                        C_dvf((arith)BEMFLTSIZE);
		} else
		if ( operator=='\\')
                        C_dvi((arith)BEMINTSIZE);
		else
		if ( operator=='*') 
                        C_mli((arith)BEMINTSIZE);
		else	
		if ( operator==MODSYM)
                        C_rmi((arith)BEMINTSIZE);
		else	error("illegal operator");
	} else {
		if ( operator=='/') 
                        C_dvf((arith)BEMFLTSIZE);
		else
		if ( operator=='*') 
                        C_mlf((arith)BEMFLTSIZE);
		else	error("illegal operator");
	}
	return(result);
}



negate(type)
int type;
{
	switch(type)
	{
		case INTTYPE:
                	C_ngi((arith)BEMINTSIZE); 
			break;
		case DOUBLETYPE:
		case FLOATTYPE:
                	C_ngf((arith)BEMFLTSIZE); 
			break;
		default:
			error("Illegal operator");
	}
	return(type);
}



#ifdef ___
power(ltype,rtype)
int	ltype,rtype;
{
	int resulttype = exprtype(ltype, rtype);

	extraconvert(ltype,resulttype,rtype);
	conversion(rtype,resulttype);
	switch(resulttype) {
	case INTTYPE:
		C_cal("_ipower");
		break;
	case DOUBLETYPE:
	case FLOATTYPE:
        	C_cal("_power");
		break;
	default:
		error("Illegal operator");
	}
        C_asp((arith)(2*typestring(resulttype)));
        C_lfr((arith)typestring(resulttype));
	return(resulttype);
}
#else
power(ltype,rtype)
int	ltype,rtype;
{
	extraconvert(ltype,DOUBLETYPE,rtype);
	conversion(rtype,DOUBLETYPE);
        C_cal("_power");
        C_asp((arith)(2*BEMFLTSIZE));
        C_lfr((arith)BEMFLTSIZE);
	return(DOUBLETYPE);
}
#endif


int typesize(ltype)
int ltype;
{
	switch( ltype)
	{
	case INTTYPE:
		return(BEMINTSIZE);
	case FLOATTYPE:
	case DOUBLETYPE:
		return(BEMFLTSIZE);
	case STRINGTYPE:
		return(BEMPTRSIZE);
	default:
		error("typesize:unexpected");
		if (debug) print("type received %d\n",ltype);
	}
	return(BEMINTSIZE);
}



int typestring(type)
int type;
{
	switch(type)
	{
		case INTTYPE:
			return(BEMINTSIZE);
		case FLOATTYPE:
		case DOUBLETYPE:
			return(BEMFLTSIZE);
		case STRINGTYPE:
			return(BEMPTRSIZE);
		default:
			error("typestring: unexpected type");
	}
	return(0);
}



loadvar(type)
int type;
{
	/* load a simple variable  its address is on the stack*/
        C_loi((arith)typestring(type));
}



loadint(value)
int value;
{
        C_loc((arith)value);
	return(INTTYPE);
}



loaddbl(value)
char *value;
{
	int index;

	index=genlabel();
	C_df_dlb((label)index);
	C_bss_fcon((arith)BEMFLTSIZE,value,(arith)BEMFLTSIZE,1);
	C_lae_dlb((label)index,(arith)0);
	C_loi((arith)BEMFLTSIZE);
	return(DOUBLETYPE);
}



loadstr(value)
int value;
{
	C_lae_dlb((label)value,(arith)0);
}



loadaddr(s)
Symbol *s;
{
	extern Symbol *fcn;
	int i,j;
	arith sum;

	if (debug) print("load %s %d\n",s->symname,s->symtype);
	if ( s->symalias>0)
		C_lae_dlb((label)s->symalias,(arith)0);
	else {	
		j= -s->symalias;
		if (debug) print("load parm %d\n",j);
		/* first count the sizes. */
		sum = 0;
		for(i=fcn->dimensions;i>j;i--)
			sum += typesize(fcn->dimlimit[i-1]);
		C_lal(sum);
	}
	return(s->symtype);
}



/* This is a new routine */
save_address()
{
	C_lae_dnam("dummy3",(arith)0);
	C_sti((arith)BEMPTRSIZE);
}



assign(type,lt)
int type,lt;
{
	extern int e1,e2;

	conversion(lt,type);
	C_lae_dnam("dummy3",(arith)0); /* Statement added by us */
	C_loi((arith)BEMPTRSIZE);
	/* address is on stack already */
	C_sti((arith)typestring(type));
}



storevar(lab,type)
int lab,type;
{
	/*store value back */
	C_lae_dlb((label)lab,(arith)0);
	C_sti((arith)typestring(type));
}



/* maintain a stack of array references */
int	dimstk[MAXDIMENSIONS], dimtop= -1;
Symbol  *arraystk[MAXDIMENSIONS];



newarrayload(s)
Symbol *s;
{
	if ( dimtop<MAXDIMENSIONS) dimtop++;
	if ( s->dimensions==0)
	{
		s->dimensions=1;
		defarray(s);
	}
	dimstk[dimtop]= 0;
	arraystk[dimtop]= s;
	C_lae_dlb((label)s->symalias,(arith)0);
}



endarrayload()
{
	return(arraystk[dimtop--]->symtype);
}



void
loadarray(type)
int	type;
{
	int	dim;
	Symbol	*s;

	if ( dimtop<0 || dimtop>=MAXDIMENSIONS)
		fatal("too many nested array references");
	/* index expression is on top of stack */
	s=arraystk[dimtop];
	dim= dimstk[dimtop];
	if ( dim>=s->dimensions)
	{
		error("too many indices");
		dimstk[dimtop]=0;
		return;
	}
	conversion(type,INTTYPE);
	C_lae_dlb((label)s->dimalias[dim],(arith)0);
	C_aar((arith)BEMINTSIZE);
	dimstk[dimtop]++;
}



