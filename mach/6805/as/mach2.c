#define RCSID2 "$Header$"

/*
 * Motorola 6805 tokens
 */

%token <y_word> X
%token <y_word> A
%token <y_word> NOARG
%token <y_word> BRANCH
%token <y_word> BBRANCH
%token <y_word> BIT
%token <y_word> RMR
%token <y_word> RM
%token <y_word> CMOS
%type <y_expr> expr8
%type <y_valu> bitexp
