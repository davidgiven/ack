/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
 * Motorola 6800 tokens
 */

%token <y_word> X
%token <y_word> NOARG
%token <y_word> BRANCH
%token <y_word> AOP
%token <y_word> XOP

%type <y_word> memref
