#define RCSID2 "$Header$"

/*
 * INTEL 8086 tokens
 */

%token <y_word> R16
%token <y_word> R8
%token <y_word> RSEG
%token <y_word> PREFIX
%token <y_word> NOOP_1
%token <y_word> NOOP_2
%token <y_word> JOP
%token <y_word> PUSHOP
%token <y_word> IOOP
%token <y_word> ADDOP
%token <y_word> ROLOP
%token <y_word> INCOP
%token <y_word> NOTOP
%token <y_word> CALLOP
%token <y_word> CALFOP
%token <y_word> LEAOP
%token <y_word> ESC
%token <y_word> INT
%token <y_word> RET
%token <y_word> XCHG
%token <y_word> TEST
%token <y_word> MOV
