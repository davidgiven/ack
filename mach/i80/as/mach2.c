/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
 * Intel 8080 tokens
 */
%token <y_word> REG
%token <y_word> NOOPOP
%token <y_word> DSTOP
%token <y_word> SRCOP
%token <y_word> D8OP
%token <y_word> D16OP
%token <y_word> R16OP
%token <y_word> MOV
%token <y_word> MVI
%token <y_word> LXI
%token <y_word> RST
%token <y_word> STLDAX

%type <y_word> r16 r8

%token <y_word> KILL
