/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID2 "$Header$"

/*
* VAX-11 machine dependent yacc declarations
*/

%token <y_word> REG
%token <y_word>	CASE_b_b_b, CASE_l_l_l, CASE_w_w_w
%token <y_word> OP0
%token <y_word>	OP1_A, OP1_Bb, OP1_Bl, OP1_Bw, OP1_Bx, OP1_b, OP1_l, OP1_u,
		OP1_w, OP1_Be
%token <y_word>	OP2_A_A, OP2_A_l, OP2_b_b, OP2_b_l, OP2_b_u, OP2_b_w, OP2_l_A,
		OP2_l_Bb, OP2_l_b, OP2_l_l, OP2_l_u, OP2_l_w, OP2_u_b, OP2_u_l,
		OP2_u_u, OP2_u_w, OP2_w_b, OP2_w_l, OP2_w_u, OP2_w_w, OP2_l_Be
%token <y_word>	OP3_b_b_b, OP3_b_l_l, OP3_b_u_u, OP3_b_w_A, OP3_l_V_Bb,
		OP3_l_l_Bb, OP3_l_l_l, OP3_l_w_A, OP3_u_u_u, OP3_u_w_A,
		OP3_w_A_A, OP3_w_A_l, OP3_w_w_w, OP3_l_V_Be
%token <y_word>	OP4_A_l_w_A, OP4_b_b_b_Bw, OP4_l_b_V_l, OP4_l_l_b_V,
		OP4_l_l_l_Bw, OP4_l_l_l_u, OP4_l_u_l_l, OP4_u_u_u_Bw,
		OP4_w_A_A_A, OP4_w_A_A_b, OP4_w_A_w_A, OP4_w_w_w_Bw
%token <y_word>	OP5_u_b_u_l_u, OP5_u_w_u_l_u, OP5_w_A_A_w_A, OP5_w_A_b_w_A
%token <y_word>	OP6_b_w_A_b_w_A, OP6_l_l_l_l_l_l, OP6_w_A_b_A_w_A,
		OP6_w_A_w_A_w_A
%token <y_word>
%type <y_word>	OP1_O, OP1_B
%type <y_word>	OP2_O_O, OP2_A_O, OP2_O_B, OP2_O_A
%type <y_word>	OP3_O_O_O, OP3_O_O_B, OP3_O_O_A, OP3_O_A_A, OP3_O_A_O
%type <y_word>	OP4_O_O_O_O, OP4_O_O_O_B, OP4_O_A_O_A, OP4_O_A_A_O,
		OP4_O_A_A_A, OP4_A_O_O_A
%type <y_word>	OP5_O_A_A_O_A, OP5_O_A_O_O_A, OP5_O_O_O_O_O
%type <y_word>	OP6_O_O_O_O_O_O, OP6_O_A_O_A_O_A, OP6_O_O_A_O_O_A
%type <y_word>	CASE_O_O_O

%type <y_word>	oper
