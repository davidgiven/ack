%token ILLEGAL
%token ASSYM
%token BASESYM
%token CALLSYM
%token CLEARSYM
%token CLOSESYM
%token DATASYM
%token DEFINTSYM
%token DEFSNGSYM
%token DEFDBLSYM
%token DEFSTRSYM
%token DEFSYM
%token DIMSYM
%token ELSESYM
%token ERRSYM
%token ERLSYM
%token ERRORSYM
%token ELSESYM
%token FIELDSYM
%token FORSYM
%token <integer> FUNCTION
%token <Sptr> FUNCTID
%token INKEYSYM
%token GETSYM
%token GOSUBSYM
%token GOTOSYM
%token <integer> IFSYM
%token INPUTSYM
%token LETSYM
%token LINESYM
%token LSETSYM
%token MIDSYM
%token NEXTSYM
%token ONSYM
%token OPENSYM
%token OPTIONSYM
%token PRINTSYM
%token POKESYM
%token PUTSYM
%token RANDOMIZESYM
%token READSYM
%token REMSYM
%token RESTORESYM
%token RETURNSYM
%token ENDSYM
%token STOPSYM
%token STEPSYM
%token SWAPSYM
%token THENSYM
%token TOSYM
%token <integer> TRONOFFSYM
%token USINGSYM
%token USRSYM
%token WHILESYM
%token WENDSYM
%token WRITESYM
/* special tokens */
%token EOLN
%token INTVALUE
%token FLTVALUE
%token DBLVALUE
%token <integer> STRVALUE
%token UNARYSYM
%token <Sptr> IDENTIFIER
%token ANDSYM
%token ORSYM
%token VARPTR

%type <Sptr> arraydcl identifier indexed
%type <cptr> getput
%type <integer> exprlist expression negation compare sum term factor
%type <integer> parmlist variable printlist inputtail funcname funccall

%left <integer> BOOLOP
%left NOTSYM
%left '=' '<' '>' LESYM GESYM NESYM
%left <integer> RELOP
%left '+' '-'
%left '*' '/' '\\' MODSYM
%left '^'
%left UNARYMINUS

%{
#define YYDEBUG
#include "bem.h"

typedef union {
	int	integer ;
	Symbol	*Sptr ;
	char	*cptr ;
} YYSTYPE ;

int	ival;
double  dval;
char	*sval;
int 	e1,e2;

char	*formatstring;	/* formatstring used for printing */
Symbol	*s;		/* Symbol dummy */
%}
%%
programline	: INTVALUE {newblock(ival); newemblock(ival);} stmts EOLN
		| '#' INTVALUE STRVALUE EOLN
		| EOLN
		;


stmts	: singlestmt
	| stmts ':' singlestmt
	;

singlestmt : callstmt
	| clearstmt
	| closestmt
	| datastmt
	| deffnstmt
	| defvarstmt
	| defusrstmt
	| dimstmt		
	| ERRORSYM expression		{errorstmt($2);}
	| fieldstmt
	| forstmt
	| getstmt
	| gosubstmt
	| ongotostmt
	| ifstmt
	| illegalstmt
	| inputstmt
	| letstmt
	| lineinputstmt
	| lsetstmt
	| midstmt
	| exceptionstmt
	| nextstmt
	| GOTOSYM INTVALUE			{gotostmt(ival);}
	| openstmt
	| optionstmt
	| pokestmt
	| printstmt
	| randomizestmt
	| readstmt
	| REMSYM		
	| restorestmt
	| returnstmt
	| ENDSYM		{ emcode("loc","0");
				  emcode("cal","$_hlt");
				  emcode("asp",EMINTSIZE);}
	| STOPSYM		{ emcode("cal","$_stop");}
	| swapstmt
	| TRONOFFSYM		{ tronoff=$1;}
	| whilestmt
	| wendstmt
	| writestmt
	| /* EMPTY STATEMENT */
	;

illegalstmt:	ILLEGAL 		{illegalcmd();}

callstmt:	CALLSYM	IDENTIFIER parmlist ')'
	{ 
		emcode("cal",proclabel($2->symname));
		while($3 -- >0) emcode("asp",EMPTRSIZE);
	}
	|	CALLSYM	IDENTIFIER 
	{ 	emcode("cal",proclabel($2->symname));}

parmlist: '(' variable	{ $$=1;}
	| parmlist ',' variable	{ $$= $1+1;}

clearstmt:	CLEARSYM			{warning("statement ignored");}
	|	CLEARSYM ',' expression	{warning("statement ignored");}
	|	CLEARSYM ',' expression ',' expression	{warning("statement ignored");}
closestmt:	CLOSESYM filelist		
	|	CLOSESYM	{emcode("cal","$_close");}

filelist:	cross intvalue			{ emcode("loc",itoa(ival));
						 emcode("cal","$_clochn");
						 emcode("asp",EMINTSIZE);}
	|	filelist ',' cross intvalue 	{ emcode("loc",itoa(ival));
						 emcode("cal","$_clochn");
						 emcode("asp",EMINTSIZE);}

datastmt:	DATASYM  {datastmt();} datalist	{fprintf(datfile,"\n");}

dataelm : INTVALUE	{fprintf(datfile,"%d",ival);}
	| '-' INTVALUE	{fprintf(datfile,"%d",-ival);}
	| FLTVALUE	{fprintf(datfile,"%f",dval);}
	| '-' FLTVALUE	{fprintf(datfile,"%f",-dval);}
	| STRVALUE	{fprintf(datfile,"\"%s\"",sval);}
	| IDENTIFIER	{fprintf(datfile,"\"%s\"",sval);}
	;

datalist: dataelm
	| datalist ',' {fputc(',',datfile);} dataelm
	;

deffnstmt:	DEFSYM heading '=' expression {endscope($4);}

heading : FUNCTID			{ newscope($1); heading();}
	| FUNCTID {newscope($1);} '(' idlist ')'	{ heading();}

idlist : IDENTIFIER		{ dclparm($1);}
	| idlist ',' IDENTIFIER	{ dclparm($3);}
	;

defvarstmt: 	DEFINTSYM 			{ setdefaulttype( INTTYPE);}
	|	DEFSNGSYM 			{ setdefaulttype( FLOATTYPE);}
	|	DEFDBLSYM 			{ setdefaulttype( DOUBLETYPE);}
	|	DEFSTRSYM 			{ setdefaulttype( STRINGTYPE);}

defusrstmt:	DEFSYM USRSYM error ':'		{illegalcmd();}

dimstmt:	DIMSYM arraydcl ')'		{dclarray($2);}
	|	dimstmt ',' arraydcl ')'	{dclarray($3);}
	;

arraydcl : IDENTIFIER '(' INTVALUE	{$$=$1; s= $1;
					 s->dimlimit[s->dimensions]=ival;
					 s->dimensions++;
					}
	| arraydcl ',' INTVALUE		{$$=$1; s= $1;
					 if(s->dimensions<MAXDIMENSIONS)
					 {
						 s->dimlimit[s->dimensions]=ival;
						 s->dimensions++;
					} else
						error("too many dimensions");
					}



fieldstmt:	FIELDSYM cross intvalue {setchannel(ival);} ',' fieldlist	{notyetimpl();}

fieldlist:	intvalue ASSYM variable
	| fieldlist ',' intvalue ASSYM variable
	;

forstmt: FORSYM IDENTIFIER {forinit($2);} '=' expression  {forexpr($5);}
	TOSYM expression {forlimit($8);} step
	;

step	: STEPSYM expression		{forstep($2);}
	| /*EMPTY*/			{emcode("loc","1"); forstep(INTTYPE);}
	;

nextstmt: NEXTSYM IDENTIFIER			{nextstmt($2);}
	| NEXTSYM				{ nextstmt((Symbol *)0);}
	| nextstmt ',' IDENTIFIER		{ nextstmt($3);}

getstmt:	getput  	{emcode("loc",itoa(0));
				 emcode("cal",$1);
				 emcode("asp",EMINTSIZE);}
	|	getput ',' intvalue 
				{ /* position seek pointer first*/
				  emcode("loc",itoa(ival));
				  emcode("cal",$1);
				  emcode("asp",EMINTSIZE);
				}
getput: GETSYM	cross intvalue { setchannel(ival); $$= "$_getrec";}
	| PUTSYM cross intvalue { setchannel(ival); $$= "$_putsym";}

gosubstmt:	GOSUBSYM INTVALUE		{gosubstmt(ival);}

returnstmt:	RETURNSYM			{returnstmt();}

ifstmt:		IFSYM expression {$1=ifstmt($2);} thenpart 
		{$1=thenpart($1);} elsepart {elsepart($1);}
	;

thenpart:	THENSYM INTVALUE		{gotostmt(ival);}
	|	THENSYM stmts
	|	GOTOSYM INTVALUE		{gotostmt(ival);}
	;
elsepart:	ELSESYM INTVALUE 		{gotostmt(ival);}
	|	ELSESYM stmts 
	|	/* empty */
	;

inputstmt:	INPUTSYM  semiprompt  readlist 
	|	INPUTSYM  '#' intvalue {setchannel(ival);}',' readlist
	;

semiprompt : semi STRVALUE ';' 	{ loadstr($2); prompt(1);}
	| semi STRVALUE ','		{ loadstr($2); prompt(0);}
	| /*EMPTY*/			{ setchannel(-1);
					  emcode("cal","$_qstmark");}

semi	: ';'	| /* empty */ ;

letstmt:	LETSYM {e1=where();} variable {e2=where();}
		'=' expression	{assign($3,$6);}
	|	{e1=where();} variable  {e2=where();}
		'=' expression		{assign($2,$5);}

lineinputstmt: LINESYM INPUTSYM semiprompt {setchannel(-1);} variable {linestmt($5);} 
	|	LINESYM '#' intvalue {setchannel(ival);} ',' variable {linestmt($6);}
	;

readlist: readelm		
	| readlist ',' readelm	
	;
readelm:	variable	{readelm($1);}

lsetstmt:	LSETSYM variable '=' expression	{notyetimpl();}

midstmt:	MIDSYM '$'  midparms '=' expression 
	{ 	emcode("cal","$_midstmt");
		emcode("asp",EMINTSIZE);
		emcode("asp",EMINTSIZE);
		emcode("asp",EMPTRSIZE);
		emcode("asp",EMPTRSIZE);}

midparms:	'(' midfirst midsec midthird ')' 

midfirst:	expression 	{ conversion($1,STRINGTYPE); }
midsec:		',' expression 	{ conversion($2,INTTYPE); }
midthird:	',' expression 	{ conversion($2,INTTYPE); }
		| /* empty */ 	{ emcode("loc","-1");}

exceptionstmt:	ONSYM ERRORSYM GOTOSYM INTVALUE	{exceptstmt(ival);}

ongotostmt:	ONSYM expression 
		GOSUBSYM constantlist {ongosubstmt($2);}
	| 	ONSYM expression 
		GOTOSYM constantlist  {ongotostmt($2);}

constantlist: INTVALUE			{jumpelm(ival);}
	| constantlist ',' INTVALUE	{ jumpelm(ival);}

openstmt:	OPENSYM mode openchannel expression 
		{ conversion($4,STRINGTYPE); openstmt(0);}
	|	OPENSYM mode openchannel 
		expression {conversion($4,STRINGTYPE);} 
		INTVALUE { openstmt(ival);}

openchannel: cross INTVALUE ','	{ setchannel(ival);}

mode	: expression ',' 	{conversion($1,STRINGTYPE);}
	| ','			{ emcode("lae","_iomode");}
	;

optionstmt:	OPTIONSYM BASESYM intvalue { optionbase(ival);}

printstmt:	PRINTSYM 		{setchannel(-1);emcode("cal","$_nl");}
	| 	PRINTSYM file format printlist 
	{ 	if( $4) emcode("cal","$_nl");}
file	: '#' intvalue ','	{setchannel(ival);}
	| /* empty */		{setchannel(-1);}
	;
format  : USINGSYM STRVALUE ';'		{ loadstr($2);}
	| USINGSYM variable ';'		{ 
		if($2!=STRINGTYPE) error("string variable expected");}
	| /* empty */				{formatstring=0;}

printlist: expression			{ printstmt($1); $$=1;}
	| ','				{ zone(0); $$=0;}
	| ';'				{ zone(1); $$=0;}
	| printlist expression		{ printstmt($2); $$=1;}
	| printlist ','			{ zone(1);$$=0;}
	| printlist ';'			{ zone(0);$$=0;}
	;
pokestmt: POKESYM expression ',' expression	{pokestmt($2,$4);}
	;
randomizestmt:	RANDOMIZESYM 
			{ emcode("cal","$_randomize");}
	| 	RANDOMIZESYM expression
			 { conversion($2,INTTYPE);
			  emcode("cal","$_setrandom");
			  emcode("asp",EMINTSIZE);}

readstmt:	READSYM {setchannel(0);} variable	{ readelm($3);}
	|	readstmt ',' variable	{ readelm($3);}

restorestmt:	RESTORESYM INTVALUE	{ restore(ival);}
	|	RESTORESYM 		{ restore(0);}
	
swapstmt:	SWAPSYM variable ',' variable	{ swapstmt($2,$4);}

whilestmt:	WHILESYM {whilestart();} expression	 {whiletst($3);}
	;

wendstmt :	WENDSYM				{wend();}

writestmt:	WRITESYM		{setchannel(-1);emcode("cal","$_wrnl");}
	|	WRITESYM file writelist 	{emcode("cal","$_wrnl");}
	;

writelist: expression			{writestmt($1,0);}
	| writelist ',' expression	{writestmt($3,1);}
	;

cross: '#' | /* empty */

intvalue: INTVALUE
	;

variable: identifier { $$=loadaddr($1);}
	| indexed ')'	{$$=endarrayload();}
	| ERRSYM	{emcode("lae","_errsym"); $$= INTTYPE;}
	| ERLSYM	{emcode("lae","_erlsym"); $$= INTTYPE;}
	;
indexed	: identifier '(' 			{newarrayload($1);} 
	 expression 			{loadarray($4); $$=$1;}
	| indexed ',' expression	{loadarray($3); $$=$1;}
	;


expression: negation
	| negation BOOLOP expression	{$$=boolop($1,$3,$2);}

negation: NOTSYM compare		{$$=boolop($2,0,NOTSYM);}
	| compare
	;
compare	: sum
	| sum RELOP sum			{$$=relop($1,$3,$2);}
	| sum '=' sum			{$$=relop($1,$3,'=');}
	
sum	: term
	| term '-' sum			{$$=plusmin($1,$3,'-');}
	| term '+' sum			{$$=plusmin($1,$3,'+');}
term	: factor
	| factor '^' factor		{$$=power($1,$3);}
	| factor '*' term		{$$=muldiv($1,$3,'*');}
	| factor '\\' term		{$$=muldiv($1,$3,'\\');}
	| factor '/' term		{$$=muldiv($1,$3,'/');}
	| factor MODSYM term		{$$=muldiv($1,$3,MODSYM);}
factor  : INTVALUE			{$$=loadint(ival);}
	| '(' expression ')'		{$$=$2;}
	| '-' factor  { $$=negate($2);}
	| FLTVALUE			{$$=loaddbl(dval);}
	| STRVALUE			{$$= STRINGTYPE; loadstr($1);}
	| variable			{$$=$1; loadvar($1);}
	| INKEYSYM '$' 			{ emcode("cal","$_inkey");
					  emcode("lfr",EMPTRSIZE);
					  $$= STRINGTYPE;
					}
	| VARPTR '(' '#' intvalue ')'	{ warning("Not supported"); $$=INTTYPE;}
	| FUNCTION			{$$= callfcn($1,0);}
	| FUNCTION '(' cross exprlist')'	{$$=callfcn($1,$4);}
	| funcname			{ $$=fcnend(0);}
	| funcname funccall ')'	{ $$=fcnend($2);}
	| MIDSYM '$' midparms	
	{	emcode("cal","$_mid");
		emcode("asp",EMINTSIZE);
		emcode("asp",EMINTSIZE);
		emcode("asp",EMPTRSIZE);
		/* emcode("asp",itoa($3)); */
		emcode("lfr",EMPTRSIZE);
		$$= STRINGTYPE;
	}
	| INPUTSYM '$' '(' expression inputtail
	{
		emcode("cal","$_inpfcn");
		emcode("asp",EMINTSIZE);
		emcode("asp",EMINTSIZE);
		emcode("asp",EMPTRSIZE);
		$$= STRINGTYPE;
	}
inputtail: ',' expression ')'		{ conversion($2,INTTYPE); $$= INTTYPE;}
	 | ',' '#' expression ')'	{ conversion($3,INTTYPE); $$= INTTYPE;}
	 | ')'				{ emcode("loc","-1"); $$= INTTYPE;}

funcname: FUNCTID		{$$=fcncall($1);}

funccall:  '(' expression	{ callparm(0,$2); $$=1;}
	| funccall ',' expression	{ callparm($1,$3); $$=$1+1;}
	
identifier: IDENTIFIER	{ dcltype($1); $$=$1;}

exprlist: expression	{ typetable[0]= $1; $$=1;}
	| exprlist ',' expression { typetable[$1]=$3;$$=$1+1;}

%%
#ifndef NORCSID
static char rcs_id[]	= "$Header$" ;
#endif
#include "basic.lex"
