%{

#ifndef NORCSID
static char rcsid2[]="$Header$";
#endif
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 * Author: Hans van Staveren
 */

#undef input
#undef output
#undef unput

#define MAXBACKUP 50
%}
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
(\"|"%)")               { char *p; int c,tipe;
                          p=yytext;
                          for (;;) {
                                c = input();
                                switch(c) {
                                default: *p++=c;break;
                                case '\\':
                                        *p++=c; *p++=input(); break;
                                case '\n':
                                        yyerror("Unterminated string");
                                        break;
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
                          yylval.yy_string = scopy(yytext);
                          return(tipe);
                        }
^\#.*$			|
[ \t]*                  |
\n                      ;
.                       return(yytext[0]);
%%

char linebuf[256];
char prevbuf[256];
int linep;
int linepos;    /* corrected for tabs */
char charstack[MAXBACKUP];
int nbackup=0;

output(c) {

        assert(0);
}

input() {

        if(nbackup)
                return(charstack[--nbackup]);
        if(linebuf[linep]==0) {
                strcpy(prevbuf,linebuf);
                if(fgets(linebuf,256,stdin)==NULL)
                        return(0);
                lino++;
                linepos=linep=0;
        }
        if (linebuf[linep] == '\t')
                linepos = (linepos+8) & ~07;
        else    linepos++;
        return(linebuf[linep++]);
}

unput(c) {

        chktabsiz(nbackup,MAXBACKUP,"Lexical backup table");
        charstack[nbackup++] = c;
}

yyerror(s,a1,a2,a3,a4) string s; {

        fprintf(stderr,"%d\t%s%d\t%s\t%*c   ",lino-1,prevbuf,lino,linebuf,
                linepos-1,'^');
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr,"\n");
        nerrors++;
}
