%{
/* $Header$ */
#include "Lpars.h"
#include "parser.h"

struct idf	*opval;		/* opcode of returned OPCODE*/
int		lastintval;	/* value of last integer seen */
int		linenum = 1;	/*current line number of input file*/
%}
%%
sfit		return(SFIT);
ufit		return(UFIT);
rotate		return(ROTATE);
p		return(PSIZE);
w2		return(DWSIZE);
w		return(WSIZE);
defined		return(DEFINED);
undefined	return(UNDEFINED);
samesign	return(SAMESIGN);
sameext		return(SAMEEXT);
samenam		return(SAMENAM);
offset		return(OFFSET);
[a-z][a-z][a-z]	{
		opval = str2idf(yytext,0);
		return(OPCODE);
		}
[0-9]+		{
		lastintval = atoi(yytext);
		return(INT);
		}
"$"		return(PATARG);
"&&"		return(LOGAND);
"||"		return(LOGOR);
"&"		return(BITAND);
"|"		return(BITOR);
"^"		return(XOR);
"-"		return(MINUS);
"+"		return(PLUS);
"*"		return(TIMES);
"/"		return(DIV);
"%"		return(MOD);
"=="		return(EQ);
"!="		return(NE);
"<"		return(LT);
"<="		return(LE);
">"		return(GT);
">="		return(GE);
"<<"		return(LSHIFT);
">>"		return(RSHIFT);
"!"		return(NOT);
"~"		return(COMP);
","		return(COMMA);
:[ \t]*\n[ \t]+	{ linenum++; return(':'); }
^"# "[0-9]+.*\n	{ linenum=atoi(yytext+2); }
^\#.*\n		{ linenum++; }
^\n		{ linenum++; }
[ \t]		;
\n		{ linenum++; return(yytext[0]);}
.		return(yytext[0]);
%%

back_token()
{
	yyless(0);
}
