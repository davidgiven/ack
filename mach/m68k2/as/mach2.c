/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)mach2.c	1.5 */
/*
 * Motorola 68000/68010 tokens
 */

%token <y_word> SIZE
%token <y_word> DREG
%token <y_word> AREG
%token <y_word> PC
%token <y_word> CREG
%token <y_word> SPEC
%token <y_word> ABCD
%token <y_word> ADDX
%token <y_word> ADD
%token <y_word> AND
%token <y_word> BITOP
%token <y_word> SHIFT
%token <y_word> SZ_EA
%token <y_word> OP_EA
%token <y_word> OP_NOOP
%token <y_word> OP_EA_D
%token <y_word> LEA
%token <y_word> DBR
%token <y_word> BR
%token <y_word> CMP
%token <y_word> MOVE
%token <y_word> MOVEP
%token <y_word> MOVEM
%token <y_word> MOVEC
%token <y_word> MOVES
%token <y_word> SWAP
%token <y_word> LINK
%token <y_word> UNLK
%token <y_word> TRAP
%token <y_word> STOP
%token <y_word> EXG
%token <y_word> OP_EXT
%token <y_word> RTD
%token <y_word> MODEL

%type <y_word> bcdx op_ea regs rrange 
%type <y_word> reg sizedef sizenon creg
