/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*
 * Zilog Z80 tokens
 */
%token <y_word> ADCSBC
%token <y_word> ADDOP
%token <y_word> ARI8
%token <y_word> BITS
%token <y_word> CALL
%token <y_word> CC
%token <y_word> DJNZ
%token <y_word> EXOP
%token <y_word> E_ED
%token <y_word> IMOP
%token <y_word> IN
%token <y_word> INCDEC
%token <y_word> JP
%token <y_word> JR
%token <y_word> LDOP
%token <y_word> LEA
%token <y_word> NOOPOP
%token <y_word> OUT
%token <y_word> PEA
%token <y_word> PSHPOP
%token <y_word> R24
%token <y_word> R8
%token <y_word> RET
%token <y_word> ROTATE
%token <y_word> RST

%type <y_word> ari8 r8 indir index ind coco
