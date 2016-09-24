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
%token <string>     STRING

%type  <n>          cost
%type  <string>     lhs
%type  <stringlist> stringlist
%type  <stringlist> when
%type  <tree>       rhs
%type  <stringpair> labelledid
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
	| patterns error ';'		      { yyerrok; }
	;

pattern
    : lhs '=' rhs when ins outs emits cost { rule($1, $3, nextern++, $4, $8); }
    ;

lhs
    : ID				              { $$ = $1; nonterm($$); }
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

when
    : /* nothing */                   { $$ = NULL; }
    | WHEN stringlist                 { $$ = $2; }
    ;

stringlist 
    : /* nothing */                   { $$ = NULL; }
    | CFRAGMENT stringlist            { $$ = pushstring($1, $2); }
    ;
    
ins
    : /* nothing */
    | INS inslist
    ;

inslist
    : inslist ',' in
    | in
    ;

in
    : ID ':' ID
    ;

outs
    : /* nothing */
    | OUTS outslist
    ;

outslist
    : outslist ',' out
    | out
    ;

out
    : ID ':' ID
    ;

emits
    : /* nothing */
    | EMIT STRING
    | FRAGMENT STRING
    ;

cost
    : /* lambda */			          { $$ = 0; }
	| COST INT  			          {
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
