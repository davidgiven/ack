/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
 * Motorola 6809 tokens
 */

%token <y_word> REG
%token <y_word> ALL
%token <y_word> SETDP
%token <y_word> NOARG
%token <y_word> BRANCH
%token <y_word> LBRNCH
%token <y_word> SBRNCH
%token <y_word> STACK
%token <y_word> IMMED
%token <y_word> TWOREG
%token <y_word> XOP

%type <y_word> reglist xyus xmode
