/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID1 "$Id$"

/*
 * INTEL 80386 C declarations
 */

#define low6(z)		(z & 077)
#define fit6(z)		(low6(z) == z)
#define low3(z)		(z & 07)
#define fit3(z)		(low3(z) == z)

#define FESC	0xD8		/* escape for 80[23]87 processor */

#define ufitb(z)	((unsigned)(z) <= 255)

#define IS_R8		0x100
#define IS_R32		0x200
#define IS_EXPR		0x400
#define IS_RSEG		0x800

#define is_expr(reg)	((reg)&IS_EXPR)
#define is_segreg(reg)	((reg)&IS_RSEG)
#define is_reg(reg)	(((reg)&(IS_R8|IS_R32)) != 0)
#define is_acc(reg)	(is_reg(reg) && ((reg & 07) == 0))

struct operand {
	int	mod;
	int	rm;
	int	reg;
	int	sib;		/* scale-index-base */
	expr_t	exp;
};

extern struct operand	op_1, op_2;

#define mod_1	op_1.mod
#define mod_2	op_2.mod
#define rm_1	op_1.rm
#define rm_2	op_2.rm
#define reg_1	op_1.reg
#define reg_2	op_2.reg
#define sib_1	op_1.sib
#define sib_2	op_2.sib
#define exp_1	op_1.exp
#define exp_2	op_2.exp

#ifdef RELOCATION
extern int	rel_1, rel_2;
#endif

#ifndef extern
extern char	regindex_ind[8][8];
#else
/*	First index is base register; second index is index register;
	sp cannot be an index register.
	For base and index register indirect mode, bp cannot be the
	base register, but this info is not included in this array.
	This can always be handled by using the base and index register with
	displacement mode.
*/
char	regindex_ind[8][8] = {
	000,	010,	020,	030,	-1,	050,	060,	070,
	001,	011,	021,	031,	-1,	051,	061,	071,
	002,	012,	022,	032,	-1,	052,	062,	072,
	003,	013,	023,	033,	-1,	053,	063,	073,
	004,	014,	024,	034,	-1,	054,	064,	074,
	005,	015,	025,	035,	-1,	055,	065,	075,
	006,	016,	026,	036,	-1,	056,	066,	076,
	007,	017,	027,	037,	-1,	057,	067,	077,
};
#endif

extern int	address_long INIT(1), operand_long INIT(1);
extern int	use32 INIT(1);

/* For 16-bit addressing: copied from i86 assembler */
#ifndef extern
extern char     sr_m[8];
#else
char    sr_m[8] = {
        -1,     -1,     -1,     7,      -1,     6,      4,      5
};
#endif

#ifndef extern
extern char     dr_m[8][8];
#else
char    dr_m[8][8] = {
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,     -1,     -1,     -1,     -1,     -1,     0,      1,
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,     -1,     -1,     -1,     -1,     -1,     2,      3,
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
        -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1
};
#endif
