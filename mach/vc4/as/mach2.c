/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

%token <y_word> GPR
%token <y_word> CC

%token <y_word> OP
%token <y_word> OP_BRANCH OP_BRANCHLINK OP_ADDCMPB
%token <y_word> OP_ONEREG
%token <y_word> OP_ONELREG
%token <y_word> OP_ALU
%token <y_word> OP_FPU
%token <y_word> OP_MEM
%token <y_word> OP_MISC
%token <y_word> OP_MISCL
%token <y_word> OP_STACK


