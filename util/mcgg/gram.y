%{
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "iburg.h"

#define YYDEBUG 1

static char rcsid[] = "$Id$";
static int nextesn = 0;
static int nextern = 0;

%}
%union {
	int n;
	char* string;
	Tree tree;
    Stringlist stringlist;
}
%term TERMINAL
%term START
%term PPERCENT

%term PATTERNS
%term PAT
%term WHEN
%term EMIT
%term COST

%token <string>     ID
%token <string>     CFRAGMENT
%token <n>          INT

%type  <string>     lhs
%type  <tree>       rhs
%type  <n>          cost
%type  <stringlist> when
%type  <stringlist> stringlist
%%
spec
    : decls PPERCENT patterns
	| decls
	;

decls	: /* lambda */
	| decls decl
	;

decl
    : TERMINAL blist ';'
	| START lhs ';'
    {
        if (nonterm($2)->number != 1)
            yyerror("redeclaration of the start symbol\n");
    }
	| ';'
	| error ';'			{ yyerrok; }
	;

blist
    : /* nothing */
    | blist ID              { term($2, nextesn++); }
    ;

patterns
    : /* nothing */
	| patterns pattern ';'
	| patterns ';'
	| patterns error ';'		{ yyerrok; }
	;

pattern
    : lhs '=' rhs when cost         { rule($1, $3, nextern++, $4, $5); }
    ;

lhs
    : ID				            { $$ = $1; nonterm($$); }
	;

rhs
    : ID                            { $$ = tree($1, NULL, NULL); }
	| ID '(' rhs ')'                { $$ = tree($1,   $3, NULL); }
	| ID '(' rhs ',' rhs ')'        { $$ = tree($1,   $3, $5); }
	;

when
    : /* nothing */                 { $$ = NULL; }
    | WHEN stringlist               { $$ = $2; }
    ;

stringlist 
    : /* nothing */                 { $$ = NULL; }
    | CFRAGMENT stringlist          { $$ = pushstring($1, $2); }
    ;
    
cost
    : /* lambda */			        { $$ = 0; }
	| COST INT  			        {
                                        if ($2 > maxcost) {
                                            yyerror("%d exceeds maximum cost of %d\n", $2, maxcost);
                                            $$ = maxcost;
                                        } else
                                            $$ = $2;
                                    }
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
