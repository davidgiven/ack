/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Id$"

/*
** Zilog z8000 machine dependent yacc declarations
*/
/*      Some of the following mnemonics might seem a little cryptic,
** especially those like F1_1a.  The decision to choose these mnemo-
** nics is based on the fact that the manual `Z8000 PLZ/ASM Assembly
** Language  Programming Manual' distinguishes some instruction for-
** mats and gave them names like F1.1, etc.   In fact section 3.3 of
** this manual describes the instruction set and mentions with  each
** instruction also the corresponding instruction format(s).    So I
** sorted  all instructions on their instruction format(s) resulting
** in groups named F1_1, F1_2, F2_1, etc.  (Groups consisting of one
** member where named to that member).  Almost all  groups  were  so
** consistent that writing one yacc-rule per group sufficed.    Only
** some  groups had to be split up resulting in mnemonics like F1_1a
** and F1_1b.
*/
%token <y_word>  CALR  CC  CTLR  CTLRFLAGS  DJNZ  FLAG  F1_1a  F1_1b
%token <y_word>  F1_1F2_3  F1_2  F1_2F6_3  F2_1  F2_1F5_1  F5_1L  F5_1R
%token <y_word>  F6_4  F6_5  F6_6  F9_1  F9_2  F9_3  IN  INTCB  JP  JR
%token <y_word>  LD  LDA  LDAR  LDCTL  LDCTLB  LDK  LDL  LDM  LDR  MREQ
%token <y_word>  OUT  POP  PUSH  R8  R16  R32  R64  RET  SC  TCC


    /* From all addressing modes only 'im' and 'ra' might need more than
    ** 16 bits;      This is achieved by using the global var 'immed' in
    ** case of 'im' and by returning a long in case of 'ra'.
    */
%type <y_word>  ba  bx  coco1  coco2  ctlargs  ctlbargs  da  dst  flags
%type <y_word>  im  imexpr  ints  ir  opr  option  option2  reg  src  x

%type <y_expr>  ra
