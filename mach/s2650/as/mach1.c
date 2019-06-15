/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * Signetics 2650 register names
 */

#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3

#define low7(z)		(z & 0x7F)
#define low13(z)	(z & 0x1FFF)

#define fit7(z)		(low7(z) == z)
#define fit13(z)	(low13(z) == z)

extern expr_t	par_exp;
extern unsigned	p_indirect;
extern int	p_indexed;

void page(void);
void pag2(void);
void rela(expr_t exp,int ind);
void absoa(expr_t exp, int ix, unsigned int ind);
void absob(expr_t exp, unsigned int ind);
