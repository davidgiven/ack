/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)mach1.c	1.8 */
/*
 * Motorola 68000/68010 dependent C declarations
 */

#define	low3(z)		((short)(z) & 07)
#define	low4(z)		((short)(z) & 017)
#define	low7(z)		((short)(z) & 0177)

#define	fit3(z)		((((z)-1) & ~((int)07)) == 0)
#define	fit4(z)		(((z) & ~((int)017)) == 0)
#define	fit7(z)		(((z) & ~((int)0177)) == 0)
#define sfit7(z)	(fit7((z)+64))
#define	fit8(z)		(((z) & ~((int)0xFF)) == 0)
#define	fit16(z)	(((z) & ~(0xFFFFL)) == 0)

#define	SIZE_B		0000
#define	SIZE_W		0100
#define	SIZE_L		0200
#define	SIZE_NON	0300
#define	SIZE_DEF	SIZE_W

#define FSIZE_L		00
#define FSIZE_S		01
#define FSIZE_X		02
#define FSIZE_P		03
#define FSIZE_W		04
#define FSIZE_D		05
#define FSIZE_B		06

#define DEF_FP          01000   /* default floating point processor */
extern int	co_id;

extern int	mrg_1,mrg_2;
extern expr_t	exp_1,exp_2;
#ifndef ASLD
extern valu_t	rel_1,rel_2;
#endif
extern int 	model;		/* 68000/68010 */
extern int	curr_instr;

/* addressing mode bits */
#define	DTA		0x01
#define	MEM		0x02
#define	CTR		0x04
#define	ALT		0x08
#define	FITB		0x10
#define	FITW		0x20
#define	PUTW		0x40
#define	PUTL		0x80

#ifndef extern
extern short	eamode[];
#else
short	eamode[] = {
/* 00A */	DTA        |ALT,
/* 01A */	            ALT,
/* 02A */	DTA|MEM|CTR|ALT,
/* 03A */	DTA|MEM    |ALT,
/* 04A */	DTA|MEM    |ALT,
/* 05A */	DTA|MEM|CTR|ALT|FITW|PUTW | (RELO2      )<<8,
/* 06A */	DTA|MEM|CTR|ALT     |PUTW | (RELO1      )<<8,
/* 07x */	0,
/* 070 */	DTA|MEM|CTR|ALT|FITW|PUTW | (RELO2      )<<8,
/* 071 */	DTA|MEM|CTR|ALT     |PUTL | (RELO4      )<<8,
/* 072 */	DTA|MEM|CTR    |FITW|PUTW | (RELO2|RELPC)<<8,
/* 073 */	DTA|MEM|CTR         |PUTW | (RELO1|RELPC)<<8,
/* 074x */	0,
/* 074B */	DTA|MEM        |FITB|PUTW | (RELO1      )<<8,
/* 074W */	DTA|MEM        |FITW|PUTW | (RELO2      )<<8,
/* 074L */	DTA|MEM             |PUTL | (RELO4      )<<8,
};
#endif

/* 680x0 specific instructions */
void ea_1(int sz, int bits);
void ea_2(int sz, int bits);
void indexmode(int hibyte);
void checksize(int sz, int bits);
void test68010(void);
void badoperand(void);
void shift_op(int opc, int sz);
void bitop(int opc);
void add(int opc, int sz);
void and(int opc, int sz);
int to_dreg(int opc, int sz, int bits);
int from_dreg(int opc, int sz, int bits);
void cmp(int sz);
void move(int sz);
void move_special(int sz);
int reverse(register int regs, int max);
void movem(int dr, int sz, int regs);
void movep(int sz);
void branch(int opc, expr_t exp);
void ea5x73(int rg, int sz);
void ea707172(int sz);
void ea6x(int rg, int ir, int sz);
void ea72(void);
void ea73(int ri, int sz);
void Xnofit(void);
void fbranch(int opc, expr_t exp);
void ch_sz_dreg(int size, int mode);
void check_fsize(int sz, int size);
