/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
* PDP 11 machine dependent yacc declarations
*/
%token <y_word> REG
%token <y_word> FREG
%token <y_word> FRSP
%token <y_word> JMP
%token <y_word> OP_SO
%token <y_word> OP_DO
%token <y_word> OP_R_SO
%token <y_word> BR
%token <y_word> EJMP
%token <y_word> JSR
%token <y_word> RTS
%token <y_word> MARK
%token <y_word> SOB
%token <y_word> SPL
%token <y_word> TRAP
%token <y_word> OP_NO
%token <y_word> OP_SO_R
%token <y_word> MFP
%token <y_word> FOP_FSO_FR
%token <y_word> FOP_NO
%token <y_word> FOP_FSO
%token <y_word> FOP_SO
%token <y_word> MOVF
%token <y_word> FOP_SO_FR
%token <y_word> FOP_FR_SO
%token <y_word> FOP_FR_FSO
%token <y_word> CLEARCC
%token <y_word> SETCC

%type <y_word> opr1, opr2, op2, fopr2, fopr1, ari, ainc, adec
%type <y_word> fop2, clcc, secc, fregister
