#define RCSID2 "$Header$"

/*
 * Mostek 6500 tokens.
 */

%token <y_word> A
%token <y_word> X
%token <y_word> Y
%token <y_word> EXTENSION
%token <y_word> ADDOP
%token <y_word> ROLOP
%token <y_word> BRAOP
%token <y_word> BITOP
%token <y_word> NOPOP
%token <y_word> CPXOP
%token <y_word> INCOP
%token <y_word> JMPOP
%token <y_word> JSROP
%token <y_word> LDXOP
%token <y_word> LDYOP
%token <y_word> STXOP
%token <y_word> STYOP
%token <y_word> PSEU

%type <y_word> addop

%nonassoc EXTENSION
