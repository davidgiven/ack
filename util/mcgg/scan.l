%{
#include "iburg.h"
#include "y.tab.h"

static int braces = 0;

%}
%option warn
%option nodefault
%option noyywrap
%option yylineno

%x CSTRING
%x ECHO

%%

<INITIAL>"%{"               { printlineno(); BEGIN(ECHO); }

<ECHO>"%}"                  BEGIN(INITIAL);
<ECHO>[%\n]                 fputc(yytext[0], outfp);
<ECHO>[^%\n]*               fputs(yytext, outfp);

<INITIAL>"{"                {
                                yylval.string = ""; //stringf("#line %d\n", yylineno);
                                braces = 1;
                                BEGIN(CSTRING);
                                return CFRAGMENT;
                            }

<CSTRING>"{"                {
                                braces++;
                                yylval.string = strdup(yytext);
                                return CFRAGMENT;
                            }

<CSTRING>"}"                {
                                braces--;
                                if (braces == 0)
                                    BEGIN(INITIAL);
                                else
                                {
                                    yylval.string = strdup(yytext);
                                    return CFRAGMENT;
                                }
                            }

<CSTRING>[^{}]+             {
                                yylval.string = strdup(yytext);
                                return CFRAGMENT;
                            }

"%%"                        return PPERCENT;
"%term"						return TERMINAL;
"%start"					return START;

"PATTERNS"                  return PATTERNS;
"when"                      return WHEN;
"ins"                       return INS;
"outs"                      return OUTS;
"emit"                      return EMIT;
"fragment"                  return FRAGMENT;
"cost"                      return COST;

\"[^"\n]*\"                 { yylval.string = strdup(yytext); return STRING; }
[A-Za-z_][A-Za-z0-9_]*      { yylval.string = strdup(yytext); return ID; }
[0-9]+                      { yylval.n = atoi(yytext); return INT; }
[ \t\r\n]*                  ;
.                           return yytext[0];

%%
/* vim: set sw=4 ts=4 expandtab : */
