/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * INTEL 8086 C declarations
 */

#define	low6(z)		(z & 077)
#define	fit6(z)		(low6(z) == z)
#define	low3(z)		(z & 07)
#define	fit3(z)		(low3(z) == z)

#define FESC	0xD8		/* escape for 8087 processor */
extern int	mrg_1,mrg_2;
extern expr_t	exp_1,exp_2;
#ifndef ASLD
extern int	rel_1, rel_2;
#endif

#ifndef extern
extern char	sr_m[8];
#else
char	sr_m[8] = {
	-1,	-1,	-1,	7,	-1,	6,	4,	5
};
#endif

#ifndef extern
extern char	dr_m[8][8];
#else
char	dr_m[8][8] = {
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	0,	1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	2,	3,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1
};
#endif

/* 8086 specific routines */
void ea_1(int param);
void ea_2(int param);
void reverse(void);
void badsyntax(void);
void regsize(register int sz);
void indexed(void);
void branch(register int opc,expr_t exp);
void pushop(register int opc);
void addop(register int opc);
void rolop(register int opc);
void incop(register int opc);
void callop(register int opc);
void xchg(register int opc);
void test(register int opc);
void mov(register int opc);
void imul(int opc);
