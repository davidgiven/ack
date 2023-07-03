%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "iburg.h"
#include "astring.h"

#define YYDEBUG 1

extern int yylex(void);

%}
%union {
	arith n;
	const char* string;
    Nonterm nonterm;
	Tree tree;
    Rule rule;
    struct reg* reg;
    struct stringlist* stringlist;
    struct terminfo terminfo;
    struct expr* expr;
    struct constraint* constraint;
}

%term ALIASES
%term COPY
%term CORRUPTED
%term COST
%term DECLARATIONS
%term EMIT
%term EQUALS
%term FRAGMENT
%term NAMED
%term NOTEQUALS
%term OPTIONS
%term PATTERNS
%term PREFERS
%term PRESERVED
%term REGISTERS
%term WHEN
%term WITH

%token <n>          INT
%token <string>     ID
%token <string>     QFRAGMENT

%type  <constraint> constraint
%type  <constraint> constraints
%type  <expr>       predicate
%type  <expr>       predicate_arg
%type  <expr>       predicate_args
%type  <nonterm>    declaration
%type  <reg>        register
%type  <rule>       pattern
%type  <rule>       pattern_constraints
%type  <rule>       pattern_emit
%type  <string>     nodename
%type  <stringlist> aliases
%type  <stringlist> names
%type  <stringlist> qfragments
%type  <terminfo>   terminfo
%type  <tree>       rhs
%%

spec
    : optionaloptions
      REGISTERS registers
        { validate_registers(); }
      DECLARATIONS declarations
      PATTERNS patterns
	;

optionaloptions
	: /* nothing */
	| OPTIONS options
	;

options
	: /* nothing */
	| options option ';'
	| option ';'
	;

option
	: ID                              { option($1); }
	;

registers
    : /* nothing */
    | registers register ';'
    | register ';'
    ;

register
    : ID                              { $$ = makereg($1); }
    | register NAMED '(' names ')'    { $$ = $1; setregnames($$, $4); }
    | register ALIASES '(' aliases ')' { $$ = $1; addregaliases($$, $4); }
    | register ID                     { $$ = $1; addregattr($1, $2); }
    ;

names
    : QFRAGMENT                       { $$ = calloc(1, sizeof(*$$)); stringlist_add($$, $1); }
    | names ',' QFRAGMENT             { $$ = $1; stringlist_add($$, $3); }
    ;

aliases
    : ID                              { $$ = calloc(1, sizeof(*$$)); stringlist_add($$, $1); }
    | aliases ',' ID                  { $$ = $1; stringlist_add($$, $3); }
    ;

declarations
    : /* nothing */
    | declarations declaration ';'
    | declaration ';'
    ;

declaration
    : ID                              { $$ = nonterm($1, true); }
    | declaration FRAGMENT            { $$ = $1; $$->is_fragment = true; }
    ;

patterns
    : /* nothing */
	| patterns pattern ';'
	| patterns ';'
	;

pattern
    : terminfo '=' rhs                  { nonterm($1.name, false); $$ = rule(&$1,  $3); }
    | rhs                               {                          $$ = rule(NULL, $1); }
    | pattern PREFERS predicate         { $$ = $1; array_append(&$$->prefers, $3); }
    | pattern WHEN predicate            { $$ = $1; array_append(&$$->requires, $3); }
    | pattern COST INT                  { $$ = $1; $$->cost = $3; }
    | pattern_constraints               { $$ = $1; }
    | pattern_emit                      { $$ = $1; }
    ;

rhs
    : terminfo                          { $$ = tree(&$1, NULL, NULL); }
	| terminfo '(' rhs ')'              { $$ = tree(&$1,   $3, NULL); }
	| terminfo '(' rhs ',' rhs ')'      { $$ = tree(&$1,   $3, $5); }
	;

terminfo
    : nodename                          { $$.name = $1; }
    | '(' ID ')' nodename               { $$.attr = $2; $$.name = $4; }
    | ID ':' nodename                   { $$.label = $1; $$.name = $3; }
    | ID ':' '(' ID ')' nodename        { $$.label = $1; $$.attr = $4; $$.name = $6; }
    ;

nodename
    : ID                                { $$ = $1; }
    | ID '.' ID                         { $$ = aprintf("%s.%s", $1, $3); }
    ;

pattern_emit
    : pattern EMIT qfragments           {
                                            $$ = $1;
                                            if (!$$->lhs->is_fragment)
                                                stringlist_add($3, "\n");
                                            stringlist_addall(&$$->code, $3);
                                        }
    ;

pattern_constraints
    : pattern WITH constraints          {
                                            struct constraint* c = $3;
                                            $$ = $1;
                                            while (c)
                                            {
                                                array_append(&$$->constraints, c);
                                                c = c->next;
                                            }
                                        }
    ;

constraints
    : constraint                        { $$ = $1; }
    | constraints ',' constraint        { $$ = $3; $$->next = $1; }
    ;

constraint
    : '(' constraint ')'                { $$ = $2; }
    | '%' ID EQUALS '%' ID              { $$ = calloc(1, sizeof(*$$));
                                          $$->type = CONSTRAINT_EQUALS; $$->left = $2; $$->right = $5; }
    | CORRUPTED '(' ID ')'              { $$ = calloc(1, sizeof(*$$));
                                          $$->type = CONSTRAINT_CORRUPTED_ATTR; $$->left = $3; }
    | PRESERVED '(' '%' ID ')'          { $$ = calloc(1, sizeof(*$$));
                                          $$->type = CONSTRAINT_PRESERVED; $$->left = $4; }
    ;

qfragments
    : /* nothing */                     { $$ = calloc(1, sizeof *$$); }
    | qfragments QFRAGMENT              { $$ = $1; stringlist_add($$, $2); }
    ;

predicate
    : '!' predicate                     { $$ = $2; $$->negated = true; }
    | ID '(' predicate_args ')'         {
                                            $$ = calloc(1, sizeof *$$);
                                            $$->type = PREDICATE_FUNCTION;
                                            $$->u.name = $1;
                                            $$->next = $3;
                                        }
    ;

predicate_args
    : /* nothing */                     { $$ = NULL; }
    | predicate_arg                     { $$ = $1; }
    | predicate_arg ',' predicate_args  { $$ = $1; $$->next = $3; }
    ;

predicate_arg
    : '%' ID                            { $$ = calloc(1, sizeof *$$); $$->type = PREDICATE_NODE; $$->u.name = $2; }
    | INT                               { $$ = calloc(1, sizeof *$$); $$->type = PREDICATE_NUMBER; $$->u.number = $1; }
    ;

%%
#include <stdarg.h>
#include <ctype.h>

int errcnt = 0;
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
    exit(1);
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
