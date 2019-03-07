/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */


#ifndef NORCSID
static char rcs_lex[] = "$Id$" ;
#endif

/* This file contains the new lexical analizer */
typedef struct {
	char *name; 
	int token, classvalue,length;
} Key;

Key keywords [] ={
"abs",		FUNCTION,	ABSSYM,		0,
"and",		ANDSYM,		ANDSYM,		0,
"asc",		FUNCTION,	ASCSYM,		0,
"as",		 ASSYM,		0,		0,
"atn",		FUNCTION,	ATNSYM,		0,
"auto",		ILLEGAL,	0,		0,
"base",		BASESYM,	0,		0,
"call",		CALLSYM,	0,		0,
"cdbl",		FUNCTION,	CDBLSYM,	0,
"chain",	ILLEGAL,	0,		0,
"chr",		FUNCTION,	CHRSYM,		0,
"cint",		FUNCTION,	CINTSYM,	0,
"clear",	CLEARSYM,	0,		0,
"cload",	ILLEGAL,	0,		0,
"close",	CLOSESYM,	0,		0,
"common",	ILLEGAL,	0,		0,
"cont",		ILLEGAL,	0,		0,
"cos",		FUNCTION,	COSSYM,		0,
"csng",		FUNCTION,	CSNGSYM,	0,
"csave",	ILLEGAL,	0,		0,
"cvi",		FUNCTION,	CVISYM,		0,
"cvs",		FUNCTION,	CVSSYM,		0,
"cvd",		FUNCTION,	CVDSYM,		0,
"data",		DATASYM,	0,		0,
"defint",	DEFINTSYM,	0,		0,
"defsng",	DEFSNGSYM,	0,		0,
"defdbl",	DEFDBLSYM,	0,		0,
"defstr",	DEFSTRSYM,	0,		0,
"def",		DEFSYM,		0,		0,
"delete",	ILLEGAL,	0,		0,
"dim",		DIMSYM,		0,		0,
"edit",		ILLEGAL,	0,		0,
"else",		ELSESYM,	0,		0,
"end",		ENDSYM,		0,		0,
"eof",		FUNCTION,	EOFSYM,		0,
"eqv",		EQVSYM,		EQVSYM,		0,
"erase",	ILLEGAL,	0,		0,
"error",	ERRORSYM,	0,		0,
"err",		ERRSYM,		0,		0,
"erl",		ERLSYM,		0,		0,
"exp",		FUNCTION,	EXPSYM,		0,
"field",	FIELDSYM,	0,		0,
"fix",		FUNCTION,	FIXSYM,		0,
"for",		FORSYM,		0,		0,
"fre",		FUNCTION,	FRESYM,		0,
"get",		GETSYM,		0,		0,
"gosub",	GOSUBSYM,	0,		0,
"goto",		GOTOSYM,	0,		0,
"hex",		FUNCTION,	HEXSYM,		0,
"if",		IFSYM,		0,		0,
"imp",		IMPSYM,		IMPSYM,		0,
"inkey",	INKEYSYM,	0,		0,
"input",	INPUTSYM,	0,		0,
"inp",		FUNCTION,	INPSYM,		0,
"instr",	FUNCTION,	INSTRSYM,	0,
"int",		FUNCTION,	INTSYM,		0,
"kill",		ILLEGAL,	0,		0,
"left",		FUNCTION,	LEFTSYM,	0,
"len",		FUNCTION,	LENSYM,		0,
"let",		LETSYM,		0,		0,
"line",		LINESYM,	0,		0,
"list",		LISTSYM,	0,		0,
"llist",	ILLEGAL,	0,		0,
"load",		LOADSYM,	0,		0,
"loc",		FUNCTION,	LOCSYM,		0,
"log",		FUNCTION,	LOGSYM,		0,
"lpos",		FUNCTION,	LPOSSYM,	0,
"lprint",	ILLEGAL,	0,		0,
"lset",		LSETSYM,	0,		0,
"merge",	MERGESYM,	0,		0,
"mid",		MIDSYM,		0,		0,
"mki",		FUNCTION,	MKISYM,		0,
"mks",		FUNCTION,	MKSSYM,		0,
"mkd",		FUNCTION,	MKDSYM,		0,
"mod",		MODSYM,		0,		0,
"name",		ILLEGAL,	0,		0,
"new",		ILLEGAL,	0,		0,
"next",		NEXTSYM,	0,		0,
"not",		NOTSYM,		0,		0,
"null",		ILLEGAL,	0,		0,
"on",		ONSYM,		0,		0,
"oct",		FUNCTION,	OCTSYM,		0,
"open",		OPENSYM,	0,		0,
"option",	OPTIONSYM,	0,		0,
"or",		ORSYM,		ORSYM,		0,
"out",		FUNCTION,	OUTSYM,		0,
"peek",		PEEKSYM,	0,		0,
"poke",		POKESYM,	0,		0,
"print",	PRINTSYM,	0,		0,
"pos",		FUNCTION,	POSSYM,		0,
"put",		PUTSYM,		0,		0,
"randomize",	RANDOMIZESYM,	0,		0,
"read",		READSYM,	0,		0,
"rem",		REMSYM,		0,		0,
"renum",	ILLEGAL,	0,		0,
"ren",		ILLEGAL,	0,		0,
"restore",	RESTORESYM,	0,		0,
"resume",	ILLEGAL,	0,		0,
"return",	RETURNSYM,	0,		0,
"right",	FUNCTION,	RIGHTSYM,	0,
"rnd",		FUNCTION,	RNDSYM,		0,
"run",		ILLEGAL,	0,		0,
"save",		ILLEGAL,	0,		0,
"step",		STEPSYM,	0,		0,
"sgn",		FUNCTION,	SGNSYM,		0,
"sin",		FUNCTION,	SINSYM,		0,
"space",	FUNCTION,	SPACESYM,	0,
"spc",		FUNCTION,	SPCSYM,		0,
"sqr",		FUNCTION,	SQRSYM,		0,
"stop",		STOPSYM,	0,		0,
"string",	FUNCTION,	STRINGSYM,	0,
"str",		FUNCTION,	STRSYM,		0,
"swap",		SWAPSYM,	0,		0,
"tab",		FUNCTION,	TABSYM,		0,
"tan",		FUNCTION,	TANSYM,		0,
"then",		THENSYM,	0,		0,
"to",		TOSYM,		0,		0,
"tron",		TRONOFFSYM,	TRONSYM,	0,
"troff",	TRONOFFSYM,	TROFFSYM,	0,
"using",	USINGSYM,	0,		0,
"usr",		FUNCTION,	USRSYM,		0,
"val",		FUNCTION,	VALSYM,		0,
"varptr",	FUNCTION,	VARPTRSYM,	0,
"wait",		ILLEGAL,	0,		0,
"while",	WHILESYM,	0,		0,
"wend",		WENDSYM,	0,		0,
"width",	ILLEGAL,	0,		0,
"write",	WRITESYM,	0,		0,
"xor",		XORSYM,		XORSYM,		0,
0,		0,		0,		0
};

/* Keyword index table */

int	kex[27];

/* Initialize the keyword table */
void fillkex(void)
{
	Key *k;
	int i;
	for(k=keywords;k->name;k++)
		k->length= strlen(k->name);
	k=keywords;
	for(i=0;k->name && i<='z'-'a';i++)
	{
		for(;k->name && *k->name<i+'a';k++);
		if ( *k->name!=i+'a') continue;
		kex[*k->name-'a']=k-keywords;
		for(;k->name && *k->name==i+'a';k++);
		kex[*(k-1)->name-'a'+1]=k-keywords;
	}
	if (debug)
	{
		for(i=0;i<27;i++)
		print("%c:%d\n",'a'+i,kex[i]);
	}
}

#include <ctype.h>

/* Get each line separately into the buffer */
/* Lines too long are terminated and flagged illegal */

#define MAXLINELENGTH	1024

char	inputline[MAXLINELENGTH];	/* current source line */
char	*cptr;				/* next character to decode */
int	yylineno=0;			/* source line counter */

#define GETSBUFSIZE 1024

char fgets_buf[GETSBUFSIZE];



char *our_fgets(buffer,n_char,stream)
char *buffer;
int n_char;
File *stream;
{
    /* Read one line or n_char */
    static int characters_left = 0;
    static char *internal_bufp = fgets_buf;
    char *external_bufp;

    external_bufp = buffer;  /* Moves through the external buffer */
    while ( 1 ) {
        if ( characters_left ) { /* There is still something buffered */
            if ( n_char > 1 ) { /* More characters have to be copied  */
                if ( *internal_bufp == '\n' ) {
                    *external_bufp++ = *internal_bufp++;
                    characters_left--;
                    *external_bufp = '\0';
                    return(buffer); /* One line is read */
                } else {
                    *external_bufp++ = *internal_bufp++;
                    characters_left--;
                    n_char--;  /* One character is copied */
                }
            } else { /* Enough characters read */
                *external_bufp = '\0';
                return(buffer);
            }
        } else { /* Read new block */
            sys_read(stream,fgets_buf,GETSBUFSIZE,&characters_left);
            internal_bufp = fgets_buf;
                /* Move pointer  back to the beginning */
            if ( characters_left == 0 ) { /* Nothing read */
                if ( external_bufp == buffer ) {
                    *external_bufp = '\0';
                    return(0);  /* EOF */
                } else { /* Something was already copied */
                    *external_bufp = '\0';
                    return(buffer);
                }
            }
        }
    }
}

int getinputline(void)
{
	/* get next input line */

	if ( our_fgets(inputline,MAXLINELENGTH,yyin) == 0)
		return(FALSE);
	yylineno ++;
	if ( strchr(inputline,'\n') == 0)
		error("source line too long");
	inputline[MAXLINELENGTH-1]=0;
	if ( listing)
		fprint(STDERR, inputline);
	cptr= inputline;
	return(TRUE);
}





int typechar(void)
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


int lookup(void)
{
	Key *k;
	Symbol *Sym;
	char *c;
	int i, typech;

	sval= name;
	for(c=cptr; *c && isalnum(*c);c++) 
	if ( isupper(*c) )
		*c= tolower(*c);
	for (k= keywords+kex[*cptr-'a']; k->name != 0 && *(k->name)== *cptr;k++)
		if ( strncmp(cptr,k->name,k->length)==0)
		{
			/* if ( isalnum( *(cptr+k->length) )) *//* EHB */
			if ( isalnum( *(cptr+k->length) ) &&	/* EHB */
				k->token == FUNCTION)		/* EHB */
				continue; 
                                /* keywords door delimiters gescheiden */
			cptr += k->length;
			yylval.integer= k->classvalue;
			if (debug) print("lookup:%d %d\n",
					 k->classvalue,k->token);
			if ( k->token == FUNCTION)
			{
				/* stripp type character */
				typech=typechar();
			}
				/* illegals + rem */
				if ( k->token == REMSYM || k->token==ILLEGAL)
					while ( *cptr && *cptr!=':' && 
                                                *cptr!='\n')
						cptr++;
				return( k->token);
		}
	/* Is it  a function  name ? */
	c=cptr;
	/* Identifier found, update the symbol table */
	i=0;
	while (( isalnum(*c) || *c == '.') && i < SIGNIFICANT)
		name[i++]= *c++;
	while (isalnum(*c) || *c == '.') c++; /* skip rest */
	name[i]=0;
	cptr=c;
	Sym= srchsymbol(name);
	yylval.Sptr = Sym;
	typech= typechar();
	if (Sym->symtype!=DEFAULTTYPE) 
	{
		if (typech && typech!=Sym->symtype && wflag)
			warning("type re-declared,ignored");
	}
	if ( typech)
		Sym->symtype=typech;
	if (debug) print("lookup:%d Identifier\n",Sym);
	if ( (name[0]=='f' || name[0]=='F') &&
	     (name[1]=='n' || name[1]=='N') )
		return(FUNCTID);
	return(IDENTIFIER);
}



/* Parsing unsigned numbers */
int readconstant(void)
{
	/* read HEX and OCTAL numbers */
	char *c;
	cptr++;
	if ( *cptr == 'H' || *cptr=='h')
	{
		/* HEX */
		cptr++;
		c=cptr;
		while (  isdigit(*cptr) || 
			(*cptr>='a' && *cptr<='f' ) ||
			(*cptr>='A' && *cptr<='F' ) ) cptr++;
		(void) sscanf(c,"%x",&ival);
	} else 
	if ( *cptr == 'O' || *cptr == 'o')
	{
		/* OCTAL */
		cptr++;
		c=cptr;
		while ( isdigit(*cptr) ) cptr++;
		(void) sscanf(c,"%o",&ival);
	} else error("H or O expected");
	return(INTVALUE);
}



#ifdef ____
/* Computes base to the power exponent. This was not done in the old
   compiler                                                          */
double powr(double base,int exp)
{
	int i;
	double result;
	int abs_exp;

	if ( exp < 0 )
		abs_exp = -exp;
	else
		abs_exp = exp;
		
	result = 1.0;
	for ( i = 1; i <= abs_exp; i++ ) {
		result = result * base;
	}

	if ( exp < 0 )
		return ( 1.0 / result );
	else
		return ( result );
}
#endif


int number(void)
{
	long	i1;
	int overflow = 0;
	register char *c;
	static char	numbuf[256];
	register char *d = numbuf;

	dval = numbuf;
	i1=0;
	c=cptr;
	while (*c == '0') c++;
	while (isdigit(*c)){
		i1= i1*10 + *c-'0';
		if (i1 < 0) overflow = 1;
		if (d < &numbuf[255]) *d++ = *c;
		c++;
	}
	if (d == numbuf) *d++ = '0';
	cptr=c;
	if ( *c != '.'  && *c != 'e' && *c != 'E'
			&& *c != 'd' && *c != 'D' ){
		if ( i1> MAXINT || i1<MININT || overflow) {
			*d = 0;
			return(FLTVALUE);
		}
		/*NOSTRICT*/ ival= i1;
#ifdef YYDEBUG
		if (yydebug) print("number:INTVALUE %d",i1);
#endif
		return(INTVALUE);
	}
	/* handle floats */
	if (*c == '.') {
		if (d < &numbuf[255]) *d++ = *c;
		c++;
		while ( isdigit(*c)){
			if (d < &numbuf[255]) *d++ = *c;
			c++;
		}
	}
	/* handle exponential part */
	if ( *c == 'e' || *c == 'E' || *c == 'd' || *c == 'D' ){
		if (d < &numbuf[254]) *d++ = 'e';
		c++;
		if ( *c=='-' || *c=='+') {
			if (d < &numbuf[255]) *d++ = *c;
			c++;
		}
		while (isdigit(*c)){
			if (d < &numbuf[255]) *d++ = *c;
			c++;
		}
		if (*(d-1) == 'e') *d++ = '0';
	}
	*d = 0;
	cptr=c;
#ifdef YYDEBUG
	if (yydebug) print("number:FLTVALUE %s",dval);
#endif
	return(FLTVALUE);
}



/* Maximale grootte van een chunk; >= 4 */
#define CHUNKSIZE 123



int scanstring(void)
{
	int i,length=0;
	char firstchar = *cptr;
	char buffer[CHUNKSIZE],*bufp = buffer;

	/* generate label here */
	if (! in_data) yylval.integer= genemlabel();
	if ( *cptr== '"') cptr++;
	sval= cptr;
	while ( *cptr !='"')
	{
		switch(*cptr)
		{
		case 0:
		case '\n': 
#ifdef YYDEBUG
			if (yydebug) print("STRVALUE\n");
#endif
			if ( firstchar == '"')
				error("non-terminated string");
			return(STRVALUE);
		/*
		case '\'':
		case '\\':
			*bufp++ = '\\';
			*bufp++ = *cptr;
			if ( bufp >= buffer + CHUNKSIZE - 4 ) {
				if (! in_data) 
					C_con_scon(buffer,(arith)(bufp-buffer));
				bufp = buffer;
			}
			break;
		*/
		default:
			*bufp++ = *cptr;
			if ( bufp >= buffer + CHUNKSIZE - 4 ) {
				if (! in_data) 
					C_con_scon(buffer,(arith)(bufp-buffer));
				bufp = buffer;
			}
		}
		cptr++;
		length++;
	}
	*cptr = 0;
	*bufp++ = 0;
	cptr++;
	if (! in_data) {
		C_con_scon(buffer,(arith)(bufp-buffer));
		i=yylval.integer;
		yylval.integer= genemlabel();
		C_rom_dlb((label)i,(arith)0);
		C_rom_icon("9999",(arith)BEMINTSIZE);
		C_rom_icon(myitoa(length),(arith)BEMINTSIZE);
	}
#ifdef YYDEBUG
	if (yydebug) print("STRVALUE found\n");
#endif
	return(STRVALUE);
}



int yylex(void)
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
			while ( *cptr != '\n' && *cptr) cptr++;
		case '\n':
			cptr++;
			return(EOLN);
		case 0:
#ifdef YYDEBUG
			if ( yydebug) print("end of buffer");
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
		case '?':
			cptr++;
			return(PRINTSYM);
		case '>':
			if ( *(c+1)=='='){
				c++; c++;
				cptr=c;
				yylval.integer= GESYM;
				return(RELOP);
			}
			yylval.integer= '>';
			cptr++;
			return(RELOP);
		case '<':
			if ( *(c+1)=='='){
				c++; c++; 
				cptr=c; 
				yylval.integer=LESYM; 
				return(RELOP);
			} else
				if ( *(c+1)=='>'){
					c++; c++; 
					cptr=c; 
					yylval.integer=NESYM; 
					return(RELOP);
				} 
			yylval.integer= '<';
			cptr++;
			return(RELOP);
	}
	return(*cptr++);
}
