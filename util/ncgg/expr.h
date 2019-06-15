/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef EXPR_H_
#define EXPR_H_

typedef struct expr {
	int	ex_typ;
	short	ex_regset[SZOFSET(MAXREGS)];
	int	ex_index;
} expr_t,*expr_p;

#define TYPINT  1
#define TYPBOOL 2
#define TYPADDR 3
#define TYPREG  4

int ex_lookup(int operator, int lnode, int rnode);
void initnodes(void);
expr_t regvar_expr(expr_t e, int regtyp, int regsiz);
expr_t iextoaddr(expr_t e);
expr_t sum_expr(expr_t e1, expr_t e2);
int ne2expr(expr_t e1, expr_t e2);
int eq2expr(expr_t e1, expr_t e2);
expr_t all_expr(int all_no, int subreg);
expr_t perc_ident_expr(char *name);
expr_t tokm_expr(int tokarg, char *name);
int membset(int setno, char *name, short *regset, char *appearance, int restyp, int *typp);
void subregset(register short *sp, int subreg, register short *regset);
expr_t subreg_expr(int tokarg, int subreg);
expr_t ident_expr(char *name);
expr_t make_expr(int type, int operator, int op1, int op2);
int b_expr(expr_t e);
int i_expr(expr_t e);


/* When the type is register the regset contains the set of
   possible registers for checking purposes only.
*/

#endif /* EXPR_H_ */
