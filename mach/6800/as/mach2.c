#define RCSID2 "$Header$"

/*
 * Motorola 6800 tokens
 */

%token <y_word> X
%token <y_word> NOARG
%token <y_word> BRANCH
%token <y_word> AOP
%token <y_word> XOP

%type <y_word> memref
