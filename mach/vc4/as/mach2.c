/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

%token <y_word> GPR

%token <y_word> OP
%token <y_word> OP_ONEREG
%token <y_word> OP_ONELREG
%token <y_word> OP_ALU
%token <y_word> OP_MEM
%token <y_word> OP_BREG
%token <y_word> OP_STACK

/* Other token types */

/* %type <y_word> c */
%type <y_word> e16 u8 u7 u6 u5 u4 u2 u1
/* %type <y_word> nb ds bda bdl lia lil */
