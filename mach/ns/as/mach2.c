/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
 * NS 16032 tokens
 */

%token <y_word> REG
%token <y_word> AREG
%token <y_word> FREG
%token <y_word> MREG
%token <y_word> CREG
%token <y_word> EXTERNAL
%token <y_word> TOS
%token <y_word> PC
%token <y_word> INDICATOR

/*
 * Instruction types
 */
%token <y_word> SETCFG
%token <y_word> LPR
%token <y_word> LCR
%token <y_word> BR
%token <y_word> RET
%token <y_word> BSR
%token <y_word> RDVAL
%token <y_word> CATST
%token <y_word> LCSR
%token <y_word> SEQ
%token <y_word> ADJSP
%token <y_word> JSR
%token <y_word> JUMP
%token <y_word> LFSR
%token <y_word> ADD_F
%token <y_word> CCVIS
%token <y_word> CCVSI
%token <y_word> CCVSS
%token <y_word> CMOV
%token <y_word> ADD_I
%token <y_word> COM
%token <y_word> MUL_I
%token <y_word> MOVID
%token <y_word> FFS
%token <y_word> MOVIF
%token <y_word> MOVFL
%token <y_word> TRUNC
%token <y_word> MOVM
%token <y_word> INSS
%token <y_word> SAVE
%token <y_word> ENTER
%token <y_word> LMR
%token <y_word> MOVS
%token <y_word> CHECK
%token <y_word> INS
%token <y_word> MOVQ
%token <y_word> ACB
%token <y_word> WAIT

%type  <y_word> cpu_opts, cpu_list, string_opts, reg_list, reg_items
