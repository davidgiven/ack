%{
#include <stdlib.h>
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
    Nonterm nonterm;
	Tree tree;
    Rule rule;
    struct reg* reg;
    struct stringlist* stringlist;
    char* stringpair[2];
}

%term ALLOCATES
%term COST
%term DECLARATIONS
%term EMIT
%term FRAGMENT
%term INS
%term OUTS
%term PATTERNS
%term REGISTERS
%term WHEN

%token <n>          INT
%token <string>     ID
%token <string>     CFRAGMENT
%token <string>     QFRAGMENT

%type  <nonterm>    allocates
%type  <nonterm>    declaration
%type  <reg>        register
%type  <rule>       emit
%type  <rule>       pattern
%type  <stringlist> cfragments
%type  <stringlist> qfragments
%type  <stringpair> labelledid
%type  <tree>       rhs
%%

spec
    : REGISTERS registers 
      DECLARATIONS declarations
      PATTERNS patterns
	;

registers
    : /* nothing */
    | registers register ';'
    | register ';'
    ;

register
    : ID                              { $$ = makereg($1); }
    | register ID                     { $$ = $1; addregclass($1, $2); }
    ;

declarations
    : /* nothing */
    | declarations declaration ';'
    | declaration ';'
    ;

declaration
    : ID                              { $$ = nonterm($1, true); }
    | declaration FRAGMENT            { $$ = $1; $$->is_fragment = true; }
    | allocates                       { $$ = $1; }
    ;

allocates
    : declaration ALLOCATES '(' ID ')'
        {
            $$ = $1;
            if ($$->allocate)
                yyerror("pattern type is defined to already allocate a register");
            $$->allocate = getregclass($4);
        }
    ;

patterns
    : /* nothing */
	| patterns pattern ';'
	| patterns ';'
	;

pattern
    : ID '=' rhs                      { nonterm($1, false); $$ = rule($1,     $3, nextern++); }
    | rhs                             {                     $$ = rule("stmt", $1, nextern++); }
    | pattern WHEN cfragments         { $$ = $1; stringlist_addall(&$$->when, $3); }
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
    : /* nothing */                   { $$ = calloc(1, sizeof *$$); }
    | cfragments CFRAGMENT            { $$ = $1; stringlist_add($$, $2); }
    ;
    
emit
    : pattern EMIT qfragments           {
                                            $$ = $1;
                                            if (!$$->lhs->is_fragment)
                                                stringlist_add($3, "\n");
                                            stringlist_addall(&$$->code, $3);
                                        }
    ;

qfragments
    : /* nothing */                     { $$ = calloc(1, sizeof *$$); }
    | qfragments QFRAGMENT              { $$ = $1; stringlist_add($$, $2); }
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
