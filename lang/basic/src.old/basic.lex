#ifndef NORSCID
static char rcs_lex[] = "$Header$" ;
#endif

/* This file contains the new lexical analizer */
typedef struct {
	char *name; 
	int token, classvalue,length;
} Key;

Key keywords [] ={
"abs",		FUNCTION,	ABSSYM,		0,
"and",		BOOLOP,		ANDSYM,		0,
"asc",		FUNCTION,	ASCSYM,		0,
"as",		 ASSYM,		0,	0,
"atn",		FUNCTION,	ATNSYM,		0,
"auto",		ILLEGAL,	0,	0,
"base",		BASESYM,	0,	0,
"call",		CALLSYM,	0,	0,
"cdbl",		FUNCTION,	CDBLSYM,	0,
"chain",	ILLEGAL,	0,	0,
"chr",		FUNCTION,	CHRSYM,		0,
"cint",		FUNCTION,	CINTSYM,	0,
"clear",	CLEARSYM,	0,	0,
"cload",	ILLEGAL,	0,	0,
"close",	ILLEGAL,	0,	0,
"common",	ILLEGAL,	0,	0,
"cont",		ILLEGAL,	0,	0,
"cos",		FUNCTION,	COSSYM,		0,
"csng",		FUNCTION,	CSNGSYM,	0,
"csave",	ILLEGAL,	0,	0,
"cvi",		FUNCTION,	CVISYM,		0,
"cvs",		FUNCTION,	CVSSYM,		0,
"cvd",		FUNCTION,	CVDSYM,		0,
"data",		DATASYM,	0,	0,
"defint",	DEFINTSYM,	0,	0,
"defsng",	DEFSNGSYM,	0,	0,
"defdbl",	DEFDBLSYM,	0,	0,
"defstr",	DEFSTRSYM,	0,	0,
"def",		DEFSYM,		0,	0,
"delete",	ILLEGAL,	0,	0,
"dim",		DIMSYM,		0,	0,
"edit",		ILLEGAL,	0,	0,
"else",		ELSESYM,	0,	0,
"end",		ENDSYM,		0,	0,
"eof",		FUNCTION,	EOFSYM,		0,
"erase",	ILLEGAL,	0,	0,
"error",	ERRORSYM,	0,	0,
"err",		ERRSYM,		0,	0,
"erl",		ERLSYM,		0,	0,
"else",		ELSESYM,	0,	0,
"eqv",		BOOLOP,		EQVSYM,	0,
"exp",		FUNCTION,	EXPSYM,		0,
"field",	FIELDSYM,	0,	0,
"fix",		FUNCTION,	FIXSYM,		0,
"for",		FORSYM,		0,	0,
"fre",		FUNCTION,	FRESYM,		0,
"get",		GETSYM,		0,	0,
"gosub",	GOSUBSYM,	0,	0,
"goto",		GOTOSYM,	0,	0,
"hex",		FUNCTION,	HEXSYM,		0,
"if",		IFSYM,		0,	0,
"imp",		BOOLOP,		IMPSYM,	0,
"inkey",	INKEYSYM,	0,	0,
"input",	INPUTSYM,	0,	0,
"inp",		FUNCTION,	INPSYM,		0,
"instr",	FUNCTION,	INSTRSYM,	0,
"int",		FUNCTION,	INTSYM,		0,
"kill",		ILLEGAL,	0,	0,
"left",		FUNCTION,	LEFTSYM,	0,
"len",		FUNCTION,	LENSYM,		0,
"let",		LETSYM,		0,	0,
"line",		LINESYM,	0,	0,
"list",		LISTSYM,	0,	0,
"llist",	ILLEGAL,	0,	0,
"load",		LOADSYM,	0,	0,
"loc",		FUNCTION,	LOCSYM,		0,
"log",		FUNCTION,	LOGSYM,		0,
"lpos",		FUNCTION,	LPOSSYM,	0,
"lprint",	ILLEGAL,	0,	0,
"lset",		LSETSYM,	0,	0,
"merge",	MERGESYM,	0,	0,
"mid",		MIDSYM,		0,		0,
"mki",		FUNCTION,	MKISYM,		0,
"mks",		FUNCTION,	MKSSYM,		0,
"mkd",		FUNCTION,	MKDSYM,		0,
"mod",		MODSYM,		0,	0,
"name",		ILLEGAL,	0,	0,
"new",		ILLEGAL,	0,	0,
"next",		NEXTSYM,	0,	0,
"not",			NOTSYM,	0,	0,
"null",		ILLEGAL,	0,	0,
"on",		ONSYM,		0,	0,
"oct",		FUNCTION,	OCTSYM,		0,
"open",		OPENSYM,	0,	0,
"option",	OPTIONSYM,	0,	0,
"or",		BOOLOP,		ORSYM,	0,
"out",		FUNCTION,	OUTSYM,	0,
"peek",		PEEKSYM,	0,	0,
"poke",		POKESYM,	0,	0,
"print",	PRINTSYM,	0,	0,
"pos",		FUNCTION,	POSSYM,		0,
"put",		PUTSYM,		0,	0,
"randomize",	RANDOMIZESYM,	0,	0,
"read",		READSYM,	0,	0,
"rem",		REMSYM,		0,	0,
"renum",	ILLEGAL,	0,	0,
"ren",		ILLEGAL,	0,	0,
"restore",	RESTORESYM,	0,	0,
"resume",	ILLEGAL,	0,	0,
"return",	RETURNSYM,	0,	0,
"right",	FUNCTION,	RIGHTSYM,	0,
"rnd",		FUNCTION,	RNDSYM,		0,
"run",		ILLEGAL,	0,	0,
"save",		ILLEGAL,	0,	0,
"step",		STEPSYM,	0,	0,
"sgn",		FUNCTION,	SGNSYM,		0,
"sin",		FUNCTION,	SINSYM,		0,
"space",	FUNCTION,	SPACESYM,	0,
"spc",		FUNCTION,	SPCSYM,		0,
"sqr",		FUNCTION,	SQRSYM,		0,
"stop",		STOPSYM,	0,	0,
"string",	FUNCTION,	STRINGSYM,	0,
"str",		FUNCTION,	STRSYM,		0,
"swap",		SWAPSYM,	0,	0,
"tab",		FUNCTION,	TABSYM,		0,
"tan",		FUNCTION,	TANSYM,		0,
"then",		THENSYM,	0,	0,
"to",		TOSYM,		0,		0,
"tron",		TRONOFFSYM,	TRONSYM,	0,
"troff",	TRONOFFSYM,	TROFFSYM,	0,
"using",	USINGSYM,	0,	0,
"usr",		FUNCTION,	USRSYM,		0,
"val",		FUNCTION,	VALSYM,		0,
"varptr",	FUNCTION,	VARPTRSYM,	0,
"wait",		ILLEGAL,	0,	0,
"while",	WHILESYM,	0,	0,
"wend",		WENDSYM,	0,	0,
"width",	ILLEGAL,	0,	0,
"write",	WRITESYM,	0,	0,
"xor",		BOOLOP,		XORSYM,	0,
0,	0,	0,	0
};

/* Keyword index table */

int	kex[27];

/* Initialize the keyword table */
fillkex()
{
	Key *k;
	int i;
	for(k=keywords;k->name;k++)
		k->length= strlen(k->name);
	k=keywords;
	for(i=0;k->name && i<='z'-'a';i++)
	{
		for(;k->name && *k->name<i+'a';k++);
		if( *k->name!=i+'a') continue;
		kex[*k->name-'a']=k-keywords;
		for(;k->name && *k->name==i+'a';k++);
		kex[*(k-1)->name-'a'+1]=k-keywords;
	}
	if(debug)
	{
		for(i=0;i<27;i++)
		printf("%c:%d\n",'a'+i,kex[i]);
	}
}

#include <ctype.h>

/* Get each line separately into the buffer */
/* Lines too long are terminated and flagged illegal */

#define MAXLINELENGTH	1024

char	inputline[MAXLINELENGTH];	/* current source line */
char	*cptr;				/* next character to decode */
int	yylineno=0;			/* source line counter */

getline()
{
	/* get next input line */

	if( fgets(inputline,MAXLINELENGTH,yyin) == NULL)
		return(FALSE);
	yylineno ++;
	if( index(inputline,'\n') == 0)
		error("source line too long");
	inputline[MAXLINELENGTH-1]=0;
	if( listing)
		fputs(inputline,stdout);
	cptr= inputline;
	return(TRUE);
}
yyerror(str)
char *str;
{
	error("Syntax error");
}

typechar()
{
	switch(*cptr)
	{
	case '$':
		cptr++; return( STRINGTYPE);
	case '%':
		cptr++; return( INTTYPE);
	case '!':
		cptr++; return( FLOATTYPE);
	case '#':
		cptr++; return( DOUBLETYPE);
	}
	return(0);
}

/* symbols in Microsoft are significant for the first 40 characters */
#define SIGNIFICANT 40
char name[SIGNIFICANT+1];

lookup()
{
	Key *k;
	Symbol *s;
	char *c;
	int i, typech;

	sval= name;
	for(c=cptr; *c && isalnum(*c);c++) 
	if( isupper(*c) )
		*c= tolower((*c));
	for(k= keywords+kex[*cptr-'a']; *(k->name)== *cptr;k++)
	if( strncmp(cptr,k->name,k->length)==0)
	{
		/* check functions first*/
		if( isalnum( *(cptr+k->length) ) &&
		    k->token==FUNCTION) continue;
		cptr += k->length;
		yylval= k->classvalue;
		if(debug) printf("lookup:%d %d\n",
				 k->classvalue,k->token);
		if( k->token == FUNCTION)
		{
			/* stripp type character */
			typech=typechar();
		}
			/* illegals + rem */
			if( k->token == REMSYM || k->token==ILLEGAL)
				while( *cptr && *cptr!=':' && *cptr!='\n')
					cptr++;
			return( k->token);
		}
	/* Is it  a function  name ? */
	c=cptr;
	/* Identifier found, update the symbol table */
	i=0;
	while( isalnum(*c) || *c == '.')
		if( i<SIGNIFICANT) name[i++]= *c++;
	name[i]=0;
	cptr=c;
	s= (Symbol *) srchsymbol(name);
	yylval = (YYSTYPE) s;
	typech= typechar();
	if(s->symtype!=DEFAULTTYPE) 
	{
		if(typech && typech!=s->symtype && wflag)
			warning("type re-declared,ignored");
	}
	if( typech)
		s->symtype=typech;
	if(debug) printf("lookup:%d Identifier\n",s);
	if( (name[0]=='f' || name[0]=='F') &&
	    (name[1]=='n' || name[1]=='N') )
		return(FUNCTID);
	return(IDENTIFIER);
}

/* Parsing unsigned numbers */
readconstant()
{
	/* read HEX and OCTAL numbers */
	char *c;
	cptr++;
	if( *cptr == 'H' || *cptr=='h')
	{
		/* HEX */
		cptr++;
		c=cptr;
		while(  isdigit(*cptr) || 
			(*cptr>='a' && *cptr<='f' ) ||
			(*cptr>='A' && *cptr<='F' ) )cptr++;
		sscanf(c,"%x",&ival);
	} else 
	if( *cptr == 'O' || *cptr == 'o')
	{
		/* OCTAL */
		cptr++;
		c=cptr;
		while( isdigit(*cptr) ) cptr++;
		sscanf(c,"%o",&ival);
	} else
	error("H or O expected");
	return(INTVALUE);
}

number()
{
	long	i1;
	double	f,dec;
	int	minflag;
	register char *c;

	i1=0;
	c=cptr;
	while(isdigit(*c)){
		i1= i1*10 + *c-'0';
		c++;
	}
	cptr=c;
	if( *c != '.'){
		if( i1> MAXINT || i1<MININT) {
			dval= i1;
			return(FLTVALUE);
		}
		ival= i1;
#ifdef YYDEBUG
		if(yydebug) printf("number:INTVALUE %d",i1);
#endif
		return(INTVALUE);
	}
	/* handle floats */
	f= i1; dec=0.1;
	c++;
	while( isdigit(*c)){
		f= f + dec * (*c - '0');
		dec /= 10.0;
		c++;
	}
	/* handle exponential part */
	if( *c =='e' || *c == 'E'){
		c++;
		minflag= (*c== '-')? -1: 1;
		if( *c=='-' || *c=='+') c++;
		while(isdigit(*c)){
			f *= 10.0;
			c++;
		}
		if(minflag== -1) f= 1.0/f;
	}
	dval= f;
	cptr=c;
#ifdef YYDEBUG
	if(yydebug) printf("number:FLTVALUE %f",f);
#endif
	return(FLTVALUE);
}
scanstring()
{
	int i,length;
	char firstchar;
	/* skip this string value, you might as well copy it to
	   the EM file as well, because it is not used internally
	*/
	/* generate label here */
	yylval= genrom();
	length=0;
	if( fputc('"',emfile) == EOF) fatal("scanstring");
	sval= cptr;
	firstchar = *cptr;
	if( *cptr== '"') cptr++;
	while( *cptr !='"')
	{
		switch(*cptr)
		{
		case 0:
		case '\n': 
#ifdef YYDEBUG
			if(yydebug) printf("STRVALUE\n");
#endif
			if( firstchar == '"')
				error("non-terminated string");
			return(STRVALUE);
		default:
			fputc(*cptr,emfile);
		}
		cptr++;
		length++;
	}
	*cptr=0;
	cptr++;
	fprintf(emfile,"\\000\"\n");
	i=yylval;
	yylval= genrom();
	fprintf(emfile,"l%d,1,%d\n",i,length);
#ifdef YYDEBUG
	if(yydebug) printf("STRVALUE found\n");
#endif
	return(STRVALUE);
}
yylex()
{
	char *c;

	/* Here is the big switch */
	c= cptr;
	switch(*c){
	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z': case 'A': case 'B': case 'C': case 'D':
	case 'E': case 'F': case 'G': case 'H': case 'I':
	case 'J': case 'K': case 'L': case 'M': case 'N':
	case 'O': case 'P': case 'Q': case 'R': case 'S':
	case 'T': case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z': case '_': 
		return(lookup());

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
		return(number());
	case '\'':
		/* comment at end of line */
		while( *cptr != '\n' && *cptr) cptr++;
	case '\n':
		cptr++;
		return(EOLN);
	case 0:
#ifdef YYDEBUG
		if( yydebug) printf("end of buffer");
#endif
		return(0);
	case '"':
		return(scanstring());
	/* handle double operators */
	case ' ':
	case '\t':
		cptr++;
		return(yylex());
	case '&':
		return(readconstant());
	case '?': return(PRINTSYM);
	case '>':
		if( *(c+1)=='='){
			c++;c++;cptr=c; yylval= GESYM;return(RELOP);
		}
		yylval= '>';
		cptr++;
		return(RELOP);
		break;
	case '<':
		if( *(c+1)=='='){
			c++; c++; cptr=c; yylval=LESYM; return(RELOP);
		} else
		if( *(c+1)=='>'){
			c++; c++; cptr=c; yylval=NESYM; return(RELOP);
		} 
		yylval= '<';
		cptr++;
		return(RELOP);
	}
	return(*cptr++);
}
