/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif


/* Here you find all routines to evaluate expressions and
   generate code for assignment statements
*/

exprtype(ltype,rtype)
int	ltype,rtype;
{
	/* determine the result type of an expression */
	if( ltype== STRINGTYPE || rtype==STRINGTYPE)
	{
		if( ltype!=rtype)
			error("type conflict, string expected");
		return( STRINGTYPE);
	}
	/* take maximum */
	if( ltype<rtype) return(rtype);
	return(ltype);
}

conversion(oldtype,newtype)
int oldtype,newtype;
{
	/* the value on top of the stack should be converted */
	if( oldtype==newtype) return;
	switch( oldtype)
	{
	case INTTYPE:
		if( newtype==FLOATTYPE || newtype==DOUBLETYPE)
		{
			emcode("loc",EMINTSIZE);
			emcode("loc",EMFLTSIZE);
			emcode("cif","");
		}else{
			if(debug) 
				printf("type n=%d o=%d\n",newtype,oldtype);
			error("conversion error");
		}
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		if( newtype==INTTYPE)
		{
			/* rounded ! */
			emcode("cal","$_cint");
			emcode("asp",EMFLTSIZE);
			emcode("lfr",EMINTSIZE);
			break;
		}else
		if( newtype== FLOATTYPE || newtype==DOUBLETYPE)
			break;
	default:
		if(debug) 
			printf("type n=%d o=%d\n",newtype,oldtype);
		error("conversion error");
	}
}
extraconvert(oldtype,newtype,topstack)
int oldtype,newtype,topstack;
{
	/* the value below the top of the stack should be converted */
	if( oldtype==newtype ) return;
	if( debug) printf("extra convert %d %d %d\n",oldtype,newtype,topstack);
	/* save top in dummy */
	switch( topstack)
	{
	case INTTYPE:
		emcode("ste","dummy1");
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		/* rounded ! */
		emcode("lae","dummy1");
		emcode("sti",EMFLTSIZE);
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
		emcode("loe","dummy1");
		break;
	case FLOATTYPE:
	case DOUBLETYPE:
		/* rounded ! */
		emcode("lae","dummy1");
		emcode("loi",EMFLTSIZE);
	}
}

boolop(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	if( operator != NOTSYM)
	{
		extraconvert(ltype,INTTYPE,rtype);
		conversion(rtype,INTTYPE);
	} else conversion(ltype,INTTYPE);
	switch( operator)
	{
	case NOTSYM:	emcode("com",EMINTSIZE); break;
	case ANDSYM:	emcode("and",EMINTSIZE); break;
	case ORSYM:	emcode("ior",EMINTSIZE); break;
	case XORSYM:	emcode("xor",EMINTSIZE); break;
	case EQVSYM:
		emcode("xor",EMINTSIZE);
		emcode("com",EMINTSIZE);
		break;
	case IMPSYM:
		/* implies */
		emcode("com",EMINTSIZE);
		emcode("and",EMINTSIZE);
		emcode("com",EMINTSIZE);
		break;
	default:	error("boolop:unexpected");
	}
	return(INTTYPE);
}
genbool(opcode)
char *opcode;
{
	int l1,l2;
	l1= genlabel();
	l2= genlabel();
	emcode(opcode,instrlabel(l1));
	emcode("loc",itoa(0));
	emcode("bra",instrlabel(l2));
	fprintf(Tmpfile,"%d\n",l1); emlinecount++;
	emcode("loc",itoa(-1));
	fprintf(Tmpfile,"%d\n",l2); emlinecount++;
}
relop( ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int	result;
	if(debug) printf("relop %d %d op=%d\n",ltype,rtype,operator);
	result= exprtype(ltype,rtype);
	extraconvert(ltype,result,rtype);
	conversion(rtype,result);
	/* compare the objects */
	if( result== INTTYPE)
		emcode("cmi", EMINTSIZE);
	else
	if( result==FLOATTYPE || result==DOUBLETYPE)
		emcode("cmf",EMFLTSIZE);
	else
	if( result==STRINGTYPE)
	{
		emcode("cal","$_strcomp");
		emcode("asp",EMPTRSIZE);
		emcode("asp",EMPTRSIZE);
		emcode("lfr",EMINTSIZE);
	} else	error("relop:unexpected");
	/* handle the relational operators */
	switch(operator)
	{
	case '<':	genbool("zlt"); break;
	case '>':	genbool("zgt"); break;
	case '=':	genbool("zeq"); break;
	case NESYM:	genbool("zne"); break;
	case LESYM:	genbool("zle"); break;
	case GESYM:	genbool("zge"); break;
	default:	error("relop:unexpected operator");
	}
	return(INTTYPE);
}
plusmin(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int result;
	result= exprtype(ltype,rtype);

	if( result== STRINGTYPE)
	{
		if( operator== '+')
		{
			emcode("cal","$_concat");
			emcode("asp",EMPTRSIZE);
			emcode("asp",EMPTRSIZE);
			emcode("lfr",EMPTRSIZE);
		} else error("illegal operator");
	} else {
		extraconvert(ltype,result,rtype);
		conversion(rtype,result);
		if( result== INTTYPE)
		{
			if( operator=='+') 
				emcode("adi",EMINTSIZE);
			else	emcode("sbi",EMINTSIZE);
		} else{
			if( operator=='+') 
				emcode("adf",EMFLTSIZE);
			else	emcode("sbf",EMFLTSIZE);
		}
	}
	return(result);
}
muldiv(ltype,rtype,operator)
int	ltype,rtype,operator;
{
	int result;

	result= exprtype(ltype,rtype);
	if(operator==MODSYM || operator== '\\') result=INTTYPE;
	extraconvert(ltype,result,rtype);
	conversion(rtype,result);
	if( result== INTTYPE)
	{
		if( operator=='/') 
		{
			result= DOUBLETYPE;
			extraconvert(ltype,result,rtype);
			conversion(rtype,result);
			emcode("dvf",EMFLTSIZE);
		} else
		if( operator=='\\')
			emcode("dvi",EMINTSIZE);
		else
		if( operator=='*') 
			emcode("mli",EMINTSIZE);
		else	
		if( operator==MODSYM)
			emcode("rmi",EMINTSIZE);
		else	error("illegal operator");
	} else{
		if( operator=='/') 
			emcode("dvf",EMFLTSIZE);
		else
		if( operator=='*') 
			emcode("mlf",EMFLTSIZE);
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
		emcode("ngi",EMINTSIZE); break;
	case DOUBLETYPE:
	case FLOATTYPE:
		emcode("ngf",EMFLTSIZE); break;
	default:
		error("Illegal operator");
	}
	return(type);
}
power(ltype,rtype)
int	ltype,rtype;
{
	extraconvert(ltype,DOUBLETYPE,rtype);
	conversion(rtype,DOUBLETYPE);
	emcode("cal","$_power");
	emcode("asp",EMFLTSIZE);
	emcode("asp",EMFLTSIZE);
	emcode("lfr",EMFLTSIZE);
	return(DOUBLETYPE);
}
char *typesize(ltype)
int ltype;
{
	switch( ltype)
	{
	case INTTYPE:
		return(EMINTSIZE);
	case FLOATTYPE:
	case DOUBLETYPE:
		return(EMFLTSIZE);
	case STRINGTYPE:
		return(EMPTRSIZE);
	default:
		error("typesize:unexpected");
		if(debug) printf("type received %d\n",ltype);
	}
	return(EMINTSIZE);
}
/*
loadptr(s)
Symbol *s;
{
	if( POINTERSIZE==WORDSIZE)
		fprintf(Tmpfile," loe l%d\n",s->symalias);
	else 
	if( POINTERSIZE== 2*WORDSIZE)
		fprintf(Tmpfile," lde l%d\n",s->symalias);
	else error("loadptr:unexpected pointersize");
}
*/
char *typestring(type)
int type;
{
	switch(type)
	{
	case INTTYPE:
		return(EMINTSIZE);
	case FLOATTYPE:
	case DOUBLETYPE:
		return(EMFLTSIZE);
	case STRINGTYPE:
		return(EMPTRSIZE);
	default:
		error("typestring: unexpected type");
	}
	return("0");
}
loadvar(type)
int type;
{
	/* load a simple variable  its address is on the stack*/
	emcode("loi",typestring(type));
}
loadint(value)
int value;
{
	emcode("loc",itoa(value));
	return(INTTYPE);
}
loaddbl(value)
double value;
{
	int index;
	index= genlabel();
	fprintf(emfile,"l%d\n bss 8,%fF8,1\n",index,value);
	emcode("lae",datalabel(index));
	emcode("loi",EMFLTSIZE);
	return(DOUBLETYPE);
}
loadstr(value)
int value;
{
	emcode("lae",datalabel(value));
}
loadaddr(s)
Symbol *s;
{
	extern Symbol *fcn;
	int i,j;

	if(debug) printf("load %s %d\n",s->symname,s->symtype);
	if( s->symalias>0)
		emcode("lae",datalabel(s->symalias));
	else{	
		j= -s->symalias;
		if(debug) printf("load parm %d\n",j);
		fprintf(Tmpfile," lal ");
		for(i=fcn->dimensions;i>j;i--)
			fprintf(Tmpfile,"%s+",typesize(fcn->dimlimit[i-1]));
		fprintf(Tmpfile,"0\n");
		emlinecount++;
		/*
		emcode("lal",datalabel(fcn->dimalias[-s->symalias]));
		*/
	}
	return(s->symtype);
}
assign(type,lt)
int type,lt;
{
	extern int e1,e2;
	conversion(lt,type);
	exchange(e1,e2);
	/* address is on stack already */
	emcode("sti",typestring(type) );
}
storevar(lab,type)
int lab,type;
{
	/*store value back */
	emcode("lae",datalabel(lab));
	emcode("sti",typestring(type));
}

/* maintain a stack of array references */
int	dimstk[MAXDIMENSIONS], dimtop= -1;
Symbol  *arraystk[MAXDIMENSIONS];

newarrayload(s)
Symbol *s;
{
	if( dimtop<MAXDIMENSIONS) dimtop++;
	if( s->dimensions==0)
	{
		s->dimensions=1;
		defarray(s);
	}
	dimstk[dimtop]= s->dimensions;
	arraystk[dimtop]= s;
	emcode("lae",datalabel(s->symalias));
}
endarrayload()
{
	return(arraystk[dimtop--]->symtype);
}
loadarray(type)
int	type;
{
	int	dim;
	Symbol	*s;

	if( dimtop<0 || dimtop>=MAXDIMENSIONS)
		fatal("too many nested array references");
	/* index expression is on top of stack */
	s=arraystk[dimtop];
	dim= dimstk[dimtop];
	if( dim==0)
	{
		error("too many indices");
		dimstk[dim--]=0;
		return;
	}
	conversion(type,INTTYPE);
	dim--;
	/* first check index range */
	fprintf(Tmpfile," lae r%d\n",s->dimalias[dim]);
	emlinecount++;
	emcode("rck",EMINTSIZE);
	emcode("lae",datalabel(s->dimalias[dim]));
	emcode("aar",EMINTSIZE);
	dimstk[dimtop]--;
}
storearray(type)
{
	/* used only in let statement */
	extern int e1,e2;
	exchange(e1,e2);
	emcode("sti",typestring(type));
}
