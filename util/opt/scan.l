%{
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

extern long atol();
extern char patid[128];
extern int lino;
extern int mlookup(char* name);
extern void yyerror(const char *);

#include "y.tab.h"

%}
%%
\"[^"]*\"	{ strncpy(patid,yytext,sizeof(patid)); return(STRING); }
notreg		return(NOTREG);
sfit		return(SFIT);
ufit		return(UFIT);
rotate		return(ROTATE);
p		return(PSIZE);
w		return(WSIZE);
defined		return(DEFINED);
samesign	return(SAMESIGN);
rom		return(ROM);
[a-zA-Z]{3}	{
		int m;
		m = mlookup(yytext);
		if (m==0) {
			yyless(1);
			return yytext[0];
		} else {
			yylval.y_int = m;
			return(MNEM);
		}
		}
"&&"		return(AND2);
"||"		return(OR2);
"&"		return(AND1);
"|"		return(OR1);
"^"		return(XOR1);
"+"		return(ARPLUS);
"-"		return(ARMINUS);
"*"		return(ARTIMES);
"/"		return(ARDIVIDE);
"%"		return(ARMOD);
"=="		return(CMPEQ);
"!="		return(CMPNE);
"<"		return(CMPLT);
"<="		return(CMPLE);
">"		return(CMPGT);
">="		return(CMPGE);
"!"		return(NOT);
"~"		return(COMP);
"<<"		return(LSHIFT);
">>"		return(RSHIFT);
[0-9]+		{ long l= atol(yytext);
		  if (l>32767) yyerror("Number too big");
		  yylval.y_int= (int) l;
		  return(NUMBER);
		}
[ \t]		;
.		return(yytext[0]);
\n		{ lino++; return(yytext[0]); }
:[ \t]*\n[ \t]+	{ lino++; return(':'); }
^"# "[0-9]+.*\n	{ lino=atoi(yytext+2); }
^\#.*\n		{ lino++; }
