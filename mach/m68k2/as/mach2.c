/* $Id$ */
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
%token <y_word> FPCR
%token <y_word> FPREG
%token <y_word> FMOVE
%token <y_word> FMOVECR
%token <y_word> FMOVEM
%token <y_word> FDYADIC
%token <y_word> FMONADIC
%token <y_word> FSINCOS
%token <y_word> FBCC
%token <y_word> FDBCC
%token <y_word> FNOP
%token <y_word> FSCC
%token <y_word> FTST
%token <y_word> FSAVRES
%token <y_word> FTRAPCC
%token <y_word> FSIZE
%token <y_word> CP

%type <y_word> bcdx op_ea regs rrange 
%type <y_word> reg sizedef sizenon creg
%type <y_word> fsize fregs fcregs frlist frrange
