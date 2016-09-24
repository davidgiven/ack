%{
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "iburg.h"

#define YYDEBUG 1

extern int yylex(void);

static int nextern = 1;

%}
%union {
	int n;
	char* string;
	Tree tree;
    Rule rule;
    Stringlist stringlist;
    char* stringpair[2];
}
%term TERMINAL
%term START
%term PPERCENT

%term PATTERNS
%term WHEN
%term EMIT
%term FRAGMENT
%term COST
%term INS
%term OUTS

%token <n>          INT
%token <string>     ID
%token <string>     CFRAGMENT
%token <string>     QFRAGMENT

%type  <rule>       pattern
%type  <rule>       emit
%type  <stringlist> cfragments
%type  <stringlist> qfragments
%type  <tree>       rhs
%type  <stringpair> labelledid
%%

spec
    : PATTERNS patterns
	;

patterns
    : /* nothing */
	| patterns pattern ';'
	| patterns ';'
	;

pattern
    : ID '=' rhs                      { nonterm($1); $$ = rule($1, $3, nextern++); }
    | rhs                             {              $$ = rule("stmt", $1, nextern++); }
    | pattern WHEN cfragments         { $$ = $1; $$->when = $3; }
    | pattern INS ins                 { $$ = $1; }
    | pattern OUTS outs               { $$ = $1; }
    | emit                            { $$ = $1; }
    | pattern COST INT                { $$ = $1; $$->cost = $3; }
    ;

rhs
    : labelledid                      { $$ = tree($1[1], $1[0], NULL, NULL); }
	| labelledid '(' rhs ')'          { $$ = tree($1[1], $1[0],   $3, NULL); }
	| labelledid '(' rhs ',' rhs ')'  { $$ = tree($1[1], $1[0],   $3, $5); }
	;

labelledid
    : ID                              { $$[0] = NULL; $$[1] = $1; }
    | ID ':' ID                       { $$[0] = $1; $$[1] = $3; }
    ;

cfragments 
    : /* nothing */                   { $$ = NULL; }
    | CFRAGMENT cfragments            { $$ = pushstring($1, $2); }
    ;
    
ins
    : ins ',' in
    | in
    ;

in
    : ID ':' ID
    ;

outs
    : outs ',' out
    | out
    ;

out
    : ID
    | ID ':' ID
    ;

emit
    : pattern EMIT qfragments         { $$ = $1; $$->code = $3; $$->is_fragment = false; }
    | pattern FRAGMENT qfragments     { $$ = $1; $$->code = $3; $$->is_fragment = true; }
    ;

qfragments
    : QFRAGMENT                       { $$ = pushstring($1, NULL); }
    | QFRAGMENT qfragments            { $$ = pushstring($1, $2); }
    ;

%%
#include <stdarg.h>
#include <ctype.h>

int errcnt = 0;
FILE *infp = NULL;
FILE *outfp = NULL;
static char buf[BUFSIZ], *bp = buf;
static int ppercent = 0;

void yyerror(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	if (yylineno > 0)
		fprintf(stderr, "line %d: ", yylineno);
	vfprintf(stderr, fmt, ap);
	if (fmt[strlen(fmt)-1] != '\n')
		 fprintf(stderr, "\n");
	errcnt++;
}

void yywarn(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	if (yylineno > 0)
		fprintf(stderr, "line %d: ", yylineno);
	fprintf(stderr, "warning: ");
	vfprintf(stderr, fmt, ap);
}

/* vim: set sw=4 ts=4 expandtab : */
