#define RCSID1 "$Header$"

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
