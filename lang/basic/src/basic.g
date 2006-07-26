/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

%token ILLEGAL ;
%token ASSYM ;
%token BASESYM ;
%token CALLSYM ;
%token CLEARSYM ;
%token CLOSESYM ;
%token DATASYM ;
%token DEFINTSYM ;
%token DEFSNGSYM ;
%token DEFDBLSYM ;
%token DEFSTRSYM ;
%token DEFSYM ;
%token DIMSYM ;
%token ELSESYM ;
%token ERRSYM ;
%token ERLSYM ;
%token ERRORSYM ;
%token FIELDSYM ;
%token FORSYM ;
%token FUNCTION ;
%token FUNCTID ;
%token INKEYSYM ;
%token GETSYM ;
%token GOSUBSYM ;
%token GOTOSYM ;
%token IFSYM ;
%token INPUTSYM ;
%token LETSYM ;
%token LINESYM ;
%token LSETSYM ;
%token MIDSYM ;
%token NEXTSYM ;
%token ONSYM ;
%token OPENSYM ;
%token OPTIONSYM ;
%token PRINTSYM ;
%token POKESYM ;
%token PUTSYM ;
%token RANDOMIZESYM ;
%token READSYM ;
%token REMSYM ;
%token RESTORESYM ;
%token RETURNSYM ;
%token ENDSYM ;
%token STOPSYM ;
%token STEPSYM ;
%token SWAPSYM ;
%token THENSYM ;
%token TOSYM ;
%token TRONOFFSYM ;
%token USINGSYM ;
%token USRSYM ;
%token WHILESYM ;
%token WENDSYM ;
%token WRITESYM ;
/* special tokens */
%token EOLN ;
%token INTVALUE ;
%token FLTVALUE ;
%token DBLVALUE ;
%token STRVALUE ;
%token UNARYSYM ;
%token IDENTIFIER ;
%token ANDSYM ;
%token ORSYM ;
%token IMPSYM ;
%token EQVSYM ;
%token XORSYM ;
%token VARPTR ;

/* Those were originally %left */
%token BOOLOP ;
%token NOTSYM ;
%token RELOP ;
%token MODSYM ;

/* Some contstant declared as tokens (?) */
%token LESYM ;
%token GESYM ;
%token NESYM ;
%token UNARYMINUS ;

{
#define YYDEBUG
#include <stdlib.h>
#include <stdio.h>
#include "bem.h"
#include "llmess.c"

typedef union {
	int	integer ;
	Symbol	*Sptr ;
	char	*cptr ;
} YYSTYPE ;

int basicline;
 
int yydebug;

YYSTYPE yylval;

int	ival;
char	*dval;
char	*sval;
int	in_data = 0;	/* set if processing DATA statement */

char	*formatstring;	/* formatstring used for printing */
Symbol	*s;		/* Symbol dummy */

#include "yylexp.c"
#include "basic.lex"
}

%lexical yylexp;

%start LLparse,programline ;

programline
	: INTVALUE
          { basicline = ival;newblock(ival); newemblock(ival); }
          stmts EOLN
	| '#' INTVALUE STRVALUE EOLN
	| EOLN
	;


stmts	: singlestmt
	[ %while ( LLsymb == ':' ) ':' singlestmt ]*
	;

singlestmt { int d2 ; }
	: callstmt
	| clearstmt
	| CLOSESYM closestmt
	| datastmt
	| defstmt
	| defvarstmt
	| dimstmt		
	| ERRORSYM expression(&d2)	{ errorstmt(d2); }
	| fieldstmt
	| forstmt
	| getstmt
	| gosubstmt
	| onstmt
	| ifstmt
	| illegalstmt
	| inputstmt
	| letstmt
	| lineinputstmt
	| lsetstmt
	| midstmt
	| NEXTSYM nextstmt
	| GOTOSYM INTVALUE			{ gotostmt(ival); }
	| openstmt
	| optionstmt
	| pokestmt
	| printstmt
	| randomizestmt
	| readstmt
	| REMSYM		
	| restorestmt
	| returnstmt
        | ENDSYM                { C_loc((arith) 0 );
                                  C_cal("_hlt");
                                  C_asp((arith) BEMINTSIZE);
                                }
	| STOPSYM		{ C_cal("_stop"); }
	| swapstmt
	| TRONOFFSYM		{ tronoff=yylval.integer; }
	| whilestmt
	| wendstmt
	| writestmt
	| /* EMPTY STATEMENT */
	;

illegalstmt:	ILLEGAL 	{ illegalcmd(); }
	   ;

callstmt { Symbol *id; int i; }
	:	CALLSYM
		IDENTIFIER	{ id = yylval.Sptr; }
		[ parmlist(&i) 
                  		{ C_cal(id->symname);
	          		  C_asp((arith) (i*BEMPTRSIZE));
				} 
                | /* empty */ 
		  		{ C_cal(id->symname); }
                ]
	;

parmlist(int *ip;)  { int var ; }
	: '('
	  variable(&var)	{ *ip = 1; }
	  [ ',' variable(&var)	{ *ip = *ip + 1; } ]*
	  ')'
        ;


clearstmt { int exp; }
	:	CLEARSYM [ ',' expression(&exp) ]*2
				{ warning("statement ignored"); }
	;

closestmt:	filelist		
	|	/* empty */	{ C_cal("_close"); }
	;

filelist { int intv; }
	:	cross
		intvalue(&intv)
				{ C_loc((arith) ival);
                     		  C_cal("_clochn");
                     		  C_asp((arith) BEMINTSIZE);
				}
		[	','
			cross
			intvalue(&intv)
		        	{ C_loc((arith) ival);
                             	  C_cal("_clochn");
                             	  C_asp((arith) BEMINTSIZE);
                        	}
		]* ;

datastmt:	DATASYM		{ datastmt(); in_data = 1;}
                datalist	{ fprint(datfile,"\n"); in_data = 0; }
	;

dataelm : INTVALUE		{ fprint(datfile,"%d",ival); }
	| '-' [ INTVALUE	{ fprint(datfile,"%d",-ival); }
	      | FLTVALUE	{ fprint(datfile,"-%s",dval); }
	      ]
	| FLTVALUE		{ fprint(datfile,dval); }
	| STRVALUE		{ fprint(datfile,"\"%s\"",sval); }
	| IDENTIFIER		{ fprint(datfile,"\"%s\"",sval); }
	;

datalist: dataelm
	  [ ',' 		{ fprint(datfile,","); } 
          dataelm ]*
	;

defstmt : DEFSYM 
	  [ deffnstmt  
          | defusrstmt
	  ]
        ;

deffnstmt { int exp; }
	: heading '=' expression(&exp) 
				{ endscope(exp); }
	;

heading : FUNCTID		{ newscope(yylval.Sptr); }
          [ '(' idlist ')' ]? 	{ heading(); }
	;

idlist : IDENTIFIER		{ dclparm(yylval.Sptr); }
	 [ ',' IDENTIFIER	{ dclparm(yylval.Sptr); } 
         ]*
	;

defvarstmt: 	DEFINTSYM 	{ setdefaulttype( INTTYPE); }
	|	DEFSNGSYM 	{ setdefaulttype( FLOATTYPE); }
	|	DEFDBLSYM 	{ setdefaulttype( DOUBLETYPE); }
	|	DEFSTRSYM 	{ setdefaulttype( STRINGTYPE); }
	;

defusrstmt:	USRSYM ':'	{ illegalcmd(); }
          ;

dimstmt { Symbol *symp; }
	:	DIMSYM arraydcl(&symp) ')'	{ dclarray(symp); }
	[	',' arraydcl(&symp) ')'		{ dclarray(symp); } 
	]*
	;

arraydcl(Symbol **sympp;)
	: IDENTIFIER 		{ *sympp = s = yylval.Sptr; }
	  '('
	  INTVALUE
	  			{
					s->dimlimit[s->dimensions]=ival;
					s->dimensions++;
	  			}
	  [	','
		INTVALUE
				{
					if(s->dimensions<MAXDIMENSIONS) {
						s->dimlimit[s->dimensions]=ival;
						s->dimensions++;
					} else error("too many dimensions");
				}
	  ]* ;

fieldstmt { int intv; }
	:	FIELDSYM cross intvalue(&intv) 
					{ setchannel(ival); }
		',' fieldlist		{ notyetimpl(); }
        ;

fieldlist { int intv; int var; }
	:	intvalue(&intv) ASSYM variable(&var)
		[ ',' intvalue(&intv) ASSYM variable(&var) ]*
	;

forstmt { int exp; }
	: FORSYM IDENTIFIER		{ forinit(yylval.Sptr); }
	  '=' expression(&exp)		{ forexpr(exp); }
	  TOSYM expression(&exp)	{ forlimit(exp); }
	  step
	;

step { int exp; }
	: STEPSYM expression(&exp)	{ forstep(exp); }
	| /*EMPTY*/			{ 
                                     		C_loc((arith) 1);
                                     		forstep(INTTYPE); 
					}
	;

nextstmt: [ IDENTIFIER 			{ nextstmt(yylval.Sptr); } 
	  | /* empty */ 		{ nextstmt((Symbol *)0); }
	  ]
          [ ',' IDENTIFIER 		{ nextstmt(yylval.Sptr); } 
	  ]*
	  ;

getstmt { char *cp; int intv; }
	: getput(&cp)
	  [ /* empty */ 
				{ C_loc((arith) 0);
                     		  C_cal(cp);
                     		  C_asp((arith) BEMINTSIZE);
				}
	| ',' intvalue(&intv)
				{ C_loc((arith) ival);
                           	  C_cal(cp);
                           	  C_asp((arith) BEMINTSIZE);
				}
	]
	;

getput(char **cpp;) { int intv; }
	: GETSYM cross intvalue(&intv)
	  			{ setchannel(ival); 
				  *cpp = "$_getrec";
				}
	| PUTSYM cross intvalue(&intv)
	  			{ setchannel(ival); 
			  	  *cpp = "$_putsym";
				}
	;

gosubstmt:	GOSUBSYM INTVALUE	{ gosubstmt(ival); } 
	 ;

returnstmt:	RETURNSYM		{ returnstmt(); } 
	  ;

ifstmt { int exp; int d1; }
	:	IFSYM expression(&exp)  { d1=ifstmt(exp); }
		thenpart 		{ d1=thenpart(d1); }
		elsepart 		{ elsepart(d1); }
	;

thenpart:	THENSYM [ INTVALUE	{ gotostmt(ival); }
                        | stmts
			]
	|	GOTOSYM INTVALUE	{ gotostmt(ival); }
	;

elsepart:	%prefer ELSESYM 
			[ INTVALUE  	{ gotostmt(ival); }
                        | stmts
			]
	|	/* empty */
	;

inputstmt { int intv; }
	:	INPUTSYM  [ semiprompt  readlist 
                          | '#' intvalue(&intv)
		 	    		{ setchannel(ival); }
		            ',' readlist
			  ]
	;

semiprompt { int str; }
	: semi STRVALUE 	{ str = yylval.integer; }
          [ ';'			{ loadstr(str); 
				  prompt(1); 
				}
	  | ',' 		{ loadstr(str); 
				  prompt(0); 
				}
          ]
	| /*EMPTY*/
	   			{ setchannel(-1);
                                  C_cal("_qstmark"); 
				}
	;

semi	: ';'
	| /* empty */
	;

letstmt { int var; int exp; }
	:	LETSYM		
		variable(&var)		 { save_address(); }
		'=' expression(&exp)  	 { assign(var,exp); }
	|
		variable(&var)		 { save_address(); }
		'=' expression(&exp)	 { assign(var,exp); }
	;

lineinputstmt { int var; int intv; }
	: 	LINESYM
		[ INPUTSYM
	  	  semiprompt		 { setchannel(-1); }
	  	  variable(&var)	 { linestmt(var); } 
		| '#'
		  intvalue(&intv) 	 { setchannel(ival); }
		  ','
		  variable(&var)	 { linestmt(var); }
		]
	;

readlist: readelm		
	  [ ',' readelm ]*
	  ;

readelm { int var; }
	: variable(&var)	{ readelm(var); }
	;

lsetstmt { int var; int exp; }
	:	LSETSYM variable(&var) '=' expression(&exp)
				{ notyetimpl(); }
	;

midstmt { int exp; }
	:	MIDSYM '$'  midparms '=' expression(&exp) 
				{ C_cal("_midstmt");
                     		  C_asp((arith) (2*BEMINTSIZE + 2*BEMPTRSIZE));
				}
	;

midparms:	'(' midfirst midsec midthird ')' 
	;

midfirst { int exp; }
	: expression(&exp) 	{ conversion(exp,STRINGTYPE); } 
	;

midsec { int exp; }
	: ',' expression(&exp) 	{ conversion(exp,INTTYPE); } 
	;

midthird { int exp; }
	: ',' expression(&exp) 	{ conversion(exp,INTTYPE); }
	| /* empty */ 		{ C_loc((arith) -1); }
	;

onstmt : ONSYM 
	 [ exceptionstmt
	 | ongotostmt
	 ]
	 ;

exceptionstmt:	ERRORSYM GOTOSYM INTVALUE	{ exceptstmt(ival); }
	     ;

ongotostmt { int exp; }
	:	expression(&exp) 
		[ GOSUBSYM constantlist 	{ ongosubstmt(exp); }
		| GOTOSYM constantlist  	{ ongotostmt(exp); }
		]
	;
	
constantlist: INTVALUE		{ jumpelm(ival); }
	      [ ',' INTVALUE	{ jumpelm(ival); } 
              ]* 
	    ;

openstmt { int exp; }
	:	OPENSYM mode openchannel expression(&exp) 
				{ conversion(exp,STRINGTYPE); }
		[ /* empty */	{ openstmt(0);  }
		| INTVALUE	{ openstmt(ival); }
		]
	;

openchannel: cross INTVALUE ','	{ setchannel(ival); } 
	   ;

mode { int exp; }
	: expression(&exp) ',' 	{ conversion(exp,STRINGTYPE); }
	| ','	     	        { C_lae_dnam("_iomode",(arith)0); }
	;

optionstmt { int intv; }
	:	OPTIONSYM BASESYM intvalue(&intv) { optionbase(ival); }
	;

printstmt { int plist; }
	:	PRINTSYM
		[ /* empty */	{ setchannel(-1);
                                  C_cal("_nl"); 
				}
		| file format printlist(&plist) 
		  		{ if(plist) 
                                  C_cal("_nl");            
				}
		]
	;

file { int intv; }
	: '#' intvalue(&intv) ','	{ setchannel(ival); }
	| /* empty */			{ setchannel(-1); }
	;

format { int var ; }
	: USINGSYM
          [ STRVALUE 		{ loadstr(yylval.integer); } ';'		
	  | variable(&var) ';'
	    			{ if(var!=STRINGTYPE) 
					error("string variable expected"); 
				}
	  ]
	| /* empty */ 		{ formatstring=0; }
	;

printlist(int *ip;) { int exp; }
	: [ expression(&exp)		{ printstmt(exp); *ip=1; }
	  | ','				{ zone(1); *ip=0; }
	  | ';'				{ zone(0); *ip=0; }
	  ]+
	;

pokestmt { int exp1; int exp2 ; }
	: POKESYM
	  expression(&exp1)
	  ','
	  expression(&exp2)	{ pokestmt(exp1,exp2); }
	;

randomizestmt { int exp; }
	:	RANDOMIZESYM 
		[ /* empty */ 		{ C_cal("_randomi"); }
		| expression(&exp)
		  			{ conversion(exp,INTTYPE);
                       			  C_cal("_setrand");
                       			  C_asp((arith) BEMINTSIZE);     
					} 
		]
	;

readstmt { int var; }
	:	READSYM 		{ setchannel(0); }
                variable(&var)    	{ readelm(var); }
		[ ',' variable(&var)	{ readelm(var); } 
		]*
	;

restorestmt :	RESTORESYM
		[ INTVALUE	{ restore(ival); }
		| /* empty */   { restore(0); }
		]
	    ;
	
swapstmt { int var1; int var2; }
	: SWAPSYM
	  variable(&var1)
	  ','
	  variable(&var2)	{ swapstmt(var1,var2); }
	;

whilestmt { int exp; }
	: WHILESYM 	   	 { whilestart(); }
	  expression(&exp)	 { whiletst(exp); }
	;

wendstmt :	WENDSYM		 { wend(); }
	 ;

writestmt:	WRITESYM
		[ /* empty */		{ setchannel(-1);
                                          C_cal("_wrnl");    
					}
		| file writelist 	{ C_cal("_wrnl");  }
		]
	;

writelist { int exp; }
	: expression(&exp)		{ writestmt(exp,0); }
	  [ ',' expression(&exp)	{ writestmt(exp,1); } 
	  ]*
	;

cross: '#' | /* empty */ ;	

intvalue(int *ip;)
	: INTVALUE 	{ *ip = yylval.integer; } 
	;

variable(int *ip;) { Symbol *symp; int exp; }
	: identifier(&symp)
	  [ %avoid /* empty */ 		{ *ip = loadaddr(symp); }
	  | '(' 	                { newarrayload(symp); } 
	    expression(&exp) 		{ loadarray(exp); }
	    [ ',' expression(&exp)	{ loadarray(exp); } ]*
	    ')'				{ *ip = endarrayload(); }
          ]
	| ERRSYM			{ C_lae_dnam("_errsym",(arith) 0); 
					  *ip = INTTYPE; 
					} 
	| ERLSYM			{ C_lae_dnam("_erlsym",(arith) 0); 
					  *ip = INTTYPE; 
					}
	;

expression(int *ip;) { int neg; } /* NIEUW */
	: expression1(&neg)       	{ *ip = neg; } 
          [
            IMPSYM
            expression(&neg)        	{ *ip = boolop(*ip,neg,IMPSYM); }
          ]?
        ;


expression1(int *ip;) { int neg; }
	: expression2(&neg)     { *ip = neg; } 
          [ EQVSYM
	    expression2(&neg)   { *ip = boolop(*ip,neg,EQVSYM); }
          ]*
        ;

expression2(int *ip;) { int neg; } 
	: expression3(&neg)     { *ip = neg; } 
          [ XORSYM
	    expression3(&neg)   { *ip = boolop(*ip,neg,XORSYM); }
          ]*
        ;

expression3(int *ip;) { int neg; }
	: expression4(&neg)     { *ip = neg; } 
          [ ORSYM
	    expression4(&neg)   { *ip = boolop(*ip,neg,ORSYM); }
          ]*
        ;

expression4(int *ip;) { int neg; }
	: negation(&neg)     { *ip = neg; } 
          [ ANDSYM
	    negation(&neg)   { *ip = boolop(*ip,neg,ANDSYM); }
          ]*
        ;

negation(int *ip;) { int comp; }
	: NOTSYM compare(&comp)		{ *ip=boolop(comp,0,NOTSYM); }
	| compare(ip)
	;

compare(int *ip;) { int sum1,sum2,rel; }
	: sum(&sum1)
          [ /* empty */      { *ip = sum1; }
	  | RELOP            { rel=yylval.integer; } 
            sum(&sum2)       { *ip=relop(sum1,sum2,rel); }
	  | '=' sum(&sum2)   { *ip=relop(sum1,sum2,'='); }
          ]
	;

sum(int *ip;) { int term1; }
	: term(&term1)       { *ip = term1; }
          [ %while(1)  
            '-' term(&term1) { *ip=plusmin(*ip,term1,'-'); }
	  | '+' term(&term1) { *ip=plusmin(*ip,term1,'+'); }
	  ]*
	;

term(int *ip;) { int fac1; }	
	: factor(&fac1)            { *ip = fac1; }
	  [ '*'    factor(&fac1)   { *ip=muldiv(*ip,fac1,'*'); }
	  | '\\'   factor(&fac1)   { *ip=muldiv(*ip,fac1,'\\'); }
	  | '/'    factor(&fac1)   { *ip=muldiv(*ip,fac1,'/'); }
	  | MODSYM factor(&fac1)   { *ip=muldiv(*ip,fac1,MODSYM); }
          ]*
	;

factor(int *ip;)
	: '-' factor(ip)         { *ip=negate(*ip); }
	| factor1(ip)
        ;

factor1(int *ip;)  { int mant,exp; }
	: factor2(&mant)
          [ /* empty */          { *ip = mant; } 
          | '^' factor1(&exp)    { *ip = power(mant,exp); }
          ]
        ;

factor2(int *ip;)
    { int var,func,expl,funcc,exp,intv,funcn,inpt; int typetable[10]; }
	: INTVALUE			{ *ip=loadint(ival); }
	| '(' expression(&exp) ')'      { *ip=exp; }
	| FLTVALUE			{ *ip=loaddbl(dval); }
	| STRVALUE		
	  				{ *ip= STRINGTYPE; 
					  loadstr(yylval.integer); 
					}
	| variable(&var)
	  				{ *ip=var; 
					  loadvar(var); 
					}
	| INKEYSYM '$' 			{ C_cal("_inkey");
                                          C_lfr((arith) BEMPTRSIZE);
					  *ip= STRINGTYPE;
					}
	| VARPTR '(' '#' intvalue(&intv) ')'
	  				{ warning("Not supported"); 
					  *ip=INTTYPE; 
					}
	| FUNCTION    			{ func=yylval.integer; }		
          [ %avoid /* empty */       	{ *ip= callfcn(yylval.integer,0, typetable); }
	  | '(' cross exprlist(&expl, typetable) ')' 
					{ *ip=callfcn(func,expl, typetable); }
          ]
	| funcname(&funcn)
          [ %avoid /* empty */         	{ *ip=fcnend(0); }
	  | funccall(&funcc) ')'	{ *ip=fcnend(funcc); }
          ]
	| MIDSYM '$' midparms  	
					{ 
                   			  C_cal("_mid");
                   			  C_asp((arith) (2*BEMINTSIZE+BEMPTRSIZE));
                   		 	  C_lfr((arith) BEMPTRSIZE);
					  *ip= STRINGTYPE;
					}
	| INPUTSYM '$' '(' expression(&exp) inputtail(&inpt)
					{ /*waar worden inpt en  exp gebruikt?*/
             				  C_cal("_inpfcn");
             				  C_asp((arith) (2*BEMINTSIZE+BEMPTRSIZE));
	  				  *ip= STRINGTYPE;
					}
	;

inputtail(int *ip;) { int exp; }
	: ',' cross expression(&exp) ')'
					{ conversion(exp,INTTYPE); 
					  *ip= INTTYPE; 
					}
	| ')'
	  				{ C_loc((arith) -1);  
					  *ip= INTTYPE; 
					}
	;

funcname(int *ip;)
	: FUNCTID		{ *ip=fcncall(yylval.Sptr); }
        ;

funccall(int *ip;) { int exp; }
	:  '(' expression(&exp) { callparm(0,exp);*ip=1; }
	[ ',' expression(&exp)	{ callparm(*ip,exp); 
			 	  *ip = *ip+1; 
				} 
	]*
	;
	
identifier(Symbol **ident;)
	: IDENTIFIER		{ dcltype(yylval.Sptr); 
				  *ident=yylval.Sptr; 
				}
        ;

exprlist(int *ip; int *typetable;) { int exp; }
	: expression(&exp)		{ typetable[0]=exp; 
					  *ip=1; 
					}
	  [ ',' expression(&exp)	{ typetable[*ip]=exp;
					  *ip = *ip+1; 
					} 
	  ]*
	;

{
#ifndef NORCSID
static char rcs_id[]	= "$Id$" ;
#endif
}
