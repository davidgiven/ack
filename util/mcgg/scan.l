%{
#include <stdbool.h>
#include "iburg.h"
#include "y.tab.h"

static int braces = 0;

%}
%option warn
%option nodefault
%option noyywrap
%option yylineno
%option debug

%x CSTRING
%x QSTRING
%x COMMENT

%%

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

<INITIAL>"\""               BEGIN(QSTRING);
<QSTRING>"\""               BEGIN(INITIAL);

<QSTRING>%[a-zA-Z_][a-zA_Z_0-9]+ {
                                yylval.string = strdup(yytext);
                                return QFRAGMENT;
                            }

<QSTRING>[^\r\n%"]+         {
                                yylval.string = strdup(yytext);
                                return QFRAGMENT;
                            }

<INITIAL>"/*"               BEGIN(COMMENT);
<COMMENT>"*/"               BEGIN(INITIAL);
<COMMENT>[^*]*              ;
<COMMENT>"*"                ;

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

"//"[^\n]*\n                ;

[A-Za-z_][A-Za-z0-9_]*      { yylval.string = strdup(yytext); return ID; }
[0-9]+                      { yylval.n = atoi(yytext); return INT; }
[ \t\r\n]*                  ;
.                           return yytext[0];

%%
/* vim: set sw=4 ts=4 expandtab : */
