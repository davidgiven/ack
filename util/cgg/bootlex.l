%{

#ifndef NORCSID
static char rcsid2[]="$Header$";
#endif
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#define MAXBACKUP 50

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <em_spec.h>
#include <em_flag.h>
#include <em_reg.h>

#include "booth.h"
#include "y.tab.h"

int lineno = 1;
extern char *iname;
extern char *scopy();
%}

%p 2000
%%
"/*"                    { char c;
                          c = input();
                          do {
                                while (c!='*')
                                        c = input();
                                c = input();
                          } while (c!='/');
                        }
"REGISTERS:"            return(REGISTERHEAD);
"TOKENS:"               return(TOKENHEAD);
"TOKENEXPRESSIONS:"     return(EXPRESSIONHEAD);
"CODE:"                 return(CODEHEAD);
"MOVES:"                return(MOVEHEAD);
"TESTS:"                return(TESTHEAD);
"STACKS:"		return(STACKHEAD);
"SIZEFACTOR"		return(SIZEFAC);
"TIMEFACTOR"		return(TIMEFAC);
"FORMAT"		return(FORMAT);

"cost"                  return(COST);
"remove"                return(REMOVE);
"|"                     return(SEP);
"samesign"              return(SAMESIGN);
"inreg"			return(INREG);
"sfit"                  return(SFIT);
"ufit"                  return(UFIT);
"defined"               return(DEFINED);
"rom"                   return(ROM);
"loww"			return(LOWW);
"highw"			return(HIGHW);
"move"                  return(MOVE);
"erase"                 return(ERASE);
"allocate"              return(ALLOCATE);
"tostring"              return(TOSTRING);
"nocc"                  return(NOCC);
"setcc"                 return(SETCC);
"samecc"                return(SAMECC);
"test"                  return(TEST);
"STACK"                 return(STACK);
"nocoercions"		return(NOCOERC);

"&&"                    return(AND2);
"||"                    return(OR2);
"=="                    return(CMPEQ);
"!="                    return(CMPNE);
"<="                    return(CMPLE);
"<"                     return(CMPLT);
">"                     return(CMPGT);
">="                    return(CMPGE);
">>"                    return(RSHIFT);
"<<"                    return(LSHIFT);
"!"                     return(NOT);
"~"                     return(COMP);
"..."                   return(ELLIPS);

EM_WSIZE                { yylval.yy_intp = &wsize; return(CIDENT); }
EM_PSIZE                { yylval.yy_intp = &psize; return(CIDENT); }
EM_BSIZE                { yylval.yy_intp = &bsize; return(CIDENT); }
REGISTER                { yylval.yy_string = "REGISTER"; return(TYPENAME); }
INT                     { yylval.yy_string = "INT"; return(TYPENAME); }
STRING                  { yylval.yy_string = "STRING"; return(TYPENAME); }

regvar			return(REGVAR);
loop			return(LOOP);
pointer			return(POINTER);
float			return(FLOAT);
return			return(RETURN);

[_A-Za-z][_A-Za-z0-9]+  {register ident_p ip;
                         if(!lookident || (ip=ilookup(yytext,JUSTLOOKING))==0) {
                           yylval.yy_string = scopy(yytext);return(IDENT);
                         } else {
                           yylval.yy_ident = ip;
                           switch(ip->i_type) {
                           default:assert(0);
                           case IREG:return(RIDENT);
                           case IPRP:return(PIDENT);
                           case ITOK:return(TIDENT);
                           case IEXP:return(EIDENT);
                           }
                         }
                        }
[a-z]                   {yylval.yy_char = yytext[0]; return(LCASELETTER);}
[0-9]*                  {yylval.yy_int = atoi(yytext);return(NUMBER);}
(\"|"%)")               { char *p; int c,tipe; char stringbuf[BUFSIZ];
                          p=stringbuf;
                          for (;;) {
                                c = input();
                                switch(c) {
                                default: *p++=c;break;
                                case '\\':
                                        *p++=c; *p++=input(); break;
                                case '\n':
                                        yyerror("Unterminated string");
					unput(c);
                          		/* fall through */
                                case '"':
                                        tipe=STRING; goto endstr;
                                case '%':
                                        c=input();
                                        if (c == '(') {
                                                tipe=LSTRING;goto endstr;
                                        } else {
                                                *p++ = '%'; unput(c); break;
                                        }
                                }
                          }
                        endstr:
                          *p++ = 0;
                          yylval.yy_string = scopy(stringbuf);
                          return(tipe);
                        }
^\#(line)?[ \t]*[0-9]+[ \t]+\".*\".*$   {
                          int ind,ind2;
                          for (ind=0; yytext[ind] < '0' || yytext[ind]>'9'; ind++)
				;
                          lineno=atoi(&yytext[ind])-1;
                          for(;yytext[ind]!='"';ind++)
                                ;
                          for(ind2=ind+1;yytext[ind2]!='"';ind2++)
                                ;
                          yytext[ind2]=0;
                          if (!iname || strcmp(yytext+ind+1,iname)!=0)
                                iname=scopy(yytext+ind+1);
                        }
[ \t]*                  ;
\n                      { lineno++; }
.                       return(yytext[0]);
%%

yyerror(s,a1,a2,a3,a4) string s; {

	fprintf(stderr,"\"%s\", line %d:",iname ? iname : "",lineno);
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr,"\n");
        nerrors++;
}
